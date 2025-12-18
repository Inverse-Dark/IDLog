/** 
 * @Description: 日志级别头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:43:00
 * @LastEditTime: 2025-12-18 18:44:08
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_LOGLEVEL_H
#define IDLOG_CORE_LOGLEVEL_H

#include "IDLog/Core/Macro.h"

#include <string>

namespace IDLog
{
	/// @brief 日志级别枚举
	/// @details 定义了6个标准日志级别，从最详细的TRACE到最严重的FATAL
	///			 级别越高，表示日志越重要，数值也越大
	enum class LogLevel : int
	{
		TRACE = 0, ///< 最详细的跟踪信息，用于调试复杂问题
		DBG = 1, ///< 调试信息，用于开发阶段
		INFO = 2,  ///< 常规信息，表示程序正常运行
		WARN = 3,  ///< 警告信息，表示可能出现问题
		ERR = 4, ///< 错误信息，表示程序出错但还能运行
		FATAL = 5, ///< 致命错误，表示程序即将崩溃
		OFF = 6	   ///< 关闭所有日志输出
	};

	/// @brief 将日志级别转换为字符串
	/// @param level [IN] 日志级别
	/// @return 对应的字符串表示
	inline const char *LevelToString(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::TRACE:
			return "TRACE";
		case LogLevel::DBG:
			return "DEBUG";
		case LogLevel::INFO:
			return "INFO";
		case LogLevel::WARN:
			return "WARN";
		case LogLevel::ERR:
			return "ERROR";
		case LogLevel::FATAL:
			return "FATAL";
		case LogLevel::OFF:
			return "OFF";
		default:
			return "UNKNOWN";
		}
	}

	/// @brief 将字符串转换为日志级别
	/// @param str [IN] 级别字符串(不区分大小写)
	/// @return 对应的日志级别，如果无法识别则返回LogLevel::INFO
	inline LogLevel StringToLevel(const std::string &str)
	{
		if (str.empty())
			return LogLevel::INFO;

		std::string upperStr;
		for (char c : str)
		{
			upperStr.push_back(static_cast<char>(std::toupper(c)));
		}

		if (upperStr == "TRACE")
			return LogLevel::TRACE;
		if (upperStr == "DEBUG")
			return LogLevel::DBG;
		if (upperStr == "INFO")
			return LogLevel::INFO;
		if (upperStr == "WARN")
			return LogLevel::WARN;
		if (upperStr == "ERROR")
			return LogLevel::ERR;
		if (upperStr == "FATAL")
			return LogLevel::FATAL;
		if (upperStr == "OFF")
			return LogLevel::OFF;

		// 无法识别时返回默认级别
		return LogLevel::INFO;
	}

	/// @brief 检查给定的日志级别是否应该被记录
	/// @details 规则: 只有当消息级别 >= 当前logger级别时，消息才会被记录
	///			 例如: 如果logger级别为WARN，那么INFO和DEBUG消息不会被记录
	/// @param currentLevel [IN] 当前logger的级别阈值
	/// @param msgLevel [IN] 要记录的日志消息级别
	/// @return true: 应该记录, false: 不应该记录
	inline bool ShouldLog(LogLevel currentLevel, LogLevel msgLevel)
	{
		return static_cast<int>(msgLevel) >= static_cast<int>(currentLevel) && currentLevel != LogLevel::OFF;
	}

} // namespace IDLog

#endif // !IDLOG_CORE_LOGLEVEL_H