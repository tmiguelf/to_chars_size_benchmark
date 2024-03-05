//======== ======== ======== ======== ======== ======== ======== ========
///	\file
///
///	\copyright
///		
//======== ======== ======== ======== ======== ======== ======== ========

#include <benchmark/benchmark.h>

#include <cstdint>
#include <array>
#include <bit>
#include <limits>
#include <numbers>
#include <cstring>

#include <CoreLib/string/core_string_numeric.hpp>
#include <CoreLib/core_extra_compiler.hpp>
#include <CoreLib/core_alloca.hpp>


//======== ======== ======== Decimal Int test Cases ======== ======== ========
static const std::vector<int64_t> s_numbers =
{
	-0,
	-1,
	-2,
	-3,
	-4,
	-5,
	-6,
	-7,
	-8,
	-9,
	-10,
	-11,
	-15,
	-21,
	-34,
	-42,
	-99,
	-100,
	-101,
	-123,
	-127,
	-128,
	-129,
	-255,
	-256,
	-999,
	-1000,
	-1234,
	-9999,
	-10000,
	-12345,
	-32767,
	-32768,
	-51234,
	-65535,
	-65536,
	-65537,
	-99999,
	-100000,
	-999999,
	-1000000,
	-9999999,
	-10000000,
	-99999999,
	-100000000,
	-999999999,
	-1000000000,
	-2147483647,
	-2147483648,
	-2147483649,
	-4294967295,
	-4294967296,
	-4294967297,
	-9999999999,
	-10000000000,
	-99999999999,
	-100000000000,
	-999999999999,
	-1000000000000,
	-9999999999999,
	-10000000000000,
	-99999999999999,
	-100000000000000,
	-999999999999999,
	-1000000000000000,
	-9999999999999999,
	-10000000000000000,
	-99999999999999999,
	-100000000000000000,
	-999999999999999999,
	-1000000000000000000,
	-9223372036854775807,
};

static const std::vector<uint64_t> u_numbers =
{
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	15,
	21,
	34,
	42,
	99,
	100,
	101,
	123,
	127,
	128,
	255,
	256,
	999,
	1000,
	1234,
	9999,
	10000,
	12345,
	32767,
	32768,
	51234,
	65535,
	65536,
	99999,
	100000,
	999999,
	1000000,
	9999999,
	10000000,
	99999999,
	100000000,
	999999999,
	1000000000,
	2147483647,
	2147483648,
	4294967295,
	4294967296,
	9999999999,
	10000000000,
	99999999999,
	100000000000,
	999999999999,
	1000000000000,
	9999999999999,
	10000000000000,
	99999999999999,
	100000000000000,
	999999999999999,
	1000000000000000,
	9999999999999999,
	10000000000000000,
	99999999999999999,
	100000000000000000,
	999999999999999999,
	1000000000000000000,
	9223372036854775807,
	9223372036854775808ull,
	9999999999999999999ull,
	10000000000000000000ull,
	18446744073709551615ull
};

static const std::vector<double> f_numbers =
{
	0.0,
	1.0,
	-1.0,
	-0.0,
	std::numbers::e_v<double>,
	-std::numbers::pi_v<double>,
	12.0,
	123.0,
	1234.0,
	12345.0,
	123456.0,
	1234567.0,
	12345678.0,
	123456789.0,
	1234567890.0,
	12345678901.0,
	std::numeric_limits<double>::epsilon(),
	std::numeric_limits<double>::infinity(),
	-std::numeric_limits<double>::infinity(),
	std::numeric_limits<double>::quiet_NaN(),
	-12.0,
	-123.0,
	-1234.0,
	-12345.0,
	-123456.0,
	-1234567.0,
	-12345678.0,
	-123456789.0,
	-1234567890.0,
	-12345678901.0,
	std::numeric_limits<double>::max(),
	-std::numeric_limits<double>::max(),
	1258.456456834534534e12,
	1258.456456834534534e-12,
	-1258.456456834534534e12,
	-1258.456456834534534e-12,
	static_cast<double>(std::bit_cast<float>(1)),
	std::bit_cast<double>(1ull),
};


constexpr uintptr_t array_size = 128;

template <typename num_T>
using test_array_t = std::array<num_T, array_size>;


template <typename num_T>
test_array_t<num_T> get_num()
{
	constexpr uintptr_t num_cases = array_size;
	using array_t = test_array_t<num_T>;

	array_t out{};
	//out.resize(num_cases);
	uintptr_t pivot = 0;

	if constexpr(std::is_floating_point_v<num_T>)
	{
		for(const double num : f_numbers)
		{
			if(pivot >= num_cases) break;
			out[pivot++] = static_cast<const num_T>(num);
		}
	}
	else
	{
		constexpr num_T max = std::numeric_limits<num_T>::max();

		for(const uint64_t num : u_numbers)
		{
			if(pivot >= num_cases) break;
			if(num <= max)
			{
				out[pivot++] = static_cast<const num_T>(num);
			}
			else break;
		}

		if constexpr(std::is_signed_v<num_T>)
		{
			constexpr num_T min = std::numeric_limits<num_T>::min();
			for(const int64_t num : s_numbers)
			{
				if(pivot >= num_cases) break;
				if(num >= min)
				{
					out[pivot++] = static_cast<const num_T>(num);
				}
				else break;
			}
		}
	}


	uintptr_t rem = 0;
	for(; pivot < num_cases; ++pivot, ++rem)
	{
		out[pivot] = out[rem];
	}

	return out;
}

