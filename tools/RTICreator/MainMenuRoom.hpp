/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <RealEngine/graphics/batches/GeometryBatch.hpp>
#include <RealEngine/graphics/batches/SpriteBatch.hpp>
#include <RealEngine/graphics/cameras/View2D.hpp>
#include <RealEngine/program/CommandLineArguments.hpp>
#include <RealEngine/rooms/Room.hpp>

/**
 * @brief Room with the UI
 */
class MainMenuRoom: public re::Room {
public:
    MainMenuRoom(re::CommandLineArguments args);

    void sessionStart(const re::RoomTransitionArguments& args) override;
    void sessionEnd() override;
    void step() override;
    void render(const re::CommandBuffer& cmdBuf, double interpolationFactor) override;
    void windowResizedCallback(glm::ivec2 oldSize, glm::ivec2 newSize) override;

private:
    void parametersGUI();

    void selectAndLoad();
    void save(const std::string& loc);
    void load(const std::string& filePath);

    re::SpriteBatch   m_sb{re::SpriteBatchCreateInfo{
          .renderPassSubpass = mainRenderPass().subpass(0),
          .maxSprites        = 1,
          .maxTextures       = 1
    }};
    re::GeometryBatch m_gb{re::GeometryBatchCreateInfo{
        .topology          = vk::PrimitiveTopology::eLineList,
        .renderPassSubpass = mainRenderPass().subpass(0),
        .maxVertices       = 16777216u
    }};

    // Texture
    std::optional<re::TextureShaped> m_texture;
    std::string                      m_textureLoc;
    std::string                      m_lastVisitedLoc;

    glm::vec2 m_cursorPosPrev = glm::vec2(0.0f, 0.0f);

    // View
    re::View2D m_texView;

    glm::vec2 m_overlap         = glm::vec2(0.2f, 0.2f);
    glm::vec3 m_backgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

    // Texture parameters
    glm::ivec2 m_subimagesSprites{};
    glm::vec2  m_subimageDims{1.0f};
    glm::vec2  m_pivot{};

    void drawTexture(const re::CommandBuffer& commandBuffer);
    void resetView();

    float m_drawScale = 1.0f;
};