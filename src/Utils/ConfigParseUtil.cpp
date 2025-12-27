/**
 * @Description: 配置解析工具源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 10:59:44
 * @LastEditTime: 2025-12-21 11:01:11
 * @LastEditors: InverseDark
 */
#include "IDLog/Utils/ConfigParseUtil.h"
#include "IDLog/Utils/StringUtil.h"

#include <map>
#include <fstream>
#include <sstream>

namespace IDLog
{
	namespace Utils
	{
		/// @brief 配置解析工具实现结构体
		struct ConfigParseUtil::Impl
		{
			// 配置存储：节名 -> (键名 -> 值)
			std::map<std::string, std::map<std::string, std::string>> data; ///< 配置数据
			std::vector<std::function<void()>> changeCallbacks;				///< 配置变更回调函数列表
			std::string filename;											///< 配置文件名

			/// @brief 清空已解析的配置节点
			void Clear()
			{
				data.clear();
			}
		};

		ConfigParseUtil::ConfigParseUtil()
			: m_pImpl(new Impl)
		{
		}

		ConfigParseUtil::~ConfigParseUtil()
		{
			delete m_pImpl;
		}

		bool ConfigParseUtil::LoadFromFile(const std::string &filename)
		{
			std::ifstream file(filename);
			if (!file.is_open())
			{
				return false;
			}

			// 读取文件内容到字符串
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();

			m_pImpl->filename = filename;
			return LoadFromString(buffer.str());
		}

		bool ConfigParseUtil::LoadFromString(const std::string &content)
		{
			return Parse(content);
		}

		bool ConfigParseUtil::SaveToFile(const std::string &filename) const
		{
			std::ofstream file(filename);
			if (!file.is_open())
			{
				return false;
			}

			file << SaveToString();
			file.close();
			return true;
		}

		std::string ConfigParseUtil::SaveToString() const
		{
			std::stringstream ss;
			for (const auto &[section, entries] : m_pImpl->data)
			{
				ss << "[" << section << "]\n";
				for (const auto &[key, value] : entries)
				{
					ss << key << " = " << EscapeString(value) << "\n";
				}
				ss << "\n";
			}
			return ss.str();
		}

		std::string ConfigParseUtil::GetFilename() const
		{
			return m_pImpl->filename;
		}

		std::vector<std::string> ConfigParseUtil::GetSections() const
		{
			std::vector<std::string> sections;
			for (const auto &[section, _] : m_pImpl->data)
			{
				sections.push_back(section);
			}
			return sections;
		}

		bool ConfigParseUtil::HasSection(const std::string &section) const
		{
			std::string normSection = NormalizeSectionName(section);
			return m_pImpl->data.find(normSection) != m_pImpl->data.end();
		}

		std::vector<std::string> ConfigParseUtil::GetKeys(const std::string &section) const
		{
			std::vector<std::string> keys;
			std::string normSection = NormalizeSectionName(section);
			auto secIt = m_pImpl->data.find(normSection);
			if (secIt != m_pImpl->data.end())
			{
				for (const auto &[key, _] : secIt->second)
				{
					keys.push_back(key);
				}
			}
			return keys;
		}

		bool ConfigParseUtil::HasKey(const std::string &section, const std::string &key) const
		{
			std::string normSection = NormalizeSectionName(section);
			std::string normKey = NormalizeKeyName(key);
			auto secIt = m_pImpl->data.find(normSection);
			if (secIt != m_pImpl->data.end())
			{
				return secIt->second.find(normKey) != secIt->second.end();
			}
			return false;
		}

		std::string ConfigParseUtil::GetString(const std::string &section,
											  const std::string &key,
											  const std::string &defaultValue) const
		{
			std::string normSection = NormalizeSectionName(section);
			std::string normKey = NormalizeKeyName(key);
			auto secIt = m_pImpl->data.find(normSection);
			if (secIt != m_pImpl->data.end())
			{
				auto keyIt = secIt->second.find(normKey);
				if (keyIt != secIt->second.end())
				{
					return keyIt->second;
				}
			}
			return defaultValue;
		}

