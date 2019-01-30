#if !defined(_INFRA_TYPES_HPP_INCLUDED_)
#define _INFRA_TYPES_HPP_INCLUDED_

#include <array>
#include <cstddef>
#include <type_traits>

namespace remotecc
{
    namespace infra
    {
        /**
         * Removes any reference and const and/or volatile modifier(s) on given type T.
         * Returns the underlying type.
         */
        template<typename T>
        using raw_type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;


        /**
         * Returns whether given type T is a "raw" type, which is, a type with no
         * const and/or volatile modifier(s), nor reference.
         */
        template<typename T>
        struct is_raw_type
        {
            typedef typename std::is_same<T, raw_type<T>>::value_type value_type;

            static constexpr const value_type value = std::is_same<T, raw_type<T>>::value;

            constexpr operator value_type() const noexcept { return value; }

            constexpr value_type operator()() const noexcept { return value; }
        };


        /**
         * Convert to pointer type if T is reference to array
         * Keep T as-is otherwise
         */
        template<typename T>
        struct array_ref_to_ptr
        {
            typedef T type;
        };

        template<typename T, size_t _Size>
        struct array_ref_to_ptr<T[_Size]>
        {
            typedef T* type;
        };

        template<typename T, size_t _Size>
        struct array_ref_to_ptr<T(&)[_Size]>
        {
            typedef T* type;
        };

        template<typename T, size_t _Size>
        struct array_ref_to_ptr<T(&&)[_Size]>
        {
            typedef T* type;
        };


        template<typename T, size_t _Size>
        constexpr size_t length_of_array(const T(&)[_Size]) noexcept { return _Size; }

        template<typename T, size_t _Size>
        constexpr size_t length_of_array(const std::array<T, _Size>&) noexcept { return _Size; }

        template<size_t _Size>
        constexpr size_t length_of_cstr(const char(&)[_Size]) noexcept
        {
            static_assert(_Size >= 1, "cstr must contain at least NULL terminator");
            return _Size - 1;
        }

    }  // namespace infra
}  // namespace infra

#endif  // !defined(_INFRA_TYPES_HPP_INCLUDED_)
