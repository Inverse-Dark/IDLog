/*
 * @Description: 异步队列内联实现文件
 * @Author: InverseDark
 * @Date: 2025-12-24 10:29:36
 * @LastEditTime: 2025-12-27 12:47:31
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_UTILS_ASYNCQUEUE_INL
#define IDLOG_UTILS_ASYNCQUEUE_INL

namespace IDLog
{
	namespace Utils
	{
		template <typename T>
		AsyncQueue<T>::AsyncQueue(size_t capacity)
			: m_capacity(capacity), m_stopped(false)
		{
		}

		template <typename T>
		AsyncQueue<T>::~AsyncQueue()
		{
			Stop();
			Clear();
		}

		template <typename T>
		bool AsyncQueue<T>::Push(const T &element, uint64_t timeoutMs)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// 等待队列非满
			if (!WaitForNotFull(lock, timeoutMs))
			{
				return false; // 超时或已停止
			}

			// 检查是否已停止
			if (m_stopped.load())
			{
				return false;
			}

            bool wasEmpty = m_queue.empty();
            m_queue.push(element);
            
            // 只有当队列之前为空时，才需要唤醒消费者
            if (wasEmpty)
            {
                m_notEmptyCondVar.notify_one();
            }

			return true;
		}

		template <typename T>
		bool AsyncQueue<T>::Push(T &&element, uint64_t timeoutMs)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// 等待队列非满
			if (!WaitForNotFull(lock, timeoutMs))
			{
				return false; // 超时或已停止
			}

			// 检查是否已停止
			if (m_stopped.load())
			{
				return false;
			}

            bool wasEmpty = m_queue.empty();
            m_queue.push(std::move(element));
            
            // 只有当队列之前为空时，才需要唤醒消费者
            if (wasEmpty)
            {
                m_notEmptyCondVar.notify_one();
            }

			return true;
		}

		template <typename T>
		bool AsyncQueue<T>::TryPush(const T &element)
		{
			std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);

			if (!lock.owns_lock() || (m_capacity != 0 && m_queue.size() >= m_capacity))
			{
				return false;
			}

            bool wasEmpty = m_queue.empty();
            m_queue.push(element);
            
            // 只有当队列之前为空时，才需要唤醒消费者
            if (wasEmpty)
            {
                m_notEmptyCondVar.notify_one();
            }

			return true;
		}

		template <typename T>
		bool AsyncQueue<T>::Pop(T &element, uint64_t timeoutMs)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			bool wasFull = (m_capacity > 0 && m_queue.size() >= m_capacity);

			// 立即检查是否有数据，避免不必要的等待
			if (!m_queue.empty())
			{
				element = std::move(m_queue.front());
				m_queue.pop();
				if (wasFull) m_notFullCondVar.notify_one();
				return true;
			}

			// 等待队列非空
			if (!WaitForNotEmpty(lock, timeoutMs))
			{
				return false; // 超时或已停止
			}

			element = std::move(m_queue.front());
			m_queue.pop();
			if (wasFull) m_notFullCondVar.notify_one();

			return true;
		}

		template <typename T>
		bool AsyncQueue<T>::TryPop(T &element)
		{
			std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);

			if (!lock.owns_lock() || m_queue.empty())
			{
				return false;
			}

            bool wasFull = (m_capacity > 0 && m_queue.size() >= m_capacity);

			element = std::move(m_queue.front());
			m_queue.pop();
			
			// 只有当队列之前是满的，现在变不满了，才需要唤醒生产者
            if (wasFull)
            {
                m_notFullCondVar.notify_one();
            }

			return true;
		}

		template <typename T>
		size_t AsyncQueue<T>::Size() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.size();
		}

		template <typename T>
		bool AsyncQueue<T>::Empty() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_queue.empty();
		}

		template <typename T>
		size_t AsyncQueue<T>::GetCapacity() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_capacity;
		}

		template <typename T>
		void AsyncQueue<T>::SetCapacity(size_t capacity)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_capacity = capacity;
			if (m_capacity == 0 || m_queue.size() < m_capacity)
			{
				m_notFullCondVar.notify_all();
			}
		}

		template <typename T>
		void AsyncQueue<T>::Clear()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			while (!m_queue.empty())
			{
				m_queue.pop();
			}
			m_notFullCondVar.notify_all();
		}

		template <typename T>
		void AsyncQueue<T>::Stop()
		{
			m_stopped.store(true);
			m_notEmptyCondVar.notify_all();
			m_notFullCondVar.notify_all();
		}

		template <typename T>
		void AsyncQueue<T>::Resume()
		{
			m_stopped.store(false);
		}

		template <typename T>
		bool AsyncQueue<T>::IsStopped() const
		{
			return m_stopped.load();
		}

		template <typename T>
		bool AsyncQueue<T>::WaitForNotFull(std::unique_lock<std::mutex> &lock, uint64_t timeoutMs)
		{
			// 如果容量为0，表示无限制，直接返回true
			if (m_capacity == 0)
			{
				return true;
			}

			// 等待条件：队列未满或已停止
			auto predicate = [this]()
				{
					return m_stopped.load() || m_queue.size() < m_capacity;
				};

			if (timeoutMs == 0)
			{
				// 无限等待
				m_notFullCondVar.wait(lock, predicate);
				return !m_stopped.load();
			}
			else
			{
				// 有限等待
				auto status = m_notFullCondVar.wait_for(lock, std::chrono::milliseconds(timeoutMs), predicate);
				return status && !m_stopped.load();
			}
		}

		template <typename T>
		bool AsyncQueue<T>::WaitForNotEmpty(std::unique_lock<std::mutex> &lock, uint64_t timeoutMs)
		{
			// 等待条件：队列非空或已停止
			auto predicate = [this]()
				{
					return m_stopped.load() || !m_queue.empty();
				};

			if (timeoutMs == 0)
			{
				// 无限等待
				m_notEmptyCondVar.wait(lock, predicate);
				return !m_stopped.load();
			}
			else
			{
				// 有限等待
				auto status = m_notEmptyCondVar.wait_for(lock, std::chrono::milliseconds(timeoutMs), predicate);
				return status && !m_stopped.load();
			}
		}

	} // namespace Utils
} // namespace IDLog

#endif // !IDLOG_UTILS_ASYNCQUEUE_INL