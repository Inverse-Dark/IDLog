/**
 * @Description: 配置解析工具头文件
 * @Author: InverseDark
 * @Date: 2025-12-21 10:48:40
 * @LastEditTime: 2025-12-21 10:53:22
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_UTILS_CONFIGPARSEUTIL_H
#define IDLOG_UTILS_CONFIGPARSEUTIL_H

#include "IDLog/Core/LogLevel.h"

#include <vector>
#include <functional>

namespace IDLog
{
	namespace Utils
	{
		/// @brief 配置解析工具类
		/// @details 提供配置文件解析的辅助函数，支持基本的INI格式
		class IDLOG_API ConfigParseUtil
		{
		public:
			/// @brief 构造函数
			ConfigParseUtil();
			/// @brief 析构函数
			~ConfigParseUtil();

			/// @brief 加载配置文件
			/// @param filename [IN] 配置文件名
			/// @return 加载成功返回true，否则返回false
			bool LoadFromFile(const std::string &filename);

			/// @brief 加载配置文件内容
			/// @param content [IN] 配置文件内容字符串
			/// @return 加载成功返回true，否则返回false
			bool LoadFromString(const std::string &content);

			/// @brief 保存配置到文件
			/// @param filename [IN] 目标文件名
			/// @return 保存成功返回true，否则返回false
			bool SaveToFile(const std::string &filename) const;

			/// @brief 保存配置到字符串
			/// @return 配置内容字符串
			std::string SaveToString() const;

			/// @brief 获取配置文件名
			/// @return 配置文件名
			std::string GetFilename() const;

			/// @brief 获取所有节名称
			/// @return 节名称列表
			std::vector<std::string> GetSections() const;

			/// @brief 检查是否存在指定节
			/// @param section [IN] 节名称
			/// @return 如果存在指定节，返回true；否则返回false
			bool HasSection(const std::string &section) const;

			/// @brief 获取指定节下的所有键名称
			/// @param section [IN] 节名称
			/// @return 键名称列表
			std::vector<std::string> GetKeys(const std::string &section) const;

			/// @brief 检查指定节下是否存在指定键
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @return 如果存在指定键，返回true；否则返回false
			bool HasKey(const std::string &section, const std::string &key) const;

			/// @brief 获取指定节下的字符串值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param defaultValue [IN] 默认值（键不存在时返回该值）
			/// @return 字符串值
			std::string GetString(const std::string &section,
								  const std::string &key,
								  const std::string &defaultValue = "") const;

			/// @brief 获取指定节下的整数值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param defaultValue [IN] 默认值（键不存在时返回该值）
			/// @return 整数值
			int GetInt(const std::string &section,
					   const std::string &key,
					   int defaultValue = 0) const;

			/// @brief 获取指定节下的浮点值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param defaultValue [IN] 默认值（键不存在时返回该值）
			/// @return 浮点值
			double GetDouble(const std::string &section,
							 const std::string &key,
							 double defaultValue = 0.0) const;

			/// @brief 获取指定节下的布尔值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param defaultValue [IN] 默认值（键不存在时返回该值）
			/// @return 布尔值
			bool GetBool(const std::string &section,
						 const std::string &key,
						 bool defaultValue = false) const;

			/// @brief 获取指定节下的日志级别值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param defaultValue [IN] 默认值（键不存在时返回该值）
			/// @return 日志级别值
			LogLevel GetLogLevel(const std::string &section,
								 const std::string &key,
								 LogLevel defaultValue = LogLevel::INFO) const;

			/// @brief 设置指定节下的字符串值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param value [IN] 字符串值
			void SetString(const std::string &section,
						   const std::string &key,
						   const std::string &value);

			/// @brief 设置指定节下的整数值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param value [IN] 整数值
			void SetInt(const std::string &section,
						const std::string &key,
						int value);

			/// @brief 设置指定节下的浮点值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param value [IN] 浮点值
			void SetDouble(const std::string &section,
						   const std::string &key,
						   double value);

			/// @brief 设置指定节下的布尔值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param value [IN] 布尔值
			void SetBool(const std::string &section,
						 const std::string &key,
						 bool value);

			/// @brief 设置指定节下的日志级别值
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			/// @param value [IN] 日志级别值
			void SetLogLevel(const std::string &section,
							 const std::string &key,
							 LogLevel value);

			/// @brief 删除指定节
			/// @param section [IN] 节名称
			void RemoveSection(const std::string &section);

			/// @brief 删除指定节下的键
			/// @param section [IN] 节名称
			/// @param key [IN] 键名称
			void RemoveKey(const std::string &section, const std::string &key);

			/// @brief 清空已解析的配置
			void Clear();

			/// @brief 注册配置变更回调函数
			/// @param callback [IN] 回调函数
			void RegisterChangeCallback(const std::function<void()> &callback);

			/// @brief 触发配置变更
			void NotifyChanges();

		private:
			/// @brief 解析配置内容
			/// @param content [IN] 配置内容字符串
			/// @return 解析成功返回true，否则返回false
			bool Parse(const std::string &content);

			/// @brief 转义字符串
			/// @param str [IN] 待转义字符串
			/// @return 转义后的字符串
			static std::string EscapeString(const std::string &str);
			/// @brief 反转义字符串
			/// @param str [IN] 待反转义字符串
			/// @return 反转义后的字符串
			static std::string UnescapeString(const std::string &str);

			/// @brief 标准化节名称（去除多余空白等）
			/// @param section [IN] 待标准化节名称
			/// @return 标准化后的节名称
			static std::string NormalizeSectionName(const std::string &section);
			/// @brief 标准化键名称（去除多余空白等）
			/// @param key [IN] 待标准化键名称
			/// @return 标准化后的键名称
			static std::string NormalizeKeyName(const std::string &key);

		private:
			/// @brief 配置解析工具实现结构体前向声明
			struct Impl;

		private:
			Impl *m_pImpl; ///< 配置解析工具实现指针
		};

	} // namespace Utils
} // namespace IDLog

#endif // !IDLOG_UTILS_CONFIGPARSEUTIL_H