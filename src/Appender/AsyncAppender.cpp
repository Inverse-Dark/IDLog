/**
 * @Description: 异步输出器源文件
 * @Author: InverseDark
 * @Date: 2025-12-24 11:05:12
 * @LastEditTime: 2025-12-24 11:51:13
 * @LastEditors: InverseDark
 */
#include "IDLog/Appender/AsyncAppender.h"
#include "IDLog/Utils/AsyncQueue.h"
#include "IDLog/Core/Statistics.h"

namespace IDLog
{

	/// @brief 异步输出器实现结构体
	struct AsyncAppender::Impl
	{
		Pointer backendAppender;					   ///< 后端日志输出器
		Utils::AsyncQueue<LogEventPtr>::Pointer queue; ///< 日志事件异步队列
		std::vector<std::thread> threads;			   ///< 异步处理线程数组
		std::atomic<bool> running;					   ///< 异步处理线程运行标志
		std::atomic<bool> stopped;					   ///< 异步处理线程停止标志
		size_t batchSize;							   ///< 批量处理大小
		uint64_t flushIntervalMs;					   ///< 刷新间隔，单位毫秒
		OverflowPolicy overflowPolicy;				   ///< 队列溢出策略
		std::atomic<uint64_t> droppedCount;			   ///< 丢弃的日志数量
		mutable std::mutex backendMutex;			   ///< 后端输出器互斥锁

		/// @brief 构造函数
		/// @param backendAppender [IN] 后端日志输出器智能指针
		/// @param queueCapacity [IN] 队列容量
		/// @param batchSize [IN] 批量处理大小
		/// @param flushIntervalMs [IN] 刷新间隔，单位毫秒
		/// @param policy [IN] 队列溢出策略
		Impl(const Pointer &backendAppender,
			 size_t queueCapacity,
			 size_t batchSize,
			 uint64_t flushIntervalMs,
			 OverflowPolicy policy)
			: backendAppender(backendAppender),
			  queue(std::make_shared<Utils::AsyncQueue<LogEventPtr>>(queueCapacity)),
			  running(false),
			  stopped(false),
			  batchSize(batchSize),
			  flushIntervalMs(flushIntervalMs),
			  overflowPolicy(policy),
			  droppedCount(0)
		{
		}
	};

	AsyncAppender::AsyncAppender(const Pointer &backendAppender,
								 size_t queueCapacity,
								 size_t batchSize,
								 uint64_t flushIntervalMs,
								 OverflowPolicy policy)
		: m_pImpl(new Impl(backendAppender, queueCapacity, batchSize, flushIntervalMs, policy))
	{
	}

	AsyncAppender::~AsyncAppender()
	{
		Stop();
		delete m_pImpl;
	}

	void AsyncAppender::Append(const LogEventPtr &event)
	{
		if (!event || m_pImpl->stopped.load())
		{
			return;
		}

		// 检查是否有后端输出器
		{
			std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
			if (!m_pImpl->backendAppender)
			{
				return;
			}
		}

		// 尝试将事件放入队列
		bool success = false;
		switch (m_pImpl->overflowPolicy)
		{
		case OverflowPolicy::BLOCK:
		{
			// 阻塞直到成功
			success = m_pImpl->queue->Push(event);
			break;
		}
		case OverflowPolicy::DROP_OLDEST:
		{
			// 尝试放入队列，失败则丢弃最旧的日志再尝试一次
			if (!m_pImpl->queue->TryPush(event))
			{
				LogEventPtr discardedEvent;
				m_pImpl->queue->TryPop(discardedEvent);
				success = m_pImpl->queue->TryPush(event);
			}
			else
			{
				success = true;
			}
			break;
		}
		case OverflowPolicy::DROP_NEWEST:
		{
			// 尝试放入队列，失败则丢弃最新的日志
			success = m_pImpl->queue->TryPush(event);
			break;
		}
		};

		// 记录丢弃的日志
		if (!success)
		{
			m_pImpl->droppedCount.fetch_add(1);
			if (StatisticsManager::GetInstance().IsStatisticsEnabled())
			{
				StatisticsManager::GetInstance().RecordDroppedLog(event->GetLoggerName(), event->GetLogMessage().size());
			}
		}
	}

	std::string AsyncAppender::GetName() const
	{
		std::stringstream ss;
		ss << "AsyncAppender";
		{
			std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
			if (m_pImpl->backendAppender)
			{
				ss << "->[" << m_pImpl->backendAppender->GetName() << "]";
			}
		}
		return ss.str();
	}

