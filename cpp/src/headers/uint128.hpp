#pragma once
#ifndef CJM_UINT128_HPP
#define CJM_UINT128_HPP
#include<cstdint>
#include<type_traits>
#include<limits>
#include<cassert>
#include "numerics.hpp"
// Copyright 2018 CJM Screws, LLC
// 
// This contents of this file (uint128.hpp) and its inline implementation file (uint128.inl)
// are a modified version of the code released by "The Abseil Authors" in the file "int128.h"
// and "int128.cc".  The Copyright to the original Abseil code may be found below in this file.
// A copy of the original, unaltered files are included in this project as abseil_int128_h.txt (for int128.h)
// and abseil_int128_cpp.txt (for int128.cc) for your reference.  CJM Screws, LLC asserts no copyright to the
// original work of The Abseil Authors but claims copyright to its changes to the work of The Abseil Authors 
// and all additions it has made thereto.
// 
// CJM Screws, LLC releases its copyrighted work to you under the following terms and conditions:
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
// files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, 
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions: 
// 
// A copy of this notice, including the verbatim description of CJM Screws, LLC's business and the link to http://www.mdtitaniumhobbies.com as
// well as the Copyright notice of The Abseil Authors  shall be included in all copies or substantial portions of the Software. 
// 
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC (a Maryland Limited Liability Company) trades as Maryland Titanium Hobbies and may be
// reached at http://www.mdtitaniumhobbies.com.  We resell titanium alloy screws and provide the value-added
// service of decorative color anodization of these screws.  These high-strength, light weight and corrosion-resistant
// screws can make a beautiful addition to computer cases, knives, guns, drones, 
// electronic instrumentation panels and other projects.  
//
// The below notice is the Copyright asserted by The Abseil Authors on the source code CJM Screws, LLC 
// used to produce this software (which CJM Screws, LLC has modified significantly):
//
// Copyright 2017 The Abseil Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -----------------------------------------------------------------------------
// File: int128.h
// -----------------------------------------------------------------------------

//If compiled on a Big Endian system, change "true" to "false"
#define CJM_NUMERICS_LITTLE_ENDIAN true

namespace cjm
{
	namespace numerics
	{
		template<typename LimbType>
		class fixed_uint;

        template <typename Char = char, typename CharTraits=std::char_traits<Char>, typename Allocator = std::allocator<Char>>
        std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v);
		//Comparison operators
		constexpr bool operator==(uint128 lhs, uint128 rhs) noexcept;
		constexpr bool operator!=(uint128 lhs, uint128 rhs) noexcept;
		constexpr bool operator>(uint128 lhs, uint128 rhs) noexcept;
		constexpr bool operator<(uint128 lhs, uint128 rhs) noexcept;
		constexpr bool operator>=(uint128 lhs, uint128 rhs) noexcept;
		constexpr bool operator<=(uint128 lhs, uint128 rhs) noexcept;
		//Unary operators
		constexpr uint128 operator-(uint128 operand) noexcept;
		constexpr uint128 operator+(uint128 operand) noexcept;
		constexpr uint128 operator~(uint128 operand) noexcept;
		constexpr uint128 operator!(uint128 operand) noexcept;
		//Logical operators
		constexpr uint128 operator&(uint128 lhs, uint128 rhs) noexcept;
		constexpr uint128 operator|(uint128 lhs, uint128 rhs) noexcept;
		constexpr uint128 operator^(uint128 lhs, uint128 rhs) noexcept;
		//bit shift operators
		constexpr uint128 operator>>(uint128 lhs, int amount) noexcept;
		constexpr uint128 operator<<(uint128 lhs, int amount) noexcept;
		constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept;
		constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept;
		//arithmetic operators
		constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept;
		constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept;
		constexpr uint128 operator*(uint128 lhs, uint128 rhs) noexcept;
		//Division and modulus are friends declared within class

		namespace math_functions
		{
			template<>
			constexpr uint128 int_sign(uint128 val) noexcept;

			template<>
			constexpr uint128 int_gcd(uint128 first, uint128 second) noexcept;

			template<>
			constexpr uint128 int_lcm(uint128 first, uint128 second);
		}

		enum class u128_str_format
		{
			Illegal=0,
			Zero,
			Decimal,
			Hexadecimal
		};

		template<typename Chars, typename CharTraits = std::char_traits<Chars>>
		struct u128_parsing_helper
		{
			
		};

		template<typename CharTraits>
		struct u128_parsing_helper<char, CharTraits>
		{
			template<typename Allocator = std::allocator<char>>
			using str128 = std::basic_string<char, CharTraits, Allocator>;

