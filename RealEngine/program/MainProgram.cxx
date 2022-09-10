/*!
 *  @author    Dubsky Tomas
 */
module;
#include <SDL2/SDL_events.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

module RealEngine.program.MainProgram;
import std;
import RealEngine.main.rooms.Room;
import RealEngine.rendering.output.Viewport;
import RealEngine.rendering.output.Framebuffer;


namespace RE {

void MainProgram::initialize() {
    //Force initialization of the singleton instance
    instance();
}

int MainProgram::run(size_t roomName, const RoomTransitionParameters& params) {
    try {
        auto& inst = instance();
        switch (inst.m_window.getRenderer()) {
        case RendererID::OPENGL_46:
            return inst.doRun<RendererGL46>(roomName, params);
        default:
            return 1;
        }
    }
    catch (const std::exception& e) {
        fatalError(std::string("Exception: ") + e.what());
    }
    catch (const char* str) {
        fatalError(std::string("C-string exception: ") + str);
    }
    catch (int i) {
        fatalError(std::string("int exception: ") + std::to_string(i));
    }
    catch (...) {
        fatalError("Unknown exception!");
    }
}

void MainProgram::scheduleExit(int exitcode/* = EXIT_SUCCESS*/) {
    m_programShouldRun = false;
    m_programExitCode = exitcode;
}

void MainProgram::pollEventsInMainThread(bool poll) {
    auto& mainProgram = instance();
    mainProgram.m_pollEventsInMainThread = poll;
    mainProgram.m_inputManager.update();
}

std::vector<RE::DisplayInfo> MainProgram::getDisplays() const {
    std::vector<RE::DisplayInfo> infos;
    int numberOfDisplays = SDL_GetNumVideoDisplays();
    if (numberOfDisplays < 0) { return infos; }
    infos.reserve(numberOfDisplays);
    for (int i = 0; i < numberOfDisplays; ++i) {
        DisplayInfo info;
        info.name = SDL_GetDisplayName(i);
        SDL_Rect rect;
        if (SDL_GetDisplayBounds(i, &rect)) { continue; }
        info.bounds.x = rect.x;
        info.bounds.y = rect.y;
        info.bounds.z = rect.w;
        info.bounds.w = rect.h;
        if (SDL_GetDisplayUsableBounds(i, &rect)) { continue; }
        info.boundsUsable.x = rect.x;
        info.boundsUsable.y = rect.y;
        info.boundsUsable.z = rect.w;
        info.boundsUsable.w = rect.h;
        SDL_DisplayMode mode;
        if (SDL_GetCurrentDisplayMode(i, &mode)) { continue; }
        info.dims.x = mode.w;
        info.dims.y = mode.h;
        info.refreshRate = mode.refresh_rate;
        info.driverSpecific = mode.driverdata;
        info.pixelFormat = mode.format;
        infos.push_back(info);
    }
    return infos;
}

void MainProgram::setRelativeCursorMode(bool relative) {
    SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
}

void MainProgram::adoptRoomDisplaySettings(const RoomDisplaySettings& s) {
    m_clearColor = s.clearColor;
    m_synchronizer.setStepsPerSecond(s.stepsPerSecond);
    m_synchronizer.setFramesPerSecondLimit(s.framesPerSecondLimit);
    m_usingImGui = s.usingImGui;
}

template<Renderer R>
int MainProgram::doRun(size_t roomName, const RoomTransitionParameters& params) {
    scheduleRoomTransition(roomName, params);
    doRoomTransitionIfScheduled();
    if (!m_roomManager.getCurrentRoom()) {
        throw std::runtime_error("Initial room was not set");
    }

    //Adopt display settings of the first room
    adoptRoomDisplaySettings(m_roomManager.getCurrentRoom()->getDisplaySettings());

    m_programShouldRun = true;
    m_synchronizer.resumeSteps();

    //MAIN PROGRAM LOOP
    std::cout << "Entering main loop!" << std::endl;
    while (m_programShouldRun) {
        m_synchronizer.beginFrame();
        DefaultFrameBuffer<R>::clearColor(m_clearColor);

        //Perform simulation steps to catch up the time
        while (m_synchronizer.shouldStepHappen()) {
            //Check for user input
            if (m_pollEventsInMainThread) {
                m_inputManager.update();
                pollEvents();
            } else {
                SDL_PumpEvents();
            }
            //Do the simulation step
            step();
        }

        //Draw frame
        render(m_synchronizer.getDrawInterpolationFactor());

        m_window.swapBuffer();

        doRoomTransitionIfScheduled();

        m_synchronizer.endFrame();
    }
    std::cout << "Leaving main loop!" << std::endl;

    //Exit the program
    m_roomManager.getCurrentRoom()->sessionEnd();

    return m_programExitCode;
}

void MainProgram::step() {
    m_roomManager.getCurrentRoom()->step();
}

void MainProgram::render(double interpolationFactor) {
    if (m_usingImGui) {//ImGui frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    m_roomManager.getCurrentRoom()->render(interpolationFactor);

    if (m_usingImGui) {//ImGui frame end
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

void MainProgram::processEvent(SDL_Event* evnt) {
    RE::Key key = RE::Key::UNKNOWN;
    switch (evnt->type) {
    case SDL_KEYDOWN:
        key = SDLKToREKey(evnt->key.keysym.sym);
        if (evnt->key.repeat == 0) {
            m_inputManager.press(RE::Key::ANY_KEY);
            m_inputManager.press(key);
        }
        break;
    case SDL_KEYUP:
        if (evnt->key.repeat == 0) {
            m_inputManager.press(RE::Key::ANY_KEY, -1);
            m_inputManager.release(SDLKToREKey(evnt->key.keysym.sym));
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (evnt->key.repeat == 0) {
            auto key = SDLKToREKey(evnt->button.button);
            m_inputManager.press(RE::Key::ANY_KEY);
            m_inputManager.press(key, evnt->button.clicks);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (evnt->key.repeat == 0) {
            m_inputManager.press(RE::Key::ANY_KEY, -1);
            m_inputManager.release(SDLKToREKey(evnt->button.button));
        }
        break;
    case SDL_MOUSEMOTION:
        //Y coords are inverted to get standard math coordinates
        //Coords also have to be clamped to window dims
        //because SDL reports coords outside of the window when a key is held
        m_inputManager.setCursor(
            glm::clamp({evnt->motion.x, m_window.getDims().y - evnt->motion.y - 1}, glm::ivec2(0), m_window.getDims() - 1),
            {evnt->motion.xrel, -evnt->motion.yrel}
        );
        break;
    case SDL_MOUSEWHEEL:
        key = (evnt->wheel.y > 0) ? (Key::UMW) : (Key::DMW);
        m_inputManager.press(key, std::abs(evnt->wheel.y));

        key = (evnt->wheel.x > 0) ? (Key::RMW) : (Key::LMW);
        m_inputManager.press(key, std::abs(evnt->wheel.x));
        break;
    case SDL_QUIT:
        scheduleExit();
        break;
    }
}

void MainProgram::doRoomTransitionIfScheduled() {
    if (m_nextRoomName == NO_NEXT_ROOM) return;

    m_synchronizer.pauseSteps();
    auto prev = m_roomManager.getCurrentRoom();
    auto current = m_roomManager.goToRoom(m_nextRoomName, m_roomTransitionParameters);
    if (prev != current) {//If successfully changed the room
        //Adopt the display settings of the entered room
        adoptRoomDisplaySettings(current->getDisplaySettings());
        //Pressed/released events belong to the previous room
        m_inputManager.update();
        //Ensure at least one step before the first frame is rendered
        step();
    }
    m_nextRoomName = NO_NEXT_ROOM;

    m_synchronizer.resumeSteps();
}

void MainProgram::scheduleRoomTransition(size_t name, const RoomTransitionParameters& params) {
    m_nextRoomName = name;
    m_roomTransitionParameters = params;
}

void MainProgram::pollEvents() {
    SDL_Event evnt;
    if (m_usingImGui) {
        while (SDL_PollEvent(&evnt)) {
            ImGui_ImplSDL2_ProcessEvent(&evnt);
            processEvent(&evnt);
        }
    } else {
        while (SDL_PollEvent(&evnt)) {
            processEvent(&evnt);
        }
    }
}

MainProgram::MainProgram() :
    s_roomToEngineAccess(*this, m_inputManager, m_synchronizer, m_window, m_roomManager) {

    Room::setRoomSystemAccess(&s_roomToEngineAccess);
    Room::setStaticReferences(this, &m_roomManager);
}

MainProgram& MainProgram::instance() {
    static MainProgram mainProgram;
    return mainProgram;
}

template int MainProgram::doRun<RendererGL46>(size_t roomName, const RoomTransitionParameters& params);

}