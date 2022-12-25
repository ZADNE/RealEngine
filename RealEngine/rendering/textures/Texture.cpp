/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/Texture.hpp>

#include <RealEngine/rendering/CommandBuffer.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/utility/Error.hpp>

namespace RE {

using enum vk::ImageLayout;
using enum vk::PipelineStageFlagBits;

vk::ImageViewType imageViewType(vk::ImageType imageType, uint32_t layers) {
    if (layers > 0) {
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
    //Create the image
    using enum vk::MemoryPropertyFlagBits;
    using enum vk::ImageUsageFlagBits;
    auto HOST = eHostCoherent | eHostVisible;
    bool requiresStagingBuffer = !createInfo.texels.empty() && ((createInfo.memory & HOST) != HOST);
    m_image = s_device->createImage(vk::ImageCreateInfo{{},
        createInfo.type,
        createInfo.format,
        createInfo.extent,
        1u,                                             //Mip level count
        createInfo.layers,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        createInfo.usage | (requiresStagingBuffer ? eTransferDst : vk::ImageUsageFlagBits{}),
        vk::SharingMode::eExclusive
    });
    //Allocate memory for the image
    auto memReq = s_device->getImageMemoryRequirements2({m_image}).memoryRequirements;
    m_deviceMemory = s_device->allocateMemory(vk::MemoryAllocateInfo{
        memReq.size,
        selectMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
    });
    s_device->bindImageMemory(m_image, m_deviceMemory, 0u);
    //Initialize texels of the image
    if (!createInfo.texels.empty()) {
        initializeTexels(createInfo);
    }
    //Create image view
    m_imageView = s_device->createImageView(vk::ImageViewCreateInfo{{},
        m_image,
        imageViewType(createInfo.type, createInfo.layers),
        createInfo.format,
        vk::ComponentMapping{},                         //Component mapping (= identity)
        vk::ImageSubresourceRange{
            vk::ImageAspectFlagBits::eColor,
            0u,                                         //Mip level
            1u,                                         //Mip level count
            0u,                                         //Base array layer
            createInfo.layers                           //Array layer count
        }
    });
    //Create sampler
    m_sampler = s_device->createSampler(vk::SamplerCreateInfo{});
}

Texture::Texture(Texture&& other) noexcept :
    m_image(other.m_image),
    m_deviceMemory(other.m_deviceMemory),
    m_imageView(other.m_imageView),
    m_sampler(other.m_sampler) {
    other.m_image = nullptr;
    other.m_deviceMemory = nullptr;
    other.m_imageView = nullptr;
    other.m_sampler = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    std::swap(m_image, other.m_image);
    std::swap(m_deviceMemory, other.m_deviceMemory);
    std::swap(m_imageView, other.m_imageView);
    std::swap(m_sampler, other.m_sampler);
    return *this;
}

Texture::~Texture() {
    s_device->destroySampler(m_sampler);
    s_device->destroyImageView(m_imageView);
    s_device->destroyImage(m_image);
    s_device->free(m_deviceMemory);
}

void Texture::initializeTexels(const TextureCreateInfo& createInfo) {
    //Create a staging buffer
    Buffer stagingBuffer{
        createInfo.texels.size(),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    //Copy texels to the staging buffer
    {
        auto* mapped = stagingBuffer.map<unsigned char>(0u, createInfo.texels.size());
        std::memcpy(mapped, createInfo.texels.data(), createInfo.texels.size());
        stagingBuffer.unmap();
    }
    //Copy data from staging buffer to the image
    CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& commandBuffer) {
        transitionImageLayout(commandBuffer, eUndefined, eTransferDstOptimal);
    commandBuffer.copyBufferToImage(
        stagingBuffer.m_buffer,
        m_image,
        eTransferDstOptimal,
        vk::BufferImageCopy{
            0u, 0u, 0u,
            vk::ImageSubresourceLayers{
                vk::ImageAspectFlagBits::eColor,
                0u,                                     //Mip level
                0u,                                     //Base array layer
                1u                                      //Array layer count
            },
            vk::Offset3D{0u, 0u, 0u},
            createInfo.extent
        }
    );
    transitionImageLayout(commandBuffer, eTransferDstOptimal, eReadOnlyOptimal);
    });
}

void Texture::transitionImageLayout(const vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destStage;
    vk::AccessFlags sourceAccess;
    vk::AccessFlags destAccess;
    if (oldLayout == eUndefined && newLayout == eTransferDstOptimal) {
        sourceStage = eTopOfPipe;
        destStage = eTransfer;
        sourceAccess = {};
        destAccess = vk::AccessFlagBits::eTransferWrite;
    } else if (oldLayout == eTransferDstOptimal && newLayout == eReadOnlyOptimal) {
        sourceStage = eTransfer;
        destStage = eFragmentShader;
        sourceAccess = vk::AccessFlagBits::eTransferWrite;
        destAccess = vk::AccessFlagBits::eShaderRead;
    } else {
        throw Exception{"Unsupported layout transition!"};
    }
    commandBuffer.pipelineBarrier(
        sourceStage, destStage,
        vk::DependencyFlags{},
        {},                                             //Memory barriers
        {},                                             //Buffer memory barrier
        vk::ImageMemoryBarrier{
            sourceAccess, destAccess,
            oldLayout, newLayout,
            VK_QUEUE_FAMILY_IGNORED,                    //Source queue family index
            VK_QUEUE_FAMILY_IGNORED,                    //Dest queue family index
            m_image,
            vk::ImageSubresourceRange{
                vk::ImageAspectFlagBits::eColor,
                0u,                                     //Mip level
                1u,                                     //Mip level count
                0u,                                     //Base array layer
                1u                                      //Array layer count
            }
        }
    );
}

uint32_t Texture::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto memProperties = s_physicalDevice->getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw Exception{"Could not find memory that suits the texture!"};
}

}