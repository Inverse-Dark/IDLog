/**
 * @Description: 宏定义头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:31:22
 * @LastEditTime: 2025-12-18 18:31:25
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_MACRO_H
#define IDLOG_CORE_MACRO_H

#if defined(IDLOG_PLATFORM_WINDOWS)
#if defined(IDLOG_BUILD_SHARED)
#define IDLOG_API __declspec(dllexport)
#else
#define IDLOG_API __declspec(dllimport)
#endif // IDLOG_BUILD_SHARED
#elif defined(IDLOG_PLATFORM_LINUX) || defined(IDLOG_PLATFORM_MACOS)
#if defined(IDLOG_BUILD_SHARED)
#define IDLOG_API __attribute__((visibility("default")))
#else
#define IDLOG_API
#endif // IDLOG_BUILD_SHARED
#else
#define IDLOG_API
#endif // IDLOG_PLATFORM_WINDOWS

#if (defined(IDLOG_BUILD_STATIC) && !defined(IDLOG_BUILD_SHARED)) \
	|| defined(IDLOG_STATIC_LIB)
#undef IDLOG_API
#define IDLOG_API
#endif // IDLOG_BUILD_SHARED

// 主版本号定义
#ifndef IDLOG_VERSION_MAJOR
#define IDLOG_VERSION_MAJOR 0
#endif // !IDLOG_VERSION_MAJOR
// 次版本号定义
#ifndef IDLOG_VERSION_MINOR
#define IDLOG_VERSION_MINOR 0
#endif // !IDLOG_VERSION_MINOR
// 修订号定义
#ifndef IDLOG_VERSION_PATCH
#define IDLOG_VERSION_PATCH 0
#endif // !IDLOG_VERSION_PATCH

// 版本字符串定义
#ifndef IDLOG_VERSION_STRING
#define IDLOG_VERSION_STRING "0.0.0"
#endif // !IDLOG_VERSION_STRING

#endif // !IDLOG_CORE_MACRO_H