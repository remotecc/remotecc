#include <boost/test/unit_test.hpp>
#include <infra.hpp>

#if BOOST_COMP_MSVC || BOOST_COMP_MSVC_EMULATED
// C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning (disable: 4146)
#endif

BOOST_AUTO_TEST_SUITE(testsuite_infra)

    BOOST_AUTO_TEST_CASE(test_string)
    {
        using namespace remotecc;
        using namespace remotecc::infra;


        static constexpr const auto str = remotecc::infra::make_string("abc") + remotecc::infra::make_string("def");
        BOOST_STATIC_ASSERT(str == "abcdef");
        BOOST_STATIC_ASSERT(str.length() == 6);
        BOOST_STATIC_ASSERT(str > "abc");
        BOOST_STATIC_ASSERT(str >= "abcdef");
        BOOST_STATIC_ASSERT(str.find_first_of("xyad") == 0);
        BOOST_STATIC_ASSERT(str.find_first_of("xyde") == 3);
        BOOST_STATIC_ASSERT(str.find_first_of("xy") == (size_t)-1);
        BOOST_STATIC_ASSERT(str.find_last_of("xyad") == 3);
        BOOST_STATIC_ASSERT(str.find_last_of("xya!") == 0);
        BOOST_STATIC_ASSERT(str.find_last_of("xy") == (size_t)-1);
        BOOST_STATIC_ASSERT(str.find_last_of("\0") == (size_t)-1);
        BOOST_STATIC_ASSERT(str.contains_no_null_char());
        BOOST_STATIC_ASSERT(!make_string("abcx\0yz").contains_no_null_char());

        BOOST_STATIC_ASSERT(str.sub_string<1>() == "bcdef");
        BOOST_STATIC_ASSERT(str.sub_string<3>() == "def");
        BOOST_STATIC_ASSERT(str.sub_string<0>() == "abcdef");
        BOOST_STATIC_ASSERT(str.sub_string<6>() == "");
        BOOST_STATIC_ASSERT(str.sub_string<0, 0>() == "");
        BOOST_STATIC_ASSERT(str.sub_string<1, 0>() == "");
        BOOST_STATIC_ASSERT(str.sub_string<6, 0>() == "");
        BOOST_STATIC_ASSERT(str.sub_string<0, 1>() == "a");
        BOOST_STATIC_ASSERT(str.sub_string<0, 6>() == "abcdef");
        BOOST_STATIC_ASSERT(str.sub_string<1, 4>() == "bcde");
        BOOST_STATIC_ASSERT(str.sub_string<5, 1>() == "f");

        BOOST_STATIC_ASSERT(make_string("abc") == "abc");
        BOOST_STATIC_ASSERT(make_string("abc") + "d" == "abcd");
        BOOST_STATIC_ASSERT(make_string("abc") + "de" == "abcde");
        BOOST_STATIC_ASSERT(make_string("abc") + 'd' == "abcd");
        BOOST_STATIC_ASSERT(make_string("abc") + '\0' == "abc\0");
        BOOST_STATIC_ASSERT(make_string('a') == "a");
        BOOST_STATIC_ASSERT(make_string('\0') == "\0");
        BOOST_STATIC_ASSERT(make_string('\0').length() == 1);

        BOOST_STATIC_ASSERT(make_string("abc").pad_left<3>('_') == "abc");
        BOOST_STATIC_ASSERT(make_string("abc").pad_left<5>('_') == "__abc");
        BOOST_STATIC_ASSERT(make_string("abc").pad_right<3>('_') == "abc");
        BOOST_STATIC_ASSERT(make_string("abc").pad_right<5>('_') == "abc__");


        BOOST_STATIC_ASSERT(integer_to_string<int, 0>() == "0");
        BOOST_STATIC_ASSERT(integer_to_string<int, 1>() == "1");
        BOOST_STATIC_ASSERT(integer_to_string<int, -1>() == "-1");
        BOOST_STATIC_ASSERT(integer_to_string<std::int32_t, 0xffffffff, 16>() == "-1");
        BOOST_STATIC_ASSERT(integer_to_string<std::int32_t, 0x7fffffff, 16>() == "7fffffff");
        BOOST_STATIC_ASSERT(integer_to_string<std::int32_t, 0x80000000, 16>() == "-80000000");

        BOOST_STATIC_ASSERT(integer_to_string<std::uint32_t, 0x0, 16>() == "0");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint32_t, 0x1, 16>() == "1");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint32_t, -1, 16>() == "ffffffff");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint32_t, -2, 16>() == "fffffffe");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint32_t, 0x80000000, 16>() == "80000000");

        BOOST_STATIC_ASSERT(integer_to_string<std::uint64_t, 0, 16>() == "0");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint64_t, 0x8000000000000000, 16>() == "8000000000000000");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint64_t, -0x8000000000000000, 16>() == "8000000000000000");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint64_t, 0x7fffffffffffffff, 16>() == "7fffffffffffffff");
        BOOST_STATIC_ASSERT(integer_to_string<std::uint64_t, -0x7fffffffffffffff, 16>() == "8000000000000001");
    }

BOOST_AUTO_TEST_SUITE_END()
