
#ifndef GOOGLE_GLOG_DLL_DECL_H
#define GOOGLE_GLOG_DLL_DECL_H

#ifdef GLOG_STATIC_DEFINE
#  define GOOGLE_GLOG_DLL_DECL
#  define GLOG_NO_EXPORT
#else
#  ifndef GOOGLE_GLOG_DLL_DECL
#    ifdef GOOGLE_GLOG_IS_A_DLL
        /* We are building this library */
#      define GOOGLE_GLOG_DLL_DECL __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define GOOGLE_GLOG_DLL_DECL __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef GLOG_NO_EXPORT
#    define GLOG_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef GLOG_DEPRECATED
#  define GLOG_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef GLOG_DEPRECATED_EXPORT
#  define GLOG_DEPRECATED_EXPORT GOOGLE_GLOG_DLL_DECL GLOG_DEPRECATED
#endif

#ifndef GLOG_DEPRECATED_NO_EXPORT
#  define GLOG_DEPRECATED_NO_EXPORT GLOG_NO_EXPORT GLOG_DEPRECATED
#endif

#define DEFINE_NO_DEPRECATED 0
#if DEFINE_NO_DEPRECATED
# define GLOG_NO_DEPRECATED
#endif

#endif
