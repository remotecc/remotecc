#include <infra.hpp>
#include "core/daemon_service.hpp"

#include <boost/asio/signal_set.hpp>
#include <functional>

#if REMOTECC_DAEMON_MPI_SUPPORT
#include <mpi.h>
#endif


namespace remotecc
{
    class remoteccd_instance
    {
    private:
        static void signal_handler(
            remoteccd_instance* instance,
            const boost::system::error_code& error,
            const int signal_number) noexcept
        {
            if (error)
            {
                LOG_ERROR("signal: %d. error: %d", signal_number, error.value());
            }
            else
            {
                LOG_INFO("signal: %d. error: %d", signal_number, error.value());
            }
            //instance->_asio_signals->async_wait(signal_handler);
            instance->_asio_context.stop();
        }

    public:
        remoteccd_instance() = default;
        INFRA_DISABLE_COPY_CONSTRUCTOR(remoteccd_instance);
        INFRA_DISABLE_MOVE_CONSTRUCTOR(remoteccd_instance);

        int main(int argc, char* argv[], char* env[])
        {
            boost::ignore_unused(argc, argv, env);

            // Construct a signal set registered for process termination.
            _asio_signals = new boost::asio::signal_set(_asio_context);
            _asio_signals->add(SIGINT);
            _asio_signals->add(SIGABRT);
            _asio_signals->add(SIGTERM);
            // TODO: _asio_signals->add(more signals);
            _asio_signals->async_wait(std::bind(signal_handler, this, std::placeholders::_1, std::placeholders::_2));

            // TODO: load configurations

            // TODO: Connect with remote remoteccd instances

            // Create a daemon
            daemon_service_config_t config { };
            config.shmem_size = 1048576;

            if (!daemon_service::cleanup_shmem("default"))
            {
                LOG_ERROR("daemon_service::cleanup_shmem failed!");
            }

            daemon_service daemon;
            daemon.create("default", config);
            if (!daemon.initialize_successful())
            {
                LOG_ERROR("Create daemon_service failed. name: %s\n", "default");
                return 1;
            }
            daemon.start_updater_thread();
            daemon.mark_ready();

            LOG_INFO("asio_context.run()");
            _asio_context.run();
            LOG_INFO("asio_context.run() returns: now exit...");

            daemon.mark_not_ready();

            // TODO: Then: finish all serving instances

            daemon.stop_updater_thread();

            return 0;
        }


        ~remoteccd_instance() noexcept
        {
            if (_asio_signals)
            {
                delete _asio_signals;
                _asio_signals = nullptr;
            }
        }

    private:
        boost::asio::io_context _asio_context;
        boost::asio::signal_set* _asio_signals = nullptr;

    };  // class remoteccd_instance
}  // namespace remotecc

#define MPI_CALL(_Call_) \
    do { \
        int _result_ = (_Call_); \
        if (_result_ != MPI_SUCCESS) { \
            fprintf(stderr, #_Call_ " failed with %d\n", _result_); \
        } \
    } while(false)


int main(int argc, char* argv[], char* env[])
{
    static_assert(__THIS_IS_REMOTECCD__, "Expect: __THIS_IS_REMOTECCD__");

    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

#if REMOTECC_DAEMON_MPI_SUPPORT
    MPI_CALL(MPI_Init(&argc, &argv));
    print(argc, argv);

    int mpi_rank = -1, mpi_size = -1;
    MPI_CALL(MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank));
    MPI_CALL(MPI_Comm_size(MPI_COMM_WORLD, &mpi_size));

    printf("mpi_size: %d, mpi_rank: %d\n", mpi_size, mpi_rank);

    static char name[MPI_MAX_PROCESSOR_NAME];
    int len = MPI_MAX_PROCESSOR_NAME;
    MPI_CALL(MPI_Get_processor_name(name, &len));
    printf("\nMPI_Get_processor_name: %s\n", name);

    MPI_CALL(MPI_Finalize());
#endif

    printf("Hello from remoteccd!\n");

    for (size_t i = 0; env[i] != nullptr; ++i)
    {
        printf("env: %s\n", env[i]);
    }

    return remotecc::remoteccd_instance().main(argc, argv, env);
}
