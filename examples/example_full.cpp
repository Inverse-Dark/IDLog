/** 
 * @Description: IDLog 综合示例 (配置+异步+多模块)
 * @Author: InverseDark
 * @Date: 2025-12-27 13:15:58
 * @LastEditTime: 2025-12-27 14:17:50
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

// 创建一个包含异步配置的配置文件
void CreateFullConfig(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << R"(
[global]
rootLevel=INFO

[logger.ROOT]
level=INFO
appenders=AsyncFile,Console

[logger.Network]
level=DEBUG
appenders=AsyncFile
additivity=false

[logger.Database]
level=WARN
appenders=AsyncFile

# 异步文件输出器配置
[appender.AsyncFile]
type=async
queueCapacity=50000
batchSize=50
flushIntervalMs=2000
backendType=file
backend.formatter=Detailed
backend.filename=example_full.log
backend.rollPolicy=daily

# 实际的文件输出器
[appender.RealFile]
type=file
formatter=Detailed
filename=example_full.log
rollPolicy=daily

[appender.Console]
type=console
formatter=Simple
useColor=true

# 简单格式化器
[formatter.Simple]
type=pattern
pattern=%d{%H:%M:%S} %-5p %c - %m%n

[formatter.Detailed]
type=pattern
pattern=%d{%Y-%m-%d %H:%M:%S.%ms} [%t] %-5p %c - %m%n

)";
        file.close();
    }
}

void SimulateBusinessLogic()
{
    auto netLogger = IDLOG_GET_LOGGER("Network");
    auto dbLogger = IDLOG_GET_LOGGER("Database");

    // 模拟网络请求
    netLogger->Info("收到客户端连接: 10.0.0.5");
    netLogger->Debug("解析协议包头: size=1024"); // 只有 Network Logger 开启了 DEBUG

    // 模拟数据库操作
    dbLogger->Info("执行查询: SELECT * FROM users"); // INFO < WARN，不会记录
    dbLogger->Warn("查询耗时超过阈值: 500ms");     // WARN >= WARN，会记录

    // 模拟系统错误
    IDLOG_ERROR("系统内存不足!");
}

int main()
{
    std::cout << "=== IDLog 综合示例 ===" << std::endl;

    const std::string configFilename = "log_config_full.ini";
    CreateFullConfig(configFilename);

    // 加载配置
    if (!IDLog::Configuration::GetInstance().LoadFromFile(configFilename))
    {
        std::cerr << "配置加载失败" << std::endl;
        return 1;
    }

    std::cout << "配置加载成功，开始模拟业务..." << std::endl;

    // 启动多线程模拟
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i)
    {
        threads.emplace_back(SimulateBusinessLogic);
    }

    for (auto& t : threads)
    {
        t.join();
    }
	
	 // 关闭日志系统，确保所有日志写入完成
	IDLOG_SHUTDOWN();
    std::cout << "\n=== 示例结束 ===" << std::endl;
    return 0;
}