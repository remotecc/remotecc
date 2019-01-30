#if !defined(_INFRA_STRING_HPP_INCLUDED_)
#define _INFRA_STRING_HPP_INCLUDED_

#include "./types.hpp"
#include "./numeric.hpp"
#include <boost/static_assert.hpp>
#include <cstddef>

namespace remotecc
{
    namespace infra
    {
        /**
         * Compile time string, with terminating NULL character.
         * @tparam _Size  Size of the string, including the terminating NULL character
         * @tparam TCh  Character type, normally C++ "char"
         */
        template<size_t _Size, typename TCh>
        struct string
        {
            static_assert(_Size > 0, "string must contain at least NULL terminator");
            static_assert(is_raw_type<TCh>(), "TCh must be a raw type");

            static constexpr const TCh NULL_CHAR = (TCh)0;

            TCh value[_Size] { };

            /**
             * Get the length of string (NOT counting the last NULL terminator)
             */
            constexpr size_t length() const noexcept
            {
                return _Size - 1;
            }

            /**
             * Construct an empty string (full of NULL characters).
             */
            constexpr explicit string(const TCh ch = NULL_CHAR) noexcept
            {
                fill(0, ch, length());
                value[length()] = NULL_CHAR;
            }

            /**
             * Construct from an existing string.
             */
            constexpr explicit string(const TCh(&str)[_Size]) noexcept
            {
                copy_from(0, str, 0, length());
                value[length()] = NULL_CHAR;
            }

            /**
             * Construct from existing string, starting from a given position.
             */
            template<size_t _NStr>
            constexpr string(const TCh(&str)[_NStr], const size_t start) noexcept
            {
                copy_from(0, str, start, length());
                value[length()] = NULL_CHAR;
            }


            /**
             * Copy from an existing string to current string.
             *
             * @param my_start  The start position of current string destination
             * @param str       The source string
             * @param str_start The start position from source string
             * @param count     Count of characters to copy
             */
            template<size_t _NStr>
            constexpr void copy_from(
                    const size_t my_start,
                    const TCh(&str)[_NStr],
                    const size_t str_start,
                    const size_t count) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    value[i + my_start] = str[i + str_start];
                }
            }

            /**
             * Fill (part of) current string with given character.
             *
             * @param my_start  The start position of current string destination
             * @param ch        Character to fill
             * @param count     Count to fill
             */
            constexpr void fill(
                    const size_t my_start,
                    const TCh ch,
                    const size_t count) noexcept
            {
                for (size_t i = 0; i < count; ++i)
                {
                    value[i + my_start] = ch;
                }
            }

            /**
             * Compares current string with given str in dictionary order.
             * Returns -1 if current string is smaller.
             * Returns 1 if current string is larger.
             * Returns 0 if the two strings are the same.
             */
            template<size_t _NStr>
            constexpr int compare_with(const TCh (&str)[_NStr]) const noexcept
            {
                const size_t len = min(_NStr, _Size);
                for (size_t i = 0; i < len; ++i)
                {
                    if (value[i] < str[i]) return -1;
                    if (value[i] > str[i]) return 1;
                }
                if (_Size < _NStr) return -1;
                if (_Size > _NStr) return 1;
                return 0;
            }

            /**
             * Searches the string for the first character, which matches ANY of the characters
             * specified in argument "chars".
             * If "start" is specified, the search only includes characters at or after that position,
             * ignoring any possible occurrences before "start".
             * If not found, returns -1.
             */
            template<size_t _NChars>
            constexpr size_t find_first_of(const TCh (&chars)[_NChars], size_t start = 0) const noexcept
            {
                for (size_t pos = start; pos < length(); ++pos) {
                    for (size_t i = 0; i < length_of_cstr(chars); ++i) {
                        if (value[pos] == chars[i]) return pos;
                    }
                }
                return (size_t)-1;
            }

