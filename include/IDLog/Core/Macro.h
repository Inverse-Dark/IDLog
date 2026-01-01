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
	|| defined(IDLOG_STATIC_LIBRARY)
#undef IDLOG_API
#define IDLOG_API
#endif // IDLOG_BUILD_SHARED

#endif // !IDLOG_CORE_MACRO_H