/** 
 * @Description: JSON 格式化器头文件
 * @Author: InverseDark
 * @Date: 2025-12-22 13:54:00
 * @LastEditTime: 2025-12-22 21:11:48
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_FORMATTER_JSONFORMATTER_H
#define IDLOG_FORMATTER_JSONFORMATTER_H

#include "IDLog/Formatter/Formatter.h"

namespace IDLog
{
	/// @brief JSON 格式化器类
	/// @details 将日志事件格式化为JSON格式，便于结构化处理
	///			 示例输出：
	///			 {
	///				 "timestamp": "2025-12-13 22:00:00.000",
	///				 "level": "INFO",
	///				 "logger": "main",
	///				 "thread_id": "12345",
	///				 "thread_name": "main",
	///				 "file": "main.cpp",
	///				 "function": "main",
	///				 "line": 42,
	///				 "message": "Hello, World!"
	///			 }
	class IDLOG_API JsonFormatter : public Formatter
	{
	public:
		/// @brief 构造函数
		/// @param prettyPrint [IN] 是否美化输出（添加缩进和换行）
		explicit JsonFormatter(bool prettyPrint = false);

		/// @brief 格式化日志事件
		/// @param event [IN] 日志事件
		/// @return 格式化后的字符串
		std::string Format(const LogEventPtr &event) override;

		/// @brief 克隆格式化器
		/// @return 新的格式化器实例
		Pointer Clone() const override;

		/// @brief 获取格式化器名称
		/// @return 格式化器名称
		std::string GetName() const override { return "JsonFormatter"; }

		/// @brief 设置是否美化输出
		/// @param pretty [IN] 是否美化输出
		void SetPrettyPrint(bool pretty) { m_prettyPrint = pretty; }

		/// @brief 获取是否美化输出
		/// @return 是否美化输出
		bool GetPrettyPrint() const { return m_prettyPrint; }

	private:
		/// @brief 转义JSON字符串中的特殊字符
		/// @param str [IN] 待转义的字符串
		/// @return 转义后的字符串
		std::string EscapeJson(const std::string &str) const;

	private:
		bool m_prettyPrint; ///< 是否美化输出
	};

} // namespace IDLog

#endif // !IDLOG_FORMATTER_JSONFORMATTER_H