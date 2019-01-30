#include <boost/test/unit_test.hpp>
#include <cstdio>

// Needed by infra/assertion.hpp
// If TEST_INFRA_ASSERTION is defined:
//   Assertion error won't terminate current program;
//   Just increase __assertion_error_count.
// Otherwise,
//   The program is terminated.
#define TEST_INFRA_ASSERTION
extern "C" {
    size_t __assertion_error_count = 0;
};

#include <infra.hpp>


BOOST_AUTO_TEST_SUITE(testsuite_infra)

    BOOST_AUTO_TEST_CASE(test_assertion)
    {
#define TEST_SHOULD_SUCCEED(...) \
        do { \
            const size_t old = __assertion_error_count; \
            { __VA_ARGS__; } \
            BOOST_TEST(__assertion_error_count == old); \
        } while(false)

#define TEST_SHOULD_FAIL(...) \
        do { \
            const size_t old = __assertion_error_count; \
            { __VA_ARGS__; } \
            BOOST_TEST(__assertion_error_count == old + 1); \
        } while(false)

        LOG_INFO("Currently performing test_assertion...");
        LOG_INFO("There will be some warning/error/fatal/panic messages.");
        LOG_INFO("THIS IS INTENDED!");

        LOG_FATAL("fatal log here");
        LOG_ERROR("error log here");
        LOG_WARN("warn log here");
        LOG_INFO("info log here");
        LOG_DBG1("dbg1 log here: %d", 123);
        LOG_DBG2("dbg2 log here: %d", 123);
        LOG_DBG3("dbg3 log here: %d", 123);
        LOG_DBG4("dbg4 log here: %d %s", 123, "hahahy");
        LOG_DBG5("dbg5 log here: %d %s", 123, "hahahx");

        TEST_SHOULD_FAIL(ASSERT(false));
        TEST_SHOULD_FAIL(ASSERT(0, "Wow!"));
        TEST_SHOULD_FAIL(ASSERT(2 > 8, "Wow! %s", "str"));
        TEST_SHOULD_FAIL(ASSERT(5 == 9, "Wow! %d %s", 325, "str"));

        TEST_SHOULD_SUCCEED(ASSERT(true));
        TEST_SHOULD_SUCCEED(ASSERT(1, "Wow!"));
        TEST_SHOULD_SUCCEED(ASSERT(-1, "Wow! %s", "str"));
        TEST_SHOULD_SUCCEED(ASSERT(528 > 432, "Wow! %d %s", 325, "str"));
        TEST_SHOULD_SUCCEED(ASSERT(84 <= 84, "Wow! %d %s", 325, "str"));

        TEST_SHOULD_SUCCEED(ASSERT_IS_TRUE(true));
        TEST_SHOULD_SUCCEED(ASSERT_IS_FALSE(false));
        TEST_SHOULD_SUCCEED(ASSERT_IS_NULL(nullptr, "Wow! %s", "str"));
        TEST_SHOULD_SUCCEED(ASSERT_NOT_NULL((const void*)"whatever", "Wow! %s", "str"));

        TEST_SHOULD_FAIL(ASSERT_IS_TRUE(!true));
        TEST_SHOULD_FAIL(ASSERT_IS_FALSE(!false));
        TEST_SHOULD_FAIL(ASSERT_IS_NULL((const void*)"whatever", "Wow! %s", "str"));
        TEST_SHOULD_FAIL(ASSERT_NOT_NULL(nullptr, "Wow! %s", "str"));

        TEST_SHOULD_FAIL(ASSERT_BUG());
        TEST_SHOULD_FAIL(ASSERT_BUG("With message"));
        TEST_SHOULD_FAIL(ASSERT_BUG("With message and %d", 123));
        TEST_SHOULD_FAIL(ASSERT_BUG("With message and %d, %s", 123, "string!"));


        TEST_SHOULD_FAIL(ASSERT_LT(5, 3));
        TEST_SHOULD_FAIL(ASSERT_LT(5, 3, "This is message"));
        TEST_SHOULD_FAIL(ASSERT_LT(5, 3, "This is message: %d", 42));
        TEST_SHOULD_FAIL(ASSERT_LT(5, 3, "This is message: %d %d", 42, 4242));

        TEST_SHOULD_SUCCEED(ASSERT_LT(8, 10));
        TEST_SHOULD_SUCCEED(ASSERT_LE(5.0, 5.0, "This is message"));
        TEST_SHOULD_SUCCEED(ASSERT_GT(5, 3, "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_GE(3, 3, "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_EQ(true, true, "This is message: %d %d", 42, 4242));
        TEST_SHOULD_SUCCEED(ASSERT_NE(false, true, "This is message: %d %d", 42, 4242));

        TEST_SHOULD_FAIL(ASSERT_STR_LT("b", "a"));
        TEST_SHOULD_FAIL(ASSERT_STR_LT("b", "", "This is message"));
        TEST_SHOULD_FAIL(ASSERT_STR_LT("bc", "b", "This is message: %d", 42));
        TEST_SHOULD_FAIL(ASSERT_STR_LT("b", "b", "This is message: %d %d", 42, 4242));

        TEST_SHOULD_SUCCEED(ASSERT_STR_LT("a", "b"));
        TEST_SHOULD_SUCCEED(ASSERT_STR_LE("asdf", "asdf", "This is message"));
        TEST_SHOULD_SUCCEED(ASSERT_STR_GT("a", "", "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_STR_GE("abc", "abc", "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_STR_EQ("what", "what", "This is message: %d %d", 42, 4242));
        TEST_SHOULD_SUCCEED(ASSERT_STR_NE("what", "what!", "This is message: %d %d", 42, 4242));

        TEST_SHOULD_FAIL(ASSERT_STR_LT(std::string("b"), "b"));
        TEST_SHOULD_FAIL(ASSERT_STR_LE("asdfe", std::string("asdf"), "This is message"));
        TEST_SHOULD_FAIL(ASSERT_STR_GT("", (const char*)"", "This is message: %d", 42));
        TEST_SHOULD_FAIL(ASSERT_STR_GE((const char*)"abc", std::string("abcd"), "This is message: %d", 42));
        TEST_SHOULD_FAIL(ASSERT_STR_EQ(remotecc::infra::make_string("wh") + "a" + 't', std::string("what???"),
                                          "This is message: %d %d", 42, 4242));
        TEST_SHOULD_FAIL(ASSERT_STR_NE("what", remotecc::infra::make_string("what"),
                                          "This is message: %d %d", 42, 4242));

        TEST_SHOULD_SUCCEED(ASSERT_STR_LT(std::string("a"), "b"));
        TEST_SHOULD_SUCCEED(ASSERT_STR_LE("asdf", std::string("asdf"), "This is message"));
        TEST_SHOULD_SUCCEED(ASSERT_STR_GT("a", (const char*)"", "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_STR_GE((const char*)"abc", "abc", "This is message: %d", 42));
        TEST_SHOULD_SUCCEED(ASSERT_STR_EQ(remotecc::infra::make_string("wh") + "a" + 't', std::string("what"),
                                          "This is message: %d %d", 42, 4242));
        TEST_SHOULD_SUCCEED(ASSERT_STR_NE("what", remotecc::infra::make_string("what!"),
                                          "This is message: %d %d", 42, 4242));

    }

BOOST_AUTO_TEST_SUITE_END()
