/*!
 *  @author    Dubsky Tomas
 */
#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL2/SDL_events.h>
#include <glm/common.hpp>

#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/rooms/Room.hpp>

namespace re {

void MainProgram::initialize(const VulkanInitInfo& initInfo) {
    // Force initialization of the singleton instance
    instance(initInfo);
}

int MainProgram::run(size_t roomName, const RoomTransitionArguments& args) {
    try {
        return instance({}).doRun(roomName, args);
    } catch (const std::exception& e) {
        fatalError(std::string("Exception: ") + e.what());
    } catch (const char* str) {
        fatalError(std::string("C-string exception: ") + str);
    } catch (int i) {
        fatalError(std::string("int exception: ") + std::to_string(i));
    } catch (...) { fatalError("Unknown exception!"); }
}

void MainProgram::scheduleExit(int exitcode /* = EXIT_SUCCESS*/) {
    m_programShouldRun = false;
    m_programExitCode  = exitcode;
}

void MainProgram::pollEventsInMainThread(bool poll) {
    auto& mainProgram                    = instance(VulkanInitInfo{});
    mainProgram.m_pollEventsInMainThread = poll;
    mainProgram.m_inputManager.step();
}

std::vector<DisplayInfo> MainProgram::searchDisplays() const {
    std::vector<DisplayInfo> infos;
    int                      numberOfDisplays = SDL_GetNumVideoDisplays();
    if (numberOfDisplays < 0) {
        return infos;
    }
    infos.reserve(numberOfDisplays);
    for (int i = 0; i < numberOfDisplays; ++i) {
        DisplayInfo info;
        info.name = SDL_GetDisplayName(i);
        SDL_Rect rect;
        if (SDL_GetDisplayBounds(i, &rect)) {
            continue;
        }
        info.bounds.x = rect.x;
        info.bounds.y = rect.y;
        info.bounds.z = rect.w;
        info.bounds.w = rect.h;
        if (SDL_GetDisplayUsableBounds(i, &rect)) {
            continue;
        }
        info.boundsUsable.x = rect.x;
        info.boundsUsable.y = rect.y;
        info.boundsUsable.z = rect.w;
        info.boundsUsable.w = rect.h;
        SDL_DisplayMode mode;
        if (SDL_GetCurrentDisplayMode(i, &mode)) {
            continue;
        }
        info.dims.x         = mode.w;
        info.dims.y         = mode.h;
        info.refreshRate    = mode.refresh_rate;
        info.driverSpecific = mode.driverdata;
        info.pixelFormat    = mode.format;
        infos.push_back(info);
    }
    return infos;
}

void MainProgram::setRelativeCursorMode(bool relative) {
    SDL_SetRelativeMouseMode(relative ? SDL_TRUE : SDL_FALSE);
}

void MainProgram::adoptRoomDisplaySettings(const RoomDisplaySettings& s) {
    m_window.setClearValues(s.clearValues);
    m_synchronizer.setStepsPerSecond(s.stepsPerSecond);
    m_synchronizer.setFramesPerSecondLimit(s.framesPerSecondLimit);
    m_window.useImGui(s.usingImGui);
}

int MainProgram::doRun(size_t roomName, const RoomTransitionArguments& args) {
    scheduleRoomTransition(roomName, args);
    doRoomTransitionIfScheduled();
    if (!m_roomManager.currentRoom()) {
        throw std::runtime_error("Initial room was not set");
    }

    // Adopt display settings of the first room
    adoptRoomDisplaySettings(m_roomManager.currentRoom()->displaySettings());

    m_programShouldRun = true;
    m_synchronizer.resumeSteps();

    // MAIN PROGRAM LOOP
    std::cout << "Entering main loop!" << std::endl;
    while (m_programShouldRun) {
        m_synchronizer.beginFrame();

        // Perform simulation steps to catch up the time
        while (m_synchronizer.shouldStepHappen()) {
            // Check for user input
            if (m_pollEventsInMainThread) {
                m_inputManager.step();
                pollEvents();
            } else {
                SDL_PumpEvents();
            }
            // Do the simulation step
            step();
        }

        // Prepare for drawing
        const auto& cmdBuf = m_window.prepareNewFrame();

        // Draw the frame
        render(cmdBuf, m_synchronizer.drawInterpolationFactor());

        // Finish the drawing
        m_window.finishNewFrame();

        doRoomTransitionIfScheduled();

        m_synchronizer.endFrame();
    }
    std::cout << "Leaving main loop!" << std::endl;

    // Exit the program
    m_roomManager.currentRoom()->sessionEnd();
    m_window.prepareForDestructionOfRendererObjects();

    return m_programExitCode;
}

void MainProgram::step() {
    StepDoubleBufferingState::setTotalIndex(++m_stepN);
    m_roomManager.currentRoom()->step();
}

void MainProgram::render(const CommandBuffer& cmdBuf, double interpolationFactor) {
    m_roomManager.currentRoom()->render(cmdBuf, interpolationFactor);
}

void MainProgram::processEvent(SDL_Event* evnt) {
    Key key = Key::UnknownKey;
    switch (evnt->type) {
    case SDL_KEYDOWN:
        key = toKey(evnt->key.keysym.sym);
        if (evnt->key.repeat == 0) {
            m_inputManager.press(key);
        }
        break;
    case SDL_KEYUP:
        if (evnt->key.repeat == 0) {
            m_inputManager.release(toKey(evnt->key.keysym.sym));
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (evnt->key.repeat == 0) {
            auto key = toKey(evnt->button.button);
            m_inputManager.press(key, evnt->button.clicks);
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (evnt->key.repeat == 0) {
            m_inputManager.release(toKey(evnt->button.button));
        }
        break;
    case SDL_MOUSEMOTION:
        // Y coords are inverted to get standard math coordinates
        // Coords also have to be clamped to window dims
        // because SDL reports coords outside of the window when a key is held
        m_inputManager.setCursor(
            glm::clamp(
                {evnt->motion.x, m_window.dims().y - evnt->motion.y - 1},
                glm::ivec2(0),
                m_window.dims() - 1
            ),
            {evnt->motion.xrel, -evnt->motion.yrel}
        );
        break;
    case SDL_MOUSEWHEEL:
        key = (evnt->wheel.y > 0) ? (Key::UMW) : (Key::DMW);
        m_inputManager.press(key, std::abs(evnt->wheel.y));

        key = (evnt->wheel.x > 0) ? (Key::RMW) : (Key::LMW);
        m_inputManager.press(key, std::abs(evnt->wheel.x));
        break;
    case SDL_QUIT: scheduleExit(); break;
    }
}

void MainProgram::doRoomTransitionIfScheduled() {
    if (m_nextRoomName == k_noNextRoom)
        return;

    m_synchronizer.pauseSteps();
    auto prev    = m_roomManager.currentRoom();
    auto current = m_roomManager.goToRoom(m_nextRoomName, m_roomTransitionArgs);
    if (prev != current) { // If successfully changed the room
        // Adopt the display settings of the entered room
        adoptRoomDisplaySettings(current->displaySettings());
        // Pressed/released events belong to the previous room
        m_inputManager.step();
        // Ensure at least one step before the first frame is rendered
        step();
    }
    m_nextRoomName = k_noNextRoom;

    m_synchronizer.resumeSteps();
}

void MainProgram::scheduleRoomTransition(
    size_t name, const RoomTransitionArguments& args
) {
    m_nextRoomName       = name;
    m_roomTransitionArgs = args;
}

void MainProgram::pollEvents() {
    SDL_Event evnt;
    if (m_window.isImGuiUsed()) {
        while (SDL_PollEvent(&evnt)) {
            if (!m_window.passSDLEvent(evnt)) {
                // Pass the event to main application if it has not been consumed by ImGui
                processEvent(&evnt);
            }
        }
    } else {
        while (SDL_PollEvent(&evnt)) { processEvent(&evnt); }
    }
}

MainProgram::MainProgram(const VulkanInitInfo& initInfo)
    : m_window{WindowSettings{}, WindowSubsystems::RealEngineVersionString(), initInfo}
    , s_roomToEngineAccess(*this, m_inputManager, m_synchronizer, m_window, m_roomManager) {

    Room::setRoomSystemAccess(&s_roomToEngineAccess);
    Room::setStaticReferences(this, &m_roomManager);
}

MainProgram& MainProgram::instance(const VulkanInitInfo& initInfo) {
    static MainProgram mainProgram{initInfo};
    return mainProgram;
}

} // namespace re
