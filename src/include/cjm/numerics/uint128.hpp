// Copyright © 2020-2021 CJM Screws, LLC
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// CJM Screws, LLC is a Maryland Limited Liability Company.
// No copyright claimed to unmodified original work of others.
// The original, unmodified work of others, to the extent included in this library,
// is licensed to you under the same terms under which it was licensed to CJM Screws, LLC.
// For information about copyright and licensing of the original work of others,
// see Notices file in cjm/ folder.
//
// This library is modeled on the abseil C++ numerics library.  MOST OF ABSEIL'S CODE HAS BEEN MODIFIED SIGNIFICANTLY BY THIS LIBRARY.
// Some verbatim reproduction may remain in places.  The following notice was found in abseil's uint128 source code and is reproduced here in
// accordance with the terms of the Apache 2.0 license, under which Abseil's code was licensed to CJM Screws, LLC:

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

#ifndef CJM_UINT128_HPP_
#define CJM_UINT128_HPP_
#include <iostream>
#include <compare>
#include<cstdint>
#include<type_traits>
#include<limits>
#include <string>
#include<cassert>
#include<optional>
#include<array>
#include<functional>
#include <utility>
#include <algorithm>
#include <cjm/numerics/numerics.hpp>
#include <cjm/numerics/cjm_numeric_concepts.hpp>
#include <cjm/string/cjm_string.hpp>
#include <cmath>
#include <cjm/numerics/fixed_uint_container_math.hpp>
#include <cctype>
#include <cwctype>
#include <concepts>
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
//	  http://www.apache.org/licenses/LICENSE-2.0
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

namespace cjm::numerics
{
	class uint128;

	///<summary>
	///In the future, will define
	///cjm unsigned integers of larger type implemented in terms
	///of a cjm unsigned integer of smaller type.
	///</summary>
	template<concepts::cjm_unsigned_integer LimbType>
	class [[maybe_unused]] fixed_uint;

	template<concepts::integer IntegerType>
	struct divmod_result;
	
	template<cjm::numerics::concepts::can_find_most_significant_set_bitpos UI>
	constexpr int most_sign_set_bit(UI value) noexcept;

	/// <summary>
	/// The calculation mode used when not constant evaluating uint128s.
	/// For GCC and Clang, if a built-in uint128_t is available, that logic is used.
	/// On microsoft x64 (Intel/AMD), compiler intrinsics are used
	/// Otherwise, the --presumably-- slower, constant-evaluation-capable
	/// algorithm is used.
	/// </summary>
	constexpr uint128_calc_mode calculation_mode = init_eval_mode();

	namespace internal
	{
		constexpr int fls_int_part(std::uint64_t n) noexcept;

		constexpr int fls_default(std::uint64_t n) noexcept;

		template<cjm::numerics::concepts::builtin_unsigned_integer UI>
		constexpr int countl_zero(UI n) noexcept;

		constexpr int countl_zero(uint128 n) noexcept;
		
		template <typename T>
		constexpr void step(T& n, int& pos, int shift) noexcept;

		template<concepts::builtin_floating_point TFloat>
		uint128 make_from_floating_point(TFloat v) noexcept;

		template<concepts::builtin_128bit_unsigned_integer Ui128>
		Ui128 add_with_carry(Ui128 first_addend, Ui128 second_addend, unsigned char carry_in,
						  unsigned char& carry_out) noexcept;

		template<concepts::builtin_128bit_unsigned_integer Ui128>
		Ui128 sub_with_borrow(Ui128 minuend, Ui128 subtrahend, unsigned char borrow_in,
			unsigned char& borrow_out) noexcept;
		
		template<concepts::cjm_unsigned_integer Ui128>
			requires (sizeof(Ui128) == 16 && !concepts::builtin_128bit_unsigned_integer<Ui128>)
		Ui128 sub_with_borrow(Ui128 minuend, Ui128 subtrahend,
			unsigned char borrow_in, unsigned char& borrow_out) noexcept;
		