			using sv = std::basic_string_view<char, CharTraits>;
			static constexpr std::array<sv, 2> get_hex_tags();

			static constexpr sv non_decimal_separator();

			static constexpr sv decimal_separator();

			static constexpr std::uint8_t get_value_hex(char c);

			static constexpr std::uint8_t get_value_dec(char c);

			template<typename Allocator = std::allocator<char>>
			static constexpr u128_str_format get_format(const str128<Allocator>& string);

			static constexpr bool is_legal_hex_char(char c);
			template<typename Allocator = std::allocator<char>>
			static str128<Allocator> trim_and_strip(str128<Allocator> trim_and_stripme);

			static constexpr uint128 parse_decimal_str(sv decimal_str);
			
		};

		template<typename CharTraits>
		struct u128_parsing_helper<wchar_t, CharTraits>
		{
			template<typename Allocator = std::allocator<wchar_t>>
			using str128 = std::basic_string<wchar_t, CharTraits, Allocator>;

			using sv = std::basic_string_view<wchar_t, CharTraits>;
			static constexpr std::array<sv, 2> get_hex_tags();

			static constexpr sv non_decimal_separator();

			static constexpr sv decimal_separator();

			static constexpr std::uint8_t get_value_hex(wchar_t c);

			static constexpr std::uint8_t get_value_dec(wchar_t c);

			template<typename Allocator = std::allocator<wchar_t>>
			static constexpr u128_str_format get_format(const str128<Allocator>& string);

			static constexpr bool is_legal_hex_wchar_t(wchar_t c);
			template<typename Allocator = std::allocator<wchar_t>>
			static str128<Allocator> trim_and_strip(str128<Allocator> trim_and_stripme);

			static constexpr uint128 parse_decimal_str(sv decimal_str);

		};
	}
}
namespace cjm
{
	namespace numerics
	{
		/************************************************************************/
		/* An unsigned 128 bit-integer nearly identical algorithmically
		 * and based almost entirely on the uint128 provided in abseil.io's int128.h
		 * file.  Some stylistic changes have been made such as naming conventions, etc.
		 * The string and stream operations have been modified to add support 
		 * for strings and streams that are wider than const char by turning them into templates.   
		 * 
		 * The essence of CJM's modifications  to abseil.io's code is:
		 *    1- REMOVING ALL SUPPORT FOR C++ VERSIONS PRIOR TO C++17  THIS CODE WILL NOT WORK 
		 *    ON A C++14 OR ON A C++11 (OR ... SHUDDERS ... EARLIER) ENVIRONMENT.
		 *    This code makes VERY heavy use of C++17 facilities.	
		 *    2- making every mathematical operation (exception conversions to binary floating 
		 *    point representations) constexpr-enabled -- i.e. capable of being performed
		 *    at compile time (note that stream insertion and string conversion operations
		 *    ARE NOT constexpr-enabled)
		 *    3- making nearly every mathematical operation specify noexcept.
		 *       The operations that MAY throw exceptions are division and modulus,
		 *       string and stream output, and conversions to and from binary floating 
		 *       point representations.
		 *    4- Instead of being undefined behavior, division or modulus by 0 with 
		 *     the operators provided herein  WILL THROW a std::domain_error exception.
		 *    5- The support for using uint128 as a thin wrapper around GCC's unsigned __int128
		 *    type has been removed.  If we get time to test this on GCC and can verify
		 *    that restoring such support will not disable the constexpr nature of this 
		 *    class, we will consider re-adding such operations.
		 *    6- A custom literal has been added.  You will need to use
		 *    the cjm::numerics::uint128_literals namespace to make use of it.
		 *    The literal suffix for uint128 is _u128.  It currently supports decimal
		 *    and hexadecimal literals.  It supports character separator ( ' ).  OCTAL
		 *    is not supported and there are no plans to support it.  Support for binary
		 *    literals may be added later (but I can't imagine wanting to out all 128 bits in 1s and
		 *    zeros...).  For numbers that will fit within a std::uint64_t, you are free 
		 *    to use those literals if you desire octal or binary.
		 *    7- numeric limits and type traits information have been added to namespace std 
		 *    to support interoperation with this type in a metaprogramming context.
		 *    Example literals
		 *    Hex examples:
		 *		constexpr uint128 x =  0x123456789ABCDEF0123456789ABCDEF0_u128;
		 *		constexpr auto x =  0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
		 *		constexpr auto x =  0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		 *		constexpr auto x = 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
		 *		constexpr auto x =  0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128; (capital X ok too)
		 *		constexpr auto x =  0x1'23'456'789ABCDEF012345678'9AB'C'DEF'0_u128; //stupid but legal
		 *		constexpr auto x = 0x0_u128;
		 *	  Decimal examples:
		 *	    constexpr uint128 x = 340282366920938463463374607431768211455_u128; (i.e. max value)
		 *	    constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'455_u128; //thousands separators
		 *	    constexpr auto x = 0_u128; //0 is ok too
		 *	    constexpr auto x = 00_u128; //even multiple leading zeros if all are 0
		 *	  Illegal examples:
		 *		constexpr auto x = 001_u128; //Illegal: non-zero literal other than with hex (0x)
		 *		                               prefix represents octal and octal is not supported.                                                  
		 *	   	constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'456_u128;  //(too big by one)	 																		
		 *     */
		/************************************************************************/
		class uint128
		{
		public:
			
