/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IOrdering.hpp>

namespace RE {

class Ordering {
	friend class GL46_Renderer;
public:

	static void flushWork();
	static void finishWork();

	static void issueDrawBarrier();
	static void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers);

protected:
	static IOrdering* s_impl;
};

}