		//Not intended to be used, made so untaken if constexpr branch of builtin test doesn't
		//blow up ... even though it isn't taken
		template<concepts::cjm_unsigned_integer Ui128>
			requires (sizeof(Ui128) == 16 && !concepts::builtin_128bit_unsigned_integer<Ui128>)
		Ui128 add_with_carry(Ui128 first_addend, Ui128 second_addend,
			unsigned char carry_in, unsigned char& carry_out) noexcept;
	}

	/// <summary>
	/// Stream insertion operator for uint128
	/// </summary>
	/// <typeparam name="Char">The Character type</typeparam>
	/// <typeparam name="CharTraits">traits for character</typeparam>
	/// <typeparam name="Allocator">allocator for character</typeparam>
	/// <param name="os">the stream to insert</param>
	/// <param name="v">the value to stringify and insert into the stream</param>
	/// <returns>the stream with the value inserted.</returns>
	template <typename Char = char, typename CharTraits=std::char_traits<Char>, typename Allocator = std::allocator<Char>>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Char, CharTraits, Allocator>
	std::basic_ostream<Char, CharTraits>& operator<<(std::basic_ostream<Char, CharTraits>& os, uint128 v);
	/// <summary>
	/// Stream extraction operator for uint128
	/// </summary>
	/// <typeparam name="Char">The Character type</typeparam>
	/// <typeparam name="CharTraits">traits for character</typeparam>
	/// <typeparam name="Allocator">allocator for character</typeparam>
	/// <param name="is">the stream to insert</param>
	/// <param name="v">the value to stringify and insert into the stream</param>
	/// <returns>the stream with the value inserted.</returns>
	template <typename Char = char, typename CharTraits = std::char_traits<Char>>
		requires cjm::numerics::concepts::char_with_traits<Char, CharTraits>
	std::basic_istream<Char, CharTraits>& operator>>(std::basic_istream<Char, CharTraits>& is, uint128& v);

	constexpr std::uint64_t add_with_carry_u64(std::uint64_t addend_1, std::uint64_t addend_2, 
		unsigned char carry_in, unsigned char& carry_out) noexcept;

	//Comparison operators
	constexpr std::strong_ordering operator <=>(uint128 lhs, uint128 rhs) noexcept; 
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
	constexpr bool operator!(uint128 operand) noexcept;
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



	enum class u128_str_format
	{
		Illegal=0,
		Zero,
		Decimal,
		Hexadecimal
	};

	template<typename Chars, typename CharTraits = std::char_traits<Chars>>
			  requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
	struct u128_parsing_helper final
	{
		using char_t = Chars;

		template<typename Allocator = std::allocator<Chars>>
		requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
		using str128 = std::basic_string<Chars, CharTraits, Allocator>;

		using sv = std::basic_string_view<Chars, CharTraits>;


		static constexpr std::array<sv, 2> get_hex_tags();

		static constexpr sv non_decimal_separator();

		static constexpr sv decimal_separator();

		static constexpr std::uint8_t get_value_hex(char_t c);
		static constexpr std::pair<std::uint8_t, sv> get_value_hex(sv text);
		static constexpr std::uint8_t get_value_dec(char_t c);
		static constexpr bool is_legal_hex_char(char_t c);

