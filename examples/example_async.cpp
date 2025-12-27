/** 
 * @Description: IDLog 异步日志示例 (手动配置)
 * @Author: InverseDark
 * @Date: 2025-12-27 13:08:21
 * @LastEditTime: 2025-12-27 14:17:42
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

// 演示：手动组装异步 Appender
void SetupAsyncLogging()
{
    auto& loggerMgr = IDLog::LoggerManager::GetInstance();
    auto rootLogger = loggerMgr.GetRootLogger();
    rootLogger->ClearAppenders(); // 清除默认的同步 Appender

    // 1. 创建后端：文件输出器
    auto fileAppender = std::make_shared<IDLog::FileAppender>(
        "example_async.log",
        nullptr, // 使用默认格式
        IDLog::FileAppender::RollPolicy::SIZE,
        10 * 1024 * 1024 // 10MB 滚动
    );

    // 2. 创建前端：异步包装器
    auto asyncAppender = std::make_shared<IDLog::AsyncAppender>(
        fileAppender,
        100000, // 队列容量
        100,    // 批处理大小
        1000,   // 刷新间隔 (ms)
        IDLog::AsyncAppender::OverflowPolicy::BLOCK // 队列满时阻塞
    );

    // 3. 启动异步线程
    asyncAppender->Start();

    // 4. 挂载到 Logger
    rootLogger->AddAppender(asyncAppender);
    
    // 可选：同时也输出到控制台（通常控制台不需要异步，除非量极大）
    auto consoleAppender = std::make_shared<IDLog::ConsoleAppender>();
    rootLogger->AddAppender(consoleAppender);
}

void RunLoadTest()
{
    std::cout << "正在生成大量日志..." << std::endl;
    
    const int threadCount = 4;
    const int logsPerThread = 10000;
    std::vector<std::thread> threads;
    std::atomic<int> counter(0);

    auto worker = [&](int id) {
        for (int i = 0; i < logsPerThread; ++i) {
            IDLOG_INFO_FMT("Thread-%d log message #%d", id, i);
            counter++;
        }
    };

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(worker, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "完成! 写入 " << counter << " 条日志，耗时 " << duration << " ms" << std::endl;
}

int main()
{
    std::cout << "=== IDLog 异步日志示例 ===" << std::endl;

    SetupAsyncLogging();
    RunLoadTest();

	 // 关闭日志系统，确保所有日志写入完成
	IDLOG_SHUTDOWN();
    std::cout << "\n=== 示例结束 ===" << std::endl;
    return 0;
}