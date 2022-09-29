﻿/*!
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <concepts>

#include <SDL2/SDL_main.h>

#include <glm/vec2.hpp>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/rooms/RoomToEngineAccess.hpp>
#include <RealEngine/rooms/RoomManager.hpp>
#include <RealEngine/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/rooms/RoomTransitionArguments.hpp>
#include <RealEngine/window/Window.hpp>
#include <RealEngine/program/Synchronizer.hpp>
#include <RealEngine/user_input/InputManager.hpp>

union SDL_Event;

namespace RE {

class Room;

struct DisplayInfo {
    std::string name; /**< @brief UTF-8 encoded 'name' */
    glm::ivec4 bounds; /**< @brief Bounds in pixels; XYWH */
    glm::ivec4 boundsUsable; /**< @brief Usable bounds in pixels; XYWH; system-reserved parts removed */
    glm::ivec2 dims; /**< @brief Dimensions */
    int refreshRate; /**< @brief Refresh rate */
    Uint32 pixelFormat; /**< @brief Pixel format @see SDL2's SDL_PixelFormatEnum */
    void* driverSpecific;
};

/**
 * @brief This is the centre point of the whole application.
 *
 * This singleton class is used (via static functions) to initialize RealEngine,
 * add rooms to it and then run the main simulation.
 *
 * First you call MainProgram::initialize to start the engine.
 * Then you can add your rooms via MainProgram::addRoom. Rooms contain the main bussiness logic.
 * Once you have added all the room, you call MainProgram::run to start the simulation.
 * The return value of the run function should be return back to OS.
 *
 * @see Room
*/
class MainProgram final {
public:

    MainProgram(const MainProgram&) = delete;
    void operator=(const MainProgram&) = delete;

    /**
     * @brief Must be called before any Room is added
     * @warning Never call this more than once!
    */
    static void initialize();

    /**
     * @brief Adds a late-bind Room to the management
     * @tparam RoomType A class derived from Room that will be instantiated.
     * @return Raw pointer to the created room
     *
     * Your room will use the default late-bind renderer which polymorphically
     * calls the real renderer.
     *
     * Single type of room can be added multiple times, the only requirement is that
     * each room must have unique name.
    */
    template<DerivedFromRoom RoomType, typename... ConstructorArgs>
    static RoomType* addRoom(ConstructorArgs&&... args) {
        return instance().m_roomManager.addRoom<RoomType>(std::forward<ConstructorArgs>(args)...);
    }

    /**
     * @brief Adds an early-bind Room to the management
     * @tparam RoomType A class template derived from Room that will be instantiated.
     * @return Raw pointer to the created room
     *
     * Your room template will be instantiated with the highest probability
     * available renderer.
     *
     * Single type of room can be added multiple times, the only requirement is that
     * each room must have unique name.
    */
    template<template<Renderer> class RoomTemplate, typename... ConstructorArgs> requires DerivedFromRoom<RoomTemplate<RendererGL46>>
    static Room* addRoom(ConstructorArgs&&... args) {
        auto& inst = instance();
        switch (inst.m_window.getRenderer()) {
        case RendererID::VULKAN13:
            return inst.m_roomManager.addRoom<RoomTemplate<RendererVK13>>(std::forward<ConstructorArgs>(args)...);
        case RendererID::OPENGL46:
            return inst.m_roomManager.addRoom<RoomTemplate<RendererGL46>>(std::forward<ConstructorArgs>(args)...);
        default:
            return nullptr;
        }
    }

    /**
     * @brief Runs the program
     *
     * The return value represent the exit code that this
     * RealEngine program should return to the enviroment.
     * The exit code can be altered by scheduleExit().
     *
     * @warning Never call this more than once!
     *
     * @param roomName Name of the first room that will be entered
     * @param args Arguments that the room will be entered with
     * @return The exit code that should be returned to the enviroment.
    */
    static int run(size_t roomName, const RoomTransitionArguments& args);

    /**
     * @brief Schedules the program to exit.
     *
     * The program will end at the end of the current frame.
     *
     * @param exitcode The exit code that should be returned from run()
    */
    void scheduleExit(int exitcode = EXIT_SUCCESS);

    /**
     * @brief Schedules transition to another ('next') room.
     * @param name Name of the next room, no transition will happen if there is no room with such name.
     * @param args Arguments to start the next room's session with.
     *
     * The transition, which happens at the end of current frame,
     * does the following:
     * - ends session of current room via sessionEnd()
     * - start session of next room via sessionStart(params)
     * - ensures that at least one step() happens in the next room before render()
    */
    void scheduleRoomTransition(size_t name, const RoomTransitionArguments& args);

    static void pollEventsInMainThread(bool poll);

    /**
     * @brief Gets displays that can be drawn to
    */
    std::vector<DisplayInfo> getDisplays() const;

    void setRelativeCursorMode(bool relative);

    void adoptRoomDisplaySettings(const RoomDisplaySettings& rds);

private:

    /**
     * @brief Constructs the main program.
    */
    MainProgram();

    /**
     * @brief Gets the singleton instance
    */
    static MainProgram& instance();

    /**
     * @brief Does the actual game loop on the singleton instance
    */
    template<Renderer R>
    int doRun(size_t roomName, const RoomTransitionArguments& args);

    void step();
    void render(double interpolationFactor);

    void pollEvents();
    void processEvent(SDL_Event* evnt);

    Window m_window{WindowSettings{}, WindowSubsystems::getVersion()};/**< Window also creates and initializes renderer backends */
    RoomManager m_roomManager;                  /**< Manages rooms - you have to add at least 1 room to run the program */
    InputManager m_inputManager;                /**< Records key presses/releases, mouse movement etc. */
    Synchronizer m_synchronizer{50u, 50u};      /**< Maintains constant speed of simulation, can also limit FPS */
    RoomToEngineAccess s_roomToEngineAccess;    /**< Is a proxy to access RealEngine from within Rooms, see Room::engine */

    bool m_programShouldRun = false;
    int m_programExitCode = EXIT_SUCCESS;

    bool m_pollEventsInMainThread = true;
    glm::vec4 m_clearColor{};

    void doRoomTransitionIfScheduled();

    static constexpr size_t NO_NEXT_ROOM = std::numeric_limits<size_t>::max();
    size_t m_nextRoomName = NO_NEXT_ROOM;
    RoomTransitionArguments m_roomTransitionArgs;
};

}