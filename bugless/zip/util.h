
#pragma once

#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
# define WINDOWSBUILD
#endif

#if !defined( UNUSED )
# if defined( WINDOWSBUILD )
#  define UNUSED(x)
# else
#  define UNUSED(x) (void)x;
# endif
#endif
