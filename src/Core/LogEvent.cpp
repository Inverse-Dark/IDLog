/**
 * @Description: 日志事件源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 19:00:16
 * @LastEditTime: 2025-12-18 19:00:16
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/LogEvent.h"
#include "IDLog/Utils/ThreadUtil.h"

#include <iomanip>

namespace IDLog
{
	std::string SourceLocation::GetShortFileName() const
	{
		if (!fileName)
			return "";

		std::string fileStr(fileName);
		size_t pos = fileStr.find_last_of("/\\");
		if (pos != std::string::npos)
		{
			return fileStr.substr(pos + 1);
		}
		return fileStr;
	}

	std::string SourceLocation::ToString() const
	{
		std::stringstream ss;
		ss << GetShortFileName() << ":" << lineNumber << " in " << functionName;
		return ss.str();
	}

	/// @brief 日志事件实现结构体
	struct LogEvent::Impl
	{
		LogLevel level;		   ///< 日志级别
		std::string loggerName;  ///< 日志器名称
		SourceLocation location; ///< 源文件位置
		TimePoint time;		   ///< 时间戳
		std::string message;	   ///< 日志消息
		std::string threadId;	   ///< 线程ID
		std::string threadName;  ///< 线程名称

		/// @brief 构造函数
		Impl() {}
	};

	LogEvent::LogEvent(LogLevel level, const std::string& loggerName, const std::string& message, const SourceLocation& location)
		: m_pImpl(new Impl)
	{
		m_pImpl->level = level;
		m_pImpl->loggerName = loggerName;
		m_pImpl->location = location;
		m_pImpl->time = std::chrono::system_clock::now();
		m_pImpl->message = message;
		m_pImpl->threadId = Utils::ThreadUtil::GetThreadId();
		m_pImpl->threadName = Utils::ThreadUtil::GetThreadName();
	}

	LogEvent::~LogEvent()
	{
		delete m_pImpl;
	}

	LogLevel LogEvent::GetLevel() const
	{
		return m_pImpl->level;
	}

	const std::string& LogEvent::GetLoggerName() const
	{
		return m_pImpl->loggerName;
	}

	const SourceLocation& LogEvent::GetSourceLocation() const
	{
		return m_pImpl->location;
	}

	LogEvent::TimePoint LogEvent::GetTime() const
	{
		return m_pImpl->time;
	}

	const std::string& LogEvent::GetLogMessage() const
	{
		return m_pImpl->message;
	}

	const std::string& LogEvent::GetThreadId() const
	{
		return m_pImpl->threadId;
	}

	const std::string& LogEvent::GetThreadName() const
	{
		return m_pImpl->threadName;
	}

	void LogEvent::SetLogMessage(const std::string& message)
	{
		m_pImpl->message = message;
	}

	std::string LogEvent::GetFormattedTime(const std::string& format) const
	{
		// 将time_point转换为time_t
		std::time_t time = std::chrono::system_clock::to_time_t(m_pImpl->time);

		// 转换为tm结构
		std::tm tm;
#ifdef IDLOG_PLATFORM_WINDOWS
		localtime_s(&tm, &time);
#else
		localtime_r(&time, &tm);
#endif

		// 格式化时间
		std::stringstream ss;
		ss << std::put_time(&tm, format.c_str());
		return ss.str();
	}

	int LogEvent::GetMilliseconds() const
	{
		auto duration = m_pImpl->time.time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
			duration)
			.count() %
			1000;
		return static_cast<int>(millis);
	}

	std::string LogEvent::ToString() const
	{
		std::stringstream ss;
		ss << "LogEvent{"
			<< "level=" << LevelToString(m_pImpl->level)
			<< ", logger=" << m_pImpl->loggerName
			<< ", time=" << GetFormattedTime()
			<< ", thread=" << m_pImpl->threadId << "(" << m_pImpl->threadName << ")"
			<< ", location=" << m_pImpl->location.ToString()
			<< ", message=" << m_pImpl->message
			<< "}";
		return ss.str();
	}
} // namespace IDLog