/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/Ordering.hpp>

namespace RE {

IOrdering* Ordering::s_impl = nullptr;

void Ordering::flushWork() {
	s_impl->flushWork();
}

void Ordering::finishWork() {
	s_impl->finishWork();
}

void Ordering::issueDrawBarrier() {
	s_impl->issueDrawBarrier();
}

void Ordering::issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) {
	s_impl->issueIncoherentAccessBarrier(barriers);
}

}
