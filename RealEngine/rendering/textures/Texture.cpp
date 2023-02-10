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
    //Create the image
    using enum vk::MemoryPropertyFlagBits;
    using enum vk::ImageUsageFlagBits;
    auto HOST = eHostCoherent | eHostVisible;
    bool requiresStagingBuffer = !createInfo.texels.empty() && ((createInfo.memory & HOST) != HOST);
    m_image = device().createImage(vk::ImageCreateInfo{{},
        createInfo.type,
        createInfo.format,
        {createInfo.extent.x, createInfo.extent.y, createInfo.extent.z},
        1u,                                             //Mip level count
        createInfo.layers,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        createInfo.usage | (requiresStagingBuffer ? eTransferDst : vk::ImageUsageFlagBits{}),
        vk::SharingMode::eExclusive,
        {},
        eUndefined
    });
    //Allocate memory for the image
    auto memReq = device().getImageMemoryRequirements2({m_image}).memoryRequirements;
    m_deviceMemory = device().allocateMemory(vk::MemoryAllocateInfo{
        memReq.size,
        selectMemoryType(memReq.memoryTypeBits, createInfo.memory)
    });
    device().bindImageMemory(m_image, m_deviceMemory, 0u);
    if (!createInfo.texels.empty()) {
        //Initialize texels of the image and transit to initial layout
        initializeTexels(createInfo);
    } else {
        //Only transit to initial layout
        CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& commandBuffer) {
            pipelineImageBarrier(
                commandBuffer,
                eUndefined, createInfo.initialLayout,       //Image layouts
                eTopOfPipe, eTransfer,                      //Pipeline stage
                {}, {},
                createInfo.layers
            );
        });
    }
    //Create image view
    m_imageView = device().createImageView(vk::ImageViewCreateInfo{{},
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
    m_sampler = device().createSampler(vk::SamplerCreateInfo{});
}

Texture::Texture(Texture&& other) noexcept:
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
    deletionQueue().enqueueDeletion(m_sampler);
    deletionQueue().enqueueDeletion(m_imageView);
    deletionQueue().enqueueDeletion(m_image);
    deletionQueue().enqueueDeletion(m_deviceMemory);
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
        pipelineImageBarrier(
            commandBuffer,
            eUndefined, eTransferDstOptimal,            //Image layouts
            eAllCommands, eAllCommands,                 //Pipeline stage
            {}, vk::AccessFlagBits::eTransferWrite,     //Access flags
            createInfo.layers                           //Array layer count
        );
        commandBuffer.copyBufferToImage(
            *stagingBuffer,
            m_image,
            eTransferDstOptimal,
            vk::BufferImageCopy{
                0u, 0u, 0u,
                vk::ImageSubresourceLayers{
                    vk::ImageAspectFlagBits::eColor,
                    0u,                                 //Mip level
                    0u,                                 //Base array layer
                    1u                                  //Array layer count
                },
                vk::Offset3D{0u, 0u, 0u},
                {createInfo.extent.x, createInfo.extent.y, createInfo.extent.z}
            }
        );
        using enum vk::AccessFlagBits;
        pipelineImageBarrier(
            commandBuffer,
            eTransferDstOptimal, createInfo.initialLayout,//Image layouts
            eTransfer, eFragmentShader,                 //Pipeline stage
            eTransferWrite, eShaderRead,                //Access flags
            createInfo.layers                           //Array layer count
        );
    });
}

void Texture::pipelineImageBarrier(
    const vk::CommandBuffer& commandBuffer,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
    vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
    uint32_t layerCount
) {
    commandBuffer.pipelineBarrier(
        srcStage, dstStage,
        vk::DependencyFlags{},
        {},                                             //Memory barriers
        {},                                             //Buffer memory barrier
        vk::ImageMemoryBarrier{
            srcAccess, dstAccess,
            oldLayout, newLayout,
            VK_QUEUE_FAMILY_IGNORED,                    //Source queue family index
            VK_QUEUE_FAMILY_IGNORED,                    //Dest queue family index
            m_image,
            vk::ImageSubresourceRange{
                vk::ImageAspectFlagBits::eColor,
                0u,                                     //Mip level
                1u,                                     //Mip level count
                0u,                                     //Base array layer
                layerCount                              //Array layer count
            }
        }
    );
}

uint32_t Texture::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    auto memProperties = physicalDevice().getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw Exception{"Could not find memory that suits the texture!"};
}

}