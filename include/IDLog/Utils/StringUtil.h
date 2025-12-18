/**
 * @Description: 字符串工具头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:51:54
 * @LastEditTime: 2025-12-18 21:51:57
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_UTILS_STRINGUTIL_H
#define IDLOG_UTILS_STRINGUTIL_H

#include "IDLog/Core/Macro.h"

#include <string>
#include <vector>

namespace IDLog
{
	namespace Utils
	{

		/// @brief 字符串工具类
		/// @details 提供字符串处理的常用工具函数
		class IDLOG_API StringUtil
		{
		public:
			/// @brief 去除字符串左侧空白字符
			/// @param str [IN] 待处理字符串
			/// @return 去除左侧空白后的字符串引用
			static std::string &LTrim(std::string &str);

			/// @brief 去除字符串右侧空白字符
			/// @param str [IN] 待处理字符串
			/// @return 去除右侧空白后的字符串引用
			static std::string &RTrim(std::string &str);

			/// @brief 去除字符串两侧空白字符
			/// @param str [IN] 待处理字符串
			/// @return 去除两侧空白后的字符串引用
			static std::string &Trim(std::string &str);

			/// @brief 将字符串转换为大写
			/// @param str [IN] 待处理字符串
			/// @return 转换为大写后的字符串
			static std::string ToUpper(const std::string &str);

			/// @brief 将字符串转换为小写
			/// @param str [IN] 待处理字符串
			/// @return 转换为小写后的字符串
			static std::string ToLower(const std::string &str);

			/// @brief 检查字符串是否以指定前缀开头
			/// @param str [IN] 待检查字符串
			/// @param prefix [IN] 指定前缀
			/// @return 如果字符串以指定前缀开头，返回true；否则返回false
			static bool StartsWith(const std::string &str, const std::string &prefix);

			/// @brief 检查字符串是否以指定后缀结尾
			/// @param str [IN] 待检查字符串
			/// @param suffix [IN] 指定后缀
			/// @return 如果字符串以指定后缀结尾，返回true；否则返回false
			static bool EndsWith(const std::string &str, const std::string &suffix);

			/// @brief 替换字符串中所有匹配的子串
			/// @param str [IN] 待处理字符串
			/// @param from [IN] 要替换的子串
			/// @param to [IN] 替换后的子串
			/// @return 替换后的字符串
			static std::string ReplaceAll(const std::string &str,
										  const std::string &from,
										  const std::string &to);

			/// @brief 分割字符串
			/// @param str [IN] 要分割的字符串
			/// @param delimiter [IN] 分隔符
			/// @return 分割后的子串列表
			static std::vector<std::string> Split(const std::string &str,
												  const std::string &delimiter);

			/// @brief 连接字符串
			/// @param strings [IN] 要连接的字符串列表
			/// @param delimiter [IN] 连接符
			/// @return 连接后的字符串
			static std::string Join(const std::vector<std::string> &strings,
									const std::string &delimiter);

			/// @brief 格式化字符串（类似printf）
			/// @tparam Args 格式参数类型模板参数包
			/// @param format [IN] 格式字符串
			/// @param args [IN] 格式参数
			/// @return 格式化后的字符串
			template <typename... Args>
			static std::string Format(const std::string &format, Args... args)
			{
				int size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
				if (size <= 0)
					return "";

				std::vector<char> buf(size);
				snprintf(buf.data(), size, format.c_str(), args...);
				return std::string(buf.data(), buf.data() + size - 1);
			}

			/// @brief 转义字符串中的特殊字符
			/// @param str [IN] 待转义的字符串
			/// @return 转义后的字符串
			static std::string Escape(const std::string &str);

			/// @brief 检查字符串是否包含指定子串
			/// @param str [IN] 待检查字符串
			/// @param substr [IN] 指定子串
			/// @return 如果字符串包含指定子串，返回true；否则返回false
			static bool Contains(const std::string &str, const std::string &substr);

			/// @brief 重复字符串多次
			/// @param str [IN] 待重复的字符串
			/// @param times [IN] 重复次数
			/// @return 重复后的字符串
			static std::string Repeat(const std::string &str, int times);
		};

	} // namespace Utils
} // namespace IDLog

#endif // !IDLOG_UTILS_STRINGUTIL_H