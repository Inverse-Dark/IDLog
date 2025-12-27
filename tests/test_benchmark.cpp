/** 
 * @Description: 综合性能基准测试 (同步 vs 异步)
 * @Author: InverseDark
 * @Date: 2025-12-27 12:12:16
 * @LastEditTime: 2025-12-27 12:12:53
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <filesystem>

// 测试配置
const int THREAD_COUNT = 1;          // 线程数
const int LOGS_PER_THREAD = 500000;  // 每个线程的日志数
const int TOTAL_LOGS = THREAD_COUNT * LOGS_PER_THREAD;

// 辅助函数：清理旧日志文件
void CleanLogFiles() {
    std::filesystem::remove("bench_sync.log");
    std::filesystem::remove("bench_async.log");
}

// 辅助函数：执行测试任务
long long RunBenchmark(const std::string& testName, std::function<void()> logFunc) {
    std::cout << "正在运行 [" << testName << "] 测试..." << std::endl;
    
    std::atomic<int> counter(0);
    std::vector<std::thread> threads;
    
    auto worker = [&]() {
        for (int i = 0; i < LOGS_PER_THREAD; ++i) {
            logFunc();
            counter.fetch_add(1, std::memory_order_relaxed);
        }
    };

    auto start = std::chrono::steady_clock::now();
    
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back(worker);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    std::cout << "  -> 完成! 耗时: " << duration << " ms" << std::endl;
    return duration;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "      IDLog 性能基准测试 (Benchmark)     " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "配置信息:" << std::endl;
    std::cout << "  线程数: " << THREAD_COUNT << std::endl;
    std::cout << "  单线程日志数: " << LOGS_PER_THREAD << std::endl;
    std::cout << "  总日志数: " << TOTAL_LOGS << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    CleanLogFiles();
    IDLog::LoggerManager::GetInstance().GetRootLogger()->SetLevel(IDLog::LogLevel::INFO);

    // -------------------------------------------------
    // 1. 测试同步日志 (Sync Logging)
    // -------------------------------------------------
    {
        auto syncLogger = IDLOG_GET_LOGGER("SyncBench");
        syncLogger->ClearAppenders();
        
        // 使用带缓冲优化的 FileAppender
        auto fileAppender = std::make_shared<IDLog::FileAppender>(
            "bench_sync.log",
            nullptr, // 默认格式化器
            IDLog::FileAppender::RollPolicy::NONE,
            100 * 1024 * 1024
        );
        syncLogger->AddAppender(fileAppender);

        long long syncDuration = RunBenchmark("同步日志 (Sync)", [&]() {
            syncLogger->Info("Benchmark test message for sync logging 1234567890");
        });

        // -------------------------------------------------
        // 2. 测试异步日志 (Async Logging)
        // -------------------------------------------------
        auto asyncLogger = IDLOG_GET_LOGGER("AsyncBench");
        asyncLogger->ClearAppenders();

        auto asyncFileAppender = std::make_shared<IDLog::FileAppender>(
            "bench_async.log",
            nullptr,
            IDLog::FileAppender::RollPolicy::NONE,
            100 * 1024 * 1024
        );

        auto asyncAppender = std::make_shared<IDLog::AsyncAppender>(
            asyncFileAppender,
            500000, // 队列容量
            20,     // 批处理大小
            2000,   // 刷新间隔
            IDLog::AsyncAppender::OverflowPolicy::BLOCK
        );
        asyncAppender->Start();
        asyncLogger->AddAppender(asyncAppender);

        // 预热一下异步线程
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        long long asyncDuration = RunBenchmark("异步日志 (Async)", [&]() {
            asyncLogger->Info("Benchmark test message for async logging 1234567890");
        });

        // 等待异步队列处理完（为了公平对比，这里不计入主线程耗时，但实际应用中这是后台开销）
        std::cout << "  -> 等待异步队列落盘..." << std::endl;
        asyncAppender->Stop(true);

        // -------------------------------------------------
        // 3. 输出对比报告
        // -------------------------------------------------
        double syncQPS = (double)TOTAL_LOGS * 1000.0 / syncDuration;
        double asyncQPS = (double)TOTAL_LOGS * 1000.0 / asyncDuration;

        std::cout << "\n========================================" << std::endl;
        std::cout << "           测试结果对比报告             " << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << std::left << std::setw(15) << "模式" 
                  << std::setw(15) << "总耗时(ms)" 
                  << std::setw(15) << "QPS (条/秒)" 
                  << std::setw(15) << "单条耗时(us)" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << std::left << std::setw(15) << "同步 (Sync)" 
                  << std::setw(15) << syncDuration 
                  << std::setw(15) << (long long)syncQPS 
                  << std::setw(15) << (1000.0 * syncDuration / TOTAL_LOGS) << std::endl;
                  
        std::cout << std::left << std::setw(15) << "异步 (Async)" 
                  << std::setw(15) << asyncDuration 
                  << std::setw(15) << (long long)asyncQPS 
                  << std::setw(15) << (1000.0 * asyncDuration / TOTAL_LOGS) << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << "性能提升倍数: " << std::fixed << std::setprecision(2) << (asyncQPS / syncQPS) << "x" << std::endl;
        std::cout << "========================================" << std::endl;
    }

    return 0;
}