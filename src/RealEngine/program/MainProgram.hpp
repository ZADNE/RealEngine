/**
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <concepts>

#include <SDL_main.h>
#include <glm/vec2.hpp>

#include <RealEngine/program/Synchronizer.hpp>
#include <RealEngine/resources/hot_reload/HotReloadInitInfo.hpp>
#include <RealEngine/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/rooms/RoomManager.hpp>
#include <RealEngine/rooms/RoomToEngineAccess.hpp>
#include <RealEngine/rooms/RoomTransitionArguments.hpp>
#include <RealEngine/user_input/InputManager.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/Window.hpp>

union SDL_Event;

namespace re {

class Room;

struct DisplayInfo {
    std::string name;          ///< UTF-8 encoded 'name'
    glm::ivec4 bounds{};       ///< Bounds in pixels; XYWH
    glm::ivec4 boundsUsable{}; ///< Bounds with system-reserved parts removed; XYWH
    glm::ivec2 dims{};         ///< Dimensions
    int refreshRate{};         ///< Refresh rate
    uint32_t pixelFormat{};    ///< Pixel format @see SDL2's SDL_PixelFormatEnum
    void* driverSpecific{};
};

struct MainProgramInitInfo {
    VulkanInitInfo vulkan{};
    HotReloadInitInfo hotReload{};
};

/**
 * @brief This is the centre point of the whole application.
 *
 * This singleton class is used (via static functions) to initialize RealEngine,
 * add rooms to it and then run the main simulation.
 *
 * First you call MainProgram::initialize to start the engine.
 * Then you can add your rooms via MainProgram::addRoom. Rooms contain the main
 * bussiness logic. Once you have added all the room, you call MainProgram::run
 * to start the simulation. The return value of the run function should be
 * returned back to OS.
 *
 * @see Room
 */
class MainProgram final {
public:
    MainProgram(const MainProgram&)            = delete; ///< Noncopyable
    MainProgram& operator=(const MainProgram&) = delete; ///< Noncopyable

    MainProgram(MainProgram&&)            = delete;      ///< Nonmovable
    MainProgram& operator=(MainProgram&&) = delete;      ///< Nonmovable

    ~MainProgram() = default;

    /**
     * @brief Must be called before any Room is added
     * @warning Never call this more than once!
     */
    static void initialize(const MainProgramInitInfo& initInfo);

    /**
     * @brief Adds a Room to the management
     * @tparam RoomType A class derived from Room that will be instantiated.
     * @return Raw pointer to the created room
     *
     * Single type of room can be added multiple times, the only requirement is
     * that each room must have unique name.
     */
    template<DerivedFromRoom RoomType, typename... ConstructorArgs>
    static RoomType* addRoom(ConstructorArgs&&... args) {
        return instance({}).m_roomManager.addRoom<RoomType>(
            std::forward<ConstructorArgs>(args)...
        );
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
     * @param name Name of the next room, no transition will happen if there is
     * no room with such name.
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
     * @brief Searches displays that can be drawn to
     */
    std::vector<DisplayInfo> searchDisplays() const;

    void setRelativeCursorMode(bool relative);

    void adoptRoomDisplaySettings(const RoomDisplaySettings& rds);

private:
    /**
     * @brief Constructs the main program.
     */
    explicit MainProgram(const MainProgramInitInfo& initInfo);

    /**
     * @brief Gets the singleton instance
     */
    static MainProgram& instance(const MainProgramInitInfo& initInfo);

    /**
     * @brief Does the actual game loop on the singleton instance
     */
    int doRun(size_t roomName, const RoomTransitionArguments& args);

    void step();
    void render(const CommandBuffer& cb, double interpolationFactor);

    void pollEvents();
    void processEvent(SDL_Event* evnt);

    void performHotReload();

    Window m_window; ///< Window also creates and initializes renderer backends
    RoomManager m_roomManager;
    InputManager m_inputManager;
    Synchronizer m_synchronizer{k_defaultStepsPerSecond, k_defaultFramesPerSecondLimit};
    RoomToEngineAccess s_roomToEngineAccess;

    bool m_programShouldRun = false;
    int m_programExitCode   = EXIT_SUCCESS;
    int m_stepN             = 0;

    bool m_pollEventsInMainThread = true;

    void doRoomTransitionIfScheduled();

    static constexpr size_t k_noNextRoom = std::numeric_limits<size_t>::max();
    size_t m_nextRoomName                = k_noNextRoom;
    RoomTransitionArguments m_roomTransitionArgs;
};

} // namespace re
