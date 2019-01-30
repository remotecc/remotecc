#if !defined(_INFRA_NUMERIC_HPP_INCLUDED_)
#define _INFRA_NUMERIC_HPP_INCLUDED_

namespace remotecc
{
    namespace infra
    {
        template <typename T>
        constexpr T min(const T& a, const T& b) noexcept
        {
            return a < b ? a : b;
        }

        template <typename T>
        constexpr T max(const T& a, const T& b) noexcept
        {
            return a < b ? b : a;
        }

    }  // namespace infra
}  // namespace remotecc

#endif  // !defined(_INFRA_NUMERIC_HPP_INCLUDED_)
