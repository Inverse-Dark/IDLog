/**
 * @Description: 统计信息源文件
 * @Author: InverseDark
 * @Date: 2025-12-23 11:05:18
 * @LastEditTime: 2025-12-23 17:54:23
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/Statistics.h"

#include <sstream>

namespace IDLog
{
	/// @brief 级别统计信息实现结构体
	struct LevelStatistics::Impl
	{
		std::atomic<uint64_t> count; ///< 日志计数
		std::atomic<uint64_t> bytes; ///< 日志字节数
		std::atomic<uint64_t> maxMessageSize; ///< 最大消息大小

		/// @brief 构造函数
		Impl() : count(0), bytes(0), maxMessageSize(0) {}

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个Impl对象
		Impl(const Impl& other)
			: count(other.count.load()),
			bytes(other.bytes.load()),
			maxMessageSize(other.maxMessageSize.load())
		{}

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个Impl对象
		/// @return 当前对象引用
		Impl& operator=(const Impl& other)
		{
			if (this != &other)
			{
				count.store(other.count.load());
				bytes.store(other.bytes.load());
				maxMessageSize.store(other.maxMessageSize.load());
			}
			return *this;
		}
	};

	LevelStatistics::LevelStatistics()
		: m_pImpl(new Impl)
	{}

	LevelStatistics::~LevelStatistics()
	{
		delete m_pImpl;
	}

	LevelStatistics::LevelStatistics(const LevelStatistics& other)
		: m_pImpl(new Impl(*other.m_pImpl))
	{
	}

	LevelStatistics& LevelStatistics::operator=(const LevelStatistics& other)
	{
		if (this != &other)
		{
			*m_pImpl = *other.m_pImpl;
		}
		return *this;
	}

	uint64_t LevelStatistics::GetCount() const
	{
		return m_pImpl->count.load(std::memory_order_relaxed);
	}

	uint64_t LevelStatistics::GetBytes() const
	{
		return m_pImpl->bytes.load(std::memory_order_relaxed);
	}

	uint64_t LevelStatistics::GetMaxMessageSize() const
	{
		return m_pImpl->maxMessageSize.load(std::memory_order_relaxed);
	}

	void LevelStatistics::IncrementCount()
	{
		m_pImpl->count.fetch_add(1, std::memory_order_relaxed);
	}

	void LevelStatistics::DecrementCount()
	{
		m_pImpl->count.fetch_sub(1, std::memory_order_relaxed);
	}

	void LevelStatistics::AddBytes(uint64_t bytes)
	{
		m_pImpl->bytes.fetch_add(bytes, std::memory_order_relaxed);
	}

	void LevelStatistics::SubtractBytes(uint64_t bytes)
	{
		m_pImpl->bytes.fetch_sub(bytes, std::memory_order_relaxed);
	}

	void LevelStatistics::UpdateMaxMessageSize(uint64_t messageSize)
	{
		LogStatistics::UpdateMax(m_pImpl->maxMessageSize, messageSize);
	}

	void LevelStatistics::Reset()
	{
		m_pImpl->count.store(0, std::memory_order_relaxed);
		m_pImpl->bytes.store(0, std::memory_order_relaxed);
		m_pImpl->maxMessageSize.store(0, std::memory_order_relaxed);
	}

	/// @brief 时间窗口实现结构体
	struct TimeWindow::Impl
	{
		std::chrono::steady_clock::time_point startTime; ///< 窗口开始时间
		uint64_t logsPerSecond; ///< 每秒日志数
		uint64_t bytesPerSecond; ///< 每秒字节数

		/// @brief 构造函数
		Impl()
			: startTime(std::chrono::steady_clock::now()),
			logsPerSecond(0), bytesPerSecond(0)
		{}

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个Impl对象
		Impl(const Impl& other)
			: startTime(other.startTime),
			logsPerSecond(other.logsPerSecond),
			bytesPerSecond(other.bytesPerSecond)
		{}

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个Impl对象
		/// @return 当前对象引用
		Impl& operator=(const Impl& other)
		{
			if (this != &other)
			{
				startTime = other.startTime;
				logsPerSecond = other.logsPerSecond;
				bytesPerSecond = other.bytesPerSecond;
			}
			return *this;
		}

	};

	TimeWindow::TimeWindow()
		: m_pImpl(new Impl)
	{}

	TimeWindow::~TimeWindow()
	{
		delete m_pImpl;
	}

	TimeWindow::TimeWindow(const TimeWindow& other)
		: m_pImpl(new Impl(*other.m_pImpl))
	{
	}

	TimeWindow& TimeWindow::operator=(const TimeWindow& other)
	{
		if (this != &other)
		{
			*m_pImpl = *other.m_pImpl;
		}
		return *this;
	}

	std::chrono::steady_clock::time_point TimeWindow::GetStartTime() const
	{
		return m_pImpl->startTime;
	}

	uint64_t TimeWindow::GetLogsPerSecond() const
	{
		return m_pImpl->logsPerSecond;
	}

	uint64_t TimeWindow::GetBytesPerSecond() const
	{
		return m_pImpl->bytesPerSecond;
	}

	void TimeWindow::SetStartTime(const std::chrono::steady_clock::time_point& timePoint)
	{
		m_pImpl->startTime = timePoint;
	}

	void TimeWindow::SetLogsPerSecond(uint64_t logsPerSecond)
	{
		m_pImpl->logsPerSecond = logsPerSecond;
	}

	void TimeWindow::SetBytesPerSecond(uint64_t bytesPerSecond)
	{
		m_pImpl->bytesPerSecond = bytesPerSecond;
	}

	void TimeWindow::Reset()
	{
		m_pImpl->startTime = std::chrono::steady_clock::now();
		m_pImpl->logsPerSecond = 0;
		m_pImpl->bytesPerSecond = 0;
	}

	/// @brief 日志统计信息实现结构体
	struct LogStatistics::Impl
	{
		LevelStatistics trace; ///< TRACE级别统计
		LevelStatistics debug; ///< DEBUG级别统计
		LevelStatistics info;  ///< INFO级别统计
		LevelStatistics warn;  ///< WARN级别统计
		LevelStatistics error; ///< ERROR级别统计
		LevelStatistics fatal; ///< FATAL级别统计

		/// 全局统计信息
		std::atomic<uint64_t> totalLogs; ///< 总日志数量
		std::atomic<uint64_t> totalBytes; ///< 总日志字节数
		std::atomic<uint64_t> droppedLogs; ///< 丢弃的日志数量
		std::atomic<uint64_t> droppedBytes; ///< 丢弃的日志字节数

		/// 性能统计信息
		std::atomic<uint64_t> totalWaitTimeUs; ///< 总等待时间（微秒）
		std::atomic<uint64_t> maxWaitTimeUs; ///< 最大等待时间（微秒）

		mutable std::mutex timeWindowMutex; ///< 互斥锁，保护时间窗口数据
		TimeWindow timeWindow; ///< 当前时间窗口
		TimeWindow lastTimeWindow; ///< 上一个时间窗口

		/// @brief 构造函数
		Impl()
			: totalLogs(0),
			totalBytes(0),
			droppedLogs(0),
			droppedBytes(0),
			totalWaitTimeUs(0),
			maxWaitTimeUs(0)
		{}

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个Impl对象
		Impl(const Impl& other)
			: trace(other.trace),
			debug(other.debug),
			info(other.info),
			warn(other.warn),
			error(other.error),
			fatal(other.fatal),
			totalLogs(other.totalLogs.load()),
			totalBytes(other.totalBytes.load()),
			droppedLogs(other.droppedLogs.load()),
			droppedBytes(other.droppedBytes.load()),
			totalWaitTimeUs(other.totalWaitTimeUs.load()),
			maxWaitTimeUs(other.maxWaitTimeUs.load()),
			timeWindowMutex(),
			timeWindow(other.timeWindow),
			lastTimeWindow(other.lastTimeWindow)
		{}

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个Impl对象
		/// @return 当前对象引用
		Impl& operator=(const Impl& other)
		{
			if (this != &other)
			{
				trace = other.trace;
				debug = other.debug;
				info = other.info;
				warn = other.warn;
				error = other.error;
				fatal = other.fatal;
				totalLogs.store(other.totalLogs.load());
				totalBytes.store(other.totalBytes.load());
				droppedLogs.store(other.droppedLogs.load());
				droppedBytes.store(other.droppedBytes.load());
				totalWaitTimeUs.store(other.totalWaitTimeUs.load());
				maxWaitTimeUs.store(other.maxWaitTimeUs.load());
				timeWindow = other.timeWindow;
				lastTimeWindow = other.lastTimeWindow;
			}
			return *this;
		}
	};

	LogStatistics::LogStatistics()
		: m_pImpl(new Impl)
	{
	}

	LogStatistics::~LogStatistics()
	{
		delete m_pImpl;
	}

	LogStatistics::LogStatistics(const LogStatistics& other)
		: m_pImpl(new Impl(*other.m_pImpl))
	{
	}

	LogStatistics& LogStatistics::operator=(const LogStatistics& other)
	{
		if (this != &other)
		{
			*m_pImpl = *other.m_pImpl;
		}
		return *this;
	}

	void LogStatistics::Reset()
	{
		m_pImpl->trace.Reset();
		m_pImpl->debug.Reset();
		m_pImpl->info.Reset();
		m_pImpl->warn.Reset();
		m_pImpl->error.Reset();
		m_pImpl->fatal.Reset();

		m_pImpl->totalLogs.store(0);
		m_pImpl->totalBytes.store(0);
		m_pImpl->droppedLogs.store(0);
		m_pImpl->droppedBytes.store(0);

		m_pImpl->totalWaitTimeUs.store(0);
		m_pImpl->maxWaitTimeUs.store(0);

		std::lock_guard<std::mutex> lock(m_pImpl->timeWindowMutex);
		m_pImpl->timeWindow.Reset();
		m_pImpl->lastTimeWindow.Reset();
	}

	LevelStatistics& LogStatistics::GetLevelStatistics(LogLevel level)
	{
		switch (level)
		{
		case LogLevel::TRACE:
			return m_pImpl->trace;
		case LogLevel::DBG:
			return m_pImpl->debug;
		case LogLevel::INFO:
			return m_pImpl->info;
		case LogLevel::WARN:
			return m_pImpl->warn;
		case LogLevel::ERR:
			return m_pImpl->error;
		case LogLevel::FATAL:
			return m_pImpl->fatal;
		default:
			return m_pImpl->info; // 默认返回info级别统计
		}
	}

	const LevelStatistics& LogStatistics::GetLevelStatistics(LogLevel level) const
	{
		switch (level)
		{
		case LogLevel::TRACE:
			return m_pImpl->trace;
		case LogLevel::DBG:
			return m_pImpl->debug;
		case LogLevel::INFO:
			return m_pImpl->info;
		case LogLevel::WARN:
			return m_pImpl->warn;
		case LogLevel::ERR:
			return m_pImpl->error;
		case LogLevel::FATAL:
			return m_pImpl->fatal;
		default:
			return m_pImpl->info; // 默认返回info级别统计
		}
	}

	uint64_t LogStatistics::GetTotalLogs() const
	{
		return m_pImpl->totalLogs.load(std::memory_order_relaxed);
	}

	uint64_t LogStatistics::GetTotalBytes() const
	{
		return m_pImpl->totalBytes.load(std::memory_order_relaxed);
	}

	uint64_t LogStatistics::GetDroppedLogs() const
	{
		return m_pImpl->droppedLogs.load(std::memory_order_relaxed);
	}

	uint64_t LogStatistics::GetDroppedBytes() const
	{
		return m_pImpl->droppedBytes.load(std::memory_order_relaxed);
	}

	void LogStatistics::RecordLog(LogLevel level, size_t messageSize, uint64_t waitTimeUs)
	{
		// 更新级别统计
		LevelStatistics& levelStats = GetLevelStatistics(level);
		levelStats.IncrementCount();
		levelStats.AddBytes(messageSize);

		// 更新最大消息大小
		if (messageSize > 0)
		{
			levelStats.UpdateMaxMessageSize(messageSize);
		}

		// 更新全局统计
		m_pImpl->totalLogs.fetch_add(1, std::memory_order_relaxed);
		m_pImpl->totalBytes.fetch_add(messageSize, std::memory_order_relaxed);

		// 更新等待时间统计
		if (waitTimeUs > 0)
		{
			m_pImpl->totalWaitTimeUs.fetch_add(waitTimeUs, std::memory_order_relaxed);
			UpdateMax(m_pImpl->maxWaitTimeUs, waitTimeUs);
		}

		// 更新时间窗口
		UpdateTimeWindow();
	}

	void LogStatistics::RecordDroppedLog(size_t messageSize)
	{
		m_pImpl->droppedLogs.fetch_add(1, std::memory_order_relaxed);
		m_pImpl->droppedBytes.fetch_add(messageSize, std::memory_order_relaxed);
	}

	void LogStatistics::UpdateTimeWindow()
	{
		std::lock_guard<std::mutex> lock(m_pImpl->timeWindowMutex);

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_pImpl->timeWindow.GetStartTime()).count();

		// 每秒钟更新一次时间窗口统计
		if (elapsed >= 1)
		{
			// 更新时间窗口
			m_pImpl->lastTimeWindow = m_pImpl->timeWindow;
			m_pImpl->timeWindow.Reset();

			// 计算当前时间窗口的日志速率
			if (elapsed > 0)
			{
				m_pImpl->lastTimeWindow.SetLogsPerSecond(m_pImpl->totalLogs.load(std::memory_order_relaxed) / elapsed);
				m_pImpl->lastTimeWindow.SetBytesPerSecond(m_pImpl->totalBytes.load(std::memory_order_relaxed) / elapsed);
			}
		}
	}

	std::string LogStatistics::ToString() const
	{
		std::stringstream ss;

		ss << "Log Statistics:\n";
		ss << "  Total Logs: " << m_pImpl->totalLogs.load() << "\n";
		ss << "  Total Bytes: " << m_pImpl->totalBytes.load() << " ("
			<< (m_pImpl->totalBytes.load() / 1024.0 / 1024.0) << " MB)\n";
		ss << "  Dropped Logs: " << m_pImpl->droppedLogs.load() << "\n";
		ss << "  Dropped Bytes: " << m_pImpl->droppedBytes.load() << " ("
			<< (m_pImpl->droppedBytes.load() / 1024.0 / 1024.0) << " MB)\n";
		ss << "  Total Wait Time: " << m_pImpl->totalWaitTimeUs.load() << " us\n";
		ss << "  Max Wait Time: " << m_pImpl->maxWaitTimeUs.load() << " us\n";

		ss << "  By Level Statistics:\n";
		ss << "    TRACE: Count=" << m_pImpl->trace.GetCount()
			<< ", Bytes=" << m_pImpl->trace.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->trace.GetMaxMessageSize() << "\n";
		ss << "    DEBUG: Count=" << m_pImpl->debug.GetCount()
			<< ", Bytes=" << m_pImpl->debug.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->debug.GetMaxMessageSize() << "\n";
		ss << "    INFO: Count=" << m_pImpl->info.GetCount()
			<< ", Bytes=" << m_pImpl->info.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->info.GetMaxMessageSize() << "\n";
		ss << "    WARN: Count=" << m_pImpl->warn.GetCount()
			<< ", Bytes=" << m_pImpl->warn.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->warn.GetMaxMessageSize() << "\n";
		ss << "    ERROR: Count=" << m_pImpl->error.GetCount()
			<< ", Bytes=" << m_pImpl->error.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->error.GetMaxMessageSize() << "\n";
		ss << "    FATAL: Count=" << m_pImpl->fatal.GetCount()
			<< ", Bytes=" << m_pImpl->fatal.GetBytes()
			<< ", MaxMessageSize=" << m_pImpl->fatal.GetMaxMessageSize() << "\n";

		{
			std::lock_guard<std::mutex> lock(m_pImpl->timeWindowMutex);
			if (m_pImpl->lastTimeWindow.GetLogsPerSecond() > 0 || m_pImpl->lastTimeWindow.GetBytesPerSecond() > 0)
			{
				ss << "  Recent Rates:\n";
				ss << "    Logs/Second: " << m_pImpl->lastTimeWindow.GetLogsPerSecond() << "\n";
				ss << "    Bytes/Second: " << m_pImpl->lastTimeWindow.GetBytesPerSecond() << "\n";
			}
		}

		return ss.str();
	}

	std::string LogStatistics::ToJson(bool pretty) const
	{
		std::stringstream ss;

		if (pretty)
		{
			ss << "{\n";
			ss << "  \"total_logs\": " << m_pImpl->totalLogs.load() << ",\n";
			ss << "  \"total_bytes\": " << m_pImpl->totalBytes.load() << ",\n";
			ss << "  \"dropped_logs\": " << m_pImpl->droppedLogs.load() << ",\n";
			ss << "  \"dropped_bytes\": " << m_pImpl->droppedBytes.load() << ",\n";
			ss << "  \"total_wait_time_us\": " << m_pImpl->totalWaitTimeUs.load() << ",\n";
			ss << "  \"max_wait_time_us\": " << m_pImpl->maxWaitTimeUs.load() << ",\n";
			ss << "  \"levels\": {\n";
			ss << "    \"TRACE\": {\"count\": " << m_pImpl->trace.GetCount() << ", \"bytes\": " << m_pImpl->trace.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->trace.GetMaxMessageSize() << "},\n";
			ss << "    \"DEBUG\": {\"count\": " << m_pImpl->debug.GetCount() << ", \"bytes\": " << m_pImpl->debug.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->debug.GetMaxMessageSize() << "},\n";
			ss << "    \"INFO\": {\"count\": " << m_pImpl->info.GetCount() << ", \"bytes\": " << m_pImpl->info.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->info.GetMaxMessageSize() << "},\n";
			ss << "    \"WARN\": {\"count\": " << m_pImpl->warn.GetCount() << ", \"bytes\": " << m_pImpl->warn.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->warn.GetMaxMessageSize() << "},\n";
			ss << "    \"ERROR\": {\"count\": " << m_pImpl->error.GetCount() << ", \"bytes\": " << m_pImpl->error.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->error.GetMaxMessageSize() << "},\n";
			ss << "    \"FATAL\": {\"count\": " << m_pImpl->fatal.GetCount() << ", \"bytes\": " << m_pImpl->fatal.GetBytes()
				<< ", \"max_message_size\": " << m_pImpl->fatal.GetMaxMessageSize() << "}\n";
			ss << "  },\n";

			{
				std::lock_guard<std::mutex> lock(m_pImpl->timeWindowMutex);
				ss << "  \"recent_rates\": {\n";
				ss << "    \"logs_per_second\": " << m_pImpl->lastTimeWindow.GetLogsPerSecond() << ",\n";
				ss << "    \"bytes_per_second\": " << m_pImpl->lastTimeWindow.GetBytesPerSecond() << "\n";
				ss << "  }\n";
			}
			ss << "}";
		}
		else
		{
			ss << "{";
			ss << "\"total_logs\":" << m_pImpl->totalLogs.load() << ",";
			ss << "\"total_bytes\":" << m_pImpl->totalBytes.load() << ",";
			ss << "\"dropped_logs\":" << m_pImpl->droppedLogs.load() << ",";
			ss << "\"dropped_bytes\":" << m_pImpl->droppedBytes.load() << ",";
			ss << "\"total_wait_time_us\":" << m_pImpl->totalWaitTimeUs.load() << ",";
			ss << "\"max_wait_time_us\":" << m_pImpl->maxWaitTimeUs.load() << ",";
			ss << "\"levels\":{";
			ss << "\"TRACE\":{\"count\":" << m_pImpl->trace.GetCount() << ",\"bytes\":" << m_pImpl->trace.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->trace.GetMaxMessageSize() << "},";
			ss << "\"DEBUG\":{\"count\":" << m_pImpl->debug.GetCount() << ",\"bytes\":" << m_pImpl->debug.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->debug.GetMaxMessageSize() << "},";
			ss << "\"INFO\":{\"count\":" << m_pImpl->info.GetCount() << ",\"bytes\":" << m_pImpl->info.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->info.GetMaxMessageSize() << "},";
			ss << "\"WARN\":{\"count\":" << m_pImpl->warn.GetCount() << ",\"bytes\":" << m_pImpl->warn.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->warn.GetMaxMessageSize() << "},";
			ss << "\"ERROR\":{\"count\":" << m_pImpl->error.GetCount() << ",\"bytes\":" << m_pImpl->error.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->error.GetMaxMessageSize() << "},";
			ss << "\"FATAL\":{\"count\":" << m_pImpl->fatal.GetCount() << ",\"bytes\":" << m_pImpl->fatal.GetBytes()
				<< ",\"max_message_size\":" << m_pImpl->fatal.GetMaxMessageSize() << "}";
			ss << "},";
			{
				std::lock_guard<std::mutex> lock(m_pImpl->timeWindowMutex);
				ss << "\"recent_rates\":{";
				ss << "\"logs_per_second\":" << m_pImpl->lastTimeWindow.GetLogsPerSecond() << ",";
				ss << "\"bytes_per_second\":" << m_pImpl->lastTimeWindow.GetBytesPerSecond();
				ss << "}";
			}
			ss << "}";
		}

		ss << '\n';

		return ss.str();
	}

	void LogStatistics::UpdateMax(std::atomic<uint64_t>& maxValue, uint64_t newValue)
	{
		uint64_t currentMax = maxValue.load(std::memory_order_relaxed);
		while (newValue > currentMax)
		{
			if (maxValue.compare_exchange_weak(currentMax, newValue, std::memory_order_relaxed, std::memory_order_relaxed))
			{
				break;
			}
		}
	}

	/// @brief 统计管理器实现结构体
	struct StatisticsManager::Impl
	{
		mutable std::mutex mutex; ///< 互斥锁，保护统计数据
		std::atomic<bool> enabled; ///< 是否启用统计

		LogStatistics globalStats; ///< 全局统计数据
		std::map<std::string, LogStatistics> loggerStats; ///< 日志器统计数据

		std::function<void(const std::string&)> reportCallback; ///< 统计报告回调函数
		std::chrono::steady_clock::time_point lastReportTime; ///< 上次报告时间
		uint64_t statisticsInterval; ///< 日志统计间隔，单位秒，默认60秒

		/// @brief 构造函数
		Impl() : enabled(false), lastReportTime(std::chrono::steady_clock::now()), statisticsInterval(60) {}
	};

	StatisticsManager::StatisticsManager()
		: m_pImpl(new Impl)
	{
		m_pImpl->globalStats.Reset();
	}

	StatisticsManager::~StatisticsManager()
	{
		delete m_pImpl;
	}

	StatisticsManager& StatisticsManager::GetInstance()
	{
		static StatisticsManager instance;
		return instance;
	}

	void StatisticsManager::RecordLog(const std::string& loggerName, LogLevel level,
		size_t messageSize, uint64_t waitTimeUs)
	{
		if (!m_pImpl->enabled.load())
		{
			return;
		}

		// 更新全局统计
		m_pImpl->globalStats.RecordLog(level, messageSize, waitTimeUs);

		{
			std::lock_guard<std::mutex> lock(m_pImpl->mutex);
			// 更新日志器统计
			auto& loggerStat = m_pImpl->loggerStats[loggerName];
			loggerStat.RecordLog(level, messageSize, waitTimeUs);
		}

		// 检查是否需要报告统计信息
		CheckReport();
	}

	void StatisticsManager::RecordDroppedLog(const std::string& loggerName, size_t messageSize)
	{
		if (!m_pImpl->enabled.load())
		{
			return;
		}

		// 更新全局统计
		m_pImpl->globalStats.RecordDroppedLog(messageSize);

		{
			std::lock_guard<std::mutex> lock(m_pImpl->mutex);
			// 更新日志器统计
			auto& loggerStat = m_pImpl->loggerStats[loggerName];
			loggerStat.RecordDroppedLog(messageSize);
		}

		// 检查是否需要报告统计信息
		CheckReport();
	}

	LogStatistics StatisticsManager::GetGlobalStatistics() const
	{
		return m_pImpl->globalStats;
	}

	LogStatistics StatisticsManager::GetLoggerStatistics(const std::string& loggerName) const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		auto it = m_pImpl->loggerStats.find(loggerName);
		if (it != m_pImpl->loggerStats.end())
		{
			return it->second;
		}
		return LogStatistics();
	}

	std::map<std::string, LogStatistics> StatisticsManager::GetAllLoggerStatistics() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		return m_pImpl->loggerStats;
	}

	void StatisticsManager::ResetLoggerStatistics(const std::string& loggerName)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		auto it = m_pImpl->loggerStats.find(loggerName);
		if (it != m_pImpl->loggerStats.end())
		{
			it->second.Reset();
		}
	}

	void StatisticsManager::ResetAllStatistics()
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->globalStats.Reset();
		for (auto& pair : m_pImpl->loggerStats)
		{
			pair.second.Reset();
		}
	}

	void StatisticsManager::EnableStatistics(bool enabled)
	{
		m_pImpl->enabled.store(enabled);
		if (enabled)
		{
			ResetAllStatistics();
		}
	}

	bool StatisticsManager::IsStatisticsEnabled() const
	{
		return m_pImpl->enabled.load();
	}

	void StatisticsManager::SetStatisticsInterval(uint64_t intervalSeconds)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->statisticsInterval = intervalSeconds;
	}

	uint64_t StatisticsManager::GetStatisticsInterval() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		return m_pImpl->statisticsInterval;
	}

	void StatisticsManager::RegisterReportCallback(const std::function<void(const std::string&)>& callback)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->reportCallback = callback;
	}

	std::string StatisticsManager::GenerateReport() const
	{
		std::stringstream ss;
		ss << "=== IDLog Statistics Report ===\n\n";
		ss << "Global Statistics:\n";
		{
			std::lock_guard<std::mutex> lock(m_pImpl->mutex);
			ss << m_pImpl->globalStats.ToString() << "\n";

			if (!m_pImpl->loggerStats.empty())
			{
				ss << "Logger Statistics(" << m_pImpl->loggerStats.size() << " loggers):\n";
				for (const auto& [name, stats] : m_pImpl->loggerStats)
				{
					if (stats.GetTotalLogs() > 0 || stats.GetDroppedLogs() > 0)
					{
						ss << "Logger: " << name << "\n";
						ss << stats.ToString() << "\n";
					}
				}
			}
		}
		ss << "\n=== End of Report ===\n";

		return ss.str();
	}

	void StatisticsManager::CheckReport()
	{
		if (!m_pImpl->enabled.load())
		{
			return;
		}

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_pImpl->lastReportTime).count();

		// 检查是否达到报告间隔
		if (static_cast<uint64_t>(elapsed) >= m_pImpl->statisticsInterval)
		{
			std::string report = GenerateReport();
			std::lock_guard<std::mutex> lock(m_pImpl->mutex);
			if (m_pImpl->reportCallback)
			{
				m_pImpl->reportCallback(report);
				m_pImpl->lastReportTime = now;
			}
		}
	}

} // namespace IDLog