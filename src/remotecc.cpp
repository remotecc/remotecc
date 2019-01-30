#include <infra.hpp>
#include <cstdio>

int main(int argc, char* argv[], char* env[])
{
    INFRA_STATIC_ASSERT(__THIS_IS_REMOTECC__, "This should be defined");

    printf("Hello from remotecc!\n");
    return 0;
}
