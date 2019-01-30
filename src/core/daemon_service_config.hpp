#if !defined(_REMOTECC_CORE_DAEMON_SERVICE_CONFIG_HPP_INCLUDED_)
#define _REMOTECC_CORE_DAEMON_SERVICE_CONFIG_HPP_INCLUDED_

#include <thirdparty_disable_warnings.hpp>
#include <boost/process.hpp>
#include <thirdparty_restore_warnings.hpp>

#include <infra.hpp>

namespace remotecc
{
    struct daemon_service_config_t
    {
        static constexpr const unsigned int DEFAULT_KEEPALIVE_INTERVAL_MSEC = 400;
        static constexpr const unsigned int DEFAULT_KEEPALIVE_DEAD_MSEC = 1000;

        size_t shmem_size;
        unsigned int keepalive_interval_msec = DEFAULT_KEEPALIVE_INTERVAL_MSEC;
        unsigned int keepalive_dead_msec = DEFAULT_KEEPALIVE_DEAD_MSEC;
    };

    struct daemon_service_info_t
    {
        static constexpr const char SHMEM_OBJECT_KEY[] = "daemon";

        std::atomic_bool daemon_ready;
        int daemon_pid = 0;
        std::uint64_t daemon_create_timestamp = 0;
        volatile std::atomic_uint64_t now_msec;

        daemon_service_info_t() noexcept
            : daemon_ready(false),
              now_msec(0)
        { }

        INFRA_DISABLE_COPY_CONSTRUCTOR(daemon_service_info_t);
        INFRA_DISABLE_MOVE_CONSTRUCTOR(daemon_service_info_t);
    };
}

#endif  // !defined(_REMOTECC_CORE_DAEMON_SERVICE_CONFIG_HPP_INCLUDED_)
