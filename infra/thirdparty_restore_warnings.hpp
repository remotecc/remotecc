
// NOTE: This is no "include once" guard for this header!
// This is intentional!

#if !defined(_RCC_THIRDPARTY_WARNINGS_DISABLED)
#error ERROR: _RCC_THIRDPARTY_WARNINGS_DISABLED is not defined. #include <thirdparty_disable_warnings.h> and <thirdparty_restore_warnings.h> mismatch!
#else
#undef _RCC_THIRDPARTY_WARNINGS_DISABLED
#endif

#ifdef _MSC_VER

// Restore all disabled warnings by #include <thirdparty_disable_warnings.h>
#pragma warning (pop)

#endif
