/** 
 * @Description: JSON 格式化器源文件
 * @Author: InverseDark
 * @Date: 2025-12-22 13:56:52
 * @LastEditTime: 2025-12-22 15:37:11
 * @LastEditors: InverseDark
 */
#include "IDLog/Formatter/JsonFormatter.h"
#include "IDLog/Utils/StringUtil.h"

#include <iomanip>

namespace IDLog
{

	JsonFormatter::JsonFormatter(bool prettyPrint)
		: m_prettyPrint(prettyPrint)
	{
	}

	std::string JsonFormatter::Format(const LogEventPtr &event)
	{
		if (!event)
			return "{}";

		std::stringstream ss;

		if (m_prettyPrint)
		{
			ss << "{\n";
			ss << "  \"timestamp\": \"" << event->GetFormattedTime() << "."
			   << std::setfill('0') << std::setw(3) << event->GetMilliseconds() << "\",\n";
			ss << "  \"level\": \"" << LevelToString(event->GetLevel()) << "\",\n";
			ss << "  \"logger\": \"" << EscapeJson(event->GetLoggerName()) << "\",\n";
			ss << "  \"thread\": \"" << event->GetThreadId() << "\",\n";
			ss << "  \"thread_name\": \"" << EscapeJson(event->GetThreadName()) << "\",\n";
			ss << "  \"file\": \"" << EscapeJson(event->GetSourceLocation().GetShortFileName()) << "\",\n";
			ss << "  \"function\": \"" << EscapeJson(event->GetSourceLocation().functionName) << "\",\n";
			ss << "  \"line\": " << event->GetSourceLocation().lineNumber << ",\n";
			ss << "  \"message\": \"" << EscapeJson(event->GetLogMessage()) << "\"\n";
			ss << "}\n";
		}
		else
		{
			ss << "{";
			ss << "\"timestamp\":\"" << event->GetFormattedTime() << "."
			   << std::setfill('0') << std::setw(3) << event->GetMilliseconds() << "\",";
			ss << "\"level\":\"" << LevelToString(event->GetLevel()) << "\",";
			ss << "\"logger\":\"" << EscapeJson(event->GetLoggerName()) << "\",";
			ss << "\"thread\":\"" << event->GetThreadId() << "\",";
			ss << "\"thread_name\":\"" << EscapeJson(event->GetThreadName()) << "\",";
			ss << "\"file\":\"" << EscapeJson(event->GetSourceLocation().GetShortFileName()) << "\",";
			ss << "\"function\":\"" << EscapeJson(event->GetSourceLocation().functionName) << "\",";
			ss << "\"line\":" << event->GetSourceLocation().lineNumber << ",";
			ss << "\"message\":\"" << EscapeJson(event->GetLogMessage()) << "\"";
			ss << "}\n";
		}

		return ss.str();
	}

	Formatter::Pointer JsonFormatter::Clone() const
	{
		return std::make_shared<JsonFormatter>(m_prettyPrint);
	}

	std::string JsonFormatter::EscapeJson(const std::string &str) const
	{
		return Utils::StringUtil::Escape(str);
	}

} // namespace IDLog