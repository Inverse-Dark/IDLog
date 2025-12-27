/** 
 * @Description: 异步功能测试 (逻辑正确性)
 * @Author: InverseDark
 * @Date: 2025-12-25 14:50:17
 * @LastEditTime: 2025-12-27 13:21:13
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <thread>
#include <cassert>
#include <fstream>

void TestAsyncLogic()
{
    std::cout << "[Test] Async Appender Logic..." << std::endl;
    
    // 使用文件作为后端，验证最终写入
    std::string filename = "test_async_logic.log";
    std::filesystem::remove(filename);
    
    auto fileAppender = std::make_shared<IDLog::FileAppender>(filename);
    auto asyncAppender = std::make_shared<IDLog::AsyncAppender>(
        fileAppender, 1000, 10, 100, IDLog::AsyncAppender::OverflowPolicy::BLOCK);
        
    asyncAppender->Start();
    
    IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
    for(int i=0; i<100; ++i) {
        auto event = std::make_shared<IDLog::LogEvent>(
            IDLog::LogLevel::INFO, "AsyncTest", "Async msg " + std::to_string(i), loc);
        asyncAppender->Append(event);
    }
    
    // 停止并等待写入完成
    asyncAppender->Stop(true);
    
    // 验证文件行数
    std::ifstream file(filename);
    int lines = 0;
    std::string line;
    while(std::getline(file, line)) {
        lines++;
    }
    
    assert(lines == 100);
    std::cout << "  -> Passed (Written " << lines << " lines)" << std::endl;
}

int main()
{
    std::cout << "=== IDLog Async Tests ===" << std::endl;
    TestAsyncLogic();
    std::cout << "=== All Async Tests Passed ===" << std::endl;
    return 0;
}