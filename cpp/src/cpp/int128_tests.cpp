#include "int128_tests.hpp"

namespace
{
    using uint128_t = cjm::numerics::uint128;
    using uint128_ctrl = boost::multiprecision::uint128_t;
    constexpr std::string_view bool_to_yes_no(bool value) noexcept
    {
        using namespace std::string_view_literals;
        return value ? "YES"sv : "NO"sv;
    }
}

cjm::uint128_tests::ctrl_uint128_t cjm::uint128_tests::to_ctrl(uint128_t convert) 
{
    ctrl_uint128_t ret = convert.high_part();
    ret <<= 64;
    ret |= convert.low_part();
    return ret;
}

cjm::uint128_tests::uint128_t cjm::uint128_tests::to_test(const ctrl_uint128_t& convert) 
{
	const std::uint64_t low_part = static_cast<std::uint64_t>(convert);
	const std::uint64_t high_part = static_cast<std::uint64_t>(convert >> 64);
    return uint128_t::make_uint128(high_part, low_part);
}

void cjm::uint128_tests::execute_div_mod_zero_dividend_nonzero_divisor_tests()
{
    constexpr auto dividend = 0_u128;
    constexpr auto divisor = 0xd00d_u128;

    cjm_assert(dividend / divisor == 0_u128);
    cjm_assert(dividend % divisor == 0_u128);
    auto divmod_res = uint128_t::div_mod(dividend, divisor);
    cjm_assert(divmod_res.quotient == 0 && divmod_res.remainder == 0);
    auto res = uint128_t::try_div_mod(dividend, divisor);
    cjm_assert(res.has_value() && res->quotient == 0 && res->remainder == 0);
}

void cjm::uint128_tests::execute_div_mod_by_zero_tests()
{
    using namespace testing;
	
    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto quotient = dividend / divisor;
       std::cerr << "You should never see this value: [" << quotient << "]." << newl;
    });

    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto remainder = dividend % divisor;
        std::cerr << "You should never see this value: [" << remainder << "]." << newl;
    });

    cjm_assert_throws<std::domain_error>([]() -> void
    {
	    const auto dividend = 0xd00d_u128;
	    const auto divisor = 0_u128;
	    const auto result = uint128_t::div_mod(dividend, divisor);
        std::cerr << "You should never see this value: [Quotient: " << result.quotient << "; remainder: " << result.remainder << "]." << newl;
    });
    cjm_assert(uint128_t::try_div_mod(0xd00d_u128, 0_u128) == std::nullopt);
	
}

void cjm::uint128_tests::execute_uint128_tests()
{
    using tconst_prov_t [[maybe_unused]] = u128_testing_constant_providers::testing_constant_provider<uint128_t>;
    static_assert(tconst_prov_t::maximum + tconst_prov_t::one == tconst_prov_t::zero);

    constexpr auto two_fifty_five = 0xff_u128;
	constexpr auto all_at_ends = 0xff00'0000'0000'0000'0000'0000'0000'0000_u128;
    static_assert(((two_fifty_five << (15 * 8)) == all_at_ends) && ((all_at_ends >> (15 * 8)) == two_fifty_five));
    cout << "ENVIRONMENT DATA: " << newl;
    print_constexpr_bitcast_available();
    print_uint128_eval_mode();
    print_cpp20_bitops_available();
    print_builtin_uint128_data_if_present();
    cout << "END ENVIRONMENT DATA" << newl << newl;
    execute_test(execute_basic_test_one, "basic_test_one"sv);
    execute_test(execute_string_parse_test, "string_parse_text"sv);
    execute_test(execute_basic_multiplication_test, "basic_multiplication_test"sv);
    execute_test(test_fls, "test_fls"sv);
    execute_test(execute_builtin_u128fls_test_if_avail, "builtin_u128fls_test_if_avail"sv);
    execute_test(execute_first_bin_op_test, "first_bin_op_test"sv);
    execute_test(execute_div_mod_by_zero_tests, "div_mod_zero_tests"sv);
    execute_test(execute_div_mod_zero_dividend_nonzero_divisor_tests, "div_mod_zero_dividend_nonzero_divisor_tests"sv);
    cout_saver saver{cout};
    cout << "All tests PASSED." << newl;
}

void cjm::uint128_tests::execute_basic_test_one()
{
    constexpr uint128_t big = 0xc0de'd00d'fea2'b00b'c0de'd00d'fea2'b00b_u128;
    cout_saver saver{cout};
    cout << std::hex << "Printing hex: [0x" <<  big << "]." << newl;
    cout << std::dec << "Printing dec: [" << big << "]." << newl;
}