		template<typename Allocator = std::allocator<char_t>>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
		static constexpr u128_str_format get_format(const str128<Allocator>& string)
		{
			constexpr char zero = '0';
			constexpr char_t zero_cast = static_cast<char_t>(zero);
			auto length = string.length();
			if (length < 1)
				return u128_str_format::Illegal;
			bool allZero = std::all_of(string.cbegin(), string.cend(), [] (char_t c) -> bool
			{
				return c == zero_cast;
			});
			if (string[0] == zero_cast && (length == 1 || allZero))
				return u128_str_format::Zero;
			if (length < 3)
			{
				if (string[0] == zero_cast)
					return u128_str_format::Illegal;
				return std::all_of(string.cbegin(), string.cend(),
								   [](char_t c) -> bool { return static_cast<char>(c) >= 0x30 && static_cast<char>(c) <= 0x39; }) ?
					   u128_str_format::Decimal :
					   u128_str_format::Illegal;
			}
			std::basic_string_view<Chars, CharTraits>  orig_view = string;
			auto firstTwo = orig_view.substr(0, 2);
			auto hex_tags = get_hex_tags();
			bool hasHexTag = std::any_of(hex_tags.cbegin(), hex_tags.cend(), [=](sv tag) -> bool { return tag == firstTwo; });
			if (!hasHexTag)
			{
				return orig_view[0] != zero_cast
					&& std::all_of(orig_view.cbegin(), orig_view.cend(),
						[](char_t c) -> bool { return static_cast<char>(c) >= 0x30 && static_cast<char>(c) <= 0x39; })
						?	u128_str_format::Decimal :
							u128_str_format::Illegal;
			}
			sv afterFirstTwo = orig_view.substr(2, string.length() - 2);
			return std::all_of(afterFirstTwo.cbegin(), afterFirstTwo.cend(), [](char_t c) -> bool {return is_legal_hex_char(c); })
				   ?	u128_str_format::Hexadecimal :
				   u128_str_format::Illegal;
		}


		template<typename Allocator = std::allocator<char_t>>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars, CharTraits, Allocator>
		static str128<Allocator> trim_and_strip(str128<Allocator> trim_and_strip_me)
		{
			std::basic_string_view<Chars, CharTraits> view = trim_and_strip_me;
			std::basic_string<Chars, std::char_traits<Chars>, std::allocator<Chars>> trimmed;
			trimmed = cjm::string::trim_as_sv(view);
			constexpr auto non_dec_separators = non_decimal_separator();
			for (auto separator : non_dec_separators)
			{
				trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), separator), trimmed.end());
			}
			return trimmed;
		}

		static constexpr uint128 parse_decimal_str(sv decimal_str);
		static constexpr uint128 parse_hex_str(sv hex_str);
	};

}
namespace std
{
	/************************************************************************/
	/* Defines the default hash-code generator struct
	* so uint128's can be used in unordered_maps, unordered_sets,
	* or any other use for which std::hash is used by default	   */
	/************************************************************************/
	template<>
	struct hash<cjm::numerics::uint128> final
	{
		constexpr hash() noexcept = default;
		constexpr size_t operator()(const cjm::numerics::uint128& keyVal) const noexcept;	  
	};

	
}

