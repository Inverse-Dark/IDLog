/** 
 * @Description: 过滤器测试
 * @Author: InverseDark
 * @Date: 2025-12-27 13:20:17
 * @LastEditTime: 2025-12-27 13:20:20
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <cassert>

// 模拟 Appender，用于计数
class CounterAppender : public IDLog::LogAppender
{
public:
    int count = 0;
    void Append(const IDLog::LogEvent::Pointer&) override { count++; }
    std::string GetName() const override { return "Counter"; }
    void Flush() override {}
};

void TestLevelFilter()
{
    std::cout << "[Test] Level Filter..." << std::endl;
    auto logger = std::make_shared<IDLog::Logger>("FilterTest");
    auto appender = std::make_shared<CounterAppender>();
    logger->AddAppender(appender);
    
    // 只允许 ERROR 级别
    auto filter = std::make_shared<IDLog::LevelFilter>(IDLog::LogLevel::ERR);
    logger->AddFilter(filter);
    
    logger->Info("Info msg");  // Should be filtered
    logger->Error("Error msg"); // Should pass
    logger->Warn("Warn msg");  // Should be filtered
    
    assert(appender->count == 1);
    std::cout << "  -> Passed" << std::endl;
}

int main()
{
    std::cout << "=== IDLog Filter Tests ===" << std::endl;
    TestLevelFilter();
    std::cout << "=== All Filter Tests Passed ===" << std::endl;
    return 0;
}