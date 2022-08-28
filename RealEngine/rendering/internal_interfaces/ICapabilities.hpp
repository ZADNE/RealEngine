/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Lists all capabilities that can be enabled/disabled
*/
enum class Capability {
    BLENDING
};

/**
* @brief Is a renderer-agnostic interface to Capabilities' implementation.
*
* This is used internally by Capabilities.
*
* @see RE::Capabilities
*/
class ICapabilities {
public:

    virtual void set(Capability cap, bool enabled) const = 0;

};

}