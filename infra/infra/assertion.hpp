#if !defined(_INFRA_ASSERTION_HPP_INCLUDED_)
#define _INFRA_ASSERTION_HPP_INCLUDED_

#include "./string.hpp"
#include "./logging.hpp"

#include <exception>
#include <inttypes.h>
#include <cstring>
#include <string>

namespace remotecc
{
    namespace infra
    {
        namespace details
        {
#define _REMOTECC_INFRA_DECLARE_COMPARER(_OpName_, _Op_) \
            template<typename T1, typename T2> \
            struct general_comparer_##_OpName_ \
            { \
                static constexpr bool compare( \
                    const raw_type<T1>& left, \
                    const raw_type<T2>& right \
                    ) noexcept \
                { \
                    return left _Op_ right; \
                } \
            }; \
            \
            template<typename T1, typename T2> \
            struct string_comparer_##_OpName_ \
            { \
                template<size_t _Size> \
                static const char* cstr(const string<_Size, char>& s) noexcept { return s.value; } \
                template<size_t _Size> \
                static const char* cstr(const char(&s)[_Size]) noexcept { return s; } \
                static const char* cstr(const char* s) noexcept { return s; } \
                static const char* cstr(const std::string& s) noexcept { return s.c_str(); } \
                static bool compare( \
                    const typename raw_type<array_ref_to_ptr<T1>>::type& left, \
                    const typename raw_type<array_ref_to_ptr<T2>>::type& right \
                    ) noexcept \
                { \
                    return strcmp(cstr(left), cstr(right)) _Op_ 0; \
                } \
            }
            _REMOTECC_INFRA_DECLARE_COMPARER(LT, <);
            _REMOTECC_INFRA_DECLARE_COMPARER(LE, <=);
            _REMOTECC_INFRA_DECLARE_COMPARER(GT, >);
            _REMOTECC_INFRA_DECLARE_COMPARER(GE, >=);
            _REMOTECC_INFRA_DECLARE_COMPARER(EQ, ==);
            _REMOTECC_INFRA_DECLARE_COMPARER(NE, !=);
#undef _REMOTECC_INFRA_DECLARE_COMPARER


            template<typename T>
            struct printf_formatter : printf_formatter<raw_type<T>>
            { };

#define _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(Type, StrFormat, /*template*/...) \
            template<__VA_ARGS__> \
            struct printf_formatter<Type> \
            { \
                static constexpr const char value[] = (StrFormat); \
            }; \
            \
            template<__VA_ARGS__> \
            struct printf_formatter<const Type> \
            { \
                static constexpr const char value[] = (StrFormat); \
            }
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::uint16_t, "%" PRIu16);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::uint32_t, "%" PRIu32);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::uint64_t, "%" PRIu64);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::int16_t, "%" PRIi16);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::int32_t, "%" PRIi32);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::int64_t, "%" PRIi64);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(float, "%f");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(double, "%lf");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(char, "%c");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(char*, "%s");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(std::string, "%s");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(void*, "%p");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(bool, "%s");
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(char[_Size], "%s", size_t _Size);
            _REMOTECC_INFRA_DECLARE_TYPE_PRINTF(JUST(string<_Size, char>), "%s", size_t _Size);
#undef _REMOTECC_INFRA_DECLARE_TYPE_PRINTF


            struct printf_transformer
            {
                template<typename T>
                static constexpr
                T* trans(T* const v)
                {
                    return v;
                }

                template<typename T>
                static constexpr
                typename std::enable_if<
                    std::is_fundamental<raw_type<T>>::value &&
                        !std::is_same<raw_type<T>, bool>::value,
                    const raw_type<T>&>::type
                trans(const T& v)
                {
                    return v;
                }

                template<typename T>
                static constexpr
                typename std::enable_if<std::is_same<raw_type<T>, bool>::value, const char*>::type
                trans(const T& v)
                {
                    return v ? "true" : "false";
                }

                template<typename TStdString>
                static constexpr
                typename std::enable_if<std::is_same<std::string, raw_type<TStdString>>::value, const char*>::type
                trans(const TStdString& v)
                {
                    return v.c_str();
                }

                template<size_t _Size>
                static constexpr
                const char* trans(const string<_Size, char>& v)
                {
                    return v.value;
                }
            };

