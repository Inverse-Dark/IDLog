/**
 * @Description: 模式格式化器源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:50:31
 * @LastEditTime: 2025-12-18 22:01:25
 * @LastEditors: InverseDark
 */
#include "IDLog/Formatter/PatternFormatter.h"
#include "IDLog/Utils/StringUtil.h"

#include <iomanip>

namespace IDLog
{
	std::string FormatOptions::Apply(const std::string &str) const
	{
		if (width <= 0)
		{
			return str;
		}

		std::stringstream ss;

		if (leftAlign)
		{
			// 左对齐
			ss << std::left << std::setw(width) << std::setfill(fillChar) << str;
		}
		else
		{
			// 右对齐
			ss << std::right << std::setw(width) << std::setfill(fillChar) << str;
		}

		return ss.str();
	}

	// 具体模式项实现
	namespace
	{
		/// @brief 消息项
		class MessageItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetLogMessage());
			}
		};

		/// @brief 日志级别项
		class LevelItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(LevelToString(event->GetLevel()));
			}
		};

		/// @brief 日志器名称项
		class LoggerNameItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetLoggerName());
			}
		};

		/// @brief 线程ID项
		class ThreadIdItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetThreadId());
			}
		};

		/// @brief 线程名称项
		class ThreadNameItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
				{
					std::string threadName = event->GetThreadName();
					if (threadName.empty())
					{
						threadName = event->GetThreadId();
					}
					os << m_options.Apply(threadName);
					;
				}
			}
		};

		/// @brief 日期时间项
		class DateTimeItem : public PatternItem
		{
		public:
			explicit DateTimeItem(const std::string &fmt = "%Y-%m-%d %H:%M:%S")
				: m_format(fmt) {}

			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (!event)
					return;

				// 缓存上次格式化结果，避免重复计算
				static thread_local time_t t_lastSecond = 0;
				static thread_local char t_timeCache[64] = {0};
				static thread_local std::string t_lastFormat;

				auto timePoint = event->GetTime();
				time_t currentSecond = std::chrono::system_clock::to_time_t(timePoint);
				if (currentSecond != t_lastSecond || m_format != t_lastFormat)
				{
					t_lastSecond = currentSecond;
					t_lastFormat = m_format;
					std::tm tm;
#ifdef IDLOG_PLATFORM_WINDOWS
					localtime_s(&tm, &currentSecond);
#else
					localtime_r(&currentSecond, &tm);
#endif
					std::strftime(t_timeCache, sizeof(t_timeCache), m_format.c_str(), &tm);
				}

				os << m_options.Apply(t_timeCache);
			}

		private:
			std::string m_format;
		};

		/// @brief 毫秒项
		class MillisecondsItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
				{
					int ms = event->GetMilliseconds();

					// 格式化为三位数
					char buf[4];

					// 确保ms在0-999范围内
					if (ms < 0)
						ms = 0;
					if (ms > 999)
						ms = 999;

					// 格式化为三位数字符串
					buf[0] = (ms / 100) + '0';
					buf[1] = ((ms / 10) % 10) + '0';
					buf[2] = (ms % 10) + '0';
					buf[3] = '\0';

					os << m_options.Apply(buf);
				}
			}
		};

		/// @brief 源文件名项
		class FileNameItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetSourceLocation().GetShortFileName());
			}
		};

		/// @brief 函数名项
		class FunctionNameItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetSourceLocation().functionName);
			}
		};

		/// @brief 行号项
		class LineNumberItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
				{
					std::stringstream ss;
					ss << event->GetSourceLocation().lineNumber;
					os << m_options.Apply(ss.str());
				}
			}
		};

		/// @brief 源文件位置项
		class SourceLocationItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &event, std::ostream &os) override
			{
				if (event)
					os << m_options.Apply(event->GetSourceLocation().ToString());
			}
		};

		/// @brief 字面文本项
		class LiteralItem : public PatternItem
		{
		public:
			explicit LiteralItem(const std::string &text) : m_text(text) {}

			void Format(const LogEventPtr &, std::ostream &os) override
			{
				os << m_text;
			}

		private:
			std::string m_text;
		};

		/// @brief 换行符项
		class NewLineItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &, std::ostream &os) override
			{
				os << '\n';
			}
		};

		/// @brief 制表符项
		class TabItem : public PatternItem
		{
		public:
			void Format(const LogEventPtr &, std::ostream &os) override
			{
				os << '\t';
			}
		};
	} // namespace anonymous

	/// @brief 模式格式化器实现结构体
	struct PatternFormatter::Impl
	{
		std::string pattern;					   ///< 模式字符串
		std::vector<PatternItem::Pointer> items; ///< 解析后的格式化项列表
	};

	PatternFormatter::PatternFormatter(const std::string &pattern)
		: m_pImpl(new Impl)
	{
		m_pImpl->pattern = pattern;
		// 解析模式字符串
		ParsePattern();
	}

	PatternFormatter::~PatternFormatter()
	{
		delete m_pImpl;
	}

	std::string PatternFormatter::defaultPattern()
	{
		return "%d{%Y-%m-%d %H:%M:%S}.%ms [%t] %-5p %c - %m%n";
	}

	std::string PatternFormatter::Format(const LogEventPtr &event)
	{
		std::stringstream ss;
		for (const auto &item : m_pImpl->items)
		{
			item->Format(event, ss);
		}
		return ss.str();
	}

	PatternFormatter::Pointer PatternFormatter::Clone() const
	{
		return std::make_shared<PatternFormatter>(m_pImpl->pattern);
	}

	const std::string &PatternFormatter::GetPattern() const
	{
		return m_pImpl->pattern;
	}

	void PatternFormatter::SetPattern(const std::string &pattern)
	{
		m_pImpl->pattern = pattern;
		ParsePattern();
	}

	void PatternFormatter::ParsePattern()
	{
		// 清空已有项
		m_pImpl->items.clear();

		size_t pos = 0;									// 当前解析位置
		const size_t len = m_pImpl->pattern.length(); // 模式字符串长度

		// 解析循环
		while (pos < len)
		{
			// 处理转义字符
			if (m_pImpl->pattern[pos] == '%')
			{
				if (pos + 1 >= len)
				{
					// 单独的 '%'，作为字面文本处理
					m_pImpl->items.push_back(std::make_shared<LiteralItem>("%"));
					break;
				}

				// 解析格式化选项
				FormatOptions options;
				char next = m_pImpl->pattern[++pos];

				// 处理对齐和宽度
				if (next == '-')
				{
					options.leftAlign = true;
					if (pos + 1 >= len)
						break;
					next = m_pImpl->pattern[++pos];
				}

				// 检查是否有宽度数字
				if (std::isdigit(next))
				{
					std::string widthStr;
					while (pos < len && std::isdigit(m_pImpl->pattern[pos]))
					{
						widthStr.push_back(m_pImpl->pattern[pos++]);
					}
					options.width = std::stoi(widthStr);

					if (pos >= len)
						break;
					next = m_pImpl->pattern[pos];
				}

				// 根据格式字符创建对应的模式项
				PatternItem::Pointer item;
				switch (next)
				{
				case 'd':
				{
					// 日期时间
					if (pos + 1 < len && m_pImpl->pattern[pos + 1] == '{')
					{
						// 提取日期格式
						size_t endBrace = m_pImpl->pattern.find('}', pos + 2);
						if (endBrace != std::string::npos)
						{
							std::string dateFormat = m_pImpl->pattern.substr(pos + 2, endBrace - (pos + 2));
							auto datetimeItem = std::make_shared<DateTimeItem>(dateFormat);
							datetimeItem->SetOptions(options);
							item = datetimeItem;
							pos = endBrace + 1; // 更新位置到右括号后
						}
						else
						{
							auto datetimeItem = std::make_shared<DateTimeItem>();
							datetimeItem->SetOptions(options);
							item = datetimeItem;
							pos++; // 没有找到右括号，继续解析
						}
					}
					else
					{
						// 默认日期时间格式
						auto datetimeItem = std::make_shared<DateTimeItem>();
						datetimeItem->SetOptions(options);
						item = datetimeItem;
						pos++;
					}
					break;
				}
				case 'm':
				{
					// 消息或毫秒
					if (pos + 1 < len && m_pImpl->pattern[pos + 1] == 's')
					{
						auto msItem = std::make_shared<MillisecondsItem>();
						msItem->SetOptions(options);
						item = msItem;
						pos += 2;
					}
					else
					{
						auto msgItem = std::make_shared<MessageItem>();
						msgItem->SetOptions(options);
						item = msgItem;
						pos++;
					}
					break;
				}
				case 'p':
				{
					// 日志级别
					auto levelItem = std::make_shared<LevelItem>();
					levelItem->SetOptions(options);
					item = levelItem;
					pos++;
					break;
				}
				case 'c':
				{
					// 日志器名称
					auto loggerItem = std::make_shared<LoggerNameItem>();
					loggerItem->SetOptions(options);
					item = loggerItem;
					pos++;
					break;
				}
				case 't':
				{
					// 线程ID
					auto threadIdItem = std::make_shared<ThreadIdItem>();
					threadIdItem->SetOptions(options);
					item = threadIdItem;
					pos++;
					break;
				}
				case 'T':
				{
					// 线程名称
					auto threadNameItem = std::make_shared<ThreadNameItem>();
					threadNameItem->SetOptions(options);
					item = threadNameItem;
					pos++;
					break;
				}
				case 'n':
				{
					// 换行符
					item = std::make_shared<NewLineItem>();
					pos++;
					break;
				}
				case 'F':
				{
					// 源文件名
					auto fileItem = std::make_shared<FileNameItem>();
					fileItem->SetOptions(options);
					item = fileItem;
					pos++;
					break;
				}
				case 'f':
				{
					// 函数名
					auto funcItem = std::make_shared<FunctionNameItem>();
					funcItem->SetOptions(options);
					item = funcItem;
					pos++;
					break;
				}
				case 'L':
				{
					// 行号
					auto lineItem = std::make_shared<LineNumberItem>();
					lineItem->SetOptions(options);
					item = lineItem;
					pos++;
					break;
				}
				case 'l':
				{
					// 源文件位置
					auto srcLocItem = std::make_shared<SourceLocationItem>();
					srcLocItem->SetOptions(options);
					item = srcLocItem;
					pos++;
					break;
				}
				case '%':
				{
					// 百分号
					item = std::make_shared<LiteralItem>("%");
					pos++;
					break;
				}
				case '\t':
				{
					// 制表符
					item = std::make_shared<TabItem>();
					pos++;
					break;
				}
				default:
				{
					// 未知格式，作为字面文本处理
					item = std::make_shared<LiteralItem>("%" + std::string(1, next));
					pos++;
					break;
				}
				}

				if (item)
				{
					m_pImpl->items.push_back(item);
				}
			}
			else
			{
				// 处理字面文本
				size_t start = pos;
				while (pos < len && m_pImpl->pattern[pos] != '%')
				{
					pos++;
				}
				std::string literalText = m_pImpl->pattern.substr(start, pos - start);
				if (!literalText.empty())
				{
					m_pImpl->items.push_back(std::make_shared<LiteralItem>(literalText));
				}
			}
		}
	}

} // namespace IDLog