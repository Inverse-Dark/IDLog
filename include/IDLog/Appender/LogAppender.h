/**
 * @Description: 日志输出器基类头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 20:26:38
 * @LastEditTime: 2025-12-26 21:24:17
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_APPENDER_LOGAPPENDER_H
#define IDLOG_APPENDER_LOGAPPENDER_H

#include "IDLog/Formatter/Formatter.h"

#include <mutex>

namespace IDLog
{

	/// @brief 日志输出器基类
	/// @details 所有输出目标（控制台、文件、网络等）的基类
	///			 定义了统一的接口，支持多态化输出方式
	class IDLOG_API LogAppender
	{
	public:
		using Pointer = std::shared_ptr<LogAppender>;
		using LogEventPtr = LogEvent::Pointer;
		using FormatterPtr = Formatter::Pointer;

	public:
		/// @brief 构造函数
		LogAppender();
		/// @brief 析构函数
		virtual ~LogAppender();

		/// @brief 拷贝构造函数(禁用)
		/// @param other [IN] 另一个输出器对象
		LogAppender(const LogAppender &) = delete;

		/// @brief 拷贝赋值运算符(禁用)
		/// @param other [IN] 另一个输出器对象
		/// @return 当前输出器对象引用
		LogAppender &operator=(const LogAppender &) = delete;

		/// @brief 输出日志事件
		/// @param event [IN] 日志事件智能指针
		virtual void Append(const LogEventPtr &event) = 0;

		/// @brief 设置格式化器
		/// @param formatter [IN] 格式化器智能指针
		virtual void SetFormatter(FormatterPtr formatter);

		/// @brief 获取格式化器
		/// @return 格式化器智能指针
		virtual FormatterPtr GetFormatter() const;

		/// @brief 获取输出器名称
		/// @return 输出器名称
		virtual std::string GetName() const = 0;

		/// @brief 刷新输出缓冲区
		virtual void Flush() = 0;

	protected:
		/// @brief 获取格式化器（无锁版本）
		/// @return 格式化器智能指针
		virtual FormatterPtr GetFormatterNoLock() const;

	private:
		/// @brief 输出器实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl; ///< 输出器实现指针
	protected:
		mutable std::mutex m_mutex; ///< 互斥锁，用于线程安全
	};

} // namespace IDLog

#endif // !IDLOG_APPENDER_LOGAPPENDER_H