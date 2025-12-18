/**
 * @Description: 控制台输出器头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:19:49
 * @LastEditTime: 2025-12-18 21:19:49
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_APPENDER_CONSOLEAPPENDER_H
#define IDLOG_APPENDER_CONSOLEAPPENDER_H

#include "IDLog/Appender/LogAppender.h"

namespace IDLog
{
	/// @brief 控制台输出器
	/// @details 将日志输出到标准输出(stdout)或标准错误(stderr)
	class IDLOG_API ConsoleAppender : public LogAppender
	{
	public:
		/// @brief 输出目标枚举
		enum class Target
		{
			STDOUT, ///< 标准输出
			STDERR	///< 标准错误
		};

	public:
		/// @brief 构造函数
		/// @param target [IN] 输出目标，默认为标准输出
		/// @param formatter [IN] 格式化器，如果为空则使用默认模式格式化器
		explicit ConsoleAppender(Target target = Target::STDOUT,
								 FormatterPtr formatter = nullptr);
		/// @brief 析构函数
		~ConsoleAppender() override = default;

		/// @brief 输出日志事件到控制台
		/// @param event [IN] 日志事件智能指针
		void Append(const LogEventPtr &event) override;

		/// @brief 获取输出器名称
		/// @return 输出器名称
		std::string GetName() const override;

		/// @brief 刷新输出缓冲区
		void Flush() override;

		/// @brief 设置输出目标
		/// @param target [IN] 输出目标
		void SetTarget(Target target);

		/// @brief 获取输出目标
		/// @return 输出目标
		Target GetTarget() const;

		/// @brief 设置是否使用颜色输出
		/// @param useColor [IN] 是否使用颜色
		void SetUseColor(bool useColor);

		/// @brief 获取是否使用颜色输出
		/// @return 是否使用颜色
		bool GetUseColor() const;

	private:
		/// @brief 根据日志级别获取颜色代码
		/// @param level [IN] 日志级别
		/// @return 颜色代码字符串
		std::string GetLevelColor(LogLevel level) const;

		/// @brief 获取重置颜色代码
		/// @return 颜色代码字符串
		std::string GetResetColor() const;

	private:
		Target m_target; ///< 输出目标
		bool m_useColor; ///< 是否使用颜色输出
	};

} // namespace IDLog

#endif // !IDLOG_APPENDER_CONSOLEAPPENDER_H