/** 
 * @Description: 配置使用示例源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 23:16:24
 * @LastEditTime: 2025-12-21 23:18:04
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"

#include <iostream>

/// @brief 配置使用示例
int main()
{
	// 获取配置实例
	IDLog::Configuration &config = IDLog::Configuration::GetInstance();

	// 从配置文件加载配置
	if (!config.LoadFromFile("log_config.ini"))
	{
		std::cerr << "加载配置文件失败，使用默认配置。" << std::endl;
	}

	// 获取根Logger
	auto rootLogger = IDLOG_GET_ROOT_LOGGER();

	// 记录日志
	rootLogger->Info("这是通过配置文件设置的根Logger记录的INFO日志。");

	// 假设配置文件中定义了一个名为"AppLogger"的Logger
	auto appLogger = IDLOG_GET_LOGGER("AppLogger");
	appLogger->Info("这是通过配置文件设置的AppLogger记录的Info日志。");

	return 0;
}