	void AsyncAppender::Flush()
	{
		if (!m_pImpl->running.load())
		{
			return;
		}

		// 等待队列清空
		while (!m_pImpl->queue->Empty() && !m_pImpl->stopped.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// 刷新后端输出器
		std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
		if (m_pImpl->backendAppender)
		{
			m_pImpl->backendAppender->Flush();
		}
	}

	bool AsyncAppender::SetBackendAppender(const Pointer &appender)
	{
		if (!appender)
		{
			return false;
		}

		std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
		m_pImpl->backendAppender = appender;
		return true;
	}

	AsyncAppender::Pointer AsyncAppender::GetBackendAppender() const
	{
		std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
		return m_pImpl->backendAppender;
	}

	bool AsyncAppender::Start(uint32_t threadCount)
	{
		if (m_pImpl->running.load())
		{
			return false;
		}

		// 检查是否有后端输出器
		{
			std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
			if (!m_pImpl->backendAppender)
			{
				return false;
			}
		}

		m_pImpl->stopped.store(false);
		m_pImpl->running.store(true);
		m_pImpl->threads.clear();
		m_pImpl->threads.reserve(threadCount);

		// 启动异步处理线程
		for (uint32_t i = 0; i < threadCount; ++i)
		{
			m_pImpl->threads.emplace_back(&AsyncAppender::ProcessThread, this);
		}

		return true;
	}

	void AsyncAppender::Stop(bool waitForEmptyQueue)
	{
		if (!m_pImpl->running.load())
		{
			return;
		}

		// 停止接受新日志
		m_pImpl->stopped.store(true);

        if (waitForEmptyQueue)
        {
            int maxRetries = 1000; // 10秒超时
            while (!m_pImpl->queue->Empty() && maxRetries > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                maxRetries--;
            }
        }

		// 停止队列
		m_pImpl->queue->Stop();

		// 等待所有线程退出
		for (auto &thread : m_pImpl->threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}

		m_pImpl->threads.clear();
		m_pImpl->running.store(false);

		// 恢复队列操作
		m_pImpl->queue->Resume();
	}

	size_t AsyncAppender::GetQueueSize() const
	{
		return m_pImpl->queue->Size();
	}

	size_t AsyncAppender::GetQueueCapacity() const
	{
		return m_pImpl->queue->GetCapacity();
	}

	void AsyncAppender::SetQueueCapacity(size_t capacity)
	{
		m_pImpl->queue->SetCapacity(capacity);
	}

	size_t AsyncAppender::GetBatchSize() const
	{
		return m_pImpl->batchSize;
	}

	void AsyncAppender::SetBatchSize(size_t batchSize)
	{
		m_pImpl->batchSize = batchSize;
	}

	uint64_t AsyncAppender::GetFlushInterval() const
	{
		return m_pImpl->flushIntervalMs;
	}

	void AsyncAppender::SetFlushInterval(uint64_t intervalMs)
	{
		m_pImpl->flushIntervalMs = intervalMs;
	}

	AsyncAppender::OverflowPolicy AsyncAppender::GetOverflowPolicy() const
	{
		return m_pImpl->overflowPolicy;
	}

	void AsyncAppender::SetOverflowPolicy(OverflowPolicy policy)
	{
		m_pImpl->overflowPolicy = policy;
	}

	bool AsyncAppender::IsRunning() const
	{
		return m_pImpl->running.load();
	}

	uint64_t AsyncAppender::GetDroppedCount() const
	{
		return m_pImpl->droppedCount.load();
	}

	void AsyncAppender::ProcessThread()
	{
		auto lastFlushTime = std::chrono::steady_clock::now();
		std::vector<LogEventPtr> batchEvents;

		if (m_pImpl->batchSize > 0)
		{
			batchEvents.reserve(m_pImpl->batchSize);
		}

		while (true)
		{
			// 检查停止条件
			if(m_pImpl->stopped.load() && m_pImpl->queue->Empty())
			{
				break;
			}


			LogEventPtr event;

			// 从队列中获取事件
			if (m_pImpl->queue->Pop(event, 100)) // 100ms超时
			{
				if (m_pImpl->batchSize > 0)
				{
					// 批处理模式
					batchEvents.push_back(event);
					if (batchEvents.size() >= m_pImpl->batchSize)
					{
						ProcessBatch(batchEvents);
						batchEvents.clear();
					}
				}
				else
				{
					// 单个处理模式
					ProcessEvent(event);
				}
			}
			else
			{
				// 超时未获取到事件，检查停止条件
				if(m_pImpl->queue->IsStopped())
				{
					break;
				}
			}

			// 检查是否需要刷新
			auto now = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlushTime).count();

			if (static_cast<uint64_t>(elapsed) >= m_pImpl->flushIntervalMs)
			{
				if (!batchEvents.empty())
				{
					ProcessBatch(batchEvents);
					batchEvents.clear();
				}

				// 刷新后端输出器
				std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
				if (m_pImpl->backendAppender)
				{
					m_pImpl->backendAppender->Flush();
				}

				lastFlushTime = now;
			}
		}

		// 处理剩余的批量事件
		if (!batchEvents.empty())
		{
			ProcessBatch(batchEvents);
			batchEvents.clear();
		}

		// 最后刷新一次
		std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
		if (m_pImpl->backendAppender)
		{
			m_pImpl->backendAppender->Flush();
		}
	}

	void AsyncAppender::ProcessEvent(const LogEventPtr &event)
	{
		if (!event)
		{
			return;
		}

		Pointer backend;
		{
            std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
            backend = m_pImpl->backendAppender;
        }
		
		if (backend)
		{
			backend->Append(event);
		}
	}

	void AsyncAppender::ProcessBatch(const std::vector<LogEventPtr> &events)
	{
		if (events.empty())
		{
			return;
		}

		Pointer backend;
		{
            std::lock_guard<std::mutex> lock(m_pImpl->backendMutex);
            backend = m_pImpl->backendAppender;
        }

		if (backend)
		{
			for (const auto &event : events)
			{
				if (event)
				{
					backend->Append(event);
				}
			}
		}
	}

} // namespace IDLog