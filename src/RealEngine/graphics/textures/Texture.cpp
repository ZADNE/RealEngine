/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/CommandBuffer.hpp>
#include <RealEngine/graphics/buffers/BufferMapped.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/utility/Error.hpp>

using enum vk::ImageLayout;
using enum vk::PipelineStageFlagBits;
using enum vma::AllocationCreateFlagBits;
using enum vma::MemoryUsage;

namespace re {

constexpr auto k_hostAccess = eHostAccessRandom | eHostAccessSequentialWrite;

vk::ImageViewType imageViewType(vk::ImageType imageType, uint32_t layers) {
    if (layers > 1) {
        switch (imageType) {
        case vk::ImageType::e1D: return vk::ImageViewType::e1DArray;
        case vk::ImageType::e2D: return vk::ImageViewType::e2DArray;
        }
    } else {
        switch (imageType) {
        case vk::ImageType::e1D: return vk::ImageViewType::e1D;
        case vk::ImageType::e2D: return vk::ImageViewType::e2D;
        case vk::ImageType::e3D: return vk::ImageViewType::e3D;
        }
    }
    throw Exception{"Unsupported image type"};
}

Texture::Texture(const TextureCreateInfo& createInfo) {
    // Create the image
    using enum vk::ImageUsageFlagBits;
    bool requiresStagingBuffer = !createInfo.texels.empty() &&
                                 !(createInfo.allocFlags & k_hostAccess);
    auto imageCreateInfo = vk::ImageCreateInfo{
        createInfo.flags,
        createInfo.type,
        createInfo.format,
        {createInfo.extent.x, createInfo.extent.y, createInfo.extent.z},
        1u, // Mip level count
        createInfo.layers,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        createInfo.usage |
            (requiresStagingBuffer ? eTransferDst : vk::ImageUsageFlagBits{}),
        vk::SharingMode::eExclusive,
        {},
        eUndefined,
        createInfo.pNext};
    vma::AllocationCreateInfo allocCreateInfo{
        createInfo.allocFlags, createInfo.memoryUsage};
    // Create the image
    std::tie(m_image, m_allocation) =
        allocator().createImage(imageCreateInfo, allocCreateInfo);
    if (!createInfo.texels.empty()) {
        // Initialize texels of the image and transit to initial layout
        initializeTexels(createInfo);
    } else if (createInfo.initialLayout != eUndefined) {
        // Transit to initial layout
        CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& cmdBuf) {
            pipelineImageBarrier(
                cmdBuf,
                eUndefined,
                createInfo.initialLayout, // Image layouts
                eTopOfPipe,
                eTransfer, // Pipeline stage
                {},
                {},
                createInfo.layers
            );
        });
    }
    // Create image view
    m_imageView = device().createImageView(vk::ImageViewCreateInfo{
        {},
        m_image,
        imageViewType(createInfo.type, createInfo.layers),
        createInfo.format,
        vk::ComponentMapping{}, // Component mapping (= identity)
        vk::ImageSubresourceRange{
            createInfo.aspects,
            0u,               // Mip level
            1u,               // Mip level count
            0u,               // Base array layer
            createInfo.layers // Array layer count
        }});
    // Create sampler
    if (createInfo.hasSampler) {
        m_sampler = device().createSampler(vk::SamplerCreateInfo{
            {}, createInfo.magFilter, createInfo.minFilter, createInfo.mipmapMode});
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_allocation(other.m_allocation)
    , m_image(other.m_image)
    , m_imageView(other.m_imageView)
    , m_sampler(other.m_sampler) {
    other.m_allocation = nullptr;
    other.m_image      = nullptr;
    other.m_imageView  = nullptr;
    other.m_sampler    = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    std::swap(m_allocation, other.m_allocation);
    std::swap(m_image, other.m_image);
    std::swap(m_imageView, other.m_imageView);
    std::swap(m_sampler, other.m_sampler);
    return *this;
}

Texture::~Texture() {
    deletionQueue().enqueueDeletion(m_sampler);
    deletionQueue().enqueueDeletion(m_imageView);
    deletionQueue().enqueueDeletion(m_image);
    deletionQueue().enqueueDeletion(m_allocation);
}

void Texture::initializeTexels(const TextureCreateInfo& createInfo) {
    // Create a staging buffer
    BufferMapped<std::byte> stagingBuffer{BufferCreateInfo{
        .allocFlags  = eHostAccessSequentialWrite | eMapped,
        .memoryUsage = eAutoPreferHost,
        .sizeInBytes = createInfo.texels.size(),
        .usage       = vk::BufferUsageFlagBits::eTransferSrc}};
    // Copy texels to the staging buffer
    std::memcpy(
        stagingBuffer.mapped(), createInfo.texels.data(), createInfo.texels.size()
    );
    // Copy data from staging buffer to the image
    CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& cmdBuf) {
        pipelineImageBarrier(
            cmdBuf,
            eUndefined,
            eTransferDstOptimal, // Image layouts
            eAllCommands,
            eAllCommands, // Pipeline stage
            {},
            vk::AccessFlagBits::eTransferWrite, // Access flags
            createInfo.layers                   // Array layer count
        );
        cmdBuf.copyBufferToImage(
            stagingBuffer.buffer(),
            m_image,
            eTransferDstOptimal,
            vk::BufferImageCopy{
                0u,
                0u,
                0u,
                vk::ImageSubresourceLayers{
                    vk::ImageAspectFlagBits::eColor,
                    0u, // Mip level
                    0u, // Base array layer
                    1u  // Array layer count
                },
                vk::Offset3D{0u, 0u, 0u},
                {createInfo.extent.x, createInfo.extent.y, createInfo.extent.z}}
        );
        using enum vk::AccessFlagBits;
        pipelineImageBarrier(
            cmdBuf,
            eTransferDstOptimal,
            createInfo.initialLayout, // Image layouts
            eTransfer,
            eFragmentShader, // Pipeline stage
            eTransferWrite,
            eShaderRead,      // Access flags
            createInfo.layers // Array layer count
        );
    });
}

void Texture::pipelineImageBarrier(
    const vk::CommandBuffer& cmdBuf,
    vk::ImageLayout          oldLayout,
    vk::ImageLayout          newLayout,
    vk::PipelineStageFlags   srcStage,
    vk::PipelineStageFlags   dstStage,
    vk::AccessFlags          srcAccess,
    vk::AccessFlags          dstAccess,
    uint32_t                 layerCount
) {
    cmdBuf.pipelineBarrier(
        srcStage,
        dstStage,
        vk::DependencyFlags{},
        {}, // Memory barriers
        {}, // Buffer memory barrier
        vk::ImageMemoryBarrier{
            srcAccess,
            dstAccess,
            oldLayout,
            newLayout,
            vk::QueueFamilyIgnored, // Source queue family index
            vk::QueueFamilyIgnored, // Dest queue family index
            m_image,
            vk::ImageSubresourceRange{
                vk::ImageAspectFlagBits::eColor,
                0u,        // Mip level
                1u,        // Mip level count
                0u,        // Base array layer
                layerCount // Array layer count
            }}
    );
}

} // namespace re