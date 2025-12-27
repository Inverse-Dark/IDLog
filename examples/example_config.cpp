/** 
 * @Description: IDLog 配置文件加载示例
 * @Author: InverseDark
 * @Date: 2025-12-27 13:14:11
 * @LastEditTime: 2025-12-27 14:23:03
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"
#include <iostream>
#include <fstream>

// 辅助函数：创建一个临时的配置文件
void CreateDummyConfig(const std::string& filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << R"(
# 全局配置
[global]
rootLevel=INFO

# 根日志器配置
[logger.ROOT]
level=INFO
appenders=Console,File

# 自定义日志器配置
[logger.Database]
level=TRACE
appenders=Console
additivity=false

# 控制台输出器
[appender.Console]
type=console
target=stdout
formatter=Simple
useColor=true

# 文件输出器
[appender.File]
type=file
formatter=Detailed
filename=example_config.log

# 简单格式化器
[formatter.Simple]
type=pattern
pattern=%d{%H:%M:%S} [%p] %c - %m%n

[formatter.Detailed]
type=pattern
pattern=%d{%Y-%m-%d %H:%M:%S} [%t] %-5p %c - %m%n

)";
        file.close();
        std::cout << "已创建示例配置文件: " << filename << std::endl;
    }
}

int main()
{
    std::cout << "=== IDLog 配置文件示例 ===" << std::endl;

    const std::string configFilename = "log_config_example.ini";
    CreateDummyConfig(configFilename);

    // 1. 加载配置
    std::cout << "\n[1] 加载配置文件..." << std::endl;
    IDLog::Configuration& config = IDLog::Configuration::GetInstance();
    if (config.LoadFromFile(configFilename))
    {
        std::cout << "配置加载成功!" << std::endl;
    }
    else
    {
        std::cerr << "配置加载失败!" << std::endl;
        return 1;
    }

    // 2. 测试根 Logger (应该输出到控制台和文件)
    std::cout << "\n[2] 测试 RootLogger" << std::endl;
    IDLOG_INFO("这条消息应该同时出现在控制台和文件中");
    IDLOG_DEBUG("这条 DEBUG 消息也应该显示 (Level=DEBUG)");

    // 3. 测试自定义 Logger (Database)
    std::cout << "\n[3] 测试 Database Logger" << std::endl;
    auto dbLogger = IDLOG_GET_LOGGER("Database");
    dbLogger->Trace("这是一条 SQL 查询日志 (Level=TRACE)");
    dbLogger->Info("数据库连接池已初始化");

	 // 关闭日志系统，确保所有日志写入完成
	IDLOG_SHUTDOWN();
    std::cout << "\n=== 示例结束 ===" << std::endl;
    return 0;
}