/**
 * @Description:
 * @Author: InverseDark
 * @Date: 2025-12-19 12:13:16
 * @LastEditTime: 2025-12-19 12:46:27
 * @LastEditors: InverseDark
 */
#include "IDLog/Appender/FileAppender.h"
#include "IDLog/Formatter/PatternFormatter.h"

#include <fstream>
#include <iomanip>
#include <ctime>

#ifdef IDLOG_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace IDLog
{
	/// @brief 文件输出器实现结构体
	struct FileAppender::Impl
	{
		std::string filename;	 ///< 文件名
		std::ofstream fstream; ///< 文件流
		std::vector<char> fileBuffer; ///< 文件缓冲区
		RollPolicy rollPolicy; ///< 滚动策略
		size_t maxSize;		 ///< 最大文件大小
		size_t currentFileSize;	 ///< 当前文件大小
		int lastRollTime;		 ///< 上次滚动的时间

		/// @brief 构造函数
		Impl()
			: currentFileSize(0), lastRollTime(0)
		{
			// 初始化文件缓冲区为64KB
			fileBuffer.resize(64 * 1024);
		}
	};

	FileAppender::FileAppender(const std::string& filename,
		FormatterPtr formatter,
		RollPolicy rollPolicy,
		size_t maxSize)
		: m_pImpl(new Impl)
	{
		m_pImpl->filename = filename;
		m_pImpl->rollPolicy = rollPolicy;
		m_pImpl->maxSize = maxSize;

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

		Open();
	}

	FileAppender::~FileAppender()
	{
		Close();
		delete m_pImpl;
	}

	void FileAppender::Append(const LogEventPtr& event)
	{
		if (!event)
		{
			return;
		}

		// 格式化日志消息
		std::string formattedMessage;
		if (auto formatter = GetFormatter())
		{
			formattedMessage = formatter->Format(event);
		}
		else
		{
			formattedMessage = event->GetLogMessage();
		}

		std::lock_guard<std::mutex> lock(m_mutex);

		// 检查是否需要滚动
		if (ShouldRoll(event))
		{
			RollFile(event);
		}

		// 确保文件已打开
		if (!m_pImpl->fstream.is_open())
		{
			return;
		}

		// 写入文件
		m_pImpl->fstream << formattedMessage;
		m_pImpl->currentFileSize += formattedMessage.size();
	}

	std::string FileAppender::GetName() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return "FileAppender(" + m_pImpl->filename + ")";
	}

	void FileAppender::Flush()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_pImpl->fstream.is_open())
		{
			m_pImpl->fstream.flush();
		}
	}

	bool FileAppender::Open()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return OpenNoLock();
	}

	void FileAppender::Close()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		CloseNoLock();
	}

	bool FileAppender::IsOpen() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->fstream.is_open();
	}

	std::string FileAppender::GetFilename() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->filename;
	}

	void FileAppender::SetFilename(const std::string& filename)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_pImpl->filename != filename)
		{
			m_pImpl->filename = filename;
			OpenNoLock();
		}
	}

	size_t FileAppender::GetCurrentFileSize() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->currentFileSize;
	}

	void FileAppender::SetRollPolicy(RollPolicy policy)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->rollPolicy = policy;
		m_pImpl->lastRollTime = GetCurrentTimeMarker();
	}

	FileAppender::RollPolicy FileAppender::GetRollPolicy() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->rollPolicy;
	}

	void FileAppender::SetMaxFileSize(size_t maxSize)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->maxSize = maxSize;
	}

	size_t FileAppender::GetMaxFileSize() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->maxSize;
	}

	bool FileAppender::ShouldRoll(const LogEventPtr& event)
	{
		if (m_pImpl->rollPolicy == RollPolicy::NONE)
		{
			return false;
		}

		if (m_pImpl->rollPolicy == RollPolicy::SIZE)
		{
			return m_pImpl->currentFileSize >= m_pImpl->maxSize;
		}

		// 基于时间的滚动
		int currentTimeMarker = GetCurrentTimeMarker();
		if (currentTimeMarker != m_pImpl->lastRollTime)
		{
			m_pImpl->lastRollTime = currentTimeMarker;
			return true;
		}

		return false;
	}

	void FileAppender::RollFile(const LogEventPtr& event)
	{
		// 关闭当前文件
		if (m_pImpl->fstream.is_open())
		{
			m_pImpl->fstream.close();
		}

		// 生成新的文件名
		std::string rolledFilename = GenerateRolledFilename(event);
		if (!rolledFilename.empty())
		{
			// 重命名当前文件
			std::error_code ec;
			std::filesystem::rename(m_pImpl->filename, rolledFilename, ec);
			if (ec)
			{
				// TODO: 处理重命名失败的情况
			}
		}

		// 打开新文件
		OpenNoLock();
	}

	std::string FileAppender::GenerateRolledFilename(const LogEventPtr& event)
	{
		if (m_pImpl->rollPolicy == RollPolicy::NONE)
		{
			return "";
		}

		// 根据当前时间生成时间戳
		auto now_tp = event ? event->GetTime() : std::chrono::system_clock::now();
		std::time_t now = std::chrono::system_clock::to_time_t(now_tp);
		std::tm tm;

#ifdef IDLOG_PLATFORM_WINDOWS
		localtime_s(&tm, &now);
#else
		localtime_r(&now, &tm);
#endif

		std::stringstream ss;
		ss << m_pImpl->filename << ".";

		switch (m_pImpl->rollPolicy)
		{
		case RollPolicy::YEARLY:
			ss << std::put_time(&tm, "%Y");
			break;
		case RollPolicy::MONTHLY:
			ss << std::put_time(&tm, "%Y%m");
			break;
		case RollPolicy::DAILY:
			ss << std::put_time(&tm, "%Y%m%d");
			break;
		case RollPolicy::HOURLY:
			ss << std::put_time(&tm, "%Y%m%d%H");
			break;
		case RollPolicy::MINUTELY:
			ss << std::put_time(&tm, "%Y%m%d%H%M");
			break;
		case RollPolicy::SIZE:
			ss << std::put_time(&tm, "%Y%m%d_%H%M%S") << "_tmp" << rand() % 10000;
			break;
		default:
			return "";
		}
		return ss.str();
	}

	int FileAppender::GetCurrentTimeMarker() const
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		std::tm tm;

