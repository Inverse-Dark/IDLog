/**
 * @Description: 日志管理器头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 22:32:19
 * @LastEditTime: 2025-12-18 22:37:11
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_LOGGERMANAGER_H
#define IDLOG_CORE_LOGGERMANAGER_H

#include "IDLog/Core/Logger.h"

namespace IDLog
{
	/// @brief 日志管理器类
	/// @details 单例模式，管理所有Logger实例
	///			 提供按名称获取Logger的功能，支持日志器层次结构
	class IDLOG_API LoggerManager
	{
	public:
		using LoggerPtr = Logger::Pointer;

	public:
		/// @brief 获取日志管理器单例实例
		/// @return 日志管理器实例引用
		static LoggerManager &GetInstance();

		/// @brief 拷贝构造函数(禁用)
		/// @param [IN] 另一个LoggerManager对象
		LoggerManager(const LoggerManager &) = delete;
		/// @brief 拷贝赋值运算符(禁用)
		/// @param [IN] 另一个LoggerManager对象
		/// @return 当前LoggerManager对象的引用
		LoggerManager &operator=(const LoggerManager &) = delete;

		/// @brief 获取或创建日志器
		/// @param name [IN] 日志器名称
		/// @return 日志器智能指针
		LoggerPtr GetLogger(const std::string &name);

		/// @brief 添加日志器
		/// @param name [IN] 日志器名称
		/// @param logger [IN] 日志器智能指针
		void AddLogger(const std::string &name, const LoggerPtr &logger);

		/// @brief 检查是否存在指定名称的日志器
		/// @param name [IN] 日志器名称
		/// @return 存在返回true，否则返回false
		bool HasLogger(const std::string &name);

		/// @brief 设置指定名称日志器的级别
		/// @param name [IN] 日志器名称
		/// @param level [IN] 新的日志级别
		void SetLoggerLevel(const std::string &name, LogLevel level);

		/// @brief 删除指定名称的日志器
		/// @param name [IN] 日志器名称
		void RemoveLogger(const std::string &name);

		/// @brief 获取根日志器
		/// @return 根日志器智能指针
		LoggerPtr GetRootLogger();

		/// @brief 设置根日志器的级别
		/// @param level [IN] 新的日志级别
		void SetRootLevel(LogLevel level);

		/// @brief 清空所有日志器
		void Clear();

		/// @brief 关闭日志管理器，释放资源
		void Shutdown();

	private:
		/// @brief 构造函数
		LoggerManager();
		/// @brief 析构函数
		~LoggerManager();

	private:
		/// @brief 日志管理器实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl;				///< 日志管理器实现指针
		mutable std::mutex m_mutex; ///< 互斥锁，用于线程安全
	};

/// @brief 便捷宏：获取指定名称的Logger
#define IDLOG_GET_LOGGER(name) IDLog::LoggerManager::GetInstance().GetLogger(name)
/// @brief 便捷宏：获取根Logger
#define IDLOG_GET_ROOT_LOGGER() IDLog::LoggerManager::GetInstance().GetRootLogger()
/// @brief 便捷宏：关闭日志管理器
#define IDLOG_SHUTDOWN() IDLog::LoggerManager::GetInstance().Shutdown()

/// @brief 便捷宏：快速记录TRACE级别的日志（使用根日志器）
#define IDLOG_TRACE(msg) IDLOG_GET_ROOT_LOGGER()->Trace(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录DEBUG级别的日志（使用根日志器）
#define IDLOG_DEBUG(msg) IDLOG_GET_ROOT_LOGGER()->Debug(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录INFO级别的日志（使用根日志器）
#define IDLOG_INFO(msg) IDLOG_GET_ROOT_LOGGER()->Info(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录WARN级别的日志（使用根日志器）
#define IDLOG_WARN(msg) IDLOG_GET_ROOT_LOGGER()->Warn(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录ERROR级别的日志（使用根日志器）
#define IDLOG_ERROR(msg) IDLOG_GET_ROOT_LOGGER()->Error(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录FATAL级别的日志（使用根日志器）
#define IDLOG_FATAL(msg) IDLOG_GET_ROOT_LOGGER()->Fatal(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))

/// @brief 便捷宏：快速记录格式化的TRACE级别日志（使用根日志器）
#define IDLOG_TRACE_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->TraceFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的DEBUG级别日志（使用根日志器）
#define IDLOG_DEBUG_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->DebugFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的INFO级别日志（使用根日志器）
#define IDLOG_INFO_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->InfoFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的WARN级别日志（使用根日志器）
#define IDLOG_WARN_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->WarnFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的ERROR级别日志（使用根日志器）
#define IDLOG_ERROR_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->ErrorFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的FATAL级别日志（使用根日志器）
#define IDLOG_FATAL_FMT(format, ...) IDLOG_GET_ROOT_LOGGER()->FatalFmt(format, ##__VA_ARGS__)

/// @brief 便捷宏：快速记录TRACE级别的日志（指定日志器名称）
#define IDLOG_LOGGER_TRACE(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Trace(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录DEBUG级别的日志（指定日志器名称）
#define IDLOG_LOGGER_DEBUG(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Debug(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录INFO级别的日志（指定日志器名称）
#define IDLOG_LOGGER_INFO(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Info(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录WARN级别的日志（指定日志器名称）
#define IDLOG_LOGGER_WARN(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Warn(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录ERROR级别的日志（指定日志器名称）
#define IDLOG_LOGGER_ERROR(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Error(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))
/// @brief 便捷宏：快速记录FATAL级别的日志（指定日志器名称）
#define IDLOG_LOGGER_FATAL(loggerName, msg) IDLOG_GET_LOGGER(loggerName)->Fatal(msg, IDLog::SourceLocation(__FILE__, __FUNCTION__, __LINE__))

/// @brief 便捷宏：快速记录格式化的TRACE级别日志（指定日志器名称）
#define IDLOG_LOGGER_TRACE_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->TraceFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的DEBUG级别日志（指定日志器名称）
#define IDLOG_LOGGER_DEBUG_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->DebugFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的INFO级别日志（指定日志器名称）
#define IDLOG_LOGGER_INFO_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->InfoFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的WARN级别日志（指定日志器名称）
#define IDLOG_LOGGER_WARN_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->WarnFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的ERROR级别日志（指定日志器名称）
#define IDLOG_LOGGER_ERROR_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->ErrorFmt(format, ##__VA_ARGS__)
/// @brief 便捷宏：快速记录格式化的FATAL级别日志（指定日志器名称）
#define IDLOG_LOGGER_FATAL_FMT(loggerName, format, ...) IDLOG_GET_LOGGER(loggerName)->FatalFmt(format, ##__VA_ARGS__)

} // namespace IDLog

#endif // !IDLOG_CORE_LOGGERMANAGER_H