            /**
             * Searches the string for the last character, which matches ANY of the characters
             * specified in argument "chars".
             * If "start" is specified, the search only includes characters at or before that position,
             * ignoring any possible occurrences after "start".
             * If not found, returns -1.
             */
            template<size_t _NChars>
            constexpr size_t find_last_of(const TCh (&chars)[_NChars], size_t start = _Size - 1) const noexcept
            {
                for (size_t pos = min(start, length() - 1); pos != (size_t)-1; --pos) {
                    for (size_t i = 0; i < length_of_cstr(chars); ++i) {
                        if (value[pos] == chars[i]) return pos;
                    }
                }
                return (size_t)-1;
            }

            /**
             * Return the substring of current string starting from "_Start"
             */
            template<size_t _Start>
            constexpr auto sub_string() const noexcept
            {
                BOOST_STATIC_ASSERT(_Start < _Size);
                return string<_Size - _Start, TCh>(value, _Start);
            }

            /**
             * Return the substring of current string starting from "_Start", with length "_Length"
             */
            template<size_t _Start, size_t _Length>
            constexpr auto sub_string() const noexcept
            {
                // Make sure not overflow!
                BOOST_STATIC_ASSERT(_Start + _Length >= _Start);
                BOOST_STATIC_ASSERT(_Start + _Length >= _Length);

                BOOST_STATIC_ASSERT(_Start < _Size);
                BOOST_STATIC_ASSERT(_Start + _Length < _Size);
                return string<_Length + 1, TCh>(value, _Start);
            }

            /**
             * Add padding at end of current string with specific character
             */
            template<size_t _Length>
            constexpr auto pad_right(const TCh ch) const noexcept
            {
                BOOST_STATIC_ASSERT(_Length + 1 >= _Size);

                string<_Length + 1, TCh> result(ch);
                result.copy_from(0, value, 0, length());
                return result;
            }

            /**
             * Add padding at left of current string with specific character
             */
            template<size_t _Length>
            constexpr auto pad_left(const TCh ch) const noexcept
            {
                BOOST_STATIC_ASSERT(_Length + 1 >= _Size);

                string<_Length + 1, TCh> result(ch);
                result.copy_from(_Length - length(), value, 0, length());
                return result;
            }

            /**
             * Returns true if this string is NOT truncated by inner NULL terminator.
             */
            constexpr bool contains_no_null_char() const noexcept
            {
                for (size_t pos = 0; pos < length(); ++pos) {
                    if (value[pos] == NULL_CHAR) return false;
                }
                return true;
            }

            /**
             * Append "str" to current string to form a new string.
             */
            template<size_t _NStr>
            constexpr auto operator+(const TCh (&str)[_NStr]) const noexcept
            {
                static_assert(_NStr > 0, "str must contain at least NULL terminator");

                string<(_Size - 1) + (_NStr - 1) + 1, TCh> result;
                result.copy_from(0, value, 0, length());
                result.copy_from(length(), str, 0, _NStr - 1);
                return result;
            }

            /**
             * Append a character to current string.
             */
            constexpr auto operator+(const TCh ch) const noexcept
            {
                string<(_Size + 1), TCh> result;
                result.copy_from(0, value, 0, length());
                result.value[length()] = ch;
                return result;
            }

            /**
             * Append "str" to current string to form a new string.
             */
            template<size_t _NStr>
            constexpr string<(_Size - 1) + (_NStr - 1) + 1, TCh> operator+(const string<_NStr, TCh>& str) const noexcept
            {
                return operator+(str.value);
            }


#define _REMOTECC_INFRA_STRING_COMP(_Op_) \
            template<size_t _NStr> \
            constexpr bool operator _Op_(const TCh (&str)[_NStr]) const noexcept \
            { \
                return compare_with(str) _Op_ 0; \
            } \
            template<size_t _NStr> \
            constexpr bool operator _Op_(const string<_NStr, TCh>& str) const noexcept \
            { \
                return operator _Op_(str.value); \
            }