void cjm::uint128_tests::execute_string_parse_test()
{
    constexpr uint128_t test_hex = 0xc0de'd00d'face'cafe'babe'b00b'fea2'dad0_u128;
    constexpr uint128_t test_dec = 256'368'684'942'083'501'355'085'096'987'188'714'192_u128;

    static_assert(test_hex == test_dec, "Values are not the same!");
    cout_saver saver{cout};
    cout << std::hex << "Printing hex: [0x" << test_hex << "]." << newl;
    cout << std::dec << "Printing dec: [" << test_hex << "]." << newl;

    using namespace std;
    using namespace cjm::string;
    using cjm::testing::cjm_assert;
    {
        auto default_strm = make_throwing_sstream<char>();
        auto wide_strm = make_throwing_sstream<wchar_t>();
        auto default_hex_strm = make_throwing_sstream<char>();
        auto wide_hex_strm = make_throwing_sstream<wchar_t>();
//        auto utf8_stream = make_throwing_sstream<char8_t, std::char_traits<char8_t>, std::allocator<char8_t>>();
//        auto utf16_stream = make_throwing_sstream<char16_t>();
//        auto utf32_stream = make_throwing_sstream<char32_t>();

        default_strm << test_dec;
        wide_strm << test_dec;
        default_hex_strm << test_hex;
        wide_hex_strm << test_hex;
    	
        std::string text = default_strm.str();
        std::wstring wtext = wide_strm.str();
        std::string hex_text = default_hex_strm.str();
        std::wstring w_hex_text = wide_hex_strm.str();

    	 	    	
        uint128_t x = uint128_t::make_from_string(std::string_view{text});
        uint128_t y = uint128_t::make_from_string(std::wstring_view{wtext});
        uint128_t z = uint128_t::make_from_string(std::string_view{ hex_text });
    	uint128_t a = uint128_t::make_from_string(std::wstring_view{ w_hex_text });

        cjm_assert(x == test_hex);
        cjm_assert(y == test_hex);
        cjm_assert(z == test_hex);
        cjm_assert(a == test_hex);
        cjm_assert(x == y && y == z && z == a);
    }

    {
        constexpr std::u8string_view u8as_hex = u8"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u8string_view u8as_dec = u8"256368684942083501355085096987188714192"sv;
        constexpr std::u16string_view u16as_hex = u"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u16string_view u16as_dec = u"256368684942083501355085096987188714192"sv;
        constexpr std::u32string_view u32as_hex = U"0xc0ded00dfacecafebabeb00bfea2dad0"sv;
        constexpr std::u32string_view u32as_dec = U"256368684942083501355085096987188714192"sv;

        uint128_t x = test_hex;
            //uint128_t ::make_from_string(u8as_hex);
        uint128_t y = uint128_t ::make_from_string(u8as_dec);
        cjm_assert(x == y && y == test_hex);
        y = uint128_t::make_from_string(u8as_hex);
        cjm_assert(y == x);

    	//todo fixit add hex parse routines for string conversion ... should not be hard ... the literals already do this.
        x = uint128_t::make_from_string(u16as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);
        x = uint128_t::make_from_string(u16as_hex);
        cjm_assert(x == y && y == test_hex && x == test_dec);
    	
        x = uint128_t::make_from_string(u32as_dec);
        y = test_hex;
        cjm_assert(x == y && y == test_hex && x == test_dec);
        x = uint128_t::make_from_string(u32as_hex);
        cjm_assert(x == y && y == test_hex && x == test_dec);
    }



}

