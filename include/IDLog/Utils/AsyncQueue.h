/** 
 * @Description: 异步队列头文件
 * @Author: InverseDark
 * @Date: 2025-12-24 09:36:02
 * @LastEditTime: 2025-12-24 13:59:00
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_UTILS_ASYNCQUEUE_H
#define IDLOG_UTILS_ASYNCQUEUE_H

#include "IDLog/Core/Macro.h"

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace IDLog
{
	namespace Utils
	{
		/// @brief 异步队列类模板
		/// @details 提供线程安全的队列操作，支持阻塞和非阻塞模式
		/// @tparam T 队列元素类型
		template <typename T>
		class AsyncQueue
		{
		public:
			using Pointer = std::shared_ptr<AsyncQueue<T>>;
			using QueueType = std::queue<T>;
		public:
			/// @brief 构造函数
			/// @param capacity [IN] 队列容量，0表示无限制
			explicit AsyncQueue(size_t capacity = 0);

			/// @brief 析构函数
			~AsyncQueue();

			/// @brief 将元素推入队列
			/// @param element [IN] 待推入的元素
			/// @param timeoutMs [IN] 超时时间（毫秒），0表示无限等待
			/// @return 如果推入成功返回true，否则返回false
			bool Push(const T& element, uint64_t timeoutMs = 0);

			/// @brief 将元素推入队列（移动语义）
			/// @param element [IN] 待推入的元素
			/// @param timeoutMs [IN] 超时时间（毫秒），0表示无限等待
			/// @return 如果推入成功返回true，否则返回false
			bool Push(T&& element, uint64_t timeoutMs = 0);

			/// @brief 尝试将元素推入队列（非阻塞）
			/// @param element [IN] 待推入的元素
			/// @return 如果推入成功返回true，否则返回false
			bool TryPush(const T& element);

			/// @brief 从队列弹出元素
			/// @param element [OUT] 接收弹出的元素
			/// @param timeoutMs [IN] 超时时间（毫秒），0表示无限等待
			/// @return 如果弹出成功返回true，否则返回false
			bool Pop(T& element, uint64_t timeoutMs = 0);

			/// @brief 尝试从队列弹出元素（非阻塞）
			/// @param element [OUT] 接收弹出的元素
			/// @return 如果弹出成功返回true，否则返回false
			bool TryPop(T& element);

			/// @brief 获取队列当前大小
			/// @return 队列大小
			size_t Size() const;

			/// @brief 检查队列是否为空
			/// @return 如果队列为空返回true，否则返回false
			bool Empty() const;

			/// @brief 获取队列容量
			/// @return 队列容量
			size_t GetCapacity() const;

			/// @brief 设置队列容量
			/// @param capacity [IN] 新的队列容量，0表示无限制
			void SetCapacity(size_t capacity);

			/// @brief 清空队列
			void Clear();

			/// @brief 停止队列（所有等待的操作将被唤醒并返回失败）
			void Stop();

			/// @brief 恢复队列操作
			void Resume();

			/// @brief 检查队列是否已停止
			/// @return 如果已停止返回true，否则返回false
			bool IsStopped() const;

		private:
			/// @brief 等待队列非满
			/// @param lock [IN] 互斥锁的引用
			/// @param timeoutMs [IN] 超时时间（毫秒）
			/// @return 如果队列非满返回true，否则返回false
			bool WaitForNotFull(std::unique_lock<std::mutex>& lock, uint64_t timeoutMs);

			/// @brief 等待队列非空
			/// @param lock [IN] 互斥锁的引用
			/// @param timeoutMs [IN] 超时时间（毫秒）
			/// @return 如果队列非空返回true，否则返回false
			bool WaitForNotEmpty(std::unique_lock<std::mutex>& lock, uint64_t timeoutMs);

		private:
			mutable std::mutex m_mutex;					///< 互斥锁，保护队列数据
			std::condition_variable m_notEmptyCondVar;	///< 非空条件变量
			std::condition_variable m_notFullCondVar;	///< 非满条件变量
			std::atomic<bool> m_stopped;				///< 队列停止标志
			QueueType m_queue;							///< 队列容器
			size_t m_capacity;							///< 队列容量
		};
	} // namespace Utils
} // namespace IDLog

#include "IDLog/Utils/AsyncQueue.inl"

#endif // !IDLOG_UTILS_ASYNCQUEUE_H