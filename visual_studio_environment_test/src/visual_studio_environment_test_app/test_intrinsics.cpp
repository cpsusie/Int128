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
#include "test_intrinsics.hpp"


#ifdef CJM_CLANG_TEST_INTRINSICS_AVAILABLE
void cjm::clang::test_program::intrinsics_test::print_intrinsic_availability_info()
{
	constexpr auto newl = '\n';
	const std::uint64_t first_operand = 0xc0de'd00d'fea2'b00b;
	const std::uint64_t second_operand = 0xdead'beef'600d'f00d;
	const std::uint64_t third_operand = 0x01ff'ffff'ffff'ffff;
	std::cout << "Printing results of intrinsics test .... " << newl;
	
	unsigned long bsrev_out = 0;
	unsigned char result = CJM_TEST_BITSCAN_REV_64(&bsrev_out, first_operand);
	if (result)
	{
		std::cout << "Bitscan rev intrin result: [" << bsrev_out << "]." << newl;
	}
	else
	{
		std::cout << "Bitscan rev intrin result found no set bits." << newl;
	}
	unsigned long long high_result = 0;
	auto low_result = CJM_TEST_UMUL128(first_operand, second_operand, &high_result);
	std::cout << "Umul result is high: [0x" << std::hex << std::setw(16) << std::setfill('0') << high_result << "]; low result: [0x" << std::hex << std::setw(16) << std::setfill('0') << low_result << "]." << newl;

	auto res = CJM_TEST_LZCNT_64(third_operand);
	std::cout << "Lzcount test result: [" << std::dec << res << "]." << newl;

	//This doesn't seem to work in clang!
	/*res = CJM_TEST_UDIV128(first_operand, second_operand, third_operand, &high_result);
	std::cout << "Udiv quot: [0x" << std::hex << std::setw(16) << std::setfill('0') << res << "]; remainder: [0x" << std::hex << std::setw(16) << std::setfill('0') << high_result << "]." << newl; */

	res = CJM_TEST_LSHIFT128(first_operand, second_operand, 63);
	std::cout << "Lshift128 res: [0x" << std::hex << std::setw(16) << std::setfill('0') << res << "]." << newl;

	res = CJM_TEST_RSHIFT128(first_operand, second_operand, 63);
	std::cout << "Rshift128 res: [0x" << std::hex << std::setw(16) << std::setfill('0') << res << "]." << newl;
	
	unsigned char carry_out = CJM_TEST_ADDCARRY64(0, first_operand, second_operand, &high_result);
	std::cout << "Adc res: [0x" << std::hex << std::setw(16) << std::setfill('0') << high_result << "]; carry_out: [" << std::dec << +carry_out << "]." << newl;

	carry_out = CJM_TEST_SUBBORROW_64(0, second_operand, first_operand, &high_result);
	std::cout << "Sub-borrow res: [0x" << std::hex << std::setw(16) << std::setfill('0') << high_result << "]; carry_out: [" << std::dec << +carry_out << "]." << newl;
	std::cout << "END results of intrinsics test." << newl;	
}
#else
void cjm::clang::test_program::intrinsics_test::print_intrinsic_availability_info()
{
	constexpr auto newl = '\n';
	std::cout << "Intrinsics not available in this environment." << newl;
}
#endif