#pragma once

 // Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define KPLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#define KPLATFORM_LINUX 1
#if defined(__ANDROID__)
#define KPLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
// Catch anything not caught by the above.
#define KPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
// Posix
#define KPLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#define KPLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#define KPLATFORM_IOS 1
#define KPLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define KPLATFORM_IOS 1
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef IG_EXPORT
// Exports
#ifdef _MSC_VER
#define IG_API __declspec(dllexport)
#else
#define IG_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
/** @brief Import/export qualifier */
#define IG_API __declspec(dllimport)
#else
/** @brief Import/export qualifier */
#define IG_API
#endif
#endif
