/** 
 * @Description: IDLog 基础用法示例
 * @Author: InverseDark
 * @Date: 2025-12-27 13:07:38
 * @LastEditTime: 2025-12-27 13:13:38
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <string>

// 演示：使用全局宏记录日志（最常用）
void DemoMacros()
{
    std::cout << "\n[1] 使用全局宏记录日志" << std::endl;
    
    IDLOG_TRACE("这是一条 TRACE 消息 (可能不会显示，取决于默认级别)");
    IDLOG_DEBUG("这是一条 DEBUG 消息");
    IDLOG_INFO("这是一条 INFO 消息");
    IDLOG_WARN("这是一条 WARN 消息");
    IDLOG_ERROR("这是一条 ERROR 消息");
    IDLOG_FATAL("这是一条 FATAL 消息");
}

// 演示：使用格式化宏
void DemoFormattedMacros()
{
    std::cout << "\n[2] 使用格式化宏" << std::endl;
    
    int userId = 1001;
    std::string userName = "Alice";
    double processingTime = 12.5;

    IDLOG_INFO_FMT("用户登录: ID=%d, Name=%s", userId, userName.c_str());
    IDLOG_WARN_FMT("处理耗时较长: %.2f ms", processingTime);
}

// 演示：手动获取 Logger 对象
void DemoLoggerObject()
{
    std::cout << "\n[3] 手动获取 Logger 对象" << std::endl;

    // 获取根 Logger
    auto rootLogger = IDLog::LoggerManager::GetInstance().GetRootLogger();
    rootLogger->Info("通过 RootLogger 对象记录的消息");

    // 获取（或创建）一个具名 Logger
    auto networkLogger = IDLog::LoggerManager::GetInstance().GetLogger("NetworkModule");
    networkLogger->SetLevel(IDLog::LogLevel::DBG); // 单独设置级别
    
    networkLogger->Debug("网络连接初始化...");
    networkLogger->Info("网络连接成功: 192.168.1.100");
}

int main()
{
    std::cout << "=== IDLog 基础示例 ===" << std::endl;

    // 初始化（可选，默认已初始化）
    // IDLog::LoggerManager::GetInstance().GetRootLogger()->SetLevel(IDLog::LogLevel::INFO);

    DemoMacros();
    DemoFormattedMacros();
    DemoLoggerObject();

	 // 关闭日志系统，确保所有日志写入完成
	IDLOG_SHUTDOWN();
    std::cout << "\n=== 示例结束 ===" << std::endl;
    return 0;
}