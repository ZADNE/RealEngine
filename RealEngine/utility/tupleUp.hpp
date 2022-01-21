#pragma once
#include <tuple>
#include <utility>

//NOT MAH CODE!


//For getting index of a type in a tuple
namespace RE {
	template <class T, class Tuple>
	struct Index;

	template <class T, class... Types>
	struct Index<T, std::tuple<T, Types...>> {
		static const std::size_t value = 0;
	};

	template <class T, class U, class... Types>
	struct Index<T, std::tuple<U, Types...>> {
		static const std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
	};
}


//For each in a tuple
namespace TU {
	template<int... Is>
	struct seq { };

	template<int N, int... Is>
	struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

	template<int... Is>
	struct gen_seq<0, Is...> : seq<Is...> { };

	template<typename T, typename F, int... Is>
	void for_each(T&& t, F f, seq<Is...>) {
		auto l = { (f(std::get<Is>(t)), 0)... };
	}
}
namespace RE{
	template<typename... Ts, typename F>
	void for_each_in_tuple(std::tuple<Ts...> & t, F f){
		TU::for_each(t, f, TU::gen_seq<sizeof...(Ts)>());
	}
}