#if !defined(_INFRA_DATETIME_HPP_INCLUDED_)
#define _INFRA_DATETIME_HPP_INCLUDED_

#include <boost/date_time/posix_time/posix_time.hpp>

namespace remotecc
{
    namespace infra
    {
        struct simple_date_time_t
        {
            unsigned int year;
            unsigned int month;
            unsigned int day_of_month;
            unsigned int hour;
            unsigned int minute;
            unsigned int second;
        };

        inline bool now_date_time(simple_date_time_t* result)
        {
            using namespace boost::posix_time;
            using namespace boost::gregorian;

            ptime now = second_clock::local_time();
            if (now.is_special())
            {
                result->year = 0;
                result->month = 0;
                result->day_of_month = 0;
                result->hour = 0;
                result->minute = 0;
                result->second = 0;
                return false;
            }
            else
            {
                result->year = (unsigned int)now.date().year();
                result->month = (unsigned int)now.date().month();
                result->day_of_month = (unsigned int)now.date().day();
                result->hour = (unsigned int)now.time_of_day().hours();
                result->minute = (unsigned int)now.time_of_day().minutes();
                result->second = (unsigned int)now.time_of_day().seconds();
                return true;
            }
        }

    }  //namespace infra
}  // namespace remotecc

#endif  // !defined(_INFRA_DATETIME_HPP_INCLUDED_)