            _REMOTECC_INFRA_STRING_COMP(<)
            _REMOTECC_INFRA_STRING_COMP(<=)
            _REMOTECC_INFRA_STRING_COMP(>)
            _REMOTECC_INFRA_STRING_COMP(>=)
            _REMOTECC_INFRA_STRING_COMP(==)
            _REMOTECC_INFRA_STRING_COMP(!=)

#undef _REMOTECC_INFRA_STRING_COMP

        };  // struct string


        /**
         * Make a compile time string from TCh array.
         */
        template<typename TCh, size_t _Size>
        constexpr string<_Size, TCh> make_string(const TCh(&str)[_Size]) noexcept
        {
            return string<_Size, TCh>(str);
        }

        /**
         * Make a compile time string from a character.
         */
        template<typename TCh>
        constexpr auto make_string(const TCh ch) noexcept
        {
            return make_string("") + ch;
        }

        /**
         * Make a compile time string from existing compile time string:
         * just return a copy of itself.
         */
        template<typename TCh, size_t _Size>
        constexpr string<_Size, TCh> make_string(const string<_Size, TCh>& str) noexcept
        {
            return str;
        }


        namespace details
        {
            template<typename TU, TU _Value, bool _IsZero, size_t _Base>
            struct positive_to_string;

            template<typename TU, TU _Value, size_t _Base>
            struct positive_to_string<TU, _Value, /*_IsZero*/true, _Base>
            {
                static constexpr const auto value = make_string("");
            };

            template<typename TU, TU _Value, size_t _Base>
            struct positive_to_string<TU, _Value, /*_IsZero*/false, _Base>
            {
                BOOST_STATIC_ASSERT(_Base > 0);
                BOOST_STATIC_ASSERT(_Base <= 36);
                static constexpr const auto value =
                    positive_to_string<TU, _Value / _Base, (_Value / _Base == 0), _Base>::value +
                    "0123456789abcdefghijklmnopqrstuvwxyz"[_Value % _Base];
            };



            template<typename TInt, bool _IsZero, bool _IsNegative, size_t _Base>
            struct integer_to_string_helper;

            template<typename TUInt, size_t _Base>
            struct integer_to_string_helper<TUInt, /*_IsZero*/false, /*_IsNegative*/false, _Base>
            {
                template<TUInt _Value>
                static constexpr auto convert()
                {
                    BOOST_STATIC_ASSERT(_Value > 0);
                    return positive_to_string<
                        TUInt,
                        /*_IsZero*/ _Value,
                        /*_IsNegative*/ false,
                        _Base>::value;
                }
            };

            template<typename TInt, size_t _Base>
            struct integer_to_string_helper<TInt, /*_IsZero*/true, /*_IsNegative*/false, _Base>
            {
                template<TInt _Value>
                static constexpr auto convert()
                {
                    BOOST_STATIC_ASSERT(_Value == 0);
                    return make_string("0");
                }
            };

            template<typename TInt, size_t _Base>
            struct integer_to_string_helper<TInt, /*_IsZero*/false, /*_IsNegative*/true, _Base>
            {
                template<TInt _Value>
                static constexpr auto convert()
                {
                    typedef typename std::make_unsigned<TInt>::type TUInt;
                    return make_string("-") +
                        integer_to_string_helper<
                            TUInt,
                            /*_IsZero*/ false,
                            /*_IsNegative*/ false,
                            _Base>
                            ::template convert<(~(TUInt)_Value + 1)>();
                }
            };

        }  // namespace details

        /**
         * Convert an integer to string representation.
         */
        template<typename TInt, TInt _Value, size_t _Base = 10>
        constexpr auto integer_to_string() noexcept
        {
            //BOOST_STATIC_ASSERT(std::is_signed<TInt>::value || std::is_unsigned<TInt>::value);
            return details::integer_to_string_helper<
                TInt,
                /*_IsZero*/ (_Value == 0),
                /*_IsNegative*/ (_Value < 0),
                _Base>
                ::template convert<_Value>();
        }

    }  // namespace infra
}  // namespace remotecc

#endif  // !defined(_INFRA_STRING_HPP_INCLUDED_)