namespace cjm::numerics
{
	/************************************************************************/
	/* An unsigned 128 bit-integer nearly identical algorithmically
	 * and based almost entirely on the uint128 provided in abseil.io's int128.h
	 * file.  Some stylistic changes have been made such as naming conventions, etc.
	 * The string and stream operations have been modified to add support
	 * for strings and streams that are wider than const char by turning them into templates.
	 *
	 * The essence of CJM's modifications  to abseil.io's code is:
	 *	1- REMOVING ALL SUPPORT FOR C++ VERSIONS PRIOR TO C++20  THIS CODE WILL NOT WORK
	 *	ON A C++17 OR EARLIER (OR ... SHUDDERS ... EARLIER) ENVIRONMENT.
	 *	This code makes VERY heavy use of C++17 and C++20 facilities.
	 *	2- making every mathematical operation (exception conversions to binary floating
	 *	point representations) constexpr-enabled -- i.e. capable of being performed
	 *	at compile time (note that stream insertion and string conversion operations
	 *	ARE NOT constexpr-enabled)
	 *	3- making nearly every mathematical operation specify noexcept.
	 *	 The operations that MAY throw exceptions are division and modulus,
	 *	 string and stream output, and conversions to and from binary floating
	 *	 point representations.
	 *	4- Instead of being undefined behavior, division or modulus by 0 with
	 *	 the operators provided herein  WILL THROW a std::domain_error exception.
	 *	5- A custom literal has been added.  You will need to use
	 *	the cjm::numerics::uint128_literals namespace to make use of it.
	 *	The literal suffix for uint128 is _u128.  It currently supports decimal
	 *	and hexadecimal literals.  It supports character separator ( ' ).  OCTAL
	 *	is not supported and there are no plans to support it.  Support for binary
	 *	literals may be added later (but I can't imagine wanting to out all 128 bits in 1s and
	 *	zeros...).  For numbers that will fit within a std::uint64_t, you are free
	 *	to use those literals if you desire octal or binary.
	 *	6- numeric limits and type traits information have been added to namespace std
	 *	to support interoperation with this type in a metaprogramming context.
	 *	7- where available (e.g. 64-bit versions of GCC and Clang), this type serves
	 *	   as a thin wrapper around the native support those compilers add for unsigned __int128.
	 *	   Using this type still provides advantages over direct use of the native version:
	 *		  a. Literal support (constexpr of course!)
	 *		  b. stream insertion and extraction support / string conversion and parsing support
	 *		  c. uses built-in type for arithmetic anyway
	 *		  d. portable to compilers that DO NOT offer such a type
	 *		  e. full constexpr support (except string / float ops) even if those compilers do not treat unsigned __int128
	 *			 as a completely constexpr-enabled type
	 *	8- if using microsoft's compiler targeting windows with an Intel or AMD 64 bit processor,
	 *	   compiler intrinsics will be used for heavy arithmetic computation in lieu of fallback arithmetic
	 *	   used when calculations are PERFORMED AT RUNTIME.  This means you get the full-benefit of constexpr
	 *	   for compile-time evaluation but (presumably) more efficient compiler intrinsics for runtime evaluation.
	 *   9- if using microsoft's compiler targeting windows with an Intel or AMD 64 bit processor the RUNTIME division algorithm
	 *	  is based on Clang-LLVM's division algorithm for the native unsigned __int128.  The paths in that algorithm
	 *	  that make use of inline assembly in Clang make use of x64 compiler intrinsics for this adaptation.  NOTE:
	 *	  constant-evaluated division uses the adapted fallback algorithm from abseil/google.
	 *	Example literals
	 *	Hex examples:
	 *		constexpr uint128 x =  0x123456789ABCDEF0123456789ABCDEF0_u128;
	 *		constexpr auto x =  0x1234'5678'9ABC'DEF0'1234'5678'9ABC'DEF0_u128;
	 *		constexpr auto x =  0x1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
	 *		constexpr auto x = 0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128;
	 *		constexpr auto x =  0X1234'5678'9abc'def0'1234'5678'9abc'def0_u128; (capital X ok too)
	 *		constexpr auto x =  0x1'23'456'789ABCDEF012345678'9AB'C'DEF'0_u128; //stupid but legal
	 *		constexpr auto x = 0x0_u128;
	 *	  Decimal examples:
	 *		constexpr uint128 x = 340282366920938463463374607431768211455_u128; (i.e. max value)
	 *		constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'455_u128; //thousands separators
	 *		constexpr auto x = 0_u128; //0 is ok too
	 *		constexpr auto x = 00_u128; //even multiple leading zeros if all are 0
	 *	  Illegal examples:
	 *		constexpr auto x = 001_u128; //Illegal: non-zero literal other than with hex (0x)
	 *									   prefix represents octal and octal is not supported.
	 *	   	constexpr auto x = 340'282'366'920'938'463'463'374'607'431'768'211'456_u128;  //(too big by one)
	 *	 */
	/************************************************************************/
	class alignas(uint128_align) uint128 final
	{
	public:
		static constexpr size_t byte_array_size = 16;

		using int_part = std::uint64_t;
		using byte_array = std::array<unsigned char, byte_array_size>;
		using divmod_result_t = divmod_result<uint128>;

		/// <summary>
		/// Parses a string_view into a uint128
		/// </summary>
		/// <typeparam name="Chars">Character type</typeparam>
		/// <typeparam name="CharTraits">Character traits type</typeparam>
		/// <param name="parse_me">the string_view to parse.</param>
		/// <returns>A uint128 parsed from parse_me.</returns>
		/// <exception cref="std::invalid_argument">A uint128 could not be parsed from
		/// the supplied value.</exception>
		template<typename Chars, typename CharTraits = std::char_traits<Chars>>
		requires cjm::numerics::concepts::char_with_traits<Chars, CharTraits>
			static uint128 make_from_string(std::basic_string_view<Chars, CharTraits>
				parse_me);

