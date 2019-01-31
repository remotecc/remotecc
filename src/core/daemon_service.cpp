#include "daemon_service.hpp"
#include <chrono>

namespace remotecc
{
    /*static*/ void daemon_service::make_fullname(
        std::string& result,
        const char *name)
    {
        static constexpr const char __prefix[] = "remotecc-1de4a3b41981471c91a31eb4bd1ba79c-";
        result = __prefix;
        result += name;
    }


    /*static*/ bool daemon_service::cleanup_shmem(const char *name) noexcept
    {
        using namespace boost::interprocess;

        std::string fullname;
        try
        {
            make_fullname(fullname, name);
        }
        catch (const std::exception& ex)
        {
            // WTF?
            LOG_ERROR("daemon_service::cleanup_shmem(): Unexpected exception when constructing daemon fullname from %s"
                      "what(): %s",
                name, ex.what());
        }

        try
        {
            (void)managed_shared_memory_type(open_only, fullname.c_str(), /*addr*/nullptr);
        }
        catch (const interprocess_exception& ex)
        {
            if (ex.get_error_code() == error_code_t::not_found_error)
            {
                // this is expected. The mapping with "name" does not exist
                LOG_DBG4("daemon_service::cleanup_shmem(): daemon %s does not exist", name);
                return true;
            }

            // Unexpected error when trying to open this managed_shared_memory
            // However, we still try to cleanup it...
            LOG_ERROR("daemon_service::cleanup_shmem(): Unexpected interprocess_exception. "
                      "what(): %s. error_code: %d, native_error: %d (name: %s)",
                ex.what(), ex.get_error_code(), ex.get_native_error(),
                name);
        }
        catch (...)
        {
            // Still, unexpected error when trying to open this managed_shared_memory
            // Still, we still try to cleanup it...
            LOG_ERROR("daemon_service::cleanup_shmem(): Unexpected exception when constructing daemon fullname from %s",
                name);
        }

        // Try to remove it!
        return shared_memory_object::remove(fullname.c_str());
    }


    bool daemon_service::create(const char *name, const daemon_service_config_t& config)
    {
        using namespace boost::interprocess;

        // Set initialized to true
        ASSERT_IS_FALSE(_initialize_called);
        _initialize_called = true;

        _shmem_is_create= true;
        _name = name;
        _config = config;
        LOG_DBG4("daemon_service::create name: %s", _name.c_str());

        make_fullname(_fullname, _name.c_str());
        LOG_DBG5("daemon_service::create fullname: %s", _fullname.c_str());

        // Create managed shared memory
        permissions perm;
        perm.set_unrestricted();  // The new managed_shared_memory allows universal access
        try
        {
            _shmem = new managed_shared_memory_type(create_only, _fullname.c_str(), _config.shmem_size, /*addr*/nullptr, perm);
        }
        catch (const interprocess_exception& ex)
        {
            if (ex.get_error_code() == error_code_t::already_exists_error)
            {
                LOG_ERROR("daemon already exists. Is daemon already started? (name: %s)", _name.c_str());
                dispose();
                return false;
            }
            else
            {
                LOG_FATAL("Unexpected interprocess_exception. Daemon failed creating managed shared memory. "
                          "shmem_size: %llu. what(): %s. error_code: %d, native_error: %d (name: %s)",
                    (unsigned long long)_config.shmem_size,
                    ex.what(), ex.get_error_code(), ex.get_native_error(),
                    _name.c_str());
                dispose();
                throw;  // throw any exception other than already_exists_error
            }
        }

        // Construct daemon_service_info_t
        _daemon_service_info = _shmem->construct<daemon_service_info_t>(
            daemon_service_info_t::SHMEM_OBJECT_KEY,
            std::nothrow)();
        if (_daemon_service_info == nullptr)
        {
            LOG_FATAL("Construct daemon_service_info_t failed. Shared memory too small?"
                      "shmem_size(): %llu (name: %s)",
                      (unsigned long long)shmem_size(), _name.c_str());
            dispose();
            return false;
        }

        // Fill _daemon_service_info with daemon information
        _daemon_service_info->daemon_pid = boost::this_process::get_id();
        LOG_DBG1("daemon process id: %d (name: %s)", _daemon_service_info->daemon_pid, _name.c_str());

        // TODO: _daemon_service_info->...

        _initialize_successful = true;
        return true;
    }