#ifdef TEST_INFRA_ASSERTION   // See test/infra/test_assertion.cpp for details
            extern "C" {
                extern size_t __assertion_error_count;  // declared in test/infra/test_assertion.cpp
            };
            inline void force_terminate() noexcept
            {
                ++__assertion_error_count;
            }
#else
            [[noreturn]]
            inline void force_terminate()
            {
                std::terminate();
            }
#endif

        }  // namespace details

    }  // namespace infra
}  // namespace remotecc



#define _REMOTECC_INFRA_ASSERT_1(_Name_, _What_, _StrWhat_, _UserMsg_, ...) \
    do { \
        auto _value_ = (_What_); \
        if (!(_value_)) \
        { \
            ::remotecc::infra::simple_date_time_t _now_ { }; \
            (void)now_date_time(&_now_); \
            static constexpr const auto _message_ = ::remotecc::infra::make_string("") + \
                "[%04u-%02u-%02u %02u:%02u:%02u] [PANIC] " + \
                "[" + INFRA_CURRENT_FILE + ":" + INFRA_CURRENT_LINE + "] " + \
                _Name_ + "(" + _StrWhat_ + ") failed. " + \
                _UserMsg_ + "\n"; \
            /* For MSVC, we don't need to "EXPAND" __VA_ARGS__ here, */ \
            /* even if with its strange behavior with __VA_ARGS__. Don't now why. */ \
            /* See: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly */ \
            fprintf(::remotecc::infra::details::__logging_file, _message_.value, \
                _now_.year, _now_.month, _now_.day_of_month, \
                _now_.hour, _now_.minute, _now_.second, \
                ##__VA_ARGS__); \
            ::remotecc::infra::details::force_terminate(); \
        } \
    } while(false)


#if INFRA_IDE_MODE

#define ASSERT(What, ...)           printf("" __VA_ARGS__)
#define ASSERT_IS_NULL(What, ...)   printf("" __VA_ARGS__)
#define ASSERT_NOT_NULL(What, ...)  printf("" __VA_ARGS__)
#define ASSERT_IS_TRUE(What, ...)   printf("" __VA_ARGS__)
#define ASSERT_IS_FALSE(What, ...)  printf("" __VA_ARGS__)
#define ASSERT_BUG(...)             printf("" __VA_ARGS__)

#else  // INFRA_IDE_MODE

/* Use JUST to work around MSVC's strange behavior with __VA_ARGS__. */ \
/* See: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly */ \
#define ASSERT(What, ...)           JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT", What, #What, "" __VA_ARGS__))
#define ASSERT_IS_NULL(What, ...)   JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT_IS_NULL", ((What) == nullptr), #What, "" __VA_ARGS__))
#define ASSERT_NOT_NULL(What, ...)  JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT_NOT_NULL", ((What) != nullptr), #What, "" __VA_ARGS__))
#define ASSERT_IS_TRUE(What, ...)   JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT_TRUE", ((What) == true), #What, "" __VA_ARGS__))
#define ASSERT_IS_FALSE(What, ...)  JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT_FALSE", ((What) == false), #What, "" __VA_ARGS__))
#define ASSERT_BUG(...)             JUST(_REMOTECC_INFRA_ASSERT_1("ASSERT_BUG", false, "", "Should not reach here! " __VA_ARGS__))

#endif  // INFRA_IDE_MODE


#define _REMOTECC_INFRA_ASSERT_COMPARE_2(Name, Comp, Left, Right, StrLeft, StrRight, UserMsg, ...) \
    do { \
        auto left_value = (Left); \
        auto right_value = (Right); \
        if (!(::remotecc::infra::details::Comp<decltype(Left), decltype(Right)> \
            ::compare(left_value, right_value))) \
        { \
            ::remotecc::infra::simple_date_time_t now { }; \
            (void)now_date_time(&now); \
            static constexpr const auto message = ::remotecc::infra::make_string("") + \
                "[%04u-%02u-%02u %02u:%02u:%02u] [PANIC] " + \
                "[" + INFRA_CURRENT_FILE + ":" + INFRA_CURRENT_LINE + "] " + \
                Name + "(" + StrLeft + ", " + StrRight + ") failed. " + \
                "LHS: " + StrLeft + " = " + ::remotecc::infra::details::printf_formatter<decltype(Left)>::value + ", " + \
                "RHS: " + StrRight + " = " + ::remotecc::infra::details::printf_formatter<decltype(Right)>::value + ". " + \
                UserMsg + "\n"; \
            /* For MSVC, we don't need to "EXPAND" __VA_ARGS__ here, */ \
            /* even if with its strange behavior with __VA_ARGS__. Don't now why. */ \
            /* See: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly */ \
            fprintf(::remotecc::infra::details::__logging_file, message.value, \
                now.year, now.month, now.day_of_month, now.hour, now.minute, now.second, \
                ::remotecc::infra::details::printf_transformer::trans(left_value), \
                ::remotecc::infra::details::printf_transformer::trans(right_value), \
                ##__VA_ARGS__); \
            ::remotecc::infra::details::force_terminate(); \
        } \
    } while(false)


#if INFRA_IDE_MODE

#define ASSERT_LT(Left, Right, ...)     printf("" __VA_ARGS__)
#define ASSERT_LE(Left, Right, ...)     printf("" __VA_ARGS__)
#define ASSERT_GT(Left, Right, ...)     printf("" __VA_ARGS__)
#define ASSERT_GE(Left, Right, ...)     printf("" __VA_ARGS__)
#define ASSERT_EQ(Left, Right, ...)     printf("" __VA_ARGS__)
#define ASSERT_NE(Left, Right, ...)     printf("" __VA_ARGS__)

#define ASSERT_STR_LT(Left, Right, ...) printf("" __VA_ARGS__)
#define ASSERT_STR_LE(Left, Right, ...) printf("" __VA_ARGS__)
#define ASSERT_STR_GT(Left, Right, ...) printf("" __VA_ARGS__)
#define ASSERT_STR_GE(Left, Right, ...) printf("" __VA_ARGS__)
#define ASSERT_STR_EQ(Left, Right, ...) printf("" __VA_ARGS__)
#define ASSERT_STR_NE(Left, Right, ...) printf("" __VA_ARGS__)

#else  // INFRA_IDE_MODE

/* Use JUST to work around MSVC's strange behavior with __VA_ARGS__. */ \
/* See: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly */ \
#define ASSERT_LT(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_LT", general_comparer_LT, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_LE(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_LE", general_comparer_LE, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_GT(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_GT", general_comparer_GT, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_GE(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_GE", general_comparer_GE, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_EQ(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_EQ", general_comparer_EQ, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_NE(Left, Right, ...)     JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_NE", general_comparer_NE, Left, Right, #Left, #Right, "" __VA_ARGS__))

#define ASSERT_STR_LT(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_LT", string_comparer_LT, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_STR_LE(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_LE", string_comparer_LE, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_STR_GT(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_GT", string_comparer_GT, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_STR_GE(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_GE", string_comparer_GE, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_STR_EQ(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_EQ", string_comparer_EQ, Left, Right, #Left, #Right, "" __VA_ARGS__))
#define ASSERT_STR_NE(Left, Right, ...) JUST(_REMOTECC_INFRA_ASSERT_COMPARE_2("ASSERT_STR_NE", string_comparer_NE, Left, Right, #Left, #Right, "" __VA_ARGS__))

#endif  // INFRA_IDE_MODE


#endif  // !defined(_INFRA_ASSERTION_HPP_INCLUDED_)