		/// <summary>
		/// Parses a string into a uint128
		/// </summary>
		/// <typeparam name="Chars">Character type</typeparam>
		/// <typeparam name="CharTraits">Character traits type</typeparam>
		/// <param name="parse_me">the string to parse.</param>
		/// <returns>A uint128 parsed from parse_me.</returns>
		/// <exception cref="std::invalid_argument">A uint128 could not be parsed from
		/// the supplied value.</exception>
		template<typename Chars, typename CharTraits = std::char_traits<Chars>,
			typename Allocator = std::allocator<Chars>>
				requires cjm::numerics::concepts::char_with_traits_and_allocator<Chars,
					CharTraits, Allocator>
		static uint128 make_from_string(const std::basic_string<Chars, CharTraits,
			Allocator>& parse_me);

		/// <summary>
		/// For future functionality -- will facilitate building addition operations
		/// for a uint256 type.
		/// Perform add with carry on two unsigned 128 bit integers
		/// </summary>
		/// <param name="first_addend">first addend</param>
		/// <param name="second_addend">second addend</param>
		/// <param name="carry_in">0 for no carry-in, 1 for carry-in</param>
		/// <returns>a pair whose first member is the sum and the second number
		/// represents carry_out.  carry_out will be non-zero if the addition overflowed.</returns>
		/// <remarks>Not yet fully tested.</remarks>
		friend constexpr std::pair<uint128, unsigned char>
			add_with_carry(uint128 first_addend, uint128 second_addend,
				unsigned char carry_in) noexcept;

		/// <summary>
		/// For future functionality -- will facilitate building subtraction operations
		/// for a uint256 type.
		/// </summary>
		/// <param name="minuend">minuend</param>
		/// <param name="subtrahend">subtrahend</param>
		/// <param name="borrow_in">0 for no borrow-in, 1 for borrow-in</param>
		/// <returns>a pair whose first member is the difference and the second number
		/// represents borrow_out.  borrow_out will be non-zero if the subtraction overflowed.</returns>
		/// <remarks>Not yet fully tested.</remarks>
		friend constexpr std::pair<uint128, unsigned char> sub_with_borrow(uint128 minuend,
			uint128 subtrahend, unsigned char borrow_in) noexcept;

		/// <summary>
		/// add with carry for limbs / int parts
		/// </summary>
		/// <param name="first_addend">first addend</param>
		/// <param name="second_addend">second addend</param>
		/// <param name="carry_in">carry in</param>
		/// <param name="carry_out">carry out</param>
		/// <returns>sum and carry out</returns>
		/// <remarks>Not yet fully tested.</remarks>
		friend constexpr int_part add_with_carry(int_part first_addend,
			int_part second_addend, unsigned char carry_in,
			unsigned char& carry_out) noexcept;
		/// <summary>
		/// subtract with borrow for limbs / int parts
		/// </summary>
		/// <param name="minuend">the minuend</param>
		/// <param name="subtrahend">the subtrahend</param>
		/// <param name="borrow_in">borrow in</param>
		/// <param name="borrow_out">borrow out</param>
		/// <returns>difference and borrow out</returns>
		/// <remarks>Not yet fully tested.</remarks>
		friend constexpr int_part sub_with_borrow(int_part minuend, int_part subtrahend,
			unsigned char borrow_in, unsigned char& borrow_out) noexcept;
		static constexpr uint128 make_from_bytes_little_endian(byte_array bytes) noexcept;
		static constexpr uint128 make_from_bytes_big_endian(byte_array bytes) noexcept;
		static constexpr uint128 make_uint128(std::uint64_t high,
			std::uint64_t low) noexcept;
		static constexpr std::optional<divmod_result_t> try_div_mod(uint128 dividend,
			uint128 divisor) noexcept;
		static constexpr divmod_result_t div_mod(uint128 dividend, uint128 divisor);
		static constexpr divmod_result_t unsafe_div_mod(uint128 dividend,
			uint128 divisor) noexcept; //NOLINT (bugprone-exception-escape)
		static inline void instrumented_div_mod(std::basic_ostream<char>& stream, uint128 dividend,
			uint128 divisor, uint128* quotient_ret, uint128* remainder_ret);
		static constexpr int_part int_part_bits{ sizeof(int_part) * CHAR_BIT };
		static constexpr int_part int_part_bottom_half_bits{ int_part_bits / 2 };
		static constexpr int_part int_part_bottom_half_bitmask{
			std::numeric_limits<int_part>::max() >> int_part_bottom_half_bits };
		static constexpr int most_sign_set_bit(uint128 value) noexcept;
		constexpr uint128() noexcept;
		constexpr uint128(const uint128& other) noexcept = default;
		constexpr uint128(uint128&& other) noexcept = default;
		constexpr uint128& operator=(const uint128& other) noexcept = default;
		constexpr uint128& operator=(uint128&& other) noexcept = default;
		constexpr explicit uint128(int_part high, int_part low) noexcept;
		constexpr ~uint128() noexcept = default;
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