template<typename num_T>
NO_INLINE void impl_to_chars_allocate(std::span<const num_T> const data)
{
	constexpr uintptr_t required_buffer_size = core::to_chars_dec_max_size_v<num_T>;
	std::array<char8_t, required_buffer_size> overflow_buffer;

	std::vector<char8_t> buffer;
	uintptr_t current_offset = 0;

	for(const num_T num : data)
	{
		char8_t* pivot = core::_p::to_chars_unsafe<char8_t, num_T>(num, overflow_buffer.data());
		const uintptr_t used_size = pivot - overflow_buffer.data();
		buffer.resize(buffer.size() + used_size);

		memcpy(buffer.data() + current_offset, overflow_buffer.data(), used_size);
		current_offset += used_size;
	}

	benchmark::DoNotOptimize(buffer);
}

template<typename num_T>
NO_INLINE void impl_to_chars_over_allocate(std::span<const num_T> const data)
{
	constexpr uintptr_t required_buffer_size = core::to_chars_dec_max_size_v<num_T>;

	uintptr_t const max_buffer_size = required_buffer_size * data.size();

	std::vector<char8_t> buffer;
	buffer.resize(max_buffer_size);

	char8_t* pivot = buffer.data();
	for(const num_T num : data)
	{
		pivot = core::_p::to_chars_unsafe<char8_t, num_T>(num, pivot);
	}

	buffer.resize(pivot - buffer.data());

	benchmark::DoNotOptimize(buffer);
}


template<typename num_T>
NO_INLINE void impl_to_chars_double_iterate(std::span<const num_T> const data)
{
	constexpr uintptr_t required_buffer_size = core::to_chars_dec_max_size_v<num_T>;
	uintptr_t total_size = 0;

	{
		std::array<char8_t, required_buffer_size> overflow_buffer;
		for(const num_T num : data)
		{
			total_size += core::_p::to_chars_unsafe<char8_t, num_T>(num, overflow_buffer.data()) - overflow_buffer.data();
		}
	}

	char8_t* const buffer = reinterpret_cast<char8_t*>(core_alloca(total_size));
	char8_t* pivot = buffer;
	for(const num_T num : data)
	{
		pivot = core::_p::to_chars_unsafe<char8_t, num_T>(num, pivot);
	}
	benchmark::DoNotOptimize(pivot);
	benchmark::DoNotOptimize(total_size);
}

template<typename num_T>
NO_INLINE void impl_to_chars_size(std::span<const num_T> const data)
{
	uintptr_t total_size = 0;

	for(const num_T num : data)
	{
		total_size += core::_p::to_chars_estimate<num_T>(num);
	}

	char8_t* const buffer = reinterpret_cast<char8_t*>(core_alloca(total_size));
	char8_t* pivot = buffer;
	for(const num_T num : data)
	{
		pivot = core::_p::to_chars_unsafe<char8_t, num_T>(num, pivot);
	}

	benchmark::DoNotOptimize(pivot);
	benchmark::DoNotOptimize(total_size);
}

template<typename num_T>
static void impl_to_chars_solo_size(std::span<const num_T> const data)
{
	uintptr_t total_size = 0;
	for(const num_T num : data)
	{
		total_size += core::_p::to_chars_estimate<num_T>(num);
	}
	benchmark::DoNotOptimize(total_size);
}


#define ConstructTemplate(TestName, FuncName) \
template<typename num_T> \
static void TestName(benchmark::State& state) \
{ \
	const test_array_t<num_T> testList = get_num<num_T>(); \
	for (auto _ : state) \
	{ \
		FuncName<num_T>(std::span<const num_T>{testList.data(), testList.size()}); \
	} \
}

ConstructTemplate(to_chars_allocate, impl_to_chars_allocate)
ConstructTemplate(to_chars_over_allocate, impl_to_chars_over_allocate)
ConstructTemplate(to_chars_double_iterate, impl_to_chars_double_iterate)
ConstructTemplate(to_chars_size, impl_to_chars_size)
ConstructTemplate(to_chars_solo_size, impl_to_chars_solo_size)

//rows
#if 0
#define TEST_SET(Type) \
BENCHMARK_TEMPLATE(to_chars_allocate, Type); \
BENCHMARK_TEMPLATE(to_chars_over_allocate, Type); \
BENCHMARK_TEMPLATE(to_chars_double_iterate, Type); \
BENCHMARK_TEMPLATE(to_chars_size, Type); \
BENCHMARK_TEMPLATE(to_chars_solo_size, Type);


TEST_SET(uint8_t );
TEST_SET(uint16_t);
TEST_SET(uint32_t);
TEST_SET(uint64_t);
TEST_SET( int8_t );
TEST_SET( int16_t);
TEST_SET( int32_t);
TEST_SET( int64_t);
TEST_SET( float  );
TEST_SET( double );

//columns
#else
#define MAKE_TEST(FuncName) \
BENCHMARK_TEMPLATE(FuncName, uint8_t ); \
BENCHMARK_TEMPLATE(FuncName, uint16_t); \
BENCHMARK_TEMPLATE(FuncName, uint32_t); \
BENCHMARK_TEMPLATE(FuncName, uint64_t); \
BENCHMARK_TEMPLATE(FuncName,  int8_t ); \
BENCHMARK_TEMPLATE(FuncName,  int16_t); \
BENCHMARK_TEMPLATE(FuncName,  int32_t); \
BENCHMARK_TEMPLATE(FuncName,  int64_t); \
BENCHMARK_TEMPLATE(FuncName,  float  ); \
BENCHMARK_TEMPLATE(FuncName,  double );

MAKE_TEST(to_chars_allocate);
MAKE_TEST(to_chars_over_allocate);
MAKE_TEST(to_chars_double_iterate);
MAKE_TEST(to_chars_size);
MAKE_TEST(to_chars_solo_size);


#endif
