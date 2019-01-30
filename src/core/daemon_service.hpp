#if !defined(_CORE_DAEMON_HPP_INCLUDED_)
#define _CORE_DAEMON_HPP_INCLUDED_

#include <infra.hpp>

#include <thirdparty_disable_warnings.hpp>
#if BOOST_OS_WINDOWS && !BOOST_OS_CYGWIN
#include <boost/thread/locks.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#else
#include <boost/interprocess/managed_shared_memory.hpp>
#endif
#include <boost/interprocess/shared_memory_object.hpp>
#include <thirdparty_restore_warnings.hpp>

#include "daemon_service_config.hpp"
#include <thread>
#include <mutex>

namespace remotecc
{
    class daemon_service
    {
    public:
#if BOOST_OS_WINDOWS && !BOOST_OS_CYGWIN
        typedef boost::interprocess::managed_windows_shared_memory managed_shared_memory_type;
#else
        typedef boost::interprocess::managed_shared_memory managed_shared_memory_type;
#endif

    public:
        static bool cleanup_shmem(const char* name) noexcept;

    private:
        static void make_fullname(std::string& result, const char* name);

    public:
        daemon_service() = default;
        ~daemon_service() noexcept;

        bool create(const char* name, const daemon_service_config_t& config);
        void mark_ready() noexcept;
        void mark_not_ready() noexcept;
        void start_updater_thread();
        void stop_updater_thread();

        bool open(const char* name);

        INFRA_DISABLE_COPY_CONSTRUCTOR(daemon_service);
        INFRA_DISABLE_MOVE_CONSTRUCTOR(daemon_service);

    public:
        void dispose() noexcept;

        bool initialize_successful() const noexcept
        {
            return _initialize_successful;
        }

        const std::string& name() const noexcept
        {
            return _name;
        }

        size_t shmem_size() const noexcept
        {
            return _config.shmem_size;
        }


    private:

        // Is it initialized by calling open() / create()?
        // Is the initialization successful?
        // Is it create (true) or open (false) ?
        bool _initialize_called = false;
        bool _initialize_successful = false;
        bool _shmem_is_create = false;

        // Name and fullname of the daemon.
        // Name is user-given, while full name contains a "magic" prefix
        std::string _name { };
        std::string _fullname { };

        // Valid only for remoteccd
        daemon_service_config_t _config { };

        // The managed shmem object
        managed_shared_memory_type* _shmem = nullptr;

        // Daemon service information. Stored in shared memory.
        // Read/write for remoteccd
        // Readonly for remotecc
        daemon_service_info_t* _daemon_service_info = nullptr;

        // Use timed_mutex to simulate timer.
        // Usually, the timed_mutex is locked. Timer thread tries to lock it but timeout.
        // When stopping, unlock timed_mutex. Timer thread locks it successfully.
        std::thread _updater_thread;
        std::timed_mutex _updater_thread_mutex;

    };  // class daemon_service

}  // namespace remotecc


#endif  // !defined(_CORE_DAEMON_HPP_INCLUDED_)
