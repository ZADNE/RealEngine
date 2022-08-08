﻿/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <RealEngine/main/room/Room.hpp>
#include <RealEngine/rendering/cameras/View2D.hpp>
#include <RealEngine/rendering/targets/Viewport.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>

/**
 * @brief Room with the UI
*/
class MainMenuRoom : public RE::Room {
public:
	MainMenuRoom(RE::CommandLineArguments args);

	void sessionStart(const RE::RoomTransitionParameters& params) override;
	void sessionEnd() override;
	void step() override;
	void render(double interpolationFactor) override;

	const RE::RoomDisplaySettings& getDisplaySettings() override {
		static RE::RoomDisplaySettings settings{
			.clearColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
			.framesPerSecondLimit = 144,
			.usingImGui = true
		};
		return settings;
	}

	void windowResized(const glm::ivec2& newDims) override;
private:
	void parametersGUI();

	void selectAndLoad();
	void save(const std::string& loc);
	void load(const std::string& filePath);

	//Texture
	std::optional<RE::Texture> m_texture;
	std::string m_textureLoc;
	std::string m_lastVisitedLoc;

	glm::vec2 m_cursorPosPrev = glm::vec2(0.0f, 0.0f);

	//View
	RE::View2D m_texView;
	RE::TypedBuffer m_texViewUBO{ RE::UNIF_BUF_VIEWPORT_MATRIX, sizeof(glm::mat4), RE::BufferUsageFlags::DYNAMIC_STORAGE };
	glm::vec2 m_overlap = glm::vec2(0.2f, 0.2f);
	glm::vec3 m_backgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

	//Texture parameters
	size_t m_minFilter = 0u;
	size_t m_magFilter = 0u;
	size_t m_wrapStyleX = 0u;
	size_t m_wrapStyleY = 0u;
	glm::ivec2 m_subimagesSprites{};
	glm::vec2 m_subimageDims{ 1.0f };
	glm::vec2 m_pivot{};
	glm::vec4 m_borderColor{ 0.0f, 0.0f, 0.0f, 1.0f };


	glm::vec2 m_offset = glm::vec2(0.0f, 0.0f);
	void drawTexture();
	void resetView();

	float m_drawScale = 1.0f;
};