			friend class fixed_uint<uint128>;
			using int_part = std::uint64_t;
			static constexpr size_t byte_array_size{ (128 / CHAR_BIT) / sizeof(unsigned char) };

			using byte_array = std::array<unsigned char, byte_array_size>;

			template<typename Chars, typename CharTraits = std::char_traits<Chars>, typename Allocator = std::allocator<Chars>>
			static uint128 make_from_string(std::basic_string<Chars, CharTraits, Allocator> parseMe);
			static constexpr uint128 make_from_bytes_little_endian(byte_array bytes) noexcept;
			static constexpr uint128 make_from_bytes_big_endian(byte_array bytes) noexcept;
			static constexpr uint128 MakeUint128(std::uint64_t high, std::uint64_t low) noexcept;

			static void instrumented_div_mod(std::basic_ostream<char>& stream, uint128 dividend, uint128 divisor,
				uint128* quotient_ret, uint128* remainder_ret);
			static constexpr int_part int_part_bits{ sizeof(int_part) * CHAR_BIT };
			static constexpr int_part int_part_bottom_half_bits{ int_part_bits / 2 };
			static constexpr int_part int_part_bottom_half_bitmask{std::numeric_limits<int_part>::max() >> int_part_bottom_half_bits};
			constexpr uint128() noexcept = default;
			constexpr uint128(const uint128& other) noexcept = default;
			constexpr uint128(uint128&& other) noexcept = default;
			constexpr uint128& operator=(const uint128& other) noexcept = default;
			constexpr uint128& operator=(uint128&& other) noexcept = default;
			~uint128() noexcept = default;
			// Constructors from arithmetic types
			constexpr uint128(int v) noexcept;
			constexpr uint128(unsigned int v) noexcept;
			constexpr uint128(long v) noexcept;
			constexpr uint128(unsigned long v) noexcept;
			constexpr uint128(long long v) noexcept;
			constexpr uint128(unsigned long long v) noexcept;
			// Assignment operators from arithmetic types
			constexpr uint128& operator=(int v) noexcept;
			constexpr uint128& operator=(unsigned int v) noexcept;
			constexpr uint128& operator=(long v) noexcept;
			constexpr uint128& operator=(unsigned long v) noexcept;
			constexpr uint128& operator=(long long v) noexcept;
			constexpr uint128& operator=(unsigned long long v) noexcept;

			// Conversion operators to other arithmetic types
			constexpr explicit operator bool() const noexcept;
			constexpr explicit operator char() const noexcept;
			constexpr explicit operator signed char() const noexcept;
			constexpr explicit operator unsigned char() const noexcept;
			constexpr explicit operator char16_t() const noexcept;
			constexpr explicit operator char32_t() const noexcept;
			constexpr explicit operator wchar_t() const noexcept;
			constexpr explicit operator short() const noexcept;
			constexpr explicit operator unsigned short() const noexcept;
			constexpr explicit operator int() const noexcept;
			constexpr explicit operator unsigned int() const noexcept;
			constexpr explicit operator long() const noexcept;
			constexpr explicit operator unsigned long() const noexcept;
			constexpr explicit operator long long() const noexcept;
			constexpr explicit operator unsigned long long() const noexcept;
			explicit operator float() const;
			explicit operator double() const;
			explicit operator long double() const;

			//hash code function and comparison operators
			constexpr size_t hash_code() const noexcept;

