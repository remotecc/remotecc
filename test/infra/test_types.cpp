#include <boost/test/unit_test.hpp>
#include <infra.hpp>
#include <array>
#include <string>


BOOST_AUTO_TEST_SUITE(testsuite_infra)

    BOOST_AUTO_TEST_CASE(test_types)
    {
        using namespace remotecc;
        using namespace remotecc::infra;

        BOOST_STATIC_ASSERT(length_of_array("") == 1);
        BOOST_STATIC_ASSERT(length_of_array("\0") == 2);
        BOOST_STATIC_ASSERT(length_of_array("normal") == 7);
        BOOST_STATIC_ASSERT(length_of_array({1, 2, 3}) == 3);
        BOOST_STATIC_ASSERT(length_of_array(std::array<int, 5>()) == 5);

        BOOST_STATIC_ASSERT(length_of_cstr("") == 0);
        BOOST_STATIC_ASSERT(length_of_cstr("\0") == 1);
        BOOST_STATIC_ASSERT(length_of_cstr("normal") == 6);
        BOOST_STATIC_ASSERT(length_of_cstr("ev\0il") == 5);

        BOOST_STATIC_ASSERT(is_raw_type<int>());
        BOOST_STATIC_ASSERT(is_raw_type<unsigned long>());
        BOOST_STATIC_ASSERT(is_raw_type<char>());
        BOOST_STATIC_ASSERT(is_raw_type<bool>());
        BOOST_STATIC_ASSERT(is_raw_type<short*>());
        BOOST_STATIC_ASSERT(is_raw_type<const short*>());
        BOOST_STATIC_ASSERT(is_raw_type<std::string>());
        BOOST_STATIC_ASSERT(is_raw_type<std::array<int, 123>>());
        BOOST_STATIC_ASSERT(!is_raw_type<const int>());
        BOOST_STATIC_ASSERT(!is_raw_type<volatile unsigned long>());
        BOOST_STATIC_ASSERT(!is_raw_type<const volatile char>());
        BOOST_STATIC_ASSERT(!is_raw_type<volatile const bool>());
        BOOST_STATIC_ASSERT(!is_raw_type<short* const>());
        BOOST_STATIC_ASSERT(!is_raw_type<const short* const>());
        BOOST_STATIC_ASSERT(!is_raw_type<std::string&>());
        BOOST_STATIC_ASSERT(!is_raw_type<std::array<int, 123>&&>());

        BOOST_STATIC_ASSERT(std::is_same<raw_type<const int>, int>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<volatile unsigned long>, unsigned long>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<const volatile char>, char>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<volatile const bool>, bool>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<short* const>, short*>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<const short* const>, const short*>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<std::string&>, std::string>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<std::array<int, 123>&&>, std::array<int, 123>>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<const std::array<int, 123>&&>, std::array<int, 123>>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<volatile std::array<int, 123>&&>, std::array<int, 123>>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<volatile const std::array<int, 123>&&>, std::array<int, 123>>());
        BOOST_STATIC_ASSERT(std::is_same<raw_type<const std::array<int, 123>&>, std::array<int, 123>>());
    }

BOOST_AUTO_TEST_SUITE_END()
