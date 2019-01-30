#include <infra.hpp>
#include <cstdio>
#include <core/daemon_service.hpp>

int main(int argc, char* argv[], char* env[])
{
    static_assert(__THIS_IS_REMOTECC__, "This should be defined");

    boost::ignore_unused(argc, argv, env);

    remotecc::daemon_service daemon;
    daemon.open("default");

    printf("Hello from remotecc!\n");
    return 0;
}
