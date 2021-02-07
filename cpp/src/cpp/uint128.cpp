#include "uint128.hpp"
#include <array>

namespace
{
	using namespace cjm::numerics;
	using namespace uint128_literals;
	using ones_arr = std::array<uint128, 129>;
	constexpr uint128 all_ones = 0xffff'ffff'ffff'ffff'ffff'ffff'ffff'ffff_u128;
	constexpr ones_arr init_ones_lookup() noexcept
	{
		auto ret = ones_arr{};
		uint128 ones = all_ones;
		for (auto i = 128; i > -1; --i)
		{
			ret[i] = all_ones;
			ones <<= 1;
		}
		return ret;
	}
    [[maybe_unused]]
    constexpr inline ones_arr ones_lookup = init_ones_lookup();
	
}

constexpr cjm::numerics::uint128 GetZero() noexcept
{
	constexpr cjm::numerics::uint128 ret{};
	static_assert(ret == 0);
	return ret;
}

void cjm::numerics::uint128::instrumented_div_mod(std::basic_ostream<char>& stream, uint128 dividend, uint128 divisor,
	uint128* quotient_ret, uint128* remainder_ret)
{
	constexpr auto newl = "\n";
	if (divisor == 0)
	{
		throw std::domain_error("Division and/or modulus by zero is forbidden.");
	}
	if (divisor > dividend)
	{
		*quotient_ret = 0;
		*remainder_ret = dividend;
		return;
	}
	if (divisor == dividend)
	{
		*quotient_ret = 1;
		*remainder_ret = 0;
		return;
	}

	stream << std::hex;
	stream << "BEGIN INSTRUMENTED DIV MOD 128" << newl;
	stream << "dividend: 0x" << dividend << newl;
	stream << "divisor: 0x" << divisor << newl;

	auto denominator = divisor;
	auto quotient = make_uint128(0, 0);

	stream << "denominator: 0x" << denominator << newl;
	stream << "quotient: 0x" << quotient << newl;

	const auto flsDividend = fls(dividend);
	const auto flsDenominator = fls(denominator);
	const auto shiftAmount = flsDividend - flsDenominator;
	denominator <<= shiftAmount;
	stream << std::dec;
	stream << "DECIMAL FLS RESULTS:" << newl;
	stream << "flsDividend: " << flsDividend << newl;
	stream << "flsDenominator: " << flsDenominator << newl;
	stream << "shiftAmount: " << shiftAmount << newl;

	stream << std::hex;
	stream << "HEX FLS RESULTS:" << newl;
	stream << "flsDividend: 0x" << flsDividend << newl;
	stream << "flsDenominator: 0x" << flsDenominator << newl;
	stream << "shiftAmount: 0x" << shiftAmount << newl;

	stream << "denominator <<= 0x" << shiftAmount << " == 0x" << denominator << newl;
	stream << std::boolalpha;
	stream << newl << newl << "STARTING CALCULATION LOOP" << newl;
	for (int i = 0; i <= shiftAmount; i++)
	{
		stream << "i = " << std::dec << i << " i <= " << shiftAmount 
			<< "= " << (i <= shiftAmount) << std::hex << newl;
		stream << "quotient: 0x" << quotient << newl;
		quotient <<= 1;
		stream << "(quotient <<= 1): 0x" << quotient << newl;
		stream << "denominator: 0x" << denominator << newl;
		bool dividendGreaterThanOrEqualToDenom = dividend >= denominator;
		stream << "dividendGreaterThanOrEqualToDenom: " << dividendGreaterThanOrEqualToDenom << newl;
		if (dividendGreaterThanOrEqualToDenom)
		{
			dividend -= denominator;
			quotient |= 1;
			stream << "dividend -= denominator: 0x" << dividend << newl;
			stream << "quotient |= 1: 0x" << quotient << newl;
		}
		denominator >>= 1;
		stream << "denominator >>= 1: 0x" << denominator << newl;
	}

	stream << "final quotient: 0x" << quotient << newl;
	stream << "final remainder (dividend): 0x" << dividend << newl;
	*quotient_ret = quotient;
	*remainder_ret = dividend;
	stream << "END INSTRUMENTED DIV MOD 128" << newl;
} 

cjm::numerics::uint128::operator float() const 
{
	return static_cast<float>(m_low) + std::ldexp(static_cast<float>(m_high), 64);
}

cjm::numerics::uint128::operator double() const 
{
	return static_cast<double>(m_low) + std::ldexp(static_cast<double>(m_high), 64);
}