void cjm::uint128_tests::execute_basic_multiplication_test()
{
    using cjm::testing::cjm_assert;
    using namespace numerics::uint128_literals;

    auto print_res = [](uint128_t res, std::string_view n) -> void
    {
        auto saver = cout_saver{ cout };
        std::cout << n << " hex: [0x" << std::hex << std::setw(32) << std::setfill('0') << res << "]." << newl;
        std::cout << n << " dec: [" << std::dec << res << "]." << newl;
    };

	constexpr uint128_t zero = 0;
    constexpr uint128_t one = 1;
    constexpr uint128_t max64 = std::numeric_limits<std::uint64_t>::max();
    constexpr uint128_t max64_less_one = max64 - 1;
    constexpr uint128_t fit_64 = 0xdead'beef'b00b'face;
    constexpr uint128_t big_one = 0xc0de'd00d'fea2'cafe'babe'b00b'600d'f00d_u128;
    constexpr uint128_t another_big_one = 0xdead'beef'600d'f00d'badd'd00d'c0de'face_u128;

    constexpr auto zero_times_one = zero * one;
    constexpr auto one_times_one = one * one;
    constexpr auto max64_less_one_times_one = max64_less_one * one;
    static_assert(zero_times_one == 0);
    static_assert(one_times_one == 1);
    static_assert(max64_less_one_times_one == max64_less_one);
	
    constexpr auto max64_times_max64_less_one = max64 * max64_less_one; 
    constexpr auto fit_times_big_one = fit_64 * big_one;
    constexpr auto fit_times_another_big_one = fit_64 * another_big_one;
    constexpr auto big_ones_product = big_one * another_big_one; 
	
    auto test_zero_times_one = zero * one;
    auto test_one_times_one = one * one;
    auto test_max64_less_one_times_one = max64_less_one * one;
    auto test_max64_times_max64_less_one = max64 * max64_less_one;
    auto test_fit_times_big_one = fit_64 * big_one;
    auto test_fit_times_another_big_one = fit_64 * another_big_one;
    auto test_big_ones_product = big_one * another_big_one;

    cjm_assert(test_zero_times_one == zero_times_one);
    cjm_assert(test_one_times_one == one_times_one);
    cjm_assert(test_max64_less_one_times_one == max64_less_one_times_one);
    cjm_assert(test_max64_times_max64_less_one == max64_times_max64_less_one);
    cjm_assert(test_fit_times_big_one == fit_times_big_one);
    cjm_assert(test_fit_times_another_big_one == fit_times_another_big_one);
    cjm_assert(test_big_ones_product == big_ones_product);

    cjm_assert(test_zero_times_one / one == zero);
    cjm_assert(test_one_times_one / one == one);
    cjm_assert(test_max64_times_max64_less_one / max64_less_one == max64 && test_max64_times_max64_less_one / max64 == max64_less_one);


    ctrl_uint128_t ctrl_fit_times_big_one = to_ctrl(test_fit_times_big_one);
    test_interconversion(ctrl_fit_times_big_one, test_fit_times_big_one);
    ctrl_uint128_t ctrl_big_one = to_ctrl(big_one);
    test_interconversion(ctrl_big_one, big_one);
    ctrl_uint128_t ctrl_fit_64 = to_ctrl(fit_64);
    test_interconversion(ctrl_fit_64, fit_64);

    {
        auto save_me = cout_saver{ cout };
        ctrl_uint128_t ctrl_fit_big_div_res = ctrl_fit_times_big_one / ctrl_big_one;

        cjm_assert(to_test(ctrl_fit_times_big_one) == fit_times_big_one && to_test(ctrl_big_one) == big_one);
    	
        cout    << "ctrl_fit_big_div_res:= [0x" << std::hex << std::setw(32)
    			<< std::setfill('0') << ctrl_fit_big_div_res << "]" << newl;
    	
        uint128_t back_test_div_big = to_test(ctrl_fit_big_div_res);
    	cout    << "back_test_div_big := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_big << "]" << newl;

        uint128_t back_test_div_res = fit_times_big_one / big_one;
    	cout    << "back_test_div_res := [0x" << std::hex << std::setw(32)
				<< std::setfill('0') << back_test_div_res << "]" << newl;
    	
    	cjm_assert(back_test_div_big == fit_times_big_one / big_one);
    }

	ctrl_uint128_t ctrl_fit_fit_div_res = ctrl_fit_times_big_one / ctrl_fit_64;
    uint128_t back_test_div_fit = to_test(ctrl_fit_fit_div_res);
    cjm_assert(back_test_div_fit == fit_times_big_one / fit_64);
		
    print_res(test_zero_times_one, "test_zero_times_one"sv);
    print_res(test_one_times_one, "test_one_times_one"sv);
    print_res(test_max64_less_one_times_one, "test_max64_less_one_times_one"sv);
    print_res(test_max64_times_max64_less_one, "test_max64_times_max64_less_one"sv);
    print_res(test_fit_times_big_one, "test_fit_times_big_one"sv);
    print_res(test_fit_times_another_big_one, "test_fit_times_another_big_one"sv);
    print_res(test_big_ones_product, "test_big_ones_product"sv);
	
}

void cjm::uint128_tests::test_fls()
{
    constexpr auto final_test = 0x8000'0000'0000'0000u;
    constexpr auto final_res = cjm::numerics::internal::fls_default(final_test);
    constexpr auto numbers = get_pow2_arr();
    constexpr auto results = get_pow2_res_arr();
    static_assert(numbers.size() == results.size());
    static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<uint128_t>);
    using namespace cjm::numerics::uint128_literals;
    static_assert(cjm::numerics::most_sign_set_bit(3_u128) == 1);
    static_assert(cjm::numerics::most_sign_set_bit(3_u128 << 120) == 121);
    auto saver = cout_saver{ cout };
	for (auto i = 0_szt; i < numbers.size(); ++i)
	{
        cjm_assert(cjm::numerics::internal::fls_int_part(numbers[i]) == results[i]);
        cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << numbers[i] << "], the result is: [" << std::dec << results[i] << "]." << newl;
	}
    cjm_assert(cjm::numerics::internal::fls_int_part(final_test) == final_res);
    cout << "For [0x" << std::hex << std::setw(16) << std::setfill('0') << final_test << "], the result is: [" << std::dec << final_res << "]." << newl;
}

