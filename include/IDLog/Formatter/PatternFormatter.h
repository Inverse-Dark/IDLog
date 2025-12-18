/**
 * @Description: 模式格式化器头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 21:46:30
 * @LastEditTime: 2025-12-18 21:49:45
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_FORMATTER_PATTERNFORMATTER_H
#define IDLOG_FORMATTER_PATTERNFORMATTER_H

#include "IDLog/Formatter/Formatter.h"

namespace IDLog
{
	/// @brief 格式化选项结构体
	struct IDLOG_API FormatOptions
	{
		int width = 0;			///< 最小宽度
		bool leftAlign = false; ///< 是否左对齐
		char fillChar = ' ';	///< 填充字符

		/// @brief 构造函数
		FormatOptions() = default;

		/// @brief 应用格式化到字符串
		/// @param str [IN] 待格式化字符串
		/// @return 格式化后的字符串
		std::string Apply(const std::string &str) const;
	};

	/// @brief 模式格式化项基类
	class IDLOG_API PatternItem
	{
	public:
		using Pointer = std::shared_ptr<PatternItem>;
		using LogEventPtr = LogEvent::Pointer;

	public:
		/// @brief 构造函数
		PatternItem() = default;
		/// @brief 析构函数
		virtual ~PatternItem() = default;

		/// @brief 设置格式化选项
		/// @param options [IN] 新的格式化选项
		virtual void SetOptions(const FormatOptions &options)
		{
			m_options = options;
		}

		/// @brief 获取格式化选项
		/// @return 格式化选项
		const FormatOptions &GetOptions() const
		{
			return m_options;
		}

		/// @brief 格式化输出
		/// @param event [IN] 日志事件
		/// @param os [IN/OUT] 输出流
		virtual void Format(const LogEventPtr &event, std::ostream &os) = 0;

	protected:
		FormatOptions m_options; ///< 格式化选项
	};

	/// @brief 模式格式化器
	/// @details 支持类似log4j/logback的模式字符串，如：
	///			 %d{format} - 日期时间
	///			 %t - 线程ID
	///			 %T - 线程名称
	///			 %p - 日志级别
	///			 %c - 日志器名称
	///			 %F - 源文件名
	///			 %f - 函数名
	///			 %L - 源代码行号
	///			 %m - 消息
	///			 %% - 百分号
	///			 %n - 换行符
	class IDLOG_API PatternFormatter : public Formatter
	{
	public:
		/// @brief 构造函数
		/// @param pattern [IN] 模式字符串
		explicit PatternFormatter(const std::string &pattern = defaultPattern());
		/// @brief 析构函数
		~PatternFormatter() override;

		/// @brief 获取默认的模式字符串
		/// @return 默认模式字符串
		static std::string defaultPattern();

		/// @brief 格式化日志事件
		/// @param event [IN] 日志事件
		/// @return 格式化后的字符串
		std::string Format(const LogEventPtr &event) override;

		/// @brief 克隆格式化器
		/// @return 新的格式化器实例
		Pointer Clone() const override;

		/// @brief 获取格式化器名称
		/// @return 格式化器名称
		std::string GetName() const override { return "PatternFormatter"; }

		/// @brief 获取当前模式字符串
		/// @return 当前模式字符串
		const std::string &GetPattern() const;

		/// @brief 设置新的模式字符串
		/// @param pattern [IN] 新的模式字符串
		void SetPattern(const std::string &pattern);

	private:
		/// @brief 解析模式字符串
		void ParsePattern();

	private:
		/// @brief 模式格式化器实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl; ///< 模式格式化器实现指针
	};

} // namespace IDLog

#endif // !IDLOG_FORMATTER_PATTERNFORMATTER_H