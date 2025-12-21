/**
 * @Description: 控制台输出器源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:21:37
 * @LastEditTime: 2025-12-18 21:34:57
 * @LastEditors: InverseDark
 */
#include "IDLog/Appender/ConsoleAppender.h"
#include "IDLog/Formatter/PatternFormatter.h"

#include <iostream>

namespace IDLog
{
	ConsoleAppender::ConsoleAppender(Target target, FormatterPtr formatter)
		: m_target(target), m_useColor(true)
	{
		if (formatter)
		{
			SetFormatter(formatter);
		}
		else
		{
			// 如果没有提供格式化器，使用默认的模式格式化器
			FormatterPtr defaultFormatter = std::make_shared<PatternFormatter>(
				PatternFormatter::defaultPattern());
			SetFormatter(defaultFormatter);
		}
	}

	void ConsoleAppender::Append(const LogEventPtr &event)
	{
		if (!event)
		{
			return;
		}

		// 线程安全输出
		std::lock_guard<std::mutex> lock(m_mutex);

		// 格式化日志消息
		std::string formattedMessage;
		if (auto formatter = GetFormatterNoLock())
		{
			formattedMessage = formatter->Format(event);
		}
		else
		{
			formattedMessage = event->GetLogMessage();
		}

		// 添加颜色
		if (m_useColor)
		{
			formattedMessage = GetLevelColor(event->GetLevel()) +
							   formattedMessage +
							   GetResetColor();
		}

		// 输出到对应的流
		std::ostream &os = (m_target == Target::STDOUT) ? std::cout : std::cerr;
		os << formattedMessage;

		// 确保立即输出
		os.flush();
	}

	std::string ConsoleAppender::GetName() const
	{
		return (m_target == Target::STDOUT) ? "ConsoleAppender(stdout)" : "ConsoleAppender(stderr)";
	}

	void ConsoleAppender::Flush()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_target == Target::STDOUT)
		{
			std::cout.flush();
		}
		else
		{
			std::cerr.flush();
		}
	}

	void ConsoleAppender::SetTarget(Target target)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_target = target;
	}

	ConsoleAppender::Target ConsoleAppender::GetTarget() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_target;
	}

	void ConsoleAppender::SetUseColor(bool useColor)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_useColor = useColor;
	}

	bool ConsoleAppender::GetUseColor() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_useColor;
	}

	std::string ConsoleAppender::GetLevelColor(LogLevel level) const
	{
		if (!m_useColor)
			return "";

		// ANSI颜色代码
		switch (level)
		{
		case LogLevel::TRACE:
			return "\033[37m"; // 白色
		case LogLevel::DBG:
			return "\033[36m"; // 青色
		case LogLevel::INFO:
			return "\033[32m"; // 绿色
		case LogLevel::WARN:
			return "\033[33m"; // 黄色
		case LogLevel::ERR:
			return "\033[31m"; // 红色
		case LogLevel::FATAL:
			return "\033[35m"; // 洋红色
		default:
			return "";
		}
	}

	std::string ConsoleAppender::GetResetColor() const
	{
		if (!m_useColor)
			return "";
		return "\033[0m";
	}

} // namespace IDLog