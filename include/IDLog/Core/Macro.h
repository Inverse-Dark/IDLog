/**
 * @Description: 宏定义头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:31:22
 * @LastEditTime: 2025-12-18 18:31:25
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_MACRO_H
#define IDLOG_CORE_MACRO_H

// 平台检测
#if !defined(IDLOG_PLATFORM_WINDOWS) && !defined(IDLOG_PLATFORM_LINUX) && !defined(IDLOG_PLATFORM_MACOS)
#if defined(_WIN32)
#define IDLOG_PLATFORM_WINDOWS
#elif defined(__linux__)
#define IDLOG_PLATFORM_LINUX
#elif defined(__APPLE__)
#define IDLOG_PLATFORM_MACOS
#endif
#endif // !IDLOG_PLATFORM_WINDOWS && !IDLOG_PLATFORM_LINUX && !IDLOG_PLATFORM_MACOS

// 动态库导出/导入定义
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

// 静态库构建时取消导出/导入定义
#if (defined(IDLOG_BUILD_STATIC) && !defined(IDLOG_BUILD_SHARED)) || defined(IDLOG_STATIC_LIBRARY)
#undef IDLOG_API
#define IDLOG_API
#endif // (defined(IDLOG_BUILD_STATIC) && !defined(IDLOG_BUILD_SHARED)) || defined(IDLOG_STATIC_LIBRARY)

#endif // !IDLOG_CORE_MACRO_H