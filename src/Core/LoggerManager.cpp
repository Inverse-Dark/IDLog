/**
 * @Description: 日志管理器源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 22:37:22
 * @LastEditTime: 2025-12-18 22:39:50
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/LoggerManager.h"

#include <unordered_map>

namespace IDLog
{
	/// @brief 日志管理器实现结构体
	struct LoggerManager::Impl
	{
		LoggerPtr m_rootLogger;								  ///< 根日志器
		std::unordered_map<std::string, LoggerPtr> m_loggers; ///< 日志器映射表
	};

	LoggerManager::LoggerManager()
		: m_pImpl(new Impl)
	{
		// 创建根日志器
		m_pImpl->m_rootLogger = std::make_shared<Logger>("ROOT");
		m_pImpl->m_loggers["ROOT"] = m_pImpl->m_rootLogger;
	}

	LoggerManager::~LoggerManager()
	{
		delete m_pImpl;
	}

	LoggerManager &LoggerManager::GetInstance()
	{
		static LoggerManager instance;
		return instance;
	}

	LoggerManager::LoggerPtr LoggerManager::GetLogger(const std::string &name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// 查找已有的日志器
		auto it = m_pImpl->m_loggers.find(name);
		if (it != m_pImpl->m_loggers.end())
		{
			return it->second;
		}

		// 创建新的日志器
		LoggerPtr newLogger = std::make_shared<Logger>(name);

		// 设置继承关系：如果有父级日志器，则继承其级别
		size_t pos = name.find_last_of('.');
		if (pos != std::string::npos)
		{
			std::string parentName = name.substr(0, pos);
			auto parentIt = m_pImpl->m_loggers.find(parentName);
			if (parentIt != m_pImpl->m_loggers.end())
			{
				newLogger->SetLevel(parentIt->second->GetLevel());
			}
		}
		else
		{
			// 没有父级，继承根日志器级别
			newLogger->SetLevel(m_pImpl->m_rootLogger->GetLevel());
		}
		// 存储新日志器
		m_pImpl->m_loggers[name] = newLogger;
		return newLogger;
	}

	void LoggerManager::AddLogger(const std::string &name, const LoggerPtr &logger)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->m_loggers[name] = logger;
	}

	bool LoggerManager::HasLogger(const std::string &name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->m_loggers.find(name) != m_pImpl->m_loggers.end();
	}

	void LoggerManager::SetLoggerLevel(const std::string &name, LogLevel level)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_pImpl->m_loggers.find(name);
		if (it != m_pImpl->m_loggers.end())
		{
			it->second->SetLevel(level);
		}
	}

	void LoggerManager::RemoveLogger(const std::string &name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->m_loggers.erase(name);
	}

	LoggerManager::LoggerPtr LoggerManager::GetRootLogger()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->m_rootLogger;
	}

	void LoggerManager::SetRootLevel(LogLevel level)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_pImpl->m_rootLogger)
		{
			m_pImpl->m_rootLogger->SetLevel(level);
		}
	}

	void LoggerManager::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->m_loggers.clear();
		// 重新创建根日志器
		m_pImpl->m_rootLogger = std::make_shared<Logger>("ROOT");
		m_pImpl->m_loggers["ROOT"] = m_pImpl->m_rootLogger;
	}

} // namespace IDLog