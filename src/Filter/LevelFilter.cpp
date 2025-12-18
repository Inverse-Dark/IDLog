/** 
 * @Description: 级别过滤器源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 23:30:16
 * @LastEditTime: 2025-12-18 23:30:16
 * @LastEditors: InverseDark
 */
#include "IDLog/Filter/LevelFilter.h"

namespace IDLog
{
	LevelFilter::LevelFilter(LogLevel minLevel, LogLevel maxLevel, bool acceptOnMatch)
		: m_minLevel(minLevel), m_maxLevel(maxLevel), m_acceptOnMatch(acceptOnMatch)
	{
	}

	FilterDecision LevelFilter::Decide(const LogEventPtr &event)
	{
		if (!event)
		{
			return FilterDecision::NEUTRAL;
		}

		LogLevel level = event->GetLevel();
		bool match = (static_cast<int>(level) >= static_cast<int>(m_minLevel)) &&
					 (static_cast<int>(level) <= static_cast<int>(m_maxLevel));

		if (match)
		{
			return m_acceptOnMatch ? FilterDecision::ACCEPT : FilterDecision::DENY;
		}
		else
		{
			return m_acceptOnMatch ? FilterDecision::DENY : FilterDecision::ACCEPT;
		}
	}

	std::string LevelFilter::GetName() const
	{
		return "LevelFilter[" + std::string(LevelToString(m_minLevel)) +
			   "-" + std::string(LevelToString(m_maxLevel)) + "]";
	}

	Filter::Pointer LevelFilter::Clone() const
	{
		return std::make_shared<LevelFilter>(m_minLevel, m_maxLevel, m_acceptOnMatch);
	}

	void LevelFilter::SetMinLevel(LogLevel level)
	{
		m_minLevel = level;
	}

	LogLevel LevelFilter::GetMinLevel() const
	{
		return m_minLevel;
	}

	void LevelFilter::SetMaxLevel(LogLevel level)
	{
		m_maxLevel = level;
	}

	LogLevel LevelFilter::GetMaxLevel() const
	{
		return m_maxLevel;
	}

	void LevelFilter::SetAcceptOnMatch(bool accept)
	{
		m_acceptOnMatch = accept;
	}

	bool LevelFilter::GetAcceptOnMatch() const
	{
		return m_acceptOnMatch;
	}

} // namespace IDLog