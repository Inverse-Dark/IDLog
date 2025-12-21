/** 
 * @Description: Log 工厂源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 13:04:37
 * @LastEditTime: 2025-12-21 14:43:08
 * @LastEditors: InverseDark
 */
#include "IDLog/Core/LogFactory.h"

#include "IDLog/Appender/ConsoleAppender.h"
#include "IDLog/Appender/FileAppender.h"
#include "IDLog/Formatter/PatternFormatter.h"
#include "IDLog/Filter/LevelFilter.h"

namespace IDLog
{

	LogFactory::LogFactory() = default;

	LogFactory::~LogFactory() = default;

	LogAppender::Pointer LogFactory::CreateLogAppender(const std::string &type, std::map<std::string, std::string> params,
													   const std::string &formatter, std::map<std::string, std::string> formatterParams)
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
			return std::make_shared<ConsoleAppender>(target, fmtPtr);
		}
		else if (type == "file")	// 创建文件输出器
		{
			Formatter::Pointer fmtPtr = nullptr;	// 格式化器指针
			if (!formatter.empty())
			{
				fmtPtr = CreateFormatter(formatter, formatterParams);
			}
			std::string filename = "default.log";	// 默认文件名
			if (params.find("filename") != params.end())
			{
				filename = params.at("filename");
			}
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
			size_t maxSize = 10 * 1024 * 1024; // 默认最大文件大小10MB
			if (params.find("maxSize") != params.end())
			{
				maxSize = std::stoull(params.at("maxSize"));
			}
			return std::make_shared<FileAppender>(filename, fmtPtr, rollPolicy, maxSize);
		}
		return nullptr;
	}

	Formatter::Pointer LogFactory::CreateFormatter(const std::string &type, std::map<std::string, std::string> params)
	{
		// 这里可以根据type创建不同类型的Formatter实例
		// 例如，如果type是"Pattern"，则创建PatternFormatter实例
		// 具体实现根据实际需求进行扩展
		if (type == "pattern")
		{
			std::string pattern = PatternFormatter::defaultPattern();	// 默认模式字符串
			if (params.find("pattern") != params.end())
			{
				pattern = params.at("pattern");
			}
			return std::make_shared<PatternFormatter>(pattern);
		}
		return nullptr;
	}

	Filter::Pointer LogFactory::CreateFilter(const std::string &type, std::map<std::string, std::string> params)
	{
		// 这里可以根据type创建不同类型的Filter实例
		// 例如，如果type是"Level"，则创建LevelFilter实例
		// 具体实现根据实际需求进行扩展
		if (type == "level")
		{
			LogLevel minLevel = LogLevel::TRACE;	// 默认最小级别TRACE
			LogLevel maxLevel = LogLevel::FATAL;	// 默认最大级别FATAL
			if (params.find("minLevel") != params.end() && params.find("maxLevel") != params.end())
			{
				minLevel = StringToLevel(params.at("minLevel"));
				maxLevel = StringToLevel(params.at("maxLevel"));
			}
			bool acceptOnMatch = true;	// 默认匹配时接受
			if (params.find("acceptOnMatch") != params.end())
			{
				acceptOnMatch = (params.at("acceptOnMatch") == "true");
			}
			return std::make_shared<LevelFilter>(minLevel, maxLevel, acceptOnMatch);
		}
		return nullptr;
	}

} // namespace IDLog