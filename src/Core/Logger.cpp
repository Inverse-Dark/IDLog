/**
 * @Description: 日志记录器源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:22:41
 * @LastEditTime: 2025-12-18 18:22:44
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/Logger.h"
#include "IDLog/Appender/ConsoleAppender.h"

#include <vector>
#include <mutex>

namespace IDLog
{
	struct Logger::Impl
	{
		std::string name;					  ///< 日志器名称
		std::atomic<LogLevel> level;		  ///< 当前日志级别
		std::vector<AppenderPtr> appenders; ///< 输出目标列表
		std::vector<FilterPtr> filters;	  ///< 过滤器列表
		mutable std::mutex appenderMutex;	  ///< 输出器列表互斥锁
		mutable std::mutex filterMutex;	  ///< 过滤器列表互斥锁
		bool statisticsEnabled;	  ///< 是否启用统计功能

		/// @brief 构造函数
		Impl()
			: name(""), level(LogLevel::INFO), statisticsEnabled(false) {}
	};

	Logger::Logger(const std::string& name, LogLevel level)
		: m_pImpl(new Impl)
	{
		m_pImpl->name = name;
		m_pImpl->level.store(level);
		m_pImpl->statisticsEnabled = false;

		// 默认添加一个控制台输出器
		AppenderPtr consoleAppender = std::make_shared<ConsoleAppender>();
		m_pImpl->appenders.push_back(consoleAppender);
	}

	Logger::~Logger()
	{
		delete m_pImpl;
	}

	const std::string& Logger::GetName() const
	{
		return m_pImpl->name;
	}

	LogLevel Logger::GetLevel() const
	{
		return m_pImpl->level.load();
	}

	void Logger::SetLevel(LogLevel level)
	{
		m_pImpl->level.store(level);
	}

	void Logger::AddAppender(const AppenderPtr& appender)
	{
		if (appender)
		{
			std::lock_guard<std::mutex> lock(m_pImpl->appenderMutex);
			m_pImpl->appenders.push_back(appender);
		}
	}

	void Logger::ClearAppenders()
	{
		std::lock_guard<std::mutex> lock(m_pImpl->appenderMutex);
		m_pImpl->appenders.clear();
	}

	std::vector<Logger::AppenderPtr> Logger::GetAppenders() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->appenderMutex);
		return m_pImpl->appenders;
	}

	void Logger::AddFilter(const FilterPtr& filter)
	{
		if (filter)
		{
			std::lock_guard<std::mutex> lock(m_pImpl->filterMutex);
			m_pImpl->filters.push_back(filter);
		}
	}

	void Logger::ClearFilters()
	{
		std::lock_guard<std::mutex> lock(m_pImpl->filterMutex);
		m_pImpl->filters.clear();
	}

	std::vector<Logger::FilterPtr> Logger::GetFilters() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->filterMutex);
		return m_pImpl->filters;
	}

	void Logger::EnableStatistics(bool enabled)
	{
		m_pImpl->statisticsEnabled = enabled;
	}

	bool Logger::IsStatisticsEnabled() const
	{
		return m_pImpl->statisticsEnabled;
	}

	void Logger::Log(LogLevel level, const std::string& message,
		const SourceLocation& location)
	{
		// 记录开始时间
		auto startTime = std::chrono::steady_clock::now();

		// 检查是否需要记录
		if (!ShouldLog(m_pImpl->level, level))
		{
			return;
		}

		// 创建日志事件
		LogEventPtr event = std::make_shared<LogEvent>(level, GetName(), location);
		event->SetLogMessage(message);

		// 应用过滤器
		FilterDecision decision = ApplyFilters(event);
		if (decision == FilterDecision::DENY)
		{
			return; // 被拒绝，直接返回
		}

		// 输出到所有附加的输出器
		std::lock_guard<std::mutex> lock(m_pImpl->appenderMutex);
		for (const auto& appender : m_pImpl->appenders)
		{
			appender->Append(event);
		}

		// 记录统计信息
		if (m_pImpl->statisticsEnabled)
		{
			auto endTime = std::chrono::steady_clock::now();
			auto waitTime = std::chrono::duration_cast<std::chrono::microseconds>(
				endTime - startTime)
				.count();

			// TODO: 统计逻辑（如更新计数器、记录延迟等）
		}
	}

	void Logger::Trace(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::TRACE, message, location);
	}

	void Logger::Debug(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::DBG, message, location);
	}

	void Logger::Info(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::INFO, message, location);
	}

	void Logger::Warn(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::WARN, message, location);
	}

	void Logger::Error(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::ERR, message, location);
	}

	void Logger::Fatal(const std::string& message, const SourceLocation& location)
	{
		Log(LogLevel::FATAL, message, location);
	}

	FilterDecision Logger::ApplyFilters(const LogEventPtr& event) const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->filterMutex);

		// 没有过滤器，默认接受
		if (m_pImpl->filters.empty())
		{
			return FilterDecision::ACCEPT;
		}

		// 依次应用过滤器
		for (const auto& filter : m_pImpl->filters)
		{
			// 获取过滤决策
			FilterDecision decision = filter->Decide(event);
			// 如果不是中立，直接返回决策结果
			if (decision != FilterDecision::NEUTRAL)
			{
				return decision;
			}
		}
		return FilterDecision::ACCEPT;
	}

} // namespace IDLog