void cjm::uint128_tests::test_interconversion(const ctrl_uint128_t& control, uint128_t test)
{
	const uint128_t ctrl_to_test = to_test(control);
	const ctrl_uint128_t test_to_control = to_ctrl(test);
    cjm::testing::cjm_assert(test_to_control == control && ctrl_to_test == test);
}


void cjm::uint128_tests::print_uint128_eval_mode()
{
    using namespace numerics;
    cout_saver o_saver{cout};
    std::string_view eval_text = "Unknown/Unrecognized Arithmetic Mode"sv;
    switch (cjm::numerics::calculation_mode)
    {
        case uint128_calc_mode::intrinsic_u128:
            eval_text = "Built-in unsigned __int128 Arithmetic"sv;
            break;
        case uint128_calc_mode::msvc_x64:
            eval_text = "Microsoft x64 Compiler-Intrinsic Optimized Arithmetic"sv;
            break;
        case uint128_calc_mode::default_eval:
            eval_text = "Fallback Arithmetic"sv;
            break;
    }
    cout << "Arithmetic mode: [" << eval_text << "]." << newl;
}
void cjm::uint128_tests::print_cpp20_bitops_available()
{
    constexpr auto avail_text = bool_to_yes_no(cjm::numerics::has_cpp20_bitops);
    using namespace numerics;
    cout_saver o_saver{cout};
    cout << "Are C++20 bit operations available?: ["sv << avail_text << "]."sv << newl;
}
void cjm::uint128_tests::print_constexpr_bitcast_available()
{
    constexpr auto avail_text = bool_to_yes_no(cjm::numerics::constexpr_bit_casting);
    using namespace numerics;
    cout_saver o_saver{cout};
    cout << "Is constexpr bitcasting available?: [" << avail_text << "]." << newl;
}

void cjm::uint128_tests::print_builtin_uint128_data_if_present()
{
    if constexpr (cjm::numerics::calculation_mode == numerics::uint128_calc_mode::intrinsic_u128)
    {
        constexpr auto arithmetic_text = bool_to_yes_no(std::is_arithmetic_v<cjm::numerics::natuint128_t>);
        constexpr auto integral_text = bool_to_yes_no(std::is_integral_v<cjm::numerics::natuint128_t>);
        constexpr auto unsigned_text = bool_to_yes_no(std::is_unsigned_v<cjm::numerics::natuint128_t>);

        cout << "Is unsigned __int128 an arithmetic type?: [" << arithmetic_text << "]." << newl;
        cout << "Is unsigned __int128 an integral type?: [" << integral_text << "]." << newl;
        cout << "Is unsigned __int128 an unsigned type?: [" << unsigned_text << "]." << newl;
        cout << "sizeof(std::uintmax_t): [" << sizeof(std::uintmax_t) << "]." << newl;
    }
    else
    {
        cout << "No built-in  uint128 data available." << newl;
    }
}
void cjm::uint128_tests::execute_first_bin_op_test()
{
    auto op = binary_operation{ binary_op::add, 1_u128, 2_u128 };
    op.calculate_result();
    cjm_assert(op.has_correct_result());
    const auto& res = op.result();
    cjm_assert(res.value().first == res.value().second);
}

#ifdef CJM_HAVE_BUILTIN_128
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
    static_assert(cjm::numerics::concepts::can_find_most_significant_set_bitpos<unsigned __int128>);
    unsigned __int128 x = 3;
    auto most_sign_bit = cjm::numerics::most_sign_set_bit(x);
    std::cout << "Test for 3 yields bitpos: [" << most_sign_bit << "]." << newl;
    cjm_assert(most_sign_bit == 1);
    x <<= 120;
    most_sign_bit = cjm::numerics::most_sign_set_bit(x);
    std::cout << "Test for (3 << 120) yields: [" << most_sign_bit << "]." << newl;
    cjm_assert(most_sign_bit == 121);

}
#else
void cjm::uint128_tests::execute_builtin_u128fls_test_if_avail()
{
    std::cout << "Will not test builtin_u128_fls because not available in this enviornment." << newl;
}


#endif



