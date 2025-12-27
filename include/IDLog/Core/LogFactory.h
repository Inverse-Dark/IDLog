/** 
 * @Description: Log 工厂头文件
 * @Author: InverseDark
 * @Date: 2025-12-21 12:56:33
 * @LastEditTime: 2025-12-22 21:12:46
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_LOGFACTORY_H
#define IDLOG_CORE_LOGFACTORY_H

#include "IDLog/Appender/LogAppender.h"
#include "IDLog/Formatter/Formatter.h"
#include "IDLog/Filter/Filter.h"

#include <map>

namespace IDLog
{
	/// @brief IDLog 工厂类
	/// @details 提供创建日志组件实例的静态方法
	class IDLOG_API LogFactory
	{
	public:
		using Pointer = std::shared_ptr<LogFactory>;

	public:
		/// @brief 构造函数
		LogFactory();
		/// @brief 析构函数
		virtual ~LogFactory();

		/// @brief 创建日志输出器实例
		/// @param type [IN] 输出器类型
		/// @param params [IN] 输出器参数键值对
		/// @param formatter [IN] 格式化器类型
		/// @param formatterParams [IN] 格式化器参数键值对
		/// @return 输出器智能指针
		virtual LogAppender::Pointer CreateLogAppender(const std::string &type, const std::map<std::string, std::string>& params = {},
													   const std::string &formatter = "", const std::map<std::string, std::string>& formatterParams = {});

		/// @brief 创建格式化器实例
		/// @param type [IN] 格式化器类型
		/// @param params [IN] 格式化器参数键值对
		/// @return 格式化器智能指针
		virtual Formatter::Pointer CreateFormatter(const std::string &type, const std::map<std::string, std::string>& params = {});

		/// @brief 创建过滤器实例
		/// @param type [IN] 过滤器类型
		/// @param params [IN] 过滤器参数键值对
		/// @return 过滤器智能指针
		virtual Filter::Pointer CreateFilter(const std::string &type, const std::map<std::string, std::string>& params = {});
	};
} // namespace IDLog

#endif // !IDLOG_CORE_LOGFACTORY_H