		//converting ctors from floating point types
		inline explicit uint128(float f) noexcept;
		inline explicit uint128(double d) noexcept;
		inline explicit uint128(long double d) noexcept;

		// Conversion operators to other arithmetic types
		constexpr explicit operator bool() const noexcept;
		constexpr explicit operator char() const noexcept;
		constexpr explicit operator signed char() const noexcept;
		constexpr explicit operator unsigned char() const noexcept;
		constexpr explicit operator char8_t() const noexcept;
		constexpr explicit operator char16_t() const noexcept;
		constexpr explicit operator char32_t() const noexcept;
		constexpr explicit operator wchar_t() const noexcept;
		constexpr explicit operator std::int16_t() const noexcept;
		constexpr explicit operator std::uint16_t() const noexcept;
		constexpr explicit operator std::int32_t() const noexcept;
		constexpr explicit operator std::uint32_t() const noexcept;
		constexpr explicit operator std::int64_t() const noexcept;
		constexpr explicit operator std::uint64_t() const noexcept;
		inline explicit operator float() const;
		inline explicit operator double() const;
		inline explicit operator long double() const;

		//ctor, assign op and explicit to-conversion for intrinsic unsigned __int128
		template<fixed_uint_container::internal::is_full_builtin_u128 Ui>
		inline uint128(Ui other) noexcept
			: m_limbs{ bit_cast<fixed_uint_container::uint128_always_split_container_t,
				Ui>(other) } {}
		
		template<fixed_uint_container::internal::is_full_builtin_u128 Ui>
		inline uint128& operator=(Ui other) noexcept
		{
			m_limbs = bit_cast<fixed_uint_container::uint128_always_split_container_t,
				Ui>(other);
			return *this;
		}
		template<fixed_uint_container::internal::is_full_builtin_u128 Ui>
		inline explicit operator Ui() const noexcept
		{
			return bit_cast<Ui,
			fixed_uint_container::uint128_always_split_container_t>(m_limbs);

		}

		template<fixed_uint_container::internal::is_partial_builtin_u128 Ui>
		inline uint128(Ui other) noexcept : m_limbs{ bit_cast<fixed_uint_container::uint128_always_split_container_t,
				Ui>(other) } {}	
		
		template<fixed_uint_container::internal::is_partial_builtin_u128 Ui>
		inline uint128& operator=(Ui other) noexcept
		{
			m_limbs = bit_cast<fixed_uint_container::uint128_always_split_container_t,
				Ui>(other);
			return *this;
		}
		
		template<fixed_uint_container::internal::is_partial_builtin_u128 Ui>
		inline explicit operator Ui() const noexcept
		{
			return bit_cast<Ui, fixed_uint_container::uint128_always_split_container_t>(m_limbs);
		}

		//hash code function and comparison operators
		[[nodiscard]] constexpr size_t hash_code() const noexcept;
			   
