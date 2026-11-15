/**
 *  @author    Dubsky Tomas
 */
export module realengine:utility.Version;
import std;

/** @mainpage RealEngine
 *
 * @section overview Overview
 *
 * To get a minimal working RealEngine application, you need to create your own
 * room class that inherits from Room. The typical main function then looks like
 * this: @code {cpp} int main(int argc, char* argv[]) {
 *      MainProgram::initialize();
 *      auto* myRoom = MainProgram::addRoom<MyDerivedRoom>(constructorArgs);
 *      return MainProgram::run(myRoom->name(), transitionparameters);
 *  }
 * @endcode
 */

/**
 * @namespace re
 * @brief Everything provided by RealEngine is inside this namespace
 */

/**
 * @namespace re::glsl
 * @brief Contains exposed GLSL shaders, functions and types
 */

/**
 * @namespace re::details
 * @brief Not to be used directly, used for internal implementation
 */

export namespace re {

constexpr int k_versionMajor = 3;
constexpr int k_versionMinor = 0;
constexpr int k_versionPatch = 0;

/**
 * @brief Is integer representation of RealEngine's version
 */
constexpr int k_version = k_versionMajor * 1'000'000 + k_versionMinor * 1'000 +
                          k_versionPatch;

} // namespace re
