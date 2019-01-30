#if !defined(_INFRA_LOGGING_HPP_INCLUDED_)
#define _INFRA_LOGGING_HPP_INCLUDED_

#include "./datetime.hpp"
#include "./string.hpp"
#include <cstdio>

namespace remotecc
{
    namespace infra
    {
        namespace details
        {
            extern FILE* __logging_file;  // from ./infra.cpp
            extern int __logging_verbosity;  // from ./infra.cpp

            /**
             * NOTE: These variables are used to extract relative paths from absolute paths.
             * For example, for path
             *   "/what/ever/infra/infra/logging.hpp"
             * we only want the "relative" part:
             *   "infra/infra/logging.hpp"
             *
             * Thus, if the location of current file (logging.hpp) is changed,
             * the code below should change respectively.
             *
             * Macros:
             *   INFRA_CURRENT_FILE: relative path of current file (type: char[N]).
             *   INFRA_CURRENT_LINE: current line (type: char[N]).
             */
            static constexpr const size_t __source_dir_prefix_length_fname = make_string(__FILE__).find_last_of("\\/");
            BOOST_STATIC_ASSERT(__source_dir_prefix_length_fname != (size_t)-1);
            BOOST_STATIC_ASSERT(__source_dir_prefix_length_fname > 0);

            static constexpr const size_t __source_dir_prefix_length_infra =
                make_string(__FILE__).find_last_of("\\/", __source_dir_prefix_length_fname - 1);
            BOOST_STATIC_ASSERT(__source_dir_prefix_length_infra != (size_t)-1);
            BOOST_STATIC_ASSERT(__source_dir_prefix_length_infra > 0);

            static constexpr const size_t __source_dir_prefix_length_src =
                make_string(__FILE__).find_last_of("\\/", __source_dir_prefix_length_infra - 1);
            BOOST_STATIC_ASSERT(__source_dir_prefix_length_src != (size_t)-1);

#define INFRA_CURRENT_FILE \
            (::remotecc::infra::make_string(__FILE__) \
                .sub_string<::remotecc::infra::details::__source_dir_prefix_length_src + 1>() \
                .value)

#define INFRA_CURRENT_LINE \
            (::remotecc::infra::integer_to_string<size_t, __LINE__, 10>().value)

        }  // namespace details

        struct log_level
        {
            enum
            {
                LOG_LEVEL_FATAL = -3,
                LOG_LEVEL_ERROR = -2,
                LOG_LEVEL_WARN = -1,
                LOG_LEVEL_INFO = 0,
                LOG_LEVEL_DBG1 = 1,
                LOG_LEVEL_DBG2 = 2,
                LOG_LEVEL_DBG3 = 3,
                LOG_LEVEL_DBG4 = 4,
                LOG_LEVEL_DBG5 = 5,
            };
        };

    }  // namespace infra
}  // namespace remotecc


#define _REMOTECC_LOG_INTERNAL(Level, What, ...) \
    do { \
        if (::remotecc::infra::details::__logging_verbosity >= ::remotecc::infra::log_level::LOG_LEVEL_##Level) { \
            ::remotecc::infra::simple_date_time_t now { }; \
            (void)now_date_time(&now); \
            static constexpr const auto message = ::remotecc::infra::make_string("") + \
                "[%04u-%02u-%02u %02u:%02u:%02u] " + \
                (::remotecc::infra::make_string("[") + #Level + "]").pad_right<7>(' ') + \
                " [" + INFRA_CURRENT_FILE + ":" + INFRA_CURRENT_LINE + "] " + \
                What + "\n"; \
            /* For MSVC, we don't need to "EXPAND" __VA_ARGS__ here, */ \
            /* even if with its strange behavior with __VA_ARGS__. Don't now why. */ \
            /* See: https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly */ \
            fprintf(::remotecc::infra::details::__logging_file, \
                message.value, \
                now.year, now.month, now.day_of_month, now.hour, now.minute, now.second, \
                ##__VA_ARGS__); \
        } \
    } while(false)

#if INFRA_IDE_MODE

#define LOG_FATAL(...)          printf(__VA_ARGS__)
#define LOG_ERROR(...)          printf(__VA_ARGS__)
#define LOG_WARN(...)           printf(__VA_ARGS__)
#define LOG_INFO(...)           printf(__VA_ARGS__)
#define LOG_DBG1(...)           printf(__VA_ARGS__)
#define LOG_DBG2(...)           printf(__VA_ARGS__)
#define LOG_DBG3(...)           printf(__VA_ARGS__)
#define LOG_DBG4(...)           printf(__VA_ARGS__)
#define LOG_DBG5(...)           printf(__VA_ARGS__)

#else  // INFRA_IDE_MODE

#define LOG_FATAL(What, ...)    _REMOTECC_LOG_INTERNAL(FATAL, What, ##__VA_ARGS__)
#define LOG_ERROR(What, ...)    _REMOTECC_LOG_INTERNAL(ERROR, What, ##__VA_ARGS__)
#define LOG_WARN(What, ...)     _REMOTECC_LOG_INTERNAL(WARN, What, ##__VA_ARGS__)
#define LOG_INFO(What, ...)     _REMOTECC_LOG_INTERNAL(INFO, What, ##__VA_ARGS__)
#define LOG_DBG1(What, ...)     _REMOTECC_LOG_INTERNAL(DBG1, What, ##__VA_ARGS__)
#define LOG_DBG2(What, ...)     _REMOTECC_LOG_INTERNAL(DBG2, What, ##__VA_ARGS__)
#define LOG_DBG3(What, ...)     _REMOTECC_LOG_INTERNAL(DBG3, What, ##__VA_ARGS__)
#define LOG_DBG4(What, ...)     _REMOTECC_LOG_INTERNAL(DBG4, What, ##__VA_ARGS__)
#define LOG_DBG5(What, ...)     _REMOTECC_LOG_INTERNAL(DBG5, What, ##__VA_ARGS__)

#endif  // INFRA_IDE_MODE


#endif  // !defined(_INFRA_LOGGING_HPP_INCLUDED_)
