/**
 * @Description: 简单使用示例源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 23:22:53
 * @LastEditTime: 2025-12-18 23:22:56
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"

#include <iostream>

/// @brief 简单的使用示例
int main()
{
	// 获取根Logger（最简单的方式）
	auto logger = IDLog::LoggerManager::GetInstance().GetRootLogger();

	// 记录不同级别的日志
	logger->Trace("这是一条TRACE消息");
	logger->Debug("这是一条DEBUG消息");
	logger->Info("这是一条INFO消息");
	logger->Warn("这是一条WARN消息");
	logger->Error("这是一条ERROR消息");
	logger->Fatal("这是一条FATAL消息");

	// 使用便捷宏
	IDLOG_INFO("使用宏记录INFO消息");
	IDLOG_WARN("使用宏记录WARN消息");

	// 带格式化参数的日志
	int userId = 12345;
	std::string username = "john_doe";
	IDLOG_INFO_FMT("用户登录: ID=%d, 用户名=%s", userId, username.c_str());

	// 获取特定名称的Logger
	auto networkLogger = IDLog::LoggerManager::GetInstance().GetLogger("network");
	auto databaseLogger = IDLog::LoggerManager::GetInstance().GetLogger("database");

	networkLogger->Info("网络连接建立");
	databaseLogger->Info("数据库查询执行");

	return 0;
}