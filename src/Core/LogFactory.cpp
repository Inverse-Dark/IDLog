/** 
 * @Description: Log 工厂源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 13:04:37
 * @LastEditTime: 2025-12-25 22:14:19
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/LogFactory.h"

#include "IDLog/Appender/ConsoleAppender.h"
#include "IDLog/Appender/FileAppender.h"
#include "IDLog/Appender/AsyncAppender.h"
#include "IDLog/Formatter/PatternFormatter.h"
#include "IDLog/Formatter/JsonFormatter.h"
#include "IDLog/Filter/LevelFilter.h"

#include "IDLog/Utils/ConfigParseUtil.h"
#include "IDLog/Utils/StringUtil.h"

namespace IDLog
{
	LogFactory::LogFactory() = default;

	LogFactory::~LogFactory() = default;

	LogAppender::Pointer LogFactory::CreateLogAppender(const std::string &type, const std::map<std::string, std::string>& params,
													   const std::string &formatter, const std::map<std::string, std::string>& formatterParams)
	{
		// 这里可以根据type创建不同类型的LogAppender实例
		// 例如，如果type是"Console"，则创建ConsoleAppender实例
		// 具体实现根据实际需求进行扩展
		if (type == "console")	// 创建控制台输出器
		{
			Formatter::Pointer fmtPtr = nullptr;	// 格式化器指针
			if (!formatter.empty())
			{
				fmtPtr = CreateFormatter(formatter, formatterParams);
			}
			ConsoleAppender::Target target = ConsoleAppender::Target::STDOUT;	// 默认输出到标准输出
			if (params.find("target") != params.end())
			{
				std::string targetStr = params.at("target");
				if (targetStr == "stderr")
				{
					target = ConsoleAppender::Target::STDERR;
				}
			}
			auto appenderPtr = std::make_shared<ConsoleAppender>(target, fmtPtr);

			bool useColor = Utils::ConfigParseUtil::GetBool(params, "useColor", true);	// 默认使用颜色
			appenderPtr->SetUseColor(useColor);
			
			return appenderPtr;
		}
		else if (type == "file")	// 创建文件输出器
		{
			Formatter::Pointer fmtPtr = nullptr;	// 格式化器指针
			if (!formatter.empty())
			{
				fmtPtr = CreateFormatter(formatter, formatterParams);
			}
			std::string filename = Utils::ConfigParseUtil::GetString(params, "filename", "default.log"); // 默认文件名default.log
			FileAppender::RollPolicy rollPolicy = FileAppender::RollPolicy::NONE; // 默认不滚动
			if (params.find("rollPolicy") != params.end())
			{
				std::string policyStr = params.at("rollPolicy");
				if (policyStr == "yearly")
				{
					rollPolicy = FileAppender::RollPolicy::YEARLY;
				}
				else if (policyStr == "monthly")
				{
					rollPolicy = FileAppender::RollPolicy::MONTHLY;
				}
				else if (policyStr == "daily")
				{
					rollPolicy = FileAppender::RollPolicy::DAILY;
				}
				else if (policyStr == "hourly")
				{
					rollPolicy = FileAppender::RollPolicy::HOURLY;
				}
				else if (policyStr == "minutely")
				{
					rollPolicy = FileAppender::RollPolicy::MINUTELY;
				}
				else if (policyStr == "size")
				{
					rollPolicy = FileAppender::RollPolicy::SIZE;
				}
			}
			size_t maxSize = Utils::ConfigParseUtil::GetInt(params, "maxSize", 10 * 1024 * 1024); // 默认最大文件大小10MB
			return std::make_shared<FileAppender>(filename, fmtPtr, rollPolicy, maxSize);
		}
		else if (type == "async")	// 创建异步输出器
		{
			std::string backendType = Utils::ConfigParseUtil::GetString(params, "backendType", "console"); // 默认后端类型console
			std::map<std::string, std::string> backendParams;
			// 提取后端参数
			for (const auto& [key, value] : params)
			{
				if (Utils::StringUtil::StartsWith(key, "backend."))
				{
					std::string backendKey = key.substr(8); // 去掉"backend."前缀
					if(!backendKey.empty())
					{
						backendParams[backendKey] = value;
					}
				}
			}

			// 创建后端输出器
			auto backendAppender = CreateLogAppender(backendType, backendParams, formatter, formatterParams);
			if(!backendAppender)
			{
				return nullptr;	// 创建后端输出器失败
			}

			// 异步输出器参数
			size_t queueCapacity = Utils::ConfigParseUtil::GetInt(params, "queueCapacity", 10000); // 默认队列容量10000
			size_t batchSize = Utils::ConfigParseUtil::GetInt(params, "batchSize", 0); // 默认批量处理大小0（表示不批量处理）
			uint64_t flushIntervalMs = static_cast<uint64_t>(Utils::ConfigParseUtil::GetInt(params, "flushIntervalMs", 1000)); // 默认刷新间隔1000毫秒
			AsyncAppender::OverflowPolicy overflowPolicy = AsyncAppender::OverflowPolicy::BLOCK; // 默认阻塞策略
			if (params.find("overflowPolicy") != params.end())
			{
				std::string policyStr = params.at("overflowPolicy");
				if (policyStr == "drop_oldest")
				{
					overflowPolicy = AsyncAppender::OverflowPolicy::DROP_OLDEST;
				}
				else if (policyStr == "drop_newest")
				{
					overflowPolicy = AsyncAppender::OverflowPolicy::DROP_NEWEST;
				}
			}
			auto asyncAppender = std::make_shared<AsyncAppender>(backendAppender, queueCapacity, batchSize, flushIntervalMs, overflowPolicy);

			// 自动启动
			bool autoStart = Utils::ConfigParseUtil::GetBool(params, "autoStart", true); // 默认自动启动
			if (autoStart)
			{
				uint32_t threadCount = static_cast<uint32_t>(Utils::ConfigParseUtil::GetInt(params, "threadCount", 1)); // 默认线程数1
				asyncAppender->Start(threadCount);
			}

			return asyncAppender;
		}
		return nullptr;
	}

	Formatter::Pointer LogFactory::CreateFormatter(const std::string &type, const std::map<std::string, std::string>& params)
	{
		// 这里可以根据type创建不同类型的Formatter实例
		// 例如，如果type是"Pattern"，则创建PatternFormatter实例
		// 具体实现根据实际需求进行扩展
		if (type == "pattern")
		{
			std::string pattern = Utils::ConfigParseUtil::GetString(params, "pattern", PatternFormatter::defaultPattern());	// 默认模式字符串
			return std::make_shared<PatternFormatter>(pattern);
		}
		else if (type == "json")
		{
			bool prettyPrint = Utils::ConfigParseUtil::GetBool(params, "pretty", true);	// 默认美化输出
			return std::make_shared<JsonFormatter>(prettyPrint);
		}
		return nullptr;
	}

	Filter::Pointer LogFactory::CreateFilter(const std::string &type, const std::map<std::string, std::string>& params)
	{
		// 这里可以根据type创建不同类型的Filter实例
		// 例如，如果type是"Level"，则创建LevelFilter实例
		// 具体实现根据实际需求进行扩展
		if (type == "level")
		{
			LogLevel minLevel = Utils::ConfigParseUtil::GetLogLevel(params, "minLevel", LogLevel::TRACE);	// 默认最小级别TRACE
			LogLevel maxLevel = Utils::ConfigParseUtil::GetLogLevel(params, "maxLevel", LogLevel::FATAL);	// 默认最大级别FATAL
			bool acceptOnMatch = Utils::ConfigParseUtil::GetBool(params, "acceptOnMatch", true);	// 默认匹配时接受
			return std::make_shared<LevelFilter>(minLevel, maxLevel, acceptOnMatch);
		}
		return nullptr;
	}

} // namespace IDLog