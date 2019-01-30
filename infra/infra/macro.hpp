#if !defined(_INFRA_MACRO_HPP_INCLUDED_)
#define _INFRA_MACRO_HPP_INCLUDED_

#define INFRA_STATIC_ASSERT(_Expr_, ...) \
    static_assert(_Expr_, "INFRA_STATIC_ASSERT(" #_Expr_ ") failed. " __VA_ARGS__)

#define INFRA_DISABLE_COPY_CONSTRUCTOR(_Class_) \
    _Class_(const _Class_&) = delete; \
    _Class_& operator=(const _Class_&) = delete

#define INFRA_DISABLE_MOVE_CONSTRUCTOR(_Class_) \
    _Class_(_Class_&&) = delete; \
    _Class_& operator=(_Class_&&) = delete

#endif  // !defined(_INFRA_MACRO_HPP_INCLUDED_)