		std::string ConfigParseUtil::GetString(const std::map<std::string, std::string> &params,
											 const std::string &key,
											 const std::string &defaultValue)
		{
			auto it = params.find(key);
			if (it != params.end())
			{
				return it->second;
			}
			return defaultValue;
		}

		int ConfigParseUtil::GetInt(const std::string &section,
									   const std::string &key,
									   int defaultValue) const
		{
			std::string strValue = GetString(section, key, "");
			if (strValue.empty())
			{
				return defaultValue;
			}
			try
			{
				return std::stoi(strValue);
			}
			catch (...)
			{
				return defaultValue;
			}
		}

		int ConfigParseUtil::GetInt(const std::map<std::string, std::string> &params,
									  const std::string &key,
									  int defaultValue)
		{
			auto it = params.find(key);
			if (it != params.end())
			{
				try
				{
					return std::stoi(it->second);
				}
				catch (...)
				{
					return defaultValue;
				}
			}
			return defaultValue;
		}

		double ConfigParseUtil::GetDouble(const std::string &section,
										const std::string &key,
										double defaultValue) const
		{
			std::string strValue = GetString(section, key, "");
			if (strValue.empty())
			{
				return defaultValue;
			}
			try
			{
				return std::stod(strValue);
			}
			catch (...)
			{
				return defaultValue;
			}
		}

		double ConfigParseUtil::GetDouble(const std::map<std::string, std::string> &params,
										const std::string &key,
										double defaultValue)
		{
			auto it = params.find(key);
			if (it != params.end())
			{
				try
				{
					return std::stod(it->second);
				}
				catch (...)
				{
					return defaultValue;
				}
			}
			return defaultValue;
		}

		bool ConfigParseUtil::GetBool(const std::string &section,
								const std::string &key,
								bool defaultValue) const
		{
			std::string strValue = GetString(section, key, "");
			if (strValue.empty())
			{
				return defaultValue;
			}
			strValue = StringUtil::ToLower(strValue);
			if (strValue == "true" || strValue == "1" || strValue == "yes" || strValue == "on")
			{
				return true;
			}
			else if (strValue == "false" || strValue == "0" || strValue == "no" || strValue == "off")
			{
				return false;
			}
			return defaultValue;
		}

		bool ConfigParseUtil::GetBool(const std::map<std::string, std::string> &params,
								const std::string &key,
								bool defaultValue)
		{
			auto it = params.find(key);
			if (it != params.end())
			{
				std::string strValue = StringUtil::ToLower(it->second);
				if (strValue == "true" || strValue == "1" || strValue == "yes" || strValue == "on")
				{
					return true;
				}
				else if (strValue == "false" || strValue == "0" || strValue == "no" || strValue == "off")
				{
					return false;
				}
			}
			return defaultValue;
		}

		LogLevel ConfigParseUtil::GetLogLevel(const std::string &section,
										const std::string &key,
										LogLevel defaultValue) const
		{
			std::string strValue = GetString(section, key, "");
			if (strValue.empty())
			{
				return defaultValue;
			}
			return StringToLevel(strValue);
		}

		LogLevel ConfigParseUtil::GetLogLevel(const std::map<std::string, std::string> &params,
										const std::string &key,
										LogLevel defaultValue)
		{
			auto it = params.find(key);
			if (it != params.end())
			{
				return StringToLevel(it->second);
			}
			return defaultValue;
		}

		void ConfigParseUtil::SetString(const std::string &section,
								const std::string &key,
								const std::string &value)
		{
			std::string normSection = NormalizeSectionName(section);
			std::string normKey = NormalizeKeyName(key);
			m_pImpl->data[normSection][normKey] = value;
			NotifyChanges();
		}

		void ConfigParseUtil::SetInt(const std::string &section,
								const std::string &key,
								int value)
		{
			SetString(section, key, std::to_string(value));
		}