			// Arithmetic operators.
			constexpr uint128& operator+=(uint128 other) noexcept;
			constexpr uint128& operator-=(uint128 other) noexcept;
			constexpr uint128& operator*=(uint128 other) noexcept;
			// Long division/modulo for uint128.
			constexpr uint128& operator/=(uint128 other);
			constexpr uint128& operator%=(uint128 other);
			constexpr uint128 operator++(int) noexcept;
			constexpr uint128 operator--(int) noexcept;
			constexpr uint128& operator<<=(int amount) noexcept;
			constexpr uint128& operator>>=(int amount) noexcept;
			constexpr uint128& operator&=(uint128 other) noexcept;
			constexpr uint128& operator|=(uint128 other) noexcept;
			constexpr uint128& operator^=(uint128 other) noexcept;
			constexpr uint128& operator++() noexcept;
			constexpr uint128& operator--() noexcept;
			friend constexpr uint128 operator/(uint128 lhs, uint128 rhs);
			friend constexpr uint128 operator%(uint128 lhs, uint128 rhs);
			template<typename Char, typename CharTraits, typename Allocator>
			friend std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v);

			//Accessor for sub-compenents
			constexpr int_part low_part() const noexcept;
			constexpr int_part high_part() const noexcept;

			constexpr byte_array to_little_endian_arr() const noexcept;
			constexpr byte_array to_big_endian_arr() const noexcept;

		private:
			constexpr uint128(int_part high, int_part low) noexcept;
			static constexpr size_t calculate_hash(int_part hi, int_part low) noexcept;
			static constexpr void hash_combine(size_t& seed, size_t newVal) noexcept;
			static constexpr void div_mod_impl(uint128 dividend, uint128 divisor, 
				uint128* quotient_ret, uint128* remainder_ret);
			template<typename T>
			static constexpr void step(T& n, int& pos, int shift);
			static constexpr int fls(uint128 n);
			static constexpr int fls_int_part(std::uint64_t n);
			template<typename Char, typename CharTraits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
			static std::basic_string<Char, CharTraits, Allocator> to_string(uint128 item, std::ios_base::fmtflags flags);
			
		
#if CJM_NUMERICS_LITTLE_ENDIAN
			int_part m_low{};
			int_part m_high{};
#else //BIG ENDIAN
			int_part m_high;
			int_part m_low;
#endif
		};
		namespace math_functions
		{
			
		}
	
	}
}
namespace std
{
	template<>
	struct make_unsigned<cjm::numerics::uint128>
	{
		using type = cjm::numerics::uint128;
	};
	/************************************************************************/
	/* Defines the default hash-code generator struct
	* so uint128's can be used in unordered_maps, unordered_sets,
	* or any other use for which std::hash is used by default       */
	/************************************************************************/
	template<>
	struct hash<cjm::numerics::uint128>
	{
		constexpr hash() noexcept = default;
		constexpr size_t operator()(const cjm::numerics::uint128& keyVal) const
		{
			return keyVal.hash_code();
		}
	};
	/************************************************************************/
	/* Defines numeric limits and various traits for this object
	* to facilitate interoperability with code  that relies on these traits.  */
	/************************************************************************/
	template<>
	class numeric_limits<cjm::numerics::uint128>
	{
		static constexpr int times_log10_of_two(int x)
		{
			return x * 301'299 / 1'000'000;
		}
	public:
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer{ true };
		static constexpr bool is_exact = true;
		static constexpr bool is_bounded = true;
		static constexpr bool has_denorm = std::denorm_absent;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = std::numeric_limits<uint64_t>::has_quiet_NaN;
		static constexpr bool has_signaling_NaN = std::numeric_limits<uint64_t>::has_signaling_NaN;
		static constexpr bool has_denorm_loss = std::numeric_limits<uint64_t>::has_denorm_loss;
		static constexpr std::float_round_style round_style = std::numeric_limits<uint64_t>::round_style;
		static constexpr bool is_arithmetic = true;
		static constexpr bool is_iec559 = std::numeric_limits<uint64_t>::is_iec559;
		static constexpr bool is_modulo = std::numeric_limits<uint64_t>::is_modulo;
		static constexpr int digits = CHAR_BIT * (sizeof(std::uint64_t) + sizeof(std::uint64_t));
		static constexpr int digits10 = digits * 301'299/1'000'000;
		static constexpr int max_digits10 = std::numeric_limits<uint64_t>::max_digits10;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int max_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = true;

		static constexpr cjm::numerics::uint128 min() noexcept;

		static constexpr cjm::numerics::uint128 lowest() noexcept;

		static constexpr cjm::numerics::uint128 max() noexcept;

		static constexpr cjm::numerics::uint128 epsilon() noexcept;

		static constexpr cjm::numerics::uint128 round_error() noexcept;

		static constexpr cjm::numerics::uint128 infinity() noexcept;

		static constexpr cjm::numerics::uint128 quiet_NaN() noexcept;

		static constexpr cjm::numerics::uint128 signaling_NaN() noexcept;

		static constexpr cjm::numerics::uint128 denorm_min() noexcept;
	};