#ifdef IDLOG_PLATFORM_WINDOWS
		localtime_s(&tm, &now_c);
#else
		localtime_r(&now_c, &tm);
#endif

		switch (m_pImpl->rollPolicy)
		{
		case RollPolicy::YEARLY:
			// 返回年份作为时间标记
			return tm.tm_year;
		case RollPolicy::MONTHLY:
			// 返回年月作为时间标记
			return tm.tm_year * 100 + tm.tm_mon;
		case RollPolicy::DAILY:
			// 返回年月日作为时间标记
			return tm.tm_year * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;
		case RollPolicy::HOURLY:
			// 返回年月日小时作为时间标记
			return tm.tm_year * 1000000 + (tm.tm_mon + 1) * 10000 + tm.tm_mday * 100 + tm.tm_hour;
		case RollPolicy::MINUTELY:
			// 返回年月日小时分钟作为时间标记
			return tm.tm_year * 100000000 + (tm.tm_mon + 1) * 1000000 + tm.tm_mday * 10000 + tm.tm_hour * 100 + tm.tm_min;
		default:
			return 0;
		}
	}

	int FileAppender::GetFileTimeMarker(const std::filesystem::path& filePath) const
	{
		try
		{
			auto ftime = std::filesystem::last_write_time(filePath);
			auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
			std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
			std::tm tm;

#ifdef IDLOG_PLATFORM_WINDOWS
			localtime_s(&tm, &cftime);
#else
			localtime_r(&cftime, &tm);
#endif

			switch (m_pImpl->rollPolicy)
			{
			case RollPolicy::YEARLY:
				return tm.tm_year;
			case RollPolicy::MONTHLY:
				return tm.tm_year * 100 + tm.tm_mon;
			case RollPolicy::DAILY:
				return tm.tm_year * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;
			case RollPolicy::HOURLY:
				return tm.tm_year * 1000000 + (tm.tm_mon + 1) * 10000 + tm.tm_mday * 100 + tm.tm_hour;
			case RollPolicy::MINUTELY:
				return tm.tm_year * 100000000 + (tm.tm_mon + 1) * 1000000 + tm.tm_mday * 10000 + tm.tm_hour * 100 + tm.tm_min;
			default:
				return 0;
			}
		}
		catch (...)
		{
			return 0;
		}
	}

	bool FileAppender::OpenNoLock()
	{
		// 先关闭已打开的文件
		CloseNoLock();

		// 检查目录是否存在，不存在则创建
		std::filesystem::path filePath(m_pImpl->filename);
		if (filePath.has_parent_path())
		{
			std::filesystem::create_directories(filePath.parent_path());
		}

		// 检查现有文件是否需要滚动（仅限时间策略）
		if (m_pImpl->rollPolicy != RollPolicy::SIZE && m_pImpl->rollPolicy != RollPolicy::NONE)
		{
			if (std::filesystem::exists(filePath))
			{
				int fileTimeMarker = GetFileTimeMarker(filePath);
				int currentTimeMarker = GetCurrentTimeMarker();
				if (fileTimeMarker != 0 && fileTimeMarker != currentTimeMarker)
				{
					// 文件已过期，执行滚动
					RollFile(nullptr); // 传递nullptr因为我们没有具体的事件
				}
			}
		}

		// 设置缓冲区
		m_pImpl->fstream.rdbuf()->pubsetbuf(m_pImpl->fileBuffer.data(), m_pImpl->fileBuffer.size());

		// 打开文件
		m_pImpl->fstream.open(m_pImpl->filename, std::ios::app | std::ios::out);
		if (!m_pImpl->fstream.is_open())
		{
			return false;
		}

		// 获取当前文件大小
		m_pImpl->fstream.seekp(0, std::ios::end);
		m_pImpl->currentFileSize = static_cast<size_t>(m_pImpl->fstream.tellp());

		// 初始化滚动时间
		m_pImpl->lastRollTime = GetCurrentTimeMarker();
		return true;
	}

	void FileAppender::CloseNoLock()
	{
		if (m_pImpl->fstream.is_open())
		{
			m_pImpl->fstream.close();
		}
		m_pImpl->currentFileSize = 0;
		m_pImpl->lastRollTime = 0;
	}

} // namespace IDLog