		void ConfigParseUtil::SetDouble(const std::string &section,
								   const std::string &key,
								   double value)
		{
			SetString(section, key, std::to_string(value));
		}

		void ConfigParseUtil::SetBool(const std::string &section,
								 const std::string &key,
								 bool value)
		{
			SetString(section, key, value ? "true" : "false");
		}

		void ConfigParseUtil::SetLogLevel(const std::string &section,
										const std::string &key,
										LogLevel value)
		{
			SetString(section, key, LevelToString(value));
		}

		void ConfigParseUtil::RemoveSection(const std::string &section)
		{
			std::string normSection = NormalizeSectionName(section);
			m_pImpl->data.erase(normSection);
			NotifyChanges();
		}

		void ConfigParseUtil::RemoveKey(const std::string &section, const std::string &key)
		{
			std::string normSection = NormalizeSectionName(section);
			std::string normKey = NormalizeKeyName(key);
			auto secIt = m_pImpl->data.find(normSection);
			if (secIt != m_pImpl->data.end())
			{
				secIt->second.erase(normKey);
				if(secIt->second.empty())
				{
					// 如果节下没有键了，删除该节
					m_pImpl->data.erase(secIt);
				}
				NotifyChanges();
			}
		}

		void ConfigParseUtil::Clear()
		{
			m_pImpl->Clear();
			NotifyChanges();
		}

		void ConfigParseUtil::RegisterChangeCallback(const std::function<void()> &callback)
		{
			if (callback)
			{
				m_pImpl->changeCallbacks.push_back(callback);
			}
		}

		void ConfigParseUtil::NotifyChanges()
		{
			
			for (const auto &callback : m_pImpl->changeCallbacks)
			{
				if (callback)	// 检查回调是否有效
				{
					callback();	// 调用回调函数
				}
			}
		}

		bool ConfigParseUtil::Parse(const std::string &content)
		{
			m_pImpl->Clear();	// 清空已有配置

			std::string currentSection = "global";	// 当前节名称，默认全局节
			std::istringstream stream(content);	// 输入字符串流
			std::string line;	// 当前行内容
			int lineNumber = 0;	// 行号跟踪

			// 逐行解析配置内容
			while(std::getline(stream, line))
			{
				lineNumber++;
				// 去除行首尾空白
				line = StringUtil::Trim(line);

				// 忽略空行和注释行
				if (line.empty() || line[0] == '#' || line[0] == ';')
				{
					continue;
				}

				// 处理节头
				if (line.front() == '[' && line.back() == ']')
				{
					std::string section = line.substr(1, line.length() - 2);
					currentSection = NormalizeSectionName(section);
					continue;
				}

				// 处理键值对
				size_t equalPos = line.find('=');
				if (equalPos == std::string::npos)
				{
					// 无等号，格式错误
					continue;	// 忽略错误行
				}

				std::string key = line.substr(0, equalPos);
				std::string value = line.substr(equalPos + 1);

				key = NormalizeKeyName(key);
				value = StringUtil::Trim(value);
				value = UnescapeString(value);

				if(key.empty())
				{
					// 空键名，格式错误
					continue;	// 忽略错误行
				}

				// 存储键值对
				m_pImpl->data[currentSection][key] = value;
			}
			
			NotifyChanges();	// 通知配置变更
			return true;
		}

		std::string ConfigParseUtil::EscapeString(const std::string &str)
		{
			return StringUtil::Escape(str);
		}

		std::string ConfigParseUtil::UnescapeString(const std::string &str)
		{
			return StringUtil::Unescape(str);
		}

		std::string ConfigParseUtil::NormalizeSectionName(const std::string &section)
		{
			std::string result = section;
			result = StringUtil::Trim(result);	// 去除多余空白
			return result;
		}

		std::string ConfigParseUtil::NormalizeKeyName(const std::string &key)
		{
			std::string result = key;
			result = StringUtil::Trim(result);	// 去除多余空白
			return result;
		}

	} // namespace Utils
} // namespace IDLog