	//THE FOLLOWING SPECIALIZATIONS PROVIDE INFORMATION ON UINT128 that is
	//DEFINED in the <traits> header
	/************************************************************************/
	/*  Sets specified that uint128 is an arithmetic type                  */
	/************************************************************************/
	template <>
	struct is_arithmetic <cjm::numerics::uint128> :
		std::conditional_t<std::numeric_limits<cjm::numerics::uint128>::is_specialized,
		std::true_type, std::false_type> {};
	/************************************************************************/
	/* Specifies that uint128 is an integral type                           */
	/************************************************************************/
	template <> struct is_integral<cjm::numerics::uint128> :
		std::conditional_t<numeric_limits<cjm::numerics::uint128>::is_integer,
		std::true_type, std::false_type> {};
	/************************************************************************/
	/* Specifies that uint128 is unsigned                                   */
	/************************************************************************/
	template <> struct is_unsigned<cjm::numerics::uint128> :
		std::conditional_t<numeric_limits<cjm::numerics::uint128>::is_signed,
		std::false_type, std::true_type> {};
	/************************************************************************/
	/* Specified that uint128 IS NOT SIGNED                                  */
	/************************************************************************/
	template<> struct is_signed<cjm::numerics::uint128> :
		std::conditional_t<numeric_limits<cjm::numerics::uint128>::is_signed,
		std::true_type, std::false_type> {};
}


namespace cjm
{
	namespace numerics
	{
		namespace fixed_uint_literals
		{
			class fixed_uint_lit_helper;
		}
		namespace uint128_literals
		{
			/************************************************************************/
			/* This literal operator accepts numeric literals in decimal or hexadecimal
			 * format only.  Octal literals are not supported.  Binary literals 
			 * are not supported now but support might be added.
			 * 
			 * This literal operator supports character separators i.e. " ' "
			 * 
			 * Most of the common types of illegal literals should cause a compilation 
			 * failure.  A few of them, however, if not assigned in a constexpr context,
			 * will generate a domain error exception at run time.  
			 * 
			 * Calling this literal operator directly or using it other than to parse
			 * literals in source code is NOT RECOMMENDED AND NOT SUPPORTED.  
			 * DO NOT USE THIS LITERAL OPERATOR TO PARSE STRINGS
			 *                  */
			/************************************************************************/
			template<char... Chars>
			constexpr uint128 operator"" _u128();

			enum class lit_type
			{
				Illegal = 0,
				Decimal = 1,
				Hexadecimal = 2,
				Zero = 3,
			};

			/************************************************************************/
			/* NOT FOR EXTERNAL USE
			 * The functions provided
			 * for parsing a series of chars
			 * herein are ONLY useful in a context 
			 * of parsing a literal at compile time.
			 * The are not safe for or intended for general use.
			 * This class exists to facilitate the literal operator:
			 * constexpr uint128 operator "" _u128(const char* chars);				*/
			/************************************************************************/
			class uint128_lit_helper
			{
				friend class cjm::numerics::fixed_uint_literals::fixed_uint_lit_helper; 

				template<char... Chars>
				friend constexpr uint128 operator"" _u128();
				
				template<size_t Size>
				static constexpr lit_type get_lit_type(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr bool are_all_chars_0(std::array<char, Size> arr);

				static constexpr uint8_t get_hex_value(char c);

				template<size_t Size>
				static constexpr std::pair<bool, size_t> scan_chars_dec(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr std::pair<size_t, size_t> get_dec_val(std::array<char, Size> arr, size_t index);

				static constexpr char to_lower(char c) noexcept;

				template<size_t Size>
				static constexpr uint128 get_hex_literal(std::array<char, Size> arr);

				template<size_t Size>
				static constexpr uint128 get_decimal_literal(std::array<char, Size> arr);

				static constexpr bool is_legal_hex_char(char c) noexcept;

				template<size_t Size>
				static constexpr std::pair<uint8_t, size_t> get_byte(std::array<char, Size> arr, size_t index);

			public:

				template<size_t Size>
				static constexpr uint128 parse_from_char_array(std::array<char, Size> arr);

				template<char... Chars>
				static constexpr std::array<char, sizeof...(Chars)> get_array();

				uint128_lit_helper() = delete;
				~uint128_lit_helper() = delete;
				uint128_lit_helper(const uint128_lit_helper& other) = delete;
				uint128_lit_helper(uint128_lit_helper&& other) noexcept = delete;
				uint128_lit_helper& operator=(const uint128_lit_helper& other) = delete;
				uint128_lit_helper& operator=(uint128_lit_helper&& other) noexcept = delete;
			};
		}
	}
}

#include "uint128.inl"
#endif