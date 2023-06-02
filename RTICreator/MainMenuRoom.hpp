/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <RealEngine/program/CommandLineArguments.hpp>
#include <RealEngine/rendering/batches/GeometryBatch.hpp>
#include <RealEngine/rendering/batches/SpriteBatch.hpp>
#include <RealEngine/rendering/cameras/View2D.hpp>
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
    void render(const vk::CommandBuffer& commandBuffer, double interpolationFactor) override;
    void windowResizedCallback(
        const glm::ivec2& oldSize, const glm::ivec2& newSize
    ) override;

private:
    void parametersGUI();

    void selectAndLoad();
    void save(const std::string& loc);
    void load(const std::string& filePath);

    re::SpriteBatch   m_sb{1, 1};
    re::GeometryBatch m_gb{vk::PrimitiveTopology::eLineList, 128u, 1.0f};

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

    void drawTexture(const vk::CommandBuffer& commandBuffer);
    void resetView();

    float m_drawScale = 1.0f;
};