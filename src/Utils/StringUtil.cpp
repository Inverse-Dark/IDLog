/**
 * @Description: 字符串工具源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:53:29
 * @LastEditTime: 2025-12-18 21:54:38
 * @LastEditors: InverseDark
 */

#include "IDLog/Utils/StringUtil.h"

#include <algorithm>

namespace IDLog
{
	namespace Utils
	{
		std::string &StringUtil::LTrim(std::string &str)
		{
			str.erase(str.begin(), std::find_if(str.begin(), str.end(),
												[](unsigned char ch)
												{ return !std::isspace(ch); }));
			return str;
		}

		std::string &StringUtil::RTrim(std::string &str)
		{
			str.erase(std::find_if(str.rbegin(), str.rend(),
								   [](unsigned char ch)
								   { return !std::isspace(ch); })
						  .base(),
					  str.end());
			return str;
		}

		std::string &StringUtil::Trim(std::string &str)
		{
			return LTrim(RTrim(str));
		}

		std::string StringUtil::ToUpper(const std::string &str)
		{
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
						   [](unsigned char ch)
						   { return static_cast<char>(std::toupper(static_cast<int>(ch))); });
			return result;
		}

		std::string StringUtil::ToLower(const std::string &str)
		{
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
						   [](unsigned char ch)
						   { return static_cast<char>(std::tolower(static_cast<int>(ch))); });
			return result;
		}

		bool StringUtil::StartsWith(const std::string &str, const std::string &prefix)
		{
			if (prefix.size() > str.size())
				return false;
			return std::equal(prefix.begin(), prefix.end(), str.begin());
		}

		bool StringUtil::EndsWith(const std::string &str, const std::string &suffix)
		{
			if (suffix.size() > str.size())
				return false;
			return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
		}

		std::string StringUtil::ReplaceAll(const std::string &str,
										   const std::string &from,
										   const std::string &to)
		{
			std::string result = str;
			if (from.empty())
				return result;

			size_t start_pos = 0;
			while ((start_pos = result.find(from, start_pos)) != std::string::npos)
			{
				result.replace(start_pos, from.length(), to);
				start_pos += to.length();
			}
			return result;
		}

		std::vector<std::string> StringUtil::Split(const std::string &str,
												   const std::string &delimiter)
		{
			std::vector<std::string> tokens;
			if (str.empty())
				return tokens;

			size_t start = 0;
			size_t end = str.find(delimiter);

			while (end != std::string::npos)
			{
				tokens.push_back(str.substr(start, end - start));
				start = end + delimiter.length();
				end = str.find(delimiter, start);
			}

			tokens.push_back(str.substr(start));
			return tokens;
		}

		std::string StringUtil::Join(const std::vector<std::string> &strings,
									 const std::string &delimiter)
		{
			std::string result;
			for (size_t i = 0; i < strings.size(); ++i)
			{
				if (i > 0)
					result += delimiter;
				result += strings[i];
			}
			return result;
		}

		std::string StringUtil::Escape(const std::string &str)
		{
			std::string result;
			result.reserve(str.length());

			for (char ch : str)
			{
				switch (ch)
				{
				case '\"':
					result += "\\\"";
					break;
				case '\\':
					result += "\\\\";
					break;
				case '\b':
					result += "\\b";
					break;
				case '\f':
					result += "\\f";
					break;
				case '\n':
					result += "\\n";
					break;
				case '\r':
					result += "\\r";
					break;
				case '\t':
					result += "\\t";
					break;
				default:
					// 控制字符转义
					if (static_cast<unsigned char>(ch) < 32)
					{
						char buf[8];
						snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(ch));
						result += buf;
					}
					else
					{
						result += ch;
					}
				}
			}

			return result;
		}

		std::string StringUtil::Unescape(const std::string &str)
		{
			std::string result;
			result.reserve(str.length());

			for (size_t i = 0; i < str.length(); ++i)
			{
				if (str[i] == '\\' && i + 1 < str.length())
				{
					switch (str[i + 1])
					{
					case '\"':
						result += '\"';
						i++;
						break;
					case '\\':
						result += '\\';
						i++;
						break;
					case 'b':
						result += '\b';
						i++;
						break;
					case 'f':
						result += '\f';
						i++;
						break;
					case 'n':
						result += '\n';
						i++;
						break;
					case 'r':
						result += '\r';
						i++;
						break;
					case 't':
						result += '\t';
						i++;
						break;
					default:
						result += str[i];
						break;
					}
				}
				else
				{
					result += str[i];
				}
			}

			return result;
		}

		bool StringUtil::Contains(const std::string &str, const std::string &substr)
		{
			return str.find(substr) != std::string::npos;
		}

		std::string StringUtil::Repeat(const std::string &str, int times)
		{
			if (times <= 0)
				return "";

			std::string result;
			result.reserve(str.length() * times);
			for (int i = 0; i < times; ++i)
			{
				result += str;
			}
			return result;
		}

	} // namespace Utils
} // namespace IDLog