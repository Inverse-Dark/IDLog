/** 
 * @Description: 核心功能测试 (Level, Event, Logger, Macros)
 * @Author: InverseDark
 * @Date: 2025-12-27 13:08:58
 * @LastEditTime: 2025-12-27 13:18:46
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <cassert>
#include <string>

void TestLogLevel()
{
    std::cout << "[Test] LogLevel Conversion..." << std::endl;
    assert(std::string(IDLog::LevelToString(IDLog::LogLevel::INFO)) == "INFO");
    assert(IDLog::StringToLevel("ERROR") == IDLog::LogLevel::ERR);
    assert(IDLog::StringToLevel("UNKNOWN_LEVEL") == IDLog::LogLevel::INFO);
    std::cout << "  -> Passed" << std::endl;
}

void TestLogEvent()
{
    std::cout << "[Test] LogEvent..." << std::endl;
    IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
    IDLog::LogEvent event(IDLog::LogLevel::INFO, "TestLogger", "Test Message", loc);
    
    assert(event.GetLevel() == IDLog::LogLevel::INFO);
    assert(event.GetLoggerName() == "TestLogger");
    assert(event.GetLogMessage() == "Test Message");
    assert(event.GetThreadId() != "");
    std::cout << "  -> Passed" << std::endl;
}

void TestLoggerHierarchy()
{
    std::cout << "[Test] Logger Hierarchy..." << std::endl;
    auto root = IDLog::LoggerManager::GetInstance().GetRootLogger();
    auto child = IDLog::LoggerManager::GetInstance().GetLogger("Parent.Child");
    
    // 默认情况下，子 Logger 应该继承 Root 的 Appender
    // 这里我们简单验证获取逻辑
    assert(root != nullptr);
    assert(child != nullptr);
    assert(child->GetName() == "Parent.Child");
    std::cout << "  -> Passed" << std::endl;
}

void TestMacros()
{
    std::cout << "[Test] Logging Macros..." << std::endl;
    // 宏测试主要看编译是否通过，以及运行时是否崩溃
    IDLOG_TRACE("Trace macro test");
    IDLOG_DEBUG("Debug macro test");
    IDLOG_INFO("Info macro test");
    IDLOG_WARN("Warn macro test");
    IDLOG_ERROR("Error macro test");
    IDLOG_FATAL("Fatal macro test");
    
    IDLOG_INFO_FMT("Formatted macro test: %d, %s", 123, "abc");
    std::cout << "  -> Passed" << std::endl;
}

int main()
{
    std::cout << "=== IDLog Core Tests ===" << std::endl;
    TestLogLevel();
    TestLogEvent();
    TestLoggerHierarchy();
    TestMacros();
    std::cout << "=== All Core Tests Passed ===" << std::endl;
    return 0;
}