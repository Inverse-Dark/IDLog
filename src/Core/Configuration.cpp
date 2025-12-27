/**
 * @Description: 配置源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 11:55:37
 * @LastEditTime: 2025-12-21 11:57:23
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/Configuration.h"
#include "IDLog/Core/Statistics.h"
#include "IDLog/Core/LoggerManager.h"
#include "IDLog/Utils/ConfigParseUtil.h"

#include <thread>
#include <atomic>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

namespace IDLog
{
	bool Configuration::Options::Validate() const
	{
		// 验证全局配置选项
		if (global.enableStatistics && global.statisticsInterval == 0)
		{
			return false; // 启用统计时，统计间隔不能为0
		}

		for (const auto& [loggerName, loggerOpts] : loggers)
		{
			// 验证所有引用的Filter都存在
			for (const auto& filterName : loggerOpts.filters)
			{
				if (filters.find(filterName) == filters.end())
				{
					return false;
				}
			}

			// 验证所有引用的Appender都存在
			for (const auto& appenderName : loggerOpts.appenders)
			{
				if (appenders.find(appenderName) == appenders.end())
				{
					return false;
				}
			}
		}

		// 验证所有引用的Formatter都存在
		for (const auto& [appenderName, appenderOpts] : appenders)
		{
			if (!appenderOpts.formatter.empty() &&
				formatters.find(appenderOpts.formatter) == formatters.end())
			{
				return false;
			}
		}

		return true;
	}

	void Configuration::Options::Clear()
	{
		loggers.clear();
		filters.clear();
		appenders.clear();
		formatters.clear();
		global = Options::Global();
	}

	/// @brief 配置实现结构体
	struct Configuration::Impl
	{
		Options options;					 ///< 配置选项
		LogFactory::Pointer factory;			 ///< IDLog工厂实例
		Utils::ConfigParseUtil configParser; ///< 配置解析工具
		std::mutex mutex;					 ///< 互斥锁

		// 自动重新加载
		std::thread autoReloadThread;		 ///< 自动重新加载线程
		std::atomic<bool> autoReloadEnabled; ///< 是否启用自动重新加载
		std::atomic<bool> stopAutoReload;	 ///< 停止自动重新加载标志
		uint64_t reloadInterval;			 ///< 重新加载间隔(秒)
		std::time_t lastModTime;			 ///< 上次修改时间

		bool applied; ///< 标记配置是否已应用

		Impl()
			: factory(std::make_shared<LogFactory>()),
			  autoReloadEnabled(false),
			  stopAutoReload(false),
			  reloadInterval(60),
			  lastModTime(0),
			  applied(false)
		{
		}
	};

	Configuration::Configuration()
		: m_pImpl(new Impl)
	{
	}

	Configuration::~Configuration()
	{
		EnableAutoReload(false);
		delete m_pImpl;
	}

	Configuration& Configuration::GetInstance()
	{
		static Configuration instance;
		return instance;
	}

	bool Configuration::LoadFromFile(const std::string& filename)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		try
		{
			// 使用配置解析工具加载配置文件
			if (!m_pImpl->configParser.LoadFromFile(filename))
			{
				return false;
			}
			// 获取文件修改时间
			auto ftime = fs::last_write_time(filename);
			auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
			m_pImpl->lastModTime = std::chrono::system_clock::to_time_t(sctp);
		}
		catch (...)
		{
			m_pImpl->lastModTime = 0;
		}

		return UpdateOptionsFromParser();
	}

	bool Configuration::LoadFromString(const std::string& content)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		// 使用配置解析工具加载配置内容
		if (!m_pImpl->configParser.LoadFromString(content))
		{
			return false;
		}

		// 重置修改时间
		m_pImpl->lastModTime = 0;

		return UpdateOptionsFromParser();
	}

	bool Configuration::SaveToFile(const std::string& filename)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		// 先更新配置解析器内容
		UpdateParserFromOptions();
		// 使用配置解析工具保存配置到文件
		return m_pImpl->configParser.SaveToFile(filename);
	}

	bool Configuration::Reload()
	{
		std::string filename;
		{
			std::lock_guard<std::mutex> lock(m_pImpl->mutex);
			filename = m_pImpl->configParser.GetFilename();
			if (filename.empty())
			{
				return false; // 无配置文件名，无法重新加载
			}
		}
		// 重新加载配置文件
		return LoadFromFile(filename);
	}

	const Configuration::Options& Configuration::GetOptions() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		return m_pImpl->options;
	}

	bool Configuration::ApplyOptions(const Options& options)
	{
		if (!options.Validate())
		{
			return false;
		}
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->options = options;
		UpdateParserFromOptions();
		return ApplyOptions();
	}

	void Configuration::SetFactory(const LogFactory::Pointer& factory)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->factory = factory;
	}

	void Configuration::RegisterChangeCallback(const std::function<void()>& callback)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->configParser.RegisterChangeCallback(callback);
	}

	Filter::Pointer Configuration::CreateFilterFromConfig(const Options::FilterConfig& config)
	{
		if (m_pImpl->factory)
		{
			return m_pImpl->factory->CreateFilter(config.type, config.params);
		}

		return nullptr;
	}

	LogAppender::Pointer Configuration::CreateAppenderFromConfig(const Options::AppenderConfig& config)
	{
		if (m_pImpl->factory)
		{
			Options::FormatterConfig fmtConfig;
			if (!config.formatter.empty() &&
				m_pImpl->options.formatters.find(config.formatter) != m_pImpl->options.formatters.end())
			{
				fmtConfig = m_pImpl->options.formatters.at(config.formatter);
			}

			return m_pImpl->factory->CreateLogAppender(config.type, config.params, fmtConfig.type,
				fmtConfig.params);
		}

		return nullptr;
	}

	Formatter::Pointer Configuration::CreateFormatterFromConfig(const Options::FormatterConfig& config)
	{
		if (m_pImpl->factory)
		{
			return m_pImpl->factory->CreateFormatter(config.type, config.params);
		}

		return nullptr;
	}

	void Configuration::EnableAutoReload(bool enable, uint64_t intervalSeconds)
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		m_pImpl->autoReloadEnabled = enable;
		m_pImpl->reloadInterval = intervalSeconds;

		if (enable && !m_pImpl->autoReloadThread.joinable())
		{
			m_pImpl->stopAutoReload = false;
			m_pImpl->autoReloadThread = std::thread(&Configuration::AutoReloadThread, this);
		}
		else if (!enable && m_pImpl->autoReloadThread.joinable())
		{
			m_pImpl->stopAutoReload = true;
			m_pImpl->autoReloadThread.join();
		}
	}

	bool Configuration::IsAutoReloadEnabled() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		return m_pImpl->autoReloadEnabled;
	}

	bool Configuration::IsApplied() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->mutex);
		return m_pImpl->applied;
	}

	void Configuration::AutoReloadThread()
	{
		while (!m_pImpl->stopAutoReload)
		{
			std::this_thread::sleep_for(std::chrono::seconds(m_pImpl->reloadInterval));

			std::string filename;
			{
				std::lock_guard<std::mutex> lock(m_pImpl->mutex);
				filename = m_pImpl->configParser.GetFilename();
				if (filename.empty())
				{
					continue; // 无配置文件名，跳过
				}
			}

			try
			{
				auto ftime = fs::last_write_time(filename);
				auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
					ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
				std::time_t modTime = std::chrono::system_clock::to_time_t(sctp);

				if (modTime > m_pImpl->lastModTime)
				{
					// 文件已修改，重新加载配置
					LoadFromFile(filename);
				}
			}
			catch (...)
			{
				// 文件可能被删除或无法访问，忽略错误
			}
		}
	}

	void Configuration::UpdateParserFromOptions()
	{
		// 将当前配置选项更新到配置解析器
		Utils::ConfigParseUtil& parser = m_pImpl->configParser;

		// 全局配置
		parser.SetString("global", "rootLevel", LevelToString(m_pImpl->options.global.rootLevel));
		parser.SetBool("global", "enableStatistics", m_pImpl->options.global.enableStatistics);
		parser.SetInt("global", "statisticsInterval", static_cast<int>(m_pImpl->options.global.statisticsInterval));

		// Filter 配置
		for (const auto& [name, filterOpts] : m_pImpl->options.filters)
		{
			std::string section = "filter." + name;
			parser.SetString(section, "type", filterOpts.type);

			// 其他过滤器选项
			for (const auto& [key, value] : filterOpts.params)
			{
				parser.SetString(section, key, value);
			}
		}

		// Formatter 配置
		for (const auto& [name, fmtOpts] : m_pImpl->options.formatters)
		{
			std::string section = "formatter." + name;
			parser.SetString(section, "type", fmtOpts.type);

			// 其他格式化器选项
			for (const auto& [key, value] : fmtOpts.params)
			{
				parser.SetString(section, key, value);
			}
		}

		// Appender 配置
		for (const auto& [name, appenderOpts] : m_pImpl->options.appenders)
		{
			std::string section = "appender." + name;
			parser.SetString(section, "type", appenderOpts.type);

			if (!appenderOpts.formatter.empty())
			{
				parser.SetString(section, "formatter", appenderOpts.formatter);
			}

			// 其他输出器选项
			for (const auto& [key, value] : appenderOpts.params)
			{
				parser.SetString(section, key, value);
			}
		}

		// Logger 配置
		for (const auto& [name, loggerOpts] : m_pImpl->options.loggers)
		{
			std::string section = "logger." + name;
			parser.SetString(section, "level", LevelToString(loggerOpts.level));
			parser.SetBool(section, "additive", loggerOpts.additive);

			// 输出器列表
			if (!loggerOpts.appenders.empty())
			{
				std::string appenderList;
				for (const auto& appenderName : loggerOpts.appenders)
				{
					if (!appenderList.empty())
					{
						appenderList += ",";
					}
					appenderList += appenderName;
				}
				parser.SetString(section, "appenders", appenderList);
			}

			// 过滤器列表
			if (!loggerOpts.filters.empty())
			{
				std::string filterList;
				for (const auto& filterName : loggerOpts.filters)
				{
					if (!filterList.empty())
					{
						filterList += ",";
					}
					filterList += filterName;
				}
				parser.SetString(section, "filters", filterList);
			}
		}
	}

	bool Configuration::UpdateOptionsFromParser()
	{
		// 解析配置解析器内容到当前配置选项
		Utils::ConfigParseUtil& parser = m_pImpl->configParser;
		Options newOptions;
		// 全局配置
		newOptions.global.rootLevel = parser.GetLogLevel("global", "rootLevel", LogLevel::INFO);
		newOptions.global.enableStatistics = parser.GetBool("global", "enableStatistics", false);
		newOptions.global.statisticsInterval = static_cast<uint64_t>(parser.GetInt("global", "statisticsInterval", 60));

		for (const auto& section : parser.GetSections())
		{
			// 解析 Filter 配置
			if (Utils::StringUtil::StartsWith(section, "filter."))
			{
				std::string filterName = section.substr(7); // 去掉"filter."前缀
				Options::FilterConfig filterConfig;
				filterConfig.type = parser.GetString(section, "type", "");

				// 其他过滤器选项
				for (const auto& key : parser.GetKeys(section))
				{
					if (key != "type")
					{
						filterConfig.params[key] = parser.GetString(section, key, "");
					}
				}

				newOptions.filters[filterName] = filterConfig;
			}
			// 解析 Formatter 配置
			else if (Utils::StringUtil::StartsWith(section, "formatter."))
			{
				std::string fmtName = section.substr(10); // 去掉"formatter."前缀
				Options::FormatterConfig fmtConfig;
				fmtConfig.type = parser.GetString(section, "type", "");

				// 其他格式化器选项
				for (const auto& key : parser.GetKeys(section))
				{
					if (key != "type")
					{
						fmtConfig.params[key] = parser.GetString(section, key, "");
					}
				}

				newOptions.formatters[fmtName] = fmtConfig;
			}
			// 解析 Appender 配置
			else if (Utils::StringUtil::StartsWith(section, "appender."))
			{
				std::string appenderName = section.substr(9); // 去掉"appender."前缀
				Options::AppenderConfig appenderConfig;
				appenderConfig.type = parser.GetString(section, "type", "");
				appenderConfig.formatter = parser.GetString(section, "formatter", "");

				// 其他输出器选项
				for (const auto& key : parser.GetKeys(section))
				{
					if (key != "type" && key != "formatter")
					{
						appenderConfig.params[key] = parser.GetString(section, key, "");
					}
				}

				newOptions.appenders[appenderName] = appenderConfig;
			}
			// 解析 Logger 配置
			else if (Utils::StringUtil::StartsWith(section, "logger."))
			{
				std::string loggerName = section.substr(7); // 去掉"logger."前缀
				Options::LoggerConfig loggerConfig;
				loggerConfig.level = parser.GetLogLevel(section, "level", newOptions.global.rootLevel);
				loggerConfig.additive = parser.GetBool(section, "additive", true);

				// 输出器列表
				std::string appenderList = parser.GetString(section, "appenders", "");
				if (!appenderList.empty())
				{
					auto appenderNames = Utils::StringUtil::Split(appenderList, ",");
					for (auto& appenderName : appenderNames)
					{
						loggerConfig.appenders.push_back(Utils::StringUtil::Trim(appenderName));
					}
				}

				// 过滤器列表
				std::string filterList = parser.GetString(section, "filters", "");
				if (!filterList.empty())
				{
					auto filterNames = Utils::StringUtil::Split(filterList, ",");
					for (auto& filterName : filterNames)
					{
						loggerConfig.filters.push_back(Utils::StringUtil::Trim(filterName));
					}
				}

				newOptions.loggers[loggerName] = loggerConfig;
			}
		}

		// 验证新配置选项
		if (!newOptions.Validate())
		{
			return false;
		}
		m_pImpl->options = std::move(newOptions);

		// 应用新配置选项
		m_pImpl->applied = ApplyOptions();

		return m_pImpl->applied;
	}

	bool Configuration::ApplyOptions()
	{
		auto& loggerMgr = LoggerManager::GetInstance();
		std::map<std::string, LoggerManager::LoggerPtr> oldLoggers; // 备份旧日志器指针
		std::vector<std::string> newCreatedLoggers;					// 新创建的日志器名称列表
		bool allSuccess = true;

		// 设置统计启用
		StatisticsManager::GetInstance().EnableStatistics(m_pImpl->options.global.enableStatistics);
		// 设置统计间隔
		StatisticsManager::GetInstance().SetStatisticsInterval(m_pImpl->options.global.statisticsInterval);

		// 设置根日志级别
		loggerMgr.SetRootLevel(m_pImpl->options.global.rootLevel);
		loggerMgr.GetRootLogger()->EnableStatistics(m_pImpl->options.global.enableStatistics);

		for (const auto& [loggerName, loggerOpts] : m_pImpl->options.loggers)
		{
			// 检查日志器是否已存在
			if (loggerMgr.HasLogger(loggerName))
			{
				oldLoggers[loggerName] = loggerMgr.GetLogger(loggerName);
			}
			else
			{
				newCreatedLoggers.push_back(loggerName); // 记录新创建的日志器名称
			}

			auto logger = loggerMgr.GetLogger(loggerName);
			if (!logger)
			{
				allSuccess = false;
				break;
			}

			// 设置日志级别
			logger->SetLevel(loggerOpts.level);

			// 清除现有的Appender和Filter
			logger->ClearAppenders();
			logger->ClearFilters();

			// 添加新的Appender
			for (const auto& appenderName : loggerOpts.appenders)
			{
				auto appenderIt = m_pImpl->options.appenders.find(appenderName);
				if (appenderIt != m_pImpl->options.appenders.end())
				{
					auto appender = CreateAppenderFromConfig(appenderIt->second);
					if (appender)
					{
						logger->AddAppender(appender);
					}
					else
					{
						allSuccess = false;
						break;
					}
				}
			}
			if (!allSuccess)
			{
				break;
			}

			// 添加新的Filter
			for (const auto& filterName : loggerOpts.filters)
			{
				auto filterIt = m_pImpl->options.filters.find(filterName);
				if (filterIt != m_pImpl->options.filters.end())
				{
					auto filter = CreateFilterFromConfig(filterIt->second);
					if (filter)
					{
						logger->AddFilter(filter);
					}
					else
					{
						allSuccess = false;
						break;
					}
				}
			}
			if (!allSuccess)
			{
				break;
			}

			// 设置统计开关
			logger->EnableStatistics(m_pImpl->options.global.enableStatistics);
		}

		if (!allSuccess)
		{
			// 回滚已创建的日志器
			for (const auto& loggerName : newCreatedLoggers)
			{
				loggerMgr.RemoveLogger(loggerName);
			}
			// 恢复旧日志器
			for (const auto& [loggerName, loggerPtr] : oldLoggers)
			{
				loggerMgr.AddLogger(loggerName, loggerPtr);
			}
		}

		return allSuccess;
	}

} // namespace IDLog