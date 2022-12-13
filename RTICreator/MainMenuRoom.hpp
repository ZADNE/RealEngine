/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <RealEngine/rooms/Room.hpp>
#include <RealEngine/program/CommandLineArguments.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/batches/SpriteBatch.hpp>
#include <RealEngine/rendering/batches/GeometryBatch.hpp>
#include <RealEngine/rendering/cameras/View2D.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/basic_shaders/AllShaders.hpp>

 /**
  * @brief Room with the UI
 */
template<RE::Renderer R>
class MainMenuRoom : public RE::Room {
public:

    MainMenuRoom(RE::CommandLineArguments args);

    void sessionStart(const RE::RoomTransitionArguments& args) override;
    void sessionEnd() override;
    void step() override;
    void render(double interpolationFactor) override;
    void windowResizedCallback(const glm::ivec2& oldSize, const glm::ivec2& newSize) override;

private:

    void parametersGUI();

    void selectAndLoad();
    void save(const std::string& loc);
    void load(const std::string& filePath);

    RE::SpriteBatch<R> m_sb{{.vert = RE::sprite_vert, .frag = RE::sprite_frag}};
    RE::GeometryBatch<R> m_gb{{.vert = RE::geometry_vert, .frag = RE::geometry_frag}};

    //Texture
    std::optional<RE::Texture<R>> m_texture;
    std::string m_textureLoc;
    std::string m_lastVisitedLoc;

    glm::vec2 m_cursorPosPrev = glm::vec2(0.0f, 0.0f);

    //View
    RE::View2D m_texView;
    struct ViewMatrices {
        glm::mat4 textureView;
        glm::mat4 windowView;
    };
    RE::PerFrameInFlight<RE::Buffer<R>> m_ubos{
        RE::Buffer<R>{sizeof(ViewMatrices), vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent},
        RE::Buffer<R>{sizeof(ViewMatrices), vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent}
    };
    RE::PerFrameInFlight<ViewMatrices*> m_mappedUbos{nullptr};

    glm::vec2 m_overlap = glm::vec2(0.2f, 0.2f);
    glm::vec3 m_backgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

    //Texture parameters
    size_t m_minFilter = 0u;
    size_t m_magFilter = 0u;
    size_t m_wrapStyleX = 0u;
    size_t m_wrapStyleY = 0u;
    glm::ivec2 m_subimagesSprites{};
    glm::vec2 m_subimageDims{1.0f};
    glm::vec2 m_pivot{};
    glm::vec4 m_borderColor{0.0f, 0.0f, 0.0f, 1.0f};


    glm::vec2 m_offset = glm::vec2(0.0f, 0.0f);
    void drawTexture();
    void resetView();

    float m_drawScale = 1.0f;
};