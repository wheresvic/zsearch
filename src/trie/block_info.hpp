#pragma once 
#include <tr1/cstdint>

//#include <boost/integer/static_log2.hpp>
template <std::size_t n>
struct choose_initial_n {
	static const bool c = (std::size_t(1) << n << n) != 0;
	static const std::size_t value = !c*n + choose_initial_n<2*c*n>::value;

};
template <>
struct choose_initial_n<0> {
		static const std::size_t value = 0;
};
const std::size_t initial_n = choose_initial_n<16>::value;

template <std::size_t x,std::size_t n = initial_n>
struct static_log2 {
	static const bool c = (x >> n) > 0;
	static const std::size_t value =  c*n + (static_log2< (x>>c*n), n/2 >::value);
};
template <>
struct static_log2<1, 0> { 
	static const std::size_t value = 0;
};

namespace cindex
{
	template<typename BlockType>
	class block_info
	{
	public:
		typedef BlockType block_type;

		static const std::size_t bytes_per_block = sizeof(block_type);
		static const std::size_t bits_per_block = bytes_per_block * 8;
		static const std::size_t bit_mask = bits_per_block - 1;
		static const std::size_t log_bits_per_block = static_log2<bits_per_block>::value;

		static std::size_t
		block_count(std::size_t n) 
		{
			return (n + bits_per_block - 1) / bits_per_block;
		}

		static std::size_t
		size(std::size_t n) 
		{
			return block_count(n) * bytes_per_block;
		}
	};
}

