/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/rooms/RoomToEngineAccess.hpp>
#include <RealEngine/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/rooms/RoomTransitionArguments.hpp>

#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

/**
 * @brief Separates program into logical units, only one room is active at a time.
 *
 * Rooms allow separation of program into multiple units where only one unit
 * is active at a time (= receives step and render calls, inactive rooms
 * are resident in the memory but do not receive updates).
 * Transitions between rooms are mamaged by RoomManager.
 *
 * You use RealEngine by inheriting Room and adding your behavior to step(), render() etc.
 *
 * You can 'inherit' this in two ways: the easier way is simply creating a class that inherits from this.
 * This creates a late-bind room which means RE rendering-related objects in it
 * will polymorphically call the backing renderer.
 * The other way to 'inherit' this is to create template class (with one template parameter) that inherits from this.
 * This creates an early-bind room which means that it will be instantiated with highest priority available renderer.
 * The type of the renderer will be the template parameter but to actually use the early-bind,
 * the type has to passed (using templates) to the very RE rendering-objects (such as Buffer or ShaderProgram).
 *
 * Both early- and late-bind room are added to RealEngine (instantiated) by MainProgram::addRoom().
*/
class Room {
public:

    /**
     * @brief Constructs new room
     * @note Create new rooms using MainProgram::addRoom
     * @param name Unique identifier of the room
    */
    Room(size_t name, RoomDisplaySettings initialSettings = RoomDisplaySettings{});

    /**
     * @brief Destructs a room.
     *
     * This happens when the program ends.
    */
    virtual ~Room() = default;

    Room(const Room&) = delete;
    Room& operator=(const Room&) = delete;

    /**
     * @brief Informs the room that a new session happens inside it.
     * @param args      Arguments to initialize the session. These are
     *                  are obtained from previous room when it requests
     *                  transition to this room.
    */
    virtual void sessionStart(const RoomTransitionArguments& args) = 0;

    /**
     * @brief Informs the room that its session ends.
     *
     * This happens after it was requested from the main program.
    */
    virtual void sessionEnd() = 0;

    /**
     * @brief The main update function of the room.
     *
     * This function is called at fixed rate per second.
     * The rate can be changed via Synchronizer.
    */
    virtual void step() = 0;

    /**
     * @brief This is the function where rendering should happen.
     *
     * This function is called at a variable rate, depending on
     * the speed of hardware. Upper limit can be set via Synchronizer.
     *
     * @param interpolationFactor   Represents relative time between last
     *                              performed step and the upcoming step.
     *                              Use it to interpolate between discrete
     *                              simulation steps.
    */
    virtual void render(double interpolationFactor) = 0;

    /**
     * @brief Callback used to notify that the window's size has changed
    */
    virtual void windowResizedCallback(const glm::ivec2& oldSize, const glm::ivec2& newSize) {}

    /**
     * @brief Callback used to notify that the window's title has changed
    */
    virtual void windowTitleChangedCallback(const std::string& oldTitle, const std::string& newTitle) {}

    /**
     * @brief Callback used to notify that the window's flags have changed
    */
    virtual void windowFlagsChangedCallback(const WindowFlags& oldFlags, const WindowFlags& newFlags) {}

    /**
     * @brief Gets a proxy that can be used to read/modify
     * many parameters and variables of the RealEngine
    */
    static RoomToEngineAccess& engine() { return *s_engineAccess; }

    /**
     * @brief Gets the settings that should be used for this room
    */
    RoomDisplaySettings getDisplaySettings() const;

    /**
     * @brief Gets unique identifier of the room
    */
    size_t getName() const;

    /**
     * @brief This is set by the MainProgram at startup
    */
    static void setRoomSystemAccess(RoomToEngineAccess* systemAccess) { s_engineAccess = systemAccess; }

    /**
     * @brief This is set by the MainProgram at startup
    */
    static void setStaticReferences(MainProgram* mainProgram, RoomManager* roomManager);

protected:

    /**
     * @brief Dynamically changes the room's display settings
    */
    void setDisplaySettings(RoomDisplaySettings displaySettings);

private:

    RoomDisplaySettings m_displaySettings;
    size_t m_name;              /**< Unique identifier of the room */

    inline static RoomToEngineAccess* s_engineAccess = nullptr;
    inline static MainProgram* s_mainProgram = nullptr;
    inline static RoomManager* s_roomManager = nullptr;
};

}