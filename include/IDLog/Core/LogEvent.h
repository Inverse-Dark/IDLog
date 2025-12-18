/**
 * @Description: 日志事件头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:45:19
 * @LastEditTime: 2025-12-18 18:47:41
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_LOGEVENT_H
#define IDLOG_CORE_LOGEVENT_H

#include "IDLog/Core/LogLevel.h"

#include <sstream>
#include <chrono>
#include <memory>

namespace IDLog
{
	/// @brief 源文件位置信息
	/// @details 用于记录日志调用所在的源文件、函数和行号
	struct IDLOG_API SourceLocation
	{
		const char *fileName;	  ///< 源文件名
		const char *functionName; ///< 函数名
		int lineNumber;			  ///< 行号

		/// @brief 构造函数
		/// @param file [IN] 文件名，通常使用__FILE__
		/// @param function [IN] 函数名，通常使用__FUNCTION__或__func__
		/// @param line [IN] 行号，通常使用__LINE__
		SourceLocation(const char *file = "", const char *function = "", int line = 0)
			: fileName(file), functionName(function), lineNumber(line) {}

		/// @brief 获取简短的文件名（去掉路径）
		/// @return 简短的文件名
		std::string GetShortFileName() const;

		/// @brief 获取完整的源位置信息
		/// @return 完整的源位置信息字符串
		std::string ToString() const;
	};

	/// @brief 日志事件类
	/// @details 封装了一次日志记录的所有相关信息，包括：
	///			 1. 日志级别
	///			 2. 日志器名称
	///			 3. 源文件位置
	///			 4. 时间戳
	///			 5. 日志消息
	///			 6. 线程ID
	///			 7. 线程名称
	class IDLOG_API LogEvent
	{
	public:
		using TimePoint = std::chrono::system_clock::time_point;
		using Pointer = std::shared_ptr<LogEvent>;
	public:
		/// @brief 构造函数
		/// @param level [IN] 日志级别
		/// @param loggerName [IN] 日志器名称
		/// @param location [IN] 源文件位置
		LogEvent(LogLevel level, const std::string &loggerName, const SourceLocation &location = SourceLocation());

		/// @brief 析构函数
		~LogEvent();

		/// @brief 获取日志级别
		/// @return 日志级别
		LogLevel GetLevel() const;

		/// @brief 获取日志器名称
		/// @return 日志器名称
		const std::string &GetLoggerName() const;

		/// @brief 获取源文件位置信息
		/// @return 源文件位置信息
		const SourceLocation &GetSourceLocation() const;

		/// @brief 获取时间戳
		/// @return 时间戳
		TimePoint GetTime() const;

		/// @brief 获取日志消息
		/// @return 日志消息
		const std::string &GetLogMessage() const;

		/// @brief 获取线程ID
		/// @return 线程ID
		const std::string &GetThreadId() const;

		/// @brief 获取线程名称
		/// @return 线程名称
		const std::string &GetThreadName() const;

		/// @brief 设置日志消息
		/// @param message [IN] 日志消息
		void SetLogMessage(const std::string &message);

		/// @brief 获取格式化的时间字符串
		/// @param format [IN] 时间格式，默认为"%Y-%m-%d %H:%M:%S"
		/// @return 格式化的时间字符串
		std::string GetFormattedTime(const std::string &format = "%Y-%m-%d %H:%M:%S") const;

		/// @brief 获取时间戳的毫秒部分
		/// @return 毫秒部分（0-999）
		int GetMilliseconds() const;

		/// @brief 获取日志事件的完整字符串表示
		/// @return 日志事件字符串
		std::string ToString() const;

	private:
		/// @brief 日志事件实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl;	///< 日志事件实现指针
	};

} // namespace IDLog

#endif // !IDLOG_CORE_LOGEVENT_H