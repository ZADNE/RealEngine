/**
 *  @author    Dubsky Tomas
 */
export module realengine:window.WindowSubsystems;
import std;
import :utility.Version;

export namespace re {

/**
 * @brief Represents RealEngine's subsystems
 * @note This is used internally within Window.
 */
class WindowSubsystems {
public:
    /**
     * @brief Initializes RealEngine's subsystems
     * @throws std::runtime_error When a system failed to initialize.
     */
    WindowSubsystems();

    WindowSubsystems(const WindowSubsystems&) = delete; ///< Noncopyable
    WindowSubsystems& operator=(const WindowSubsystems&) = delete; ///< Noncopyable

    WindowSubsystems(WindowSubsystems&&)            = delete; ///< Nonmovable
    WindowSubsystems& operator=(WindowSubsystems&&) = delete; ///< Nonmovable

    ~WindowSubsystems();

    /**
     * @brief Gets human readable string with the version of RealEngine
     * @return Human readable version string
     */
    static const std::string& RealEngineVersionString() {
        using namespace std::string_literals;
        const static std::string str =
            "RealEngine:   "s + std::to_string(k_versionMajor) + "."s +
            std::to_string(k_versionMinor) + "."s + std::to_string(k_versionPatch);
        return str;
    }

    static void printRealEngineVersion();

    void printSubsystemsVersions() const;
};

} // namespace re
