#if !defined(_INFRA_MACRO_HPP_INCLUDED_)
#define _INFRA_MACRO_HPP_INCLUDED_

#define INFRA_DISABLE_COPY_CONSTRUCTOR(_Class_) \
    _Class_(const _Class_&) = delete; \
    _Class_& operator=(const _Class_&) = delete

#define INFRA_DISABLE_MOVE_CONSTRUCTOR(_Class_) \
    _Class_(_Class_&&) = delete; \
    _Class_& operator=(_Class_&&) = delete


#define JUST(...)   __VA_ARGS__


// Detect IDE for friendlier intellisense
// __INTELLISENSE__: See https://blogs.msdn.microsoft.com/vcblog/2011/03/29/troubleshooting-tips-for-intellisense-slowness
// __RESHARPER__: See https://www.jetbrains.com/help/resharper/ReSharper_by_Language_CPP.html
#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
#define INFRA_IDE_MODE 1
#else  // defined(__INTELLISENSE__) || defined(__RESHARPER__)
#define INFRA_IDE_MODE 0
#endif  // defined(__INTELLISENSE__) || defined(__RESHARPER__)

#endif  // !defined(_INFRA_MACRO_HPP_INCLUDED_)
