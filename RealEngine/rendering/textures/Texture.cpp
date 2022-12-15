/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/Texture.hpp>

#include <RealEngine/rendering/textures/TextureFlagsToString.hpp>
#include <RealEngine/resources/PNGLoader.hpp>
#include <RealEngine/utility/Error.hpp>

#include <RealEngine/rendering/buffers/Buffer.hpp>

namespace RE {

using enum vk::ImageLayout;
using enum vk::PipelineStageFlagBits;

glm::vec4 colorToFloatColor(Color color, TextureFormat type) {
    glm::vec4 borderRGBA = glm::vec4{color.r, color.g, color.b, color.a};
    switch (type) {
    case TextureFormat::NORMALIZED_UNSIGNED: borderRGBA = borderRGBA / 255.0f; break;
    case TextureFormat::NORMALIZED_SIGNED: borderRGBA = (borderRGBA / 255.0f) * 2.0f - 1.0f; break;
    case TextureFormat::INTEGRAL_UNSIGNED: break;
    case TextureFormat::INTEGRAL_SIGNED: borderRGBA = borderRGBA - 128.0f; break;
    }
    return borderRGBA;
}

Texture::Texture(Texture&& other) noexcept :
    m_image(other.m_image),
    m_deviceMemory(other.m_deviceMemory),
    m_flags(other.m_flags),
    m_subimageDims(other.m_subimageDims),
    m_pivot(other.m_pivot),
    m_subimagesSpritesCount(other.m_subimagesSpritesCount),
    m_trueDims(other.m_trueDims),
    m_borderColor(other.m_borderColor) {
    other.m_image = nullptr;
    other.m_deviceMemory = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    std::swap(m_image, other.m_image);
    std::swap(m_deviceMemory, other.m_deviceMemory);
    m_flags = other.m_flags;
    m_trueDims = other.m_trueDims;
    m_subimageDims = other.m_subimageDims;
    m_pivot = other.m_pivot;
    m_subimagesSpritesCount = other.m_subimagesSpritesCount;
    m_borderColor = other.m_borderColor;
    return *this;
}

Texture::Texture(const std::string& filePathPNG, const TextureParameters& defParams) {
    PNGLoader::PNGData pngData{
        .params = defParams
    };

    if (PNGLoader::load(filePathPNG, pngData) == 0) {
        init(Raster{pngData.dims, pngData.params.getChannels(), pngData.pixels}, pngData.params);
    }
}

Texture::Texture(const TextureSeed& seed) :
    Texture(seed.toFullPath(), DEFAULT_PARAMETERS) {
}

Texture::Texture(const Raster& raster, const TextureParameters& params/* = DEFAULT_PARAMETERS*/) {
    init(raster, params);
}

Texture::~Texture() {
    s_device->destroyImage(m_image);
    s_device->free(m_deviceMemory);
}

TextureParameters Texture::getParameters() const {
    return TextureParameters{TextureGeometry{m_subimageDims, m_pivot, m_subimagesSpritesCount}, m_flags, m_borderColor};
}

bool Texture::saveToFile(const std::string& filePathPNG) {
    return saveToFile(filePathPNG, getParameters());
}

bool Texture::saveToFile(const std::string& filePathPNG, const TextureParameters& params) {
    /*PNGLoader::PNGData pngData{
        .dims = m_trueDims,
        .params = params
    };
    pngData.pixels.resize(Raster::minimumRequiredMemory(m_trueDims, params.getChannels()));
    //Download pixels
    getTexels(pngData.pixels.size(), pngData.pixels.data());
    return PNGLoader::save(filePathPNG, pngData) == 0;*/
    //TODO
    return false;
}

void Texture::init(const Raster& raster, const TextureParameters& params) {
    if (params.isGeometryDefinedByImage()) {//Geometry defined by image
        m_subimageDims = raster.getDims();
        m_pivot = glm::vec2{0.0f, 0.0f};
        m_subimagesSpritesCount = glm::vec2{1.0f, 1.0f};
    } else {//Geometry defined by parameters
        m_subimageDims = params.getSubimageDims();
        m_pivot = params.getPivot();
        m_subimagesSpritesCount = params.getSubimagesSpritesCount();
    }
    m_flags = TextureFlags{params};
    m_trueDims = raster.getDims();
    m_borderColor = params.getBorderColor();
    //Create a staging buffer
    vk::DeviceSize nBytes = m_trueDims.x * m_trueDims.y * 4u;
    Buffer<RendererVK13> stagingBuffer{
        nBytes,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    //Copy texels to the staging buffer
    {
        auto* mapped = stagingBuffer.map<unsigned char>(0u, nBytes);
        std::memcpy(mapped, raster.getTexels().data(), nBytes);
        stagingBuffer.unmap();
    }
    //Create the image
    using enum vk::ImageUsageFlagBits;
    m_image = s_device->createImage(vk::ImageCreateInfo{{},
        vk::ImageType::e2D,                             //Type
        vk::Format::eR8G8B8A8Unorm,                     //Format
        vk::Extent3D{m_trueDims.x, m_trueDims.y, 1},    //Size
        1u,                                             //Mip level count
        1u,                                             //Array layer count
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        eTransferDst | eSampled,                        //Usage
        vk::SharingMode::eExclusive
    });
    //Allocate memory for the image
    auto memReq = s_device->getImageMemoryRequirements2({m_image}).memoryRequirements;
    m_deviceMemory = s_device->allocateMemory(vk::MemoryAllocateInfo{
        memReq.size,
        selectMemoryType(memReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)
    });
    s_device->bindImageMemory(m_image, m_deviceMemory, 0u);
    //Copy data from staging buffer to the image
    auto commandBuffer = s_device->allocateCommandBuffers({
        *s_commandPool, vk::CommandBufferLevel::ePrimary, 1u
    }).front();
    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    transitionImageLayout(commandBuffer, eUndefined, eTransferDstOptimal);
    commandBuffer.copyBufferToImage(
        stagingBuffer.m_id.m_.vk13.mainBuffer,
        m_image,
        eTransferDstOptimal,
        vk::BufferImageCopy{
            0u, 0u, 0u,
            vk::ImageSubresourceLayers{
                vk::ImageAspectFlagBits::eColor,
                0u,                                         //Mip level
                0u,                                         //Base array layer
                1u                                          //Array layer count
            },
            vk::Offset3D{0u, 0u, 0u},
            vk::Extent3D{m_trueDims.x, m_trueDims.y, 1u}
        }
    );
    transitionImageLayout(commandBuffer, eTransferDstOptimal, eReadOnlyOptimal);
    commandBuffer.end();
    s_graphicsQueue->submit(vk::SubmitInfo{nullptr, {}, commandBuffer});
    s_graphicsQueue->waitIdle();
    s_device->freeCommandBuffers(*s_commandPool, commandBuffer);
}

void Texture::transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
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
                0u,                                         //Mip level
                1u,                                         //Mip level count
                0u,                                         //Base array layer
                1u                                          //Array layer count
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