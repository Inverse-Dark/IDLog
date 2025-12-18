/** 
 * @Description: 格式化器基类头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 20:27:47
 * @LastEditTime: 2025-12-18 20:28:56
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_FORMATTER_FORMATTER_H
#define IDLOG_FORMATTER_FORMATTER_H

#include "IDLog/Core/LogEvent.h"

namespace IDLog
{
	/// @brief 格式化器基类
	/// @details 定义统一的格式化接口，支持自定义格式化方式
	class IDLOG_API Formatter
	{
	public:
		using Pointer = std::shared_ptr<Formatter>;
		using LogEventPtr = LogEvent::Pointer;
	public:
		/// @brief 析构函数
		virtual ~Formatter() = default;

		/// @brief 格式化日志事件
		/// @param event [IN] 日志事件
		/// @return 格式化后的字符串
		virtual std::string Format(const LogEventPtr& event) = 0;

		/// @brief 克隆格式化器
		/// @return 新的格式化器实例
		virtual Pointer Clone() const = 0;

		/// @brief 获取格式化器名称
		/// @return 格式化器名称
		virtual std::string GetName() const = 0;
	};

} // namespace IDLog

#endif // !IDLOG_FORMATTER_FORMATTER_H