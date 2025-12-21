/** 
 * @Description: 日志输出器基类源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 20:27:29
 * @LastEditTime: 2025-12-18 21:33:52
 * @LastEditors: InverseDark
 */
#include "IDLog/Appender/LogAppender.h"

namespace IDLog
{
	/// @brief 输出器实现结构体
	struct LogAppender::Impl
	{
		FormatterPtr formatter; ///< 日志格式化器

		/// @brief 构造函数
		Impl() : formatter(nullptr) {}
	};

	LogAppender::LogAppender()
		: m_pImpl(new Impl)
	{
	}

	LogAppender::~LogAppender()
	{
		delete m_pImpl;
	}

	void LogAppender::SetFormatter(FormatterPtr formatter)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_pImpl->formatter = formatter;
	}

	LogAppender::FormatterPtr LogAppender::GetFormatter() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_pImpl->formatter;
	}

	LogAppender::FormatterPtr LogAppender::GetFormatterNoLock() const
	{
		return m_pImpl->formatter;
	}

} // namespace IDLog