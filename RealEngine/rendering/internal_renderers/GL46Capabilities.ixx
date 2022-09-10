/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_renderers.GL46Capabilities;
import RealEngine.rendering.internal_interfaces.ICapabilities;

export namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of capabilities
*
* Do not use this directly - use *Capability classes instead.
*/
class GL46Capabilities final : public ICapabilities {
public:

    void set(Capability cap, bool enabled) const override;
};

}