cjm::numerics::uint128::operator long double() const 
{
	return static_cast<long double>(m_low) +
		std::ldexp(static_cast<long double>(m_high), 64);
}

void uint128::best_safe_div_mod(uint128 dividend, uint128 divisor, uint128* quotient, uint128* remainder)
{
	if (divisor == 0) throw std::domain_error{ "Division and/or modulus by zero is forbidden." };
	if (!quotient) throw std::invalid_argument{ "quotient is nullptr!" };
	if (!remainder) throw std::invalid_argument{ "remainder is nullptr!" };

	uint128::divmod_result_t result = unsafe_div_mod(dividend, divisor);
	*quotient = result.quotient;
	*remainder = result.remainder;
}

#ifdef CJM_HAVE_BUILTIN_128
cjm::numerics::uint128& cjm::numerics::uint128::operator=(__uint128_t other) noexcept
{
    return (*this = cjm::numerics::bit_cast<uint128, unsigned __int128>(other));
}


cjm::numerics::uint128::operator unsigned __int128() const noexcept
{
    return cjm::numerics::bit_cast<unsigned __int128, uint128>(*this);
//    unsigned __int128 ret = m_high;
//    ret <<= 64;
//    ret |= m_low;
//    return ret;
}

cjm::numerics::uint128::uint128(__uint128_t other) noexcept
{
	m_high = 0;
	m_low = 0;
    *this = cjm::numerics::bit_cast<uint128, unsigned __int128>(other);
}
#endif

//This method is based on the 128-bit unsigned integer division
//provided by clang / LLVM for a built-in unsigned __int128 type
//the original version from which this was based can be found in the
//source repository in the file "Llvm_u128div.txt" The original is
//part of the LLVM project, licensed to CJM Screws, LLC under
//the Apache-2.0 license with LLVM exceptions.  No copyright claimed
//by CJM Screws, LLC to the original, unaltered form of the LLVM code.
//Here, it was modified to use microsoft's intrinsic functions in place
//of GCC/Clang intrinsic functions as well as to use a compiler intrinsic
//for the most optimized path of the algorithm rather than inline assembly
//language (because 1- x64 Microsoft does not support inline assembly and 2-
//because compiler can deal with intrinsics more efficiently than either
//inline or separately written assembly).
//Also, the clang version used unions for type punning which 1-
//(though allowed in C) is undefined behavior in C++ and 2- isn't necessary
//anyway because this function is a static member of uint128 and has
//direct access to its m_high and m_low members anyway.
//If the optimized path from clang cannot be taken, I revert
//to using abseil's shift with subtract algorithm as elsewhere
//rather than using the slow-case handler from clang.
#if defined(_MSC_VER) && defined(_M_X64)
void uint128::div_mod_msc_x64_impl(uint128 dividend, uint128 divisor, uint128* quotient_ret, uint128* remainder_ret) noexcept
{
	constexpr size_t n_utword_bits = sizeof(uint128) * CHAR_BIT;
	assert(quotient_ret != nullptr && remainder_ret != nullptr && divisor != 0);
	uint128 quotient=0;
	uint128 remainder=0;
	if (divisor > dividend)
	{
		*remainder_ret = dividend;
		*quotient_ret = 0;
		return;
	}
	// When the divisor fits in 64 bits, we can use an optimized path.
	if (divisor.m_high == 0)
	{
		remainder.m_high = 0;
		if (dividend.m_high < divisor.m_low)
		{
			// The result fits in 64 bits.
			quotient.m_low = CJM_UDIV128(dividend.m_high, dividend.m_low,
				divisor.m_low, &remainder.m_low);
			quotient.m_high = 0;
		}
		else
		{
			// First, divide with the high part to get the remainder in dividend.s.high.
			// After that dividend.s.high < divisor.s.low.
			quotient.m_high = dividend.m_high / divisor.m_low;
			dividend.m_high = dividend.m_high % divisor.m_low;
			quotient.m_low = CJM_UDIV128(dividend.m_high, dividend.m_low,
				divisor.m_low, &remainder.m_low);
		}
		*remainder_ret = remainder;
		*quotient_ret = quotient;
		return;		
	}
	unsafe_constexpr_div_mod_impl(dividend, divisor, quotient_ret, remainder_ret);
	return;
}
#else
void uint128::div_mod_msc_x64_impl([[maybe_unused]]uint128 dividend,[[maybe_unused]] uint128 divisor, uint128* quotient_ret, uint128* remainder_ret) noexcept
{
    //do nothing ... in gcc this never gets called but still needs to be there
    *quotient_ret =0;
    *remainder_ret = 0;
}
#endif