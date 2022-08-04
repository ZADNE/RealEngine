/*! 
 *  @author    Dubsky Tomas
 */
#include "MainMenuRoom.hpp"

#include <iostream>

#if not defined(WIN32) && not defined(_WIN32) && not defined(__WIN32__)
#error "Since this tool requires file selection dialog window and I did not want another library dependency, it is a Windows-only tool. Sorry!"
#endif

#define NOMINMAX 1
#include <Windows.h>

#include <ImGui/imgui.h>
#include <RealEngine/graphics/GeometryBatch.hpp>
#include <RealEngine/graphics/SpriteBatch.hpp>
#include <RealEngine/main/Error.hpp>

#include <RTICreator/ComboConstants.hpp>

MainMenuRoom::MainMenuRoom(RE::CommandLineArguments args) :
	m_texView(window()->getDims()) {

	//Set last visited location to location of this executable
	m_lastVisitedLoc = args[0];

	if (args.size() > 1) {
		//We have a second argument which should be the texture that should be loaded on start-up
		load(args[1]);
	}
}

void MainMenuRoom::sessionStart(const RE::RoomTransitionParameters& params) {

}

void MainMenuRoom::sessionEnd() {

}

void MainMenuRoom::step() {
	auto cursorPos = (glm::vec2)input()->getCursorAbs();

	if (input()->isDown(RE::Key::MMB) && m_texture) {
		m_texView.shiftPosition((glm::vec2{ m_cursorPosPrev - cursorPos } / m_drawScale));
	}

	if (m_texture) {
		if (input()->wasPressed(RE::Key::UMW)) {
			m_texView.zoom({ 1.5f, 1.5f });
			m_drawScale *= 1.5f;
		}
		if (input()->wasPressed(RE::Key::DMW)) {
			m_texView.zoom({ 0.66666666f, 0.66666666f });
			m_drawScale *= 0.66666666f;
		}
	}
	m_cursorPosPrev = cursorPos;

	if (input()->wasReleased(RE::Key::R)) {
		resetView();
	}
}

