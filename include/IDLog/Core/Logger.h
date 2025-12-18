/**
 * @Description: 日志记录器头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 18:22:29
 * @LastEditTime: 2025-12-18 18:59:52
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_LOGGER_H
#define IDLOG_CORE_LOGGER_H

#include "IDLog/Core/LogLevel.h"
#include "IDLog/Core/LogEvent.h"
#include "IDLog/Appender/LogAppender.h"
#include "IDLog/Filter/Filter.h"
#include "IDLog/Utils/StringUtil.h"

namespace IDLog
{
	/// @brief 日志器类
	/// @details 用户主要交互的接口，提供不同级别的日志记录方法
	///			 支持日志级别过滤和多个输出目标（Appender）
	class IDLOG_API Logger
	{
	public:
		using Pointer = std::shared_ptr<Logger>;
		using LogEventPtr = LogEvent::Pointer;
		using AppenderPtr = LogAppender::Pointer;
		using FilterPtr = Filter::Pointer;

	public:
		/// @brief 构造函数
		/// @param name [IN] 日志器名称，通常使用类名或模块名
		/// @param level [IN] 初始日志级别，默认为INFO
		explicit Logger(const std::string &name, LogLevel level = LogLevel::INFO);

		/// @brief 析构函数
		~Logger();

		/// @brief 拷贝构造函数(禁用)
		/// @param [IN] 另一个Logger对象
		Logger(const Logger &) = delete;
		/// @brief 拷贝赋值运算符(禁用)
		/// @param [IN] 另一个Logger对象
		/// @return 当前Logger对象的引用
		Logger &operator=(const Logger &) = delete;

		/// @brief 获取日志器名称
		/// @return 日志器名称
		const std::string &GetName() const;

		/// @brief 获取当前日志级别
		/// @return 当前日志级别
		LogLevel GetLevel() const;

		/// @brief 设置日志级别
		/// @param level [IN] 新的日志级别
		void SetLevel(LogLevel level);

		/// @brief 添加输出目标
		/// @param appender [IN] 输出目标智能指针
		void AddAppender(const AppenderPtr &appender);

		/// @brief 移除所有输出目标
		void ClearAppenders();

		/// @brief 获取所有输出目标
		/// @return 输出目标列表
		std::vector<AppenderPtr> GetAppenders() const;

		/// @brief 添加过滤器
		/// @param filter [IN] 过滤器智能指针
		void AddFilter(const FilterPtr &filter);

		/// @brief 移除所有过滤器
		void ClearFilters();

		/// @brief 获取所有过滤器
		/// @return 过滤器列表
		std::vector<FilterPtr> GetFilters() const;

		/// @brief 启用/禁用统计
		/// @param enabled [IN] 启用/禁用
		void EnableStatistics(bool enabled = true);

		/// @brief 检查统计是否启用
		/// @return 启用返回true，否则返回false
		bool IsStatisticsEnabled() const;

		/// @brief 记录日志消息
		/// @param level [IN] 日志级别
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Log(LogLevel level, const std::string &message,
				 const SourceLocation &location = SourceLocation());

		/// @brief 记录TRACE级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Trace(const std::string &message,
				   const SourceLocation &location = SourceLocation());

		/// @brief 记录DEBUG级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Debug(const std::string &message,
				   const SourceLocation &location = SourceLocation());

		/// @brief 记录INFO级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Info(const std::string &message,
				  const SourceLocation &location = SourceLocation());

		/// @brief 记录WARN级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Warn(const std::string &message,
				  const SourceLocation &location = SourceLocation());

		/// @brief 记录ERROR级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Error(const std::string &message,
				   const SourceLocation &location = SourceLocation());

		/// @brief 记录FATAL级别的日志消息
		/// @param message [IN] 日志消息
		/// @param location [IN] 源文件位置
		void Fatal(const std::string &message,
				   const SourceLocation &location = SourceLocation());

		/// @brief 记录格式化的TRACE级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void TraceFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::TRACE))
			{
				Log(LogLevel::TRACE, Utils::StringUtil::Format(format, args...));
			}
		}
		/// @brief 记录格式化的DEBUG级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void DebugFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::DBG))
			{
				Log(LogLevel::DBG, Utils::StringUtil::Format(format, args...));
			}
		}

		/// @brief 记录格式化的INFO级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void InfoFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::INFO))
			{
				Log(LogLevel::INFO, Utils::StringUtil::Format(format, args...));
			}
		}

		/// @brief 记录格式化的WARN级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void WarnFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::WARN))
			{
				Log(LogLevel::WARN, Utils::StringUtil::Format(format, args...));
			}
		}

		/// @brief 记录格式化的ERROR级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void ErrorFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::ERR))
			{
				Log(LogLevel::ERR, Utils::StringUtil::Format(format, args...));
			}
		}

		/// @brief 记录格式化的FATAL级别日志消息
		/// @tparam Args 可变参数模板
		/// @param format [IN] 格式字符串
		/// @param args [IN] 格式参数
		template <typename... Args>
		void FatalFmt(const std::string &format, Args... args)
		{
			if (ShouldLog(GetLevel(), LogLevel::FATAL))
			{
				Log(LogLevel::FATAL, Utils::StringUtil::Format(format, args...));
			}
		}

		/// @brief 记录日志（带回调，支持延迟计算）
		/// @tparam Func 回调函数类型
		/// @param level [IN] 日志级别
		/// @param messageFunc [IN] 回调函数
		/// @param location [IN] 源文件位置
		template <typename Func, typename = std::enable_if_t<std::is_invocable_r_v<std::string, Func>>>
		void Log(LogLevel level, Func &&messageFunc,
				 const SourceLocation &location = SourceLocation())
		{
			if (!ShouldLog(GetLevel(), level))
			{
				return;
			}

			// 延迟计算消息
			std::string message = messageFunc();
			Log(level, message, location);
		}

	private:
		/// @brief 应用过滤器
		/// @param event [IN] 日志事件智能指针
		/// @return 过滤决策
		FilterDecision ApplyFilters(const LogEventPtr &event) const;

	private:
		/// @brief 日志器实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl; ///< 日志器实现指针
	};
} // namespace IDLog

#endif // !IDLOG_CORE_LOGGER_H