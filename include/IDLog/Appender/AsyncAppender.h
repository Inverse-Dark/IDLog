/**
 * @Description: 异步输出器头文件
 * @Author: InverseDark
 * @Date: 2025-12-24 10:52:54
 * @LastEditTime: 2025-12-24 11:01:39
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_APPENDER_ASYNCAPPENDER_H
#define IDLOG_APPENDER_ASYNCAPPENDER_H

#include "IDLog/Appender/LogAppender.h"

#include <vector>

namespace IDLog
{
	/// @brief 异步输出器
	/// @details 将日志事件放入队列，由后台线程异步处理，减少日志记录对主线程的影响
	class IDLOG_API AsyncAppender : public LogAppender
	{
	public:
		/// @brief 队列溢出策略
		enum class OverflowPolicy
		{
			BLOCK,		 ///< 阻塞直到有空间（默认）
			DROP_OLDEST, ///< 丢弃最旧的日志
			DROP_NEWEST	 ///< 丢弃最新的日志
		};

	public:
		/// @brief 构造函数
		/// @param backendAppender [IN] 后端日志输出器智能指针
		/// @param queueCapacity [IN] 队列容量，0表示不限制
		/// @param batchSize [IN] 批量处理大小，0表示不批量处理
		/// @param flushIntervalMs [IN] 刷新间隔，单位毫秒
		/// @param policy [IN] 队列溢出策略
		explicit AsyncAppender(const Pointer &backendAppender = nullptr,
							   size_t queueCapacity = 10000,
							   size_t batchSize = 0,
							   uint64_t flushIntervalMs = 1000,
							   OverflowPolicy policy = OverflowPolicy::BLOCK);
		/// @brief 析构函数
		~AsyncAppender() override;

		/// @brief 输出日志事件
		/// @param event [IN] 日志事件智能指针
		void Append(const LogEventPtr &event) override;

		/// @brief 获取输出器名称
		/// @return 输出器名称
		std::string GetName() const override;

		/// @brief 刷新输出缓冲区
		void Flush() override;

		/// @brief 设置后端日志输出器
		/// @param appender [IN] 后端日志输出器智能指针
		/// @return 设置成功返回true，否则返回false
		bool SetBackendAppender(const Pointer &appender);

		/// @brief 获取后端日志输出器
		/// @return 后端日志输出器智能指针
		Pointer GetBackendAppender() const;

		/// @brief 启动异步处理线程
		/// @param threadCount [IN] 线程数量，默认1
		/// @return 启动成功返回true，否则返回false
		bool Start(uint32_t threadCount = 1);

		/// @brief 停止异步处理线程
		/// @param waitForEmptyQueue [IN] 是否等待队列清空，默认true
		void Stop(bool waitForEmptyQueue = true);

		/// @brief 获取当前队列大小
		/// @return 队列大小
		size_t GetQueueSize() const;

		/// @brief 获取队列容量
		/// @return 队列容量
		size_t GetQueueCapacity() const;

		/// @brief 设置队列容量
		/// @param capacity [IN] 队列容量
		void SetQueueCapacity(size_t capacity);

		/// @brief 获取批量处理大小
		/// @return 批量处理大小
		size_t GetBatchSize() const;

		/// @brief 设置批量处理大小
		/// @param batchSize [IN] 批量处理大小
		void SetBatchSize(size_t batchSize);

		/// @brief 获取刷新间隔，单位毫秒
		/// @return 刷新间隔，单位毫秒
		uint64_t GetFlushInterval() const;

		/// @brief 设置刷新间隔，单位毫秒
		/// @param intervalMs [IN] 刷新间隔，单位毫秒
		void SetFlushInterval(uint64_t intervalMs);

		/// @brief 获取队列溢出策略
		/// @return 队列溢出策略
		OverflowPolicy GetOverflowPolicy() const;

		/// @brief 设置队列溢出策略
		/// @param policy [IN] 队列溢出策略
		void SetOverflowPolicy(OverflowPolicy policy);

		/// @brief 判断异步处理线程是否运行中
		/// @return 运行中返回true，否则返回false
		bool IsRunning() const;

		/// @brief 获取丢弃的日志数量
		/// @return 丢弃的日志数量
		uint64_t GetDroppedCount() const;

	private:
		/// @brief 异步处理线程函数
		void ProcessThread();

		/// @brief 处理单个日志事件
		/// @param event [IN] 日志事件智能指针
		void ProcessEvent(const LogEventPtr &event);

		/// @brief 处理批量日志事件
		/// @param events [IN] 日志事件智能指针数组
		void ProcessBatch(const std::vector<LogEventPtr> &events);

	private:
		/// @brief 异步处理线程实现结构体前向声明
		struct Impl;

	private:
		Impl *m_pImpl; ///< 异步处理线程实现指针
	};
} // namespace IDLog

#endif // !IDLOG_APPENDER_ASYNCAPPENDER_H