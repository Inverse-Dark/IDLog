/** 
 * @Description: 输出器测试 (Console, File)
 * @Author: InverseDark
 * @Date: 2025-12-27 13:19:37
 * @LastEditTime: 2025-12-27 13:19:40
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cassert>

void TestConsoleAppender()
{
    std::cout << "[Test] Console Appender..." << std::endl;
    auto appender = std::make_shared<IDLog::ConsoleAppender>();
    appender->SetUseColor(true);
    // 视觉检查：应该看到彩色输出
    std::cout << "  (Visual Check) Should see colored output below:" << std::endl;
    
    IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
    auto event = std::make_shared<IDLog::LogEvent>(
        IDLog::LogLevel::ERR, "ConsoleTest", "This is a colored ERROR message", loc);
    appender->Append(event);
    std::cout << "  -> Passed (Visual)" << std::endl;
}

void TestFileAppender()
{
    std::cout << "[Test] File Appender..." << std::endl;
    std::string filename = "test_appender.log";
    std::filesystem::remove(filename);
    
    auto appender = std::make_shared<IDLog::FileAppender>(filename);
    IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
    auto event = std::make_shared<IDLog::LogEvent>(
        IDLog::LogLevel::INFO, "FileTest", "File content test", loc);
        
    appender->Append(event);
    appender->Flush();
    
    // 验证文件是否存在且有内容
    std::ifstream file(filename);
    assert(file.is_open());
    std::string line;
    std::getline(file, line);
    assert(line.find("File content test") != std::string::npos);
    file.close();
    
    std::cout << "  -> Passed" << std::endl;
}

int main()
{
    std::cout << "=== IDLog Appender Tests ===" << std::endl;
    TestConsoleAppender();
    TestFileAppender();
    std::cout << "=== All Appender Tests Passed ===" << std::endl;
    return 0;
}