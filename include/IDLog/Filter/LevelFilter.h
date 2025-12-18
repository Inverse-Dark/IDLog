/** 
 * @Description: 级别过滤器头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 23:28:22
 * @LastEditTime: 2025-12-18 23:29:48
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_FILTER_LEVELFILTER_H
#define IDLOG_FILTER_LEVELFILTER_H

#include "IDLog/Filter/Filter.h"

namespace IDLog
{
	/// @brief 级别过滤器
	/// @details 根据日志级别进行过滤
	class IDLOG_API LevelFilter : public Filter
	{
	public:
		/// @brief 构造函数
		/// @param minLevel [IN] 最小级别（包含）
		/// @param maxLevel [IN] 最大级别（包含）
		/// @param acceptOnMatch [IN] 匹配时是否接受
		explicit LevelFilter(LogLevel minLevel = LogLevel::TRACE,
                        LogLevel maxLevel = LogLevel::FATAL,
                        bool acceptOnMatch = true);

		/// @brief 过滤日志事件
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision Decide(const LogEventPtr &event) override;

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override;

		/// @brief 克隆过滤器
		/// @return 过滤器智能指针
		Pointer Clone() const override;

		/// @brief 设置最小日志级别
		/// @param level [IN] 最小日志级别
		void SetMinLevel(LogLevel level);

		/// @brief 获取最小日志级别
		/// @return 最小日志级别
		LogLevel GetMinLevel() const;

		/// @brief 设置最大日志级别
		/// @param level [IN] 最大日志级别
		void SetMaxLevel(LogLevel level);

		/// @brief 获取最大日志级别
		/// @return 最大日志级别
		LogLevel GetMaxLevel() const;

		/// @brief 设置匹配时是否接受
		/// @param accept [IN] 匹配时是否接受
		void SetAcceptOnMatch(bool accept);

		/// @brief 获取匹配时是否接受
		/// @return 匹配时是否接受
		bool GetAcceptOnMatch() const;
	private:
		LogLevel m_minLevel;	///< 最小日志级别
		LogLevel m_maxLevel;	///< 最大日志级别
		bool m_acceptOnMatch;	///< 匹配时是否接受
	};

	/// @brief 范围级别过滤器
	class LevelRangeFilter : public LevelFilter
	{
	public:
		/// @brief 构造函数
		/// @param minLevel [IN] 最小级别（包含）
		/// @param maxLevel [IN] 最大级别（包含）
		LevelRangeFilter(LogLevel minLevel, LogLevel maxLevel)
			: LevelFilter(minLevel, maxLevel, true) {}

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override { return "LevelRangeFilter"; }
	};

	/// @brief 级别阈值过滤器（只允许指定级别及以上）
	class LevelThresholdFilter : public LevelFilter
	{
	public:
		/// @brief 构造函数
		/// @param threshold [IN] 阈值级别（包含） 
		explicit LevelThresholdFilter(LogLevel threshold)
			: LevelFilter(threshold, LogLevel::FATAL, true) {}

		/// @brief 获取过滤器名称
		/// @return 过滤器名称
		std::string GetName() const override { return "LevelThresholdFilter"; }
	};

} // namespace IDLog

#endif // !IDLOG_FILTER_LEVELFILTER_H
