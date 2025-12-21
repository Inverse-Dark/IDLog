/**
 * @Description: 文件输出器头文件
 * @Author: InverseDark
 * @Date: 2025-12-19 12:13:05
 * @LastEditTime: 2025-12-19 12:20:17
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_APPENDER_FILEAPPENDER_H
#define IDLOG_APPENDER_FILEAPPENDER_H

#include "IDLog/Appender/LogAppender.h"

#include <filesystem>

namespace IDLog
{
	/// @brief 文件输出器
	/// @details 将日志输出到文件，支持滚动和文件大小限制
	class IDLOG_API FileAppender : public LogAppender
	{
	public:
		/// @brief 日志轮转策略枚举
		enum class RollPolicy
		{
			NONE,	  ///< 不滚动
			YEARLY,	  ///< 按年滚动
			MONTHLY,  ///< 按月滚动
			DAILY,	  ///< 按天滚动
			HOURLY,	  ///< 按小时滚动
			MINUTELY, ///< 按分钟滚动
			SIZE	  ///< 按文件大小滚动
		};

	public:
		/// @brief 构造函数
		/// @param filename [IN] 日志文件名
		/// @param formatter [IN] 格式化器，如果为空则使用默认模式格式化器
		/// @param rollPolicy [IN] 滚动策略，默认为不滚动
		/// @param maxSize [IN] 最大文件大小，仅在按大小滚动时有效，默认为10MB
		explicit FileAppender(const std::string &filename,
							  FormatterPtr formatter = nullptr,
							  RollPolicy rollPolicy = RollPolicy::NONE,
							  size_t maxSize = 10 * 1024 * 1024);
		/// @brief 析构函数
		~FileAppender() override;

		/// @brief 输出日志事件到文件
		/// @param event [IN] 日志事件智能指针
		void Append(const LogEventPtr &event) override;

		/// @brief 获取输出器名称
		/// @return 输出器名称
		std::string GetName() const override;

		/// @brief 刷新输出缓冲区
		void Flush() override;

		/// @brief 打开日志文件
		/// @return 如果成功打开文件，返回true；否则返回false
		bool Open();

		/// @brief 关闭日志文件
		void Close();

		/// @brief 检查日志文件是否已打开
		/// @return 如果文件已打开，返回true；否则返回false
		bool IsOpen() const;

		/// @brief 设置日志文件名
		/// @param filename [IN] 新的日志文件名
		void SetFilename(const std::string &filename);

		/// @brief 获取日志文件名
		/// @return 当前日志文件名
		std::string GetFilename() const;

		/// @brief 获取当前日志文件大小
		/// @return 当前日志文件大小（字节）
		size_t GetCurrentFileSize() const;

		/// @brief 设置日志轮转策略
		/// @param policy [IN] 新的日志轮转策略
		void SetRollPolicy(RollPolicy policy);

		/// @brief 获取日志轮转策略
		/// @return 当前日志轮转策略
		RollPolicy GetRollPolicy() const;

		/// @brief 设置最大文件大小（仅在按大小滚动时有效）
		/// @param maxSize [IN] 最大文件大小（字节）
		void SetMaxFileSize(size_t maxSize);

		/// @brief 获取最大文件大小
		/// @return 最大文件大小（字节）
		size_t GetMaxFileSize() const;

	protected:
		/// @brief 检查是否需要滚动日志文件
		/// @param event [IN] 日志事件智能指针
		/// @return 如果需要滚动，返回true；否则返回false
		virtual bool ShouldRoll(const LogEventPtr &event);

		/// @brief 执行日志文件滚动
		/// @param event [IN] 日志事件智能指针
		virtual void RollFile(const LogEventPtr &event);

		/// @brief 生成滚动后的文件名
		/// @param event [IN] 日志事件智能指针
		/// @return 滚动后的文件名
		virtual std::string GenerateRolledFilename(const LogEventPtr &event);

		/// @brief 获取当前时间标记（用于时间滚动）
		/// @return 当前时间标记
		int GetCurrentTimeMarker() const;

		/// @brief 获取文件的时间标记（用于时间滚动）
		/// @param filePath [IN] 文件路径
		/// @return 文件的时间标记
		int GetFileTimeMarker(const std::filesystem::path& filePath) const;

		/// @brief 无锁打开日志文件（仅供内部使用）
		/// @return 如果成功打开文件，返回true；否则返回false
		bool OpenNoLock();

		/// @brief 无锁关闭日志文件（仅供内部使用）
		void CloseNoLock();

	private:
		/// @brief 文件输出器实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl; ///< 文件输出器实现指针
	};
} // namespace IDLog

#endif // !IDLOG_APPENDER_FILEAPPENDER_H