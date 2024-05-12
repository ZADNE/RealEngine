/*!
 *  @author    Dubsky Tomas
 */
#include <algorithm>
#include <iostream>

#if not defined(WIN32) && not defined(_WIN32) && not defined(__WIN32__)
#    error \
        "Since this tool requires file selection dialog window and I did not want another library dependency, it is a Windows-only tool. Sorry!"
#endif
#define NOMINMAX 1
#include <Windows.h>

#include <RealEngine/utility/Error.hpp>

#include <RTICreator/MainMenuRoom.hpp>

using namespace ImGui;

MainMenuRoom::MainMenuRoom(re::CommandLineArguments args)
    : Room(0, re::RoomDisplaySettings{.framesPerSecondLimit = 144, .imGuiSubpassIndex = 0})
    , m_texView(engine().windowDims()) {

    engine().setWindowTitle("RTICreator v4.0.0");

    // Set last visited location to location of this executable
    m_lastVisitedLoc = args[0];

    if (args.size() > 1) {
        // We have a second argument which should be a texture that should be
        // loaded on start-up
        load(args[1]);
    }

    // Initialize window view matrices
    auto window = engine().windowDims();
    windowResizedCallback(window, window);
}

void MainMenuRoom::sessionStart(const re::RoomTransitionArguments& args) {
}

void MainMenuRoom::sessionEnd() {
}

void MainMenuRoom::step() {
    if (m_texture) {
        // Handle input
        auto cursorPos = (glm::vec2)engine().cursorAbs();
        if (engine().isKeyDown(re::Key::MMB) && m_texture) {
            m_texView.shiftPosition((glm::vec2{m_cursorPosPrev - cursorPos} / m_drawScale)
            );
        }
        if (engine().wasKeyPressed(re::Key::UMW)) {
            m_texView.zoom({1.5f, 1.5f});
            m_drawScale *= 1.5f;
        }
        if (engine().wasKeyPressed(re::Key::DMW)) {
            m_texView.zoom({0.66666666f, 0.66666666f});
            m_drawScale *= 0.66666666f;
        }
        if (engine().wasKeyReleased(re::Key::R)) {
            resetView();
        }
        m_cursorPosPrev = cursorPos;
    }
}

