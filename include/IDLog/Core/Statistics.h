/**
 * @Description: 统计信息头文件
 * @Author: InverseDark
 * @Date: 2025-12-23 10:07:00
 * @LastEditTime: 2025-12-23 16:03:47
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_STATISTICS_H
#define IDLOG_CORE_STATISTICS_H

#include "IDLog/Core/LogLevel.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <map>
#include <functional>

namespace IDLog
{
	/// @brief 级别统计信息类
	class IDLOG_API LevelStatistics
	{
	public:
		/// @brief 构造函数
		LevelStatistics();
		/// @brief 析构函数
		~LevelStatistics();

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个LevelStatistics对象
		LevelStatistics(const LevelStatistics& other);

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个LevelStatistics对象
		/// @return 当前LevelStatistics对象的引用
		LevelStatistics& operator=(const LevelStatistics& other);

		/// @brief 获取日志计数
		/// @return 日志计数
		uint64_t GetCount() const;
		/// @brief 获取日志字节数
		/// @return 日志字节数
		uint64_t GetBytes() const;
		/// @brief 获取最大消息大小
		/// @return 最大消息大小（字节数）
		uint64_t GetMaxMessageSize() const;

		/// @brief 增加日志计数
		void IncrementCount();
		/// @brief 减少日志计数
		void DecrementCount();
		/// @brief 增加日志字节数
		/// @param bytes [IN] 增加的字节数
		void AddBytes(uint64_t bytes);
		///  @brief 减少日志字节数
		/// @param bytes [IN] 减少的字节数
		void SubtractBytes(uint64_t bytes);
		/// @brief 更新最大消息大小
		/// @param messageSize [IN] 新的消息大小
		void UpdateMaxMessageSize(uint64_t messageSize);

		/// @brief 重置级别统计信息
		void Reset();
	private:
		/// @brief 级别统计信息实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl; ///< 级别统计信息实现指针
	};

	/// @brief 时间窗口类
	class IDLOG_API TimeWindow
	{
	public:
		/// @brief 构造函数
		TimeWindow();
		/// @brief 析构函数
		~TimeWindow();

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个TimeWindow对象
		TimeWindow(const TimeWindow& other);

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个TimeWindow对象
		/// @return 当前TimeWindow对象的引用
		TimeWindow& operator=(const TimeWindow& other);


		/// @brief 获取窗口开始时间
		/// @return 窗口开始时间点
		std::chrono::steady_clock::time_point GetStartTime() const;

		/// @brief 获取每秒日志数
		/// @return 每秒日志数
		uint64_t GetLogsPerSecond() const;

		/// @brief 获取每秒字节数
		/// @return 每秒字节数
		uint64_t GetBytesPerSecond() const;

		/// @brief 设置窗口开始时间
		/// @param timePoint [IN] 窗口开始时间点
		void SetStartTime(const std::chrono::steady_clock::time_point& timePoint);

		/// @brief 设置每秒日志数
		/// @param logsPerSecond [IN] 每秒日志数
		void SetLogsPerSecond(uint64_t logsPerSecond);

		/// @brief 设置每秒字节数
		/// @param bytesPerSecond [IN] 每秒字节数
		void SetBytesPerSecond(uint64_t bytesPerSecond);

		/// @brief 重置时间窗口
		void Reset();

	private:
		/// @brief 时间窗口实现结构体前向声明
		struct Impl;

	private:
		Impl* m_pImpl; ///< 时间窗口实现指针
	};

	/// @brief 日志统计信息类
	class IDLOG_API LogStatistics
	{
	public:
		/// @brief 构造函数
		LogStatistics();

		/// @brief 析构函数
		~LogStatistics();

		/// @brief 拷贝构造函数
		/// @param other [IN] 另一个LogStatistics对象
		LogStatistics(const LogStatistics& other);

		/// @brief 拷贝赋值运算符
		/// @param other [IN] 另一个LogStatistics对象
		/// @return 当前LogStatistics对象的引用
		LogStatistics& operator=(const LogStatistics& other);

		/// @brief 重置统计信息
		void Reset();

		/// @brief 获取指定级别的统计信息
		/// @param level [IN] 日志级别
		/// @return 指定级别的统计信息引用
		LevelStatistics& GetLevelStatistics(LogLevel level);

		/// @brief 获取指定级别的统计信息（常量版本）
		/// @param level [IN] 日志级别
		/// @return 指定级别的统计信息引用
		const LevelStatistics& GetLevelStatistics(LogLevel level) const;

		/// @brief 获取总日志数量
		/// @return 总日志数量
		uint64_t GetTotalLogs() const;

		/// @brief 获取总日志字节数
		/// @return 总日志字节数
		uint64_t GetTotalBytes() const;

		/// @brief 获取丢弃的日志数量
		/// @return 丢弃的日志数量
		uint64_t GetDroppedLogs() const;

		/// @brief 获取丢弃的日志字节数
		/// @return 丢弃的日志字节数
		uint64_t GetDroppedBytes() const;

		/// @brief 记录日志
		/// @param level [IN] 日志级别
		/// @param messageSize [IN] 日志消息大小（字节数）
		/// @param waitTimeUs [IN] 等待时间（微秒）
		void RecordLog(LogLevel level, size_t messageSize, uint64_t waitTimeUs = 0);

		/// @brief 记录丢弃的日志
		/// @param messageSize [IN] 日志消息大小（字节数）
		void RecordDroppedLog(size_t messageSize);

		/// @brief 更新时间窗口统计信息
		void UpdateTimeWindow();

		/// @brief 将统计信息转换为字符串
		/// @return 统计信息的字符串表示
		std::string ToString() const;

		/// @brief 将统计信息转换为JSON字符串
		/// @param pretty [IN] 是否格式化输出
		/// @return 统计信息的JSON字符串表示
		std::string ToJson(bool pretty = false) const;

		/// @brief 更新最大值原子变量
		/// @param maxValue [IN/OUT] 最大值原子变量
		/// @param newValue [IN] 新值
		static void UpdateMax(std::atomic<uint64_t>& maxValue, uint64_t newValue);

	private:
		/// @brief 日志统计信息实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl; ///< 日志统计信息实现指针
	};

	/// @brief 统计管理类
	/// @details 提供日志统计信息的管理和查询功能
	class IDLOG_API StatisticsManager
	{
	public:
		/// @brief 获取统计管理类单例实例
		/// @return 统计管理类实例引用
		static StatisticsManager& GetInstance();

		/// @brief 拷贝构造函数(禁用)
		/// @param [IN] 另一个StatisticsManager对象
		StatisticsManager(const StatisticsManager&) = delete;
		/// @brief 拷贝赋值运算符(禁用)
		/// @param [IN] 另一个StatisticsManager对象
		/// @return 当前StatisticsManager对象的引用
		StatisticsManager& operator=(const StatisticsManager&) = delete;

		/// @brief 记录日志
		/// @param loggerName [IN] 日志器名称
		/// @param level [IN] 日志级别
		/// @param messageSize [IN] 日志消息大小（字节数）
		/// @param waitTimeUs [IN] 等待时间（微秒）
		void RecordLog(const std::string& loggerName, LogLevel level,
			size_t messageSize, uint64_t waitTimeUs = 0);

		/// @brief 记录丢弃的日志
		/// @param loggerName [IN] 日志器名称
		/// @param messageSize [IN] 日志消息大小（字节数）
		void RecordDroppedLog(const std::string& loggerName, size_t messageSize);

		/// @brief 获取全局统计信息
		/// @return 全局统计信息
		LogStatistics GetGlobalStatistics() const;

		/// @brief 获取指定日志器的统计信息
		/// @param loggerName [IN] 日志器名称
		/// @return 指定日志器的统计信息
		LogStatistics GetLoggerStatistics(const std::string& loggerName) const;

		/// @brief 获取所有日志器的统计信息
		/// @return 日志器名称到统计信息的映射
		std::map<std::string, LogStatistics> GetAllLoggerStatistics() const;

		/// @brief 重置指定日志器的统计信息
		/// @param loggerName [IN] 日志器名称
		void ResetLoggerStatistics(const std::string& loggerName);

		/// @brief 重置所有统计信息
		void ResetAllStatistics();

		/// @brief 启用/禁用统计
		/// @param enabled [IN] 启用/禁用
		void EnableStatistics(bool enabled = true);

		/// @brief 检查统计是否启用
		/// @return 启用返回true，否则返回false
		bool IsStatisticsEnabled() const;

		/// @brief 设置统计报告间隔，单位秒
		/// @param intervalSeconds [IN] 统计报告间隔，单位秒
		void SetStatisticsInterval(uint64_t intervalSeconds);

		/// @brief 获取统计报告间隔，单位秒
		/// @return 统计报告间隔，单位秒
		uint64_t GetStatisticsInterval() const;

		/// @brief 注册统计报告回调函数
		/// @param callback [IN] 回调函数
		void RegisterReportCallback(const std::function<void(const std::string&)>& callback);

		/// @brief 生成统计报告字符串
		/// @return 统计报告字符串
		std::string GenerateReport() const;

	private:
		/// @brief 构造函数
		StatisticsManager();
		/// @brief 析构函数
		~StatisticsManager();

		/// @brief 检查是否需要生成统计报告
		void CheckReport();
	private:
		/// @brief 统计管理类实现结构体前向声明
		struct Impl;
	private:
		Impl* m_pImpl;				///< 统计管理类实现指针
	};


} // namespace IDLog

#endif // !IDLOG_CORE_STATISTICS_H