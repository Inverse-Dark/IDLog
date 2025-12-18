/**
 * @Description: 线程工具头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 19:01:25
 * @LastEditTime: 2025-12-18 19:02:18
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_UTILS_THREADUTIL_H
#define IDLOG_UTILS_THREADUTIL_H

#include "IDLog/Core/Macro.h"

#include <thread>
#include <string>
#include <cstdint>

namespace IDLog
{
	namespace Utils
	{
		/// @brief 线程工具类
		/// @details 提供线程相关的工具函数，如获取线程ID和线程名称
		class IDLOG_API ThreadUtil
		{
		public:
			/// @brief 获取当前线程的ID
			/// @return 线程ID（字符串形式）
			static std::string GetThreadId();

			/// @brief 获取当前线程的ID（数值形式）
			/// @return 线程ID（数值）
			static uint64_t GetThreadIdNum();

			/// @brief 获取当前线程的名称
			/// @return 线程名称，如果未设置则返回空字符串
			static std::string GetThreadName();

			/// @brief 设置当前线程的名称
			/// @param name [IN] 线程名称
			static void SetThreadName(const std::string &name);

			/// @brief 休眠当前线程
			/// @param milliseconds [IN] 休眠毫秒数
			static void Sleep(uint64_t milliseconds);

			/// @brief 获取CPU核心数
			/// @return 系统可用的CPU核心数
			static unsigned int GetCpuCount();
		};
	} // namespace Utils
} // namespace IDLog

#endif // !IDLOG_UTILS_THREADUTIL_H