#include <cstdio>

#if REMOTECC_DAEMON_MPI_SUPPORT
#include <mpi.h>
#endif

#define MPI_CALL(_Call_) \
    do { \
        int _result_ = (_Call_); \
        if (_result_ != MPI_SUCCESS) { \
            fprintf(stderr, #_Call_ " failed with %d\n", _result_); \
        } \
    } while(false)

void print(int argc, char* argv[])
{
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
}

int main(int argc, char* argv[], char* env[])
{
    static_assert(__THIS_IS_REMOTECCD__, "Expect: __THIS_IS_REMOTECCD__");

    print(argc, argv);

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

    return 0;
}
