/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>


namespace RE {

/**
 * @brief Calculates floor(log2(arg)) in unsigned integer domain.
*/
template<std::unsigned_integral T>
constexpr T uintlog2(T arg) {
    if (arg > 0) {
        T log = 0;
        while (arg >>= 1) ++log;
        return log;
    } else {
        return 0;
    }
}

}