    bool daemon_service::open(const char *name)
    {
        using namespace boost::interprocess;

        // Set initialized to true
        ASSERT_IS_FALSE(_initialize_called);
        _initialize_called = true;

        _shmem_is_create= false;
        _name = name;
        // _config is left untouched
        LOG_DBG4("daemon_service::open with name: %s", _name.c_str());

        make_fullname(_fullname, _name.c_str());
        LOG_DBG5("daemon_service::open fullname: %s", _fullname.c_str());

        // Open shared memory
        try
        {
            _shmem = new managed_shared_memory_type(open_only, _fullname.c_str(), /*addr*/nullptr);
        }
        catch (const interprocess_exception& ex)
        {
            if (ex.get_error_code() == error_code_t::not_found_error)
            {
                LOG_ERROR("Can't find daemon. Is daemon started? (name: %s)", _name.c_str());
                dispose();
                return false;
            }
            else
            {
                LOG_ERROR("Unexpected interprocess_exception! "
                          "what(): %s. error_code: %d, native_error_code: %d (name: %s)",
                          ex.what(), (int)ex.get_error_code(), (int)ex.get_native_error(), _name.c_str());
                dispose();
                throw;  // throw any exception other than not_found_error
            }
        }

        // Find daemon service information (created by daemon)
        const auto pair = _shmem->find<daemon_service_info_t>(daemon_service_info_t::SHMEM_OBJECT_KEY);
        _daemon_service_info = pair.first;
        if (_daemon_service_info == nullptr)
        {
            LOG_ERROR("Can't find daemon service info. Is daemon initializing?");
            dispose();
            return false;
        }
        ASSERT_EQ(pair.second, (size_t)1);  // allocated object count, should be 1

        // TODO: Check: daemon service information: is it ready?
        // TODO: Check: daemon service information: is it stale? (check now_msec)

        _initialize_successful = true;
        return true;
    }


    void daemon_service::dispose() noexcept
    {
        using namespace boost::interprocess;

        // If this is not initialized, we need do nothing...
        if (!_initialize_called)
        {
            LOG_DBG5("daemon_service::dispose does nothing as not initialized (name: %s)", _name.c_str());
            return;
        }

        // Delete _shmem
        bool shmem_existed = (_shmem != nullptr);
        if (shmem_existed)
        {
            LOG_DBG5("daemon_service::dispose destruct shmem (name: %s)", _name.c_str());
            delete _shmem;
            _shmem = nullptr;
        }

        // If we created the shmem, and the creation succeeded, remove it...
        if (_shmem_is_create && shmem_existed)
        {
            // Remove only if we are using managed_shared_memory (but not managed_windows_shared_memory)
            if (std::is_same<managed_shared_memory_type, managed_shared_memory>::value)
            {
                LOG_DBG5("daemon_service::dispose remove managed_shared_memory (name: %s)", _name.c_str());
                bool success = managed_shared_memory::remove(_fullname.c_str());
                if (!success)
                {
                    LOG_ERROR("Remove managed_shared_memory failed. (name: %s)", _name.c_str());
                }
            }
        }
    }

    daemon_service::~daemon_service() noexcept
    {
        dispose();
    }

    void daemon_service::mark_ready() noexcept
    {
        ASSERT_IS_TRUE(_initialize_successful);
        ASSERT_NOT_NULL(_daemon_service_info);
        ASSERT_IS_FALSE(_daemon_service_info->daemon_ready.load(std::memory_order_seq_cst));

        LOG_DBG5("daemon_service mark as ready (name: %s)", _name.c_str());
        _daemon_service_info->daemon_ready.store(true, std::memory_order_seq_cst);
    }

    void daemon_service::mark_not_ready() noexcept
    {
        ASSERT_IS_TRUE(_initialize_successful);
        ASSERT_NOT_NULL(_daemon_service_info);
        ASSERT_IS_TRUE(_daemon_service_info->daemon_ready.load(std::memory_order_seq_cst));

        LOG_DBG5("daemon_service mark as not ready (name: %s)", _name.c_str());
        _daemon_service_info->daemon_ready.store(false, std::memory_order_seq_cst);
    }

    void daemon_service::start_updater_thread()
    {
        LOG_DBG1("daemon_service::start_updater_thread() (name: %s)", _name.c_str());
        if (!_updater_thread_mutex.try_lock())
        {
            ASSERT_BUG("_updater_thread_mutex is locked. start_updater_thread() is called more than once.");
        }

        _updater_thread = std::thread([&]() {
            LOG_DBG1("daemon_service updater thread: started. Interval is %u msec", _config.keepalive_interval_msec);
            const std::chrono::milliseconds interval(_config.keepalive_interval_msec);
            while (true)
            {
                // TODO: Update _daemon_service_info->now_msec
                LOG_INFO("Hello World~~");

                const bool stop_required = _updater_thread_mutex.try_lock_for(interval);
                if (stop_required) break;
            }
            LOG_DBG1("daemon_service updater thread: stop required");
        });
    }

    void daemon_service::stop_updater_thread()
    {
        LOG_DBG1("daemon_service::stop_updater_thread() (name: %s)", _name.c_str());

        ASSERT(_updater_thread.joinable(), "Updater thread is not joinable. "
                                           "start_updater_thread() not called? stop_updater_thread() called twice?");

        _updater_thread_mutex.unlock();
        _updater_thread.join();
    }

}  // namespace remotecc
