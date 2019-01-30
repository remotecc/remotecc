#include "infra.hpp"
#include <climits>

namespace remotecc
{
    namespace infra
    {
        namespace details
        {
            FILE* __logging_file = stderr;
            int __logging_verbosity = INT_MAX;

        }  // namespace details
    }  // namespace infra
}  // namespace remotecc
