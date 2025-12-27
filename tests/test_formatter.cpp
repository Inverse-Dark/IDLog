/** 
 * @Description: 格式化器测试
 * @Author: InverseDark
 * @Date: 2025-12-27 13:10:05
 * @LastEditTime: 2025-12-27 13:19:21
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <sstream>
#include <cassert>

// 自定义 Appender 用于捕获格式化后的输出
class StringStreamAppender : public IDLog::LogAppender
{
public:
    std::string GetOutput() { return m_ss.str(); }
    void Clear() { m_ss.str(""); m_ss.clear(); }
    
    void Append(const IDLog::LogEvent::Pointer& event) override {
        if (auto fmt = GetFormatter()) {
            m_ss << fmt->Format(event);
        }
    }
    std::string GetName() const override { return "StringStreamAppender"; }
    void Flush() override {}

private:
    std::stringstream m_ss;
};

void TestPattern()
{
    std::cout << "[Test] Pattern Formatter..." << std::endl;
    
    auto appender = std::make_shared<StringStreamAppender>();
    
    // 测试简单的消息格式
    auto fmt1 = std::make_shared<IDLog::PatternFormatter>("[%p] %m");
    appender->SetFormatter(fmt1);
    
    IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
    IDLog::LogEvent::Pointer event = std::make_shared<IDLog::LogEvent>(
        IDLog::LogLevel::INFO, "TestLogger", "Hello World", loc);
        
    appender->Append(event);
    assert(appender->GetOutput() == "[INFO] Hello World");
    appender->Clear();
    
    // 测试复杂的格式
    auto fmt2 = std::make_shared<IDLog::PatternFormatter>("Level=%p Logger=%c Msg=%m");
    appender->SetFormatter(fmt2);
    appender->Append(event);
    assert(appender->GetOutput() == "Level=INFO Logger=TestLogger Msg=Hello World");
    
    std::cout << "  -> Passed" << std::endl;
}

int main()
{
    std::cout << "=== IDLog Formatter Tests ===" << std::endl;
    TestPattern();
    std::cout << "=== All Formatter Tests Passed ===" << std::endl;
    return 0;
}