		// Arithmetic operators.
		friend constexpr uint128 operator+(uint128 lhs, uint128 rhs) noexcept;
		friend constexpr uint128 operator-(uint128 lhs, uint128 rhs) noexcept;
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
		constexpr uint128& operator<<=(unsigned amount) noexcept;
		constexpr uint128& operator>>=(unsigned amount) noexcept;
		constexpr uint128& operator<<=(uint128 amount) noexcept;
		constexpr uint128& operator>>=(uint128 amount) noexcept;
		constexpr uint128& operator&=(uint128 other) noexcept;
		constexpr uint128& operator|=(uint128 other) noexcept;
		constexpr uint128& operator^=(uint128 other) noexcept;
		constexpr uint128& operator++() noexcept;
		constexpr uint128& operator--() noexcept;
		friend constexpr uint128 operator/(uint128 lhs, uint128 rhs);
		friend constexpr uint128 operator%(uint128 lhs, uint128 rhs);
		friend constexpr uint128 operator>>(uint128 lhs, int amount) noexcept;
		friend constexpr uint128 operator<<(uint128 lhs, int amount) noexcept;
		friend constexpr uint128 operator>>(uint128 lhs, uint128 amount) noexcept;
		friend constexpr uint128 operator<<(uint128 lhs, uint128 amount) noexcept;
		template<typename Char, typename CharTraits, typename Allocator>
			requires cjm::numerics::concepts::char_with_traits_and_allocator<Char,
				CharTraits, Allocator>
		friend std::basic_ostream<Char, CharTraits>& operator<< (std::basic_ostream<Char, 
			CharTraits>& os, uint128 v);

		//Accessor for sub-components
		[[nodiscard]] constexpr int_part low_part() const noexcept;
		[[nodiscard]] constexpr int_part high_part() const noexcept;

		[[nodiscard]] constexpr byte_array to_little_endian_arr() const noexcept; //NOLINT (bugprone-exception-escape)
		[[nodiscard]] constexpr byte_array to_big_endian_arr() const noexcept; //NOLINT (bugprone-exception-escape)

	private:
		template<concepts::builtin_floating_point TFloat>
		friend uint128 internal::make_from_floating_point(TFloat v) noexcept;
		
		static constexpr size_t calculate_hash(int_part hi, int_part low) noexcept;
		static constexpr void hash_combine(size_t& seed, size_t newVal) noexcept;
		static constexpr uint128 make_from_bytes_native(byte_array b) noexcept;
		static constexpr byte_array to_bytes_native(uint128 convert_me) noexcept; //NOLINT ((bugprone-exception-escape)
		inline static uint128 lshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept;
		inline static uint128 rshift_msvc_x64(uint128 shift_me, int shift_amount) noexcept;
		static void best_safe_div_mod(uint128 dividend, uint128 divisor, 
			uint128 * quotient, uint128 * remainder);
		static constexpr void constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
			uint128 * quotient_ret, uint128 * remainder_ret);
		static constexpr void unsafe_constexpr_div_mod_impl(uint128 dividend, uint128 divisor,
			uint128 * quotient_ret, uint128 * remainder_ret) noexcept;
		static constexpr int fls(uint128 n) noexcept;
		inline static void div_mod_msc_x64_impl(uint128 dividend, uint128 divisor,
			uint128 * quotient_ret, uint128 * remainder_ret) noexcept;
		template<typename Char, typename CharTraits = std::char_traits<Char>,
			typename Allocator = std::allocator<Char>>
				requires (!cjm::numerics::concepts::utf_character<Char>)
					&& (cjm::numerics::concepts::char_with_traits_and_allocator<Char, 
							CharTraits, Allocator>)
		static std::basic_string<Char, CharTraits, Allocator>
			to_string(uint128 item, std::ios_base::fmtflags flags);
		template<typename Char, typename CharTraits = std::char_traits<Char>,
			typename Allocator = std::allocator<Char>>
				requires (cjm::numerics::concepts::utf_character<Char>)
					&& (cjm::numerics::concepts::char_with_traits_and_allocator<Char,
							CharTraits, Allocator>)
		static std::basic_string<Char, CharTraits, Allocator> to_string(uint128 item,
			std::ios_base::fmtflags flags);