void MainMenuRoom::render(double interpolationFactor) {
	auto mat = m_texView.getViewMatrix();
	m_texViewUBO.overwrite(0u, mat);
	m_texViewUBO.bindIndexed();

	//Texture
	if (m_texture) {
		drawTexture();
	}

	RE::Viewport::getWindowMatrixUniformBuffer().bindIndexed();

	//Menu
	if (ImGui::Begin("RTICreator v3.0.0")) {
		if (ImGui::BeginTabBar("##TabBar")) {
			if (ImGui::BeginTabItem("File")) {
				if (ImGui::Button("Load a texture")) selectAndLoad();
				if (!m_textureLoc.empty() && ImGui::Button("Save the texture")) save(m_textureLoc);
				ImGui::EndTabItem();
			}
			if (m_texture) {
				if (ImGui::BeginTabItem("Parameters")) {
					parametersGUI();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("View")) {
					ImGui::SliderFloat2("Overlap (to see wrapping)", &m_overlap.x, 0.0f, 1.0f);
					if (ImGui::Button("Reset view"))
						resetView();
					if (ImGui::ColorPicker3("Background color", &m_backgroundColor.x))
						glClearColor(m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, 1.0f);
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

void MainMenuRoom::windowResized(const glm::ivec2& newDims) {
	m_texView.resizeView(newDims);
}

void MainMenuRoom::parametersGUI() {
	if (imguiCombo("Minification filter", m_minFilter, MIN_FILTERS_LABELS)) {
		m_texture->setMinFilter(MIN_FILTERS[m_minFilter]);
	}
	if (imguiCombo("Magnification filter", m_magFilter, MAG_FILTERS_LABELS)) {
		m_texture->setMagFilter(MAG_FILTERS[m_magFilter]);
	}
	if (imguiCombo("Wrap style of X axis", m_wrapStyleX, WRAP_STYLES_LABELS)) {
		m_texture->setWrapStyleX(WRAP_STYLES[m_wrapStyleX]);
	}
	if (imguiCombo("Wrap style of Y axis", m_wrapStyleY, WRAP_STYLES_LABELS)) {
		m_texture->setWrapStyleX(WRAP_STYLES[m_wrapStyleY]);
	}
	if (ImGui::InputInt("Number of subimages", &m_subimagesSprites.x)) {
		m_texture->setSubimagesSpritesCount(m_subimagesSprites);
	}
	if (ImGui::InputInt("Number of sprites", &m_subimagesSprites.y)) {
		m_texture->setSubimagesSpritesCount(m_subimagesSprites);
	}
	if (ImGui::InputFloat2("Subimage dims", &m_subimageDims.x)) {
		m_texture->setSubimageDims(m_subimageDims);
	}
	if (ImGui::InputFloat2("Pivot offset", &m_pivot.x)) {
		m_texture->setPivot(m_pivot);
	}
	ImGui::SameLine();
	if (ImGui::Button("Center")) {
		m_pivot = m_subimageDims * 0.5f;
		m_texture->setPivot(m_pivot);
	}


	if ((WRAP_STYLES[m_wrapStyleY] == RE::TextureWrapStyle::CLAMP_TO_BORDER ||
		WRAP_STYLES[m_wrapStyleX] == RE::TextureWrapStyle::CLAMP_TO_BORDER)
		&& ImGui::ColorPicker4("Border color", &m_borderColor.x)) {
		m_texture->setBorderColor(m_borderColor);
	}
}

void MainMenuRoom::selectAndLoad() {
	char filename[MAX_PATH] = {};
	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = "PNG Files (*.png)\0*.png\0Any Files (*.*)\0*.*\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a texture for which you want to set parameters";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	ofn.lpstrInitialDir = m_lastVisitedLoc.c_str();

	synchronizer()->pauseSteps();
	GetOpenFileNameA(&ofn);
	synchronizer()->resumeSteps();
	m_lastVisitedLoc = filename;

	load(filename);
}

void MainMenuRoom::save(const std::string& loc) {
	if (loc.empty()) { return; }
	m_texture->saveToFile(loc);
}

void MainMenuRoom::load(const std::string& loc) {
	if (loc.empty()) { return; }
	try {
		m_texture = RE::Texture{ loc };
	}
	catch (...) {
		return;
	}

	//Save texture's location
	m_textureLoc = loc;

	//Init GUI
	m_minFilter = findSelectedFromCombo(m_texture->getMinFilter(), MIN_FILTERS);
	m_magFilter = findSelectedFromCombo(m_texture->getMagFilter(), MAG_FILTERS);
	m_wrapStyleX = findSelectedFromCombo(m_texture->getWrapStyleX(), WRAP_STYLES);
	m_wrapStyleY = findSelectedFromCombo(m_texture->getWrapStyleY(), WRAP_STYLES);
	m_subimagesSprites = m_texture->getSubimagesSpritesCount();
	m_subimageDims = m_texture->getSubimageDims();
	m_pivot = m_texture->getPivot();
	m_borderColor = glm::vec4(m_texture->getBorderColor()) / 255.0f;
}

void MainMenuRoom::drawTexture() {
	glm::vec2 windowDims = glm::vec2(window()->getDims());
	auto texDims = m_texture->getSubimagesSpritesCount() * m_texture->getSubimageDims();
	auto botLeft = -texDims * 0.5f;

	glm::vec4 posSizeRect = {
		-texDims * (m_overlap + 0.5f),
		texDims * (1.0f + 2.0f * m_overlap)
	};
	glm::vec4 uvRect = {
		-m_overlap,
		1.0f + m_overlap * 2.0f
	};

	RE::SpriteBatch::std().begin();
	RE::SpriteBatch::std().add(posSizeRect, uvRect, *m_texture, 0);
	RE::SpriteBatch::std().end(RE::GlyphSortType::POS_TOP);
	RE::SpriteBatch::std().draw();

	auto& gb = RE::GeometryBatch::std();
	gb.begin();
	std::vector<RE::VertexPOCO> vertices;
	glm::vec2 subimageSprite = m_texture->getSubimagesSpritesCount();
	vertices.reserve((size_t)(subimageSprite.x * subimageSprite.y) * 4u);
	RE::Color color{ 0, 255u, 0u, 255u };
	auto subimageDims = m_texture->getSubimageDims();
	//Subimages
	for (float x = 1.0f; x < subimageSprite.x; ++x) {
		glm::vec2 coord = botLeft + glm::vec2(x, 0.0f) * subimageDims;
		vertices.emplace_back(coord, color);
		vertices.emplace_back(coord + glm::vec2(0.0f, texDims.y), color);
	}
	for (float y = 1.0f; y < subimageSprite.y; ++y) {
		glm::vec2 coord = botLeft + glm::vec2(0.0f, y) * subimageDims;
		vertices.emplace_back(coord, color);
		vertices.emplace_back(coord + glm::vec2(texDims.x, 0.0f), color);
	}
	if (vertices.size() > 0u) {
		gb.addPrimitives(RE::PRIM::LINES, 0u, vertices.size(), vertices.data(), false);
		vertices.clear();
	}
	//Pivots
	color = { 0u, 0u, 255u, 255u };
	glm::vec2 pivotOffset = m_texture->getPivot();
	float pivotMarkRadius = glm::min(subimageDims.x, subimageDims.y) * 0.5f;
	for (float x = 0.0f; x < m_texture->getSubimagesSpritesCount().x; ++x) {
		for (float y = 0.0f; y < m_texture->getSubimagesSpritesCount().y; ++y) {
			glm::vec2 pivotPos = botLeft + glm::vec2(x, y) * subimageDims + pivotOffset;
			vertices.emplace_back(pivotPos + glm::vec2(pivotMarkRadius, pivotMarkRadius), color);
			vertices.emplace_back(pivotPos + glm::vec2(-pivotMarkRadius, -pivotMarkRadius), color);
			vertices.emplace_back(pivotPos + glm::vec2(pivotMarkRadius, -pivotMarkRadius), color);
			vertices.emplace_back(pivotPos + glm::vec2(-pivotMarkRadius, pivotMarkRadius), color);
		}
	}

	gb.addPrimitives(RE::PRIM::LINES, 0u, vertices.size(), vertices.data(), false);
	vertices.clear();
	//Whole image
	color = { 255u, 0u, 0u, 255u };
	vertices.emplace_back(botLeft, color);
	vertices.emplace_back(botLeft + glm::vec2(texDims.x, 0.0f), color);
	vertices.emplace_back(botLeft + glm::vec2(texDims.x, texDims.y), color);
	vertices.emplace_back(botLeft + glm::vec2(0.0f, texDims.y), color);
	gb.addPrimitives(RE::PRIM::LINE_LOOP, 0u, vertices.size(), vertices.data());

	gb.end();
	gb.end();

	RE::GeometryBatch::std().draw();
}

void MainMenuRoom::resetView() {
	m_texView.setScale({ 1.0f, 1.0f });
	m_texView.setPosition(glm::vec2{ 0.0f, 0.0f });
	m_drawScale = 1.0f;
}