void MainMenuRoom::render(const re::CommandBuffer& cmdBuf, double interpolationFactor) {
    vk::ClearValue clearVal = vk::ClearColorValue{
        m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, 1.0f
    };
    engine().mainRenderPassBegin({&clearVal, 1});

    // Texture
    if (m_texture) {
        drawTexture(cmdBuf);
    }

    // Menu
    if (Begin("RTICreator v4.0.0", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (BeginTabBar("##TabBar")) {
            if (BeginTabItem("File")) {
                if (Button("Load a texture"))
                    selectAndLoad();
                if (!m_textureLoc.empty() && Button("Save the texture"))
                    save(m_textureLoc);
                EndTabItem();
            }
            if (m_texture) {
                if (BeginTabItem("Parameters")) {
                    parametersGUI();
                    EndTabItem();
                }
                if (BeginTabItem("View")) {
                    SliderFloat2("Overlap (to see wrapping)", &m_overlap.x, 0.0f, 1.0f);
                    if (Button("Reset view"))
                        resetView();
                    ColorPicker3("Background color", &m_backgroundColor.x);
                    EndTabItem();
                }
            }
            EndTabBar();
        }
    }
    End();

    engine().mainRenderPassDrawImGui();
    engine().mainRenderPassEnd();
}

void MainMenuRoom::windowResizedCallback(glm::ivec2 oldSize, glm::ivec2 newSize) {
    m_texView.resizeView(newSize);
}

void MainMenuRoom::parametersGUI() {
    if (InputInt("Number of subimages", &m_subimagesSprites.x)) {
        m_texture->setSubimagesSpritesCount(m_subimagesSprites);
    }
    if (InputInt("Number of sprites", &m_subimagesSprites.y)) {
        m_texture->setSubimagesSpritesCount(m_subimagesSprites);
    }
    if (InputFloat2("Subimage dims", &m_subimageDims.x)) {
        m_texture->setSubimageDims(m_subimageDims);
    }
    SameLine();
    if (Button("Match")) {
        m_subimageDims = static_cast<glm::vec2>(m_texture->trueDims()) /
                         m_texture->subimagesSpritesCount();
        m_texture->setSubimageDims(m_subimageDims);
    }
    if (InputFloat2("Pivot offset", &m_pivot.x)) {
        m_texture->setPivot(m_pivot);
    }
    SameLine();
    if (Button("Center")) {
        m_pivot = m_subimageDims * 0.5f;
        m_texture->setPivot(m_pivot);
    }
}

void MainMenuRoom::selectAndLoad() {
    char filename[MAX_PATH] = {};
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "PNG Files (*.png)\0*.png\0Any Files (*.*)\0*.*\0";
    ofn.lpstrFile   = filename;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrTitle  = "Select a texture for which you want to set parameters";
    ofn.Flags       = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_EXPLORER;
    ofn.lpstrInitialDir = m_lastVisitedLoc.c_str();

    engine().pauseSteps();
    GetOpenFileNameA(&ofn);
    engine().resumeSteps();
    m_lastVisitedLoc = filename;

    load(filename);
}

void MainMenuRoom::save(const std::string& loc) {
    if (loc.empty()) {
        return;
    }
    re::PNGLoader::replaceParameters(
        loc,
        re::TextureShape{
            .subimageDims          = m_subimageDims,
            .pivot                 = m_pivot,
            .subimagesSpritesCount = m_subimagesSprites
        }
    );
}

void MainMenuRoom::load(const std::string& loc) {
    if (loc.empty()) {
        return;
    }
    try {
        m_texture = re::TextureShaped{loc};
    } catch (...) { return; }

    // Save texture's location
    m_textureLoc = loc;

    // Init GUI
    m_subimagesSprites = m_texture->subimagesSpritesCount();
    m_subimageDims     = m_texture->subimageDims();
    m_pivot            = m_texture->pivot();
}

void MainMenuRoom::drawTexture(const re::CommandBuffer& cmdBuf) {
    glm::vec2 windowDims = glm::vec2(engine().windowDims());
    auto texDims = m_texture->subimagesSpritesCount() * m_texture->subimageDims();
    auto botLeft = -texDims * 0.5f;

    glm::vec4 posSizeRect = {
        -texDims * (m_overlap + 0.5f), texDims * (1.0f + 2.0f * m_overlap)
    };
    glm::vec4 uvRect = {-m_overlap, 1.0f + m_overlap * 2.0f};

    m_sb.clearAndBeginFirstBatch();
    m_sb.add(*m_texture, posSizeRect, uvRect);
    m_sb.drawBatch(cmdBuf, m_texView.viewMatrix());

    m_gb.begin();
    std::vector<re::VertexPoCo> vertices;
    glm::vec2 subimageSprite = m_texture->subimagesSpritesCount();
    vertices.reserve((size_t)(subimageSprite.x * subimageSprite.y) * 4u);
    re::Color color{0, 255u, 0u, 255u};
    auto subimageDims = m_texture->subimageDims();
    // Subimages
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
        m_gb.addVertices(vertices);
        vertices.clear();
    }
    // Pivots
    color                 = {0u, 0u, 255u, 255u};
    glm::vec2 pivotOffset = m_texture->pivot();
    float pivotRadius     = glm::min(subimageDims.x, subimageDims.y) * 0.45f;
    for (float x = 0.0f; x < m_texture->subimagesSpritesCount().x; ++x) {
        for (float y = 0.0f; y < m_texture->subimagesSpritesCount().y; ++y) {
            glm::vec2 pivotPos = botLeft + glm::vec2(x, y) * subimageDims +
                                 pivotOffset;
            vertices.emplace_back(pivotPos + glm::vec2(pivotRadius, pivotRadius), color);
            vertices.emplace_back(
                pivotPos + glm::vec2(-pivotRadius, -pivotRadius), color
            );
            vertices.emplace_back(pivotPos + glm::vec2(pivotRadius, -pivotRadius), color);
            vertices.emplace_back(pivotPos + glm::vec2(-pivotRadius, pivotRadius), color);
        }
    }

    m_gb.addVertices(vertices);
    vertices.clear();
    // Whole image
    color = {255u, 0u, 0u, 255u};
    vertices.emplace_back(botLeft, color);
    vertices.emplace_back(botLeft + glm::vec2(texDims.x, 0.0f), color);
    vertices.emplace_back(botLeft + glm::vec2(texDims.x, 0.0f), color);
    vertices.emplace_back(botLeft + glm::vec2(texDims.x, texDims.y), color);
    vertices.emplace_back(botLeft + glm::vec2(texDims.x, texDims.y), color);
    vertices.emplace_back(botLeft + glm::vec2(0.0f, texDims.y), color);
    vertices.emplace_back(botLeft + glm::vec2(0.0f, texDims.y), color);
    vertices.emplace_back(botLeft, color);
    m_gb.addVertices(vertices);

    m_gb.end();
    m_gb.draw(cmdBuf, m_texView.viewMatrix());
}

void MainMenuRoom::resetView() {
    m_texView.setScale({1.0f, 1.0f});
    m_texView.setPosition(glm::vec2{0.0f, 0.0f});
    m_drawScale = 1.0f;
}