		fixed_uint_container::uint128_always_split_container_t m_limbs;
	};

	//Ensure compliance with concepts if special development flag is set
	static_assert(!internal::validate_uint128_concept_compliance_dev || concepts::nothrow_convertible<std::array<unsigned char, sizeof(uint128)>, typename uint128::byte_array>);
	static_assert(!internal::validate_uint128_concept_compliance_dev || concepts::cjm_unsigned_integer<uint128>, "Needs to comply with cjm_unsigned_integer concept.");
	static_assert(!internal::validate_uint128_concept_compliance_dev || concepts::integer<uint128>, "Needs to be an integer.");
	static_assert(!internal::validate_uint128_concept_compliance_dev || concepts::printable_subtractable_totally_ordered<uint128>,"Needs to comply with printable_subtractable_totally_ordered.");
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
			 *				  */
			/************************************************************************/

			template<char... Chars>
				requires (sizeof...(Chars) > 0)
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
			class lit_helper final
			{
			
				template<char... Chars>
					requires (sizeof...(Chars) > 0)
				friend constexpr uint128 operator"" _u128();
				

				template<char... Chars>
					requires (sizeof...(Chars) > 0)
				static CJM_LIT_CONST bool are_all_chars_0();

				static CJM_LIT_CONST bool is_legal_hex_char(char c) noexcept;

				static CJM_LIT_CONST bool is_legal_dec_char(char c) noexcept;

				template<char... Chars>
				static CJM_LIT_CONST lit_type get_chars() noexcept;

				template<char... Chars>
				static CJM_LIT_CONST lit_type get_lit_type();

				template<lit_type LiteralType>
					requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
				static CJM_LIT_CONST std::array<std::optional<unsigned short>, 256u> init_digit_lookup();

				template<concepts::unsigned_integer Ui>
				static CJM_LIT_CONST std::array<char, std::numeric_limits<Ui>::digits10 + 1> get_max_decimal();

				template<concepts::unsigned_integer Ui>
				static constexpr size_t max_hex_digits_v = std::numeric_limits<Ui>::digits / 4;

				template<char... Chars>
				static CJM_LIT_CONST std::optional<size_t> count_hex_chars();

				template<concepts::unsigned_integer Ui, char... Chars>
				static CJM_LIT_CONST bool validate_decimal_size();

				template<concepts::unsigned_integer Ui, size_t Digits, lit_type LiteralType, char... Chars>
					requires (sizeof...(Chars) > 0 && sizeof...(Chars) >= Digits && Digits > 0
						&& (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal))
				static CJM_LIT_CONST std::optional<Ui> execute_literal_parse();

				template<char... Chars>
				static CJM_LIT_CONST std::optional<size_t> count_decimal_chars();

			public:

				template<char... Chars>
				static CJM_LIT_CONST std::array<char, sizeof...(Chars)> get_array();

				template <concepts::unsigned_integer Ui, char... Chars>
				static CJM_LIT_CONST std::optional<Ui> parse_literal();

				template<lit_type LiteralType>
					requires (LiteralType == lit_type::Decimal || LiteralType == lit_type::Hexadecimal)
				static constexpr std::array<std::optional<unsigned short>, 256u>  digit_lookup_v
						= init_digit_lookup<LiteralType>();

				template<concepts::unsigned_integer Ui>
				static constexpr std::array<char, std::numeric_limits<Ui>::digits10 + 1> 
					max_decimal_digits_v = get_max_decimal<Ui>();

				lit_helper() = delete;
				~lit_helper() = delete;
				lit_helper(const lit_helper& other) = delete;
				lit_helper(lit_helper&& other) noexcept = delete;
				lit_helper& operator=(const lit_helper& other) = delete;
				lit_helper& operator=(lit_helper&& other) noexcept = delete;
			};			
		}		
	}
}
#include <cjm/numerics/uint128.inl>
#endif
