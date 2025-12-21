/**
 * @Description: 配置测试源文件
 * @Author: InverseDark
 * @Date: 2025-12-21 14:41:06
 * @LastEditTime: 2025-12-21 14:46:35
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"

#include <iostream>
#include <cstdlib>

void TestConfiguration()
{
	std::cout << "=== 测试配置功能 ===" << std::endl;

	IDLOG_INFO("测试配置前的日志消息：");
	IDLOG_TRACE("这是一个TRACE级别的测试日志消息。");
	IDLOG_DEBUG("这是一个DEBUG级别的测试日志消息。");
	IDLOG_INFO("这是一个INFO级别的测试日志消息。");
	IDLOG_WARN("这是一个WARN级别的测试日志消息。");
	IDLOG_ERROR("这是一个ERROR级别的测试日志消息。");
	IDLOG_FATAL("这是一个FATAL级别的测试日志消息。");

	IDLog::Configuration::Options options;
	options.global.rootLevel = IDLog::LogLevel::DBG;
	options.loggers["TestLogger"].level = IDLog::LogLevel::TRACE;
	options.loggers["TestLogger"].appenders.push_back("ConsoleAppender");
	options.appenders["ConsoleAppender"].type = "console";
	options.appenders["ConsoleAppender"].params["target"] = "stdout";
	options.appenders["ConsoleAppender"].formatter = "PatternFormatter";
	options.formatters["PatternFormatter"].type = "pattern";
	options.formatters["PatternFormatter"].params["pattern"] = "%d{%Y-%m-%d %H:%M:%S} [%-5p] %c - %m%n";
	IDLog::Configuration& config = IDLog::Configuration::GetInstance();
	config.ApplyOptions(options);
	std::cout << "配置应用完成。" << std::endl;

	IDLOG_INFO("配置应用后的日志消息：");
	IDLOG_TRACE("这是一个TRACE级别的测试日志消息。");
	IDLOG_DEBUG("这是一个DEBUG级别的测试日志消息。");
	IDLOG_INFO("这是一个INFO级别的测试日志消息。");
	IDLOG_WARN("这是一个WARN级别的测试日志消息。");
	IDLOG_ERROR("这是一个ERROR级别的测试日志消息。");
	IDLOG_FATAL("这是一个FATAL级别的测试日志消息。");

	IDLog::Configuration::Options loadedOptions = config.GetOptions();
	if (loadedOptions.global.rootLevel != IDLog::LogLevel::DBG)
	{
		throw std::runtime_error("全局根日志级别不匹配");
	}
	if (loadedOptions.loggers["TestLogger"].level != IDLog::LogLevel::TRACE)
	{
		throw std::runtime_error("TestLogger 日志级别不匹配");
	}
	std::cout << "配置验证通过。" << std::endl;

	auto logger = IDLOG_GET_LOGGER("TestLogger");
	logger->Trace("这是一个TRACE级别的测试日志消息。");
	logger->Debug("这是一个DEBUG级别的测试日志消息。");
	logger->Info("这是一个INFO级别的测试日志消息。");
	logger->Warn("这是一个WARN级别的测试日志消息。");
	logger->Error("这是一个ERROR级别的测试日志消息。");
	logger->Fatal("这是一个FATAL级别的测试日志消息。");

	std::string filename = "test_config.ini";
	if (!config.SaveToFile(filename))
	{
		throw std::runtime_error("保存配置文件失败");
	}

	std::cout << "配置保存到文件：" << filename << std::endl;
}

void TestLoadFromString()
{
	std::cout << "=== 测试从字符串加载配置 ===" << std::endl;
	std::string configStr = R"(
[global]
rootLevel=DBG
enableStatistics=true
statisticsInterval=120
[logger.justTest]
level=TRACE
appenders=ConsoleAppender
[appender.ConsoleAppender]
type=console
target=stdout
formatter=PatternFormatter
[formatter.PatternFormatter]
type=pattern
pattern=%d{%Y-%m-%d %H:%M:%S} [%p] %c - %m%n
)";
	IDLog::Configuration& config = IDLog::Configuration::GetInstance();
	if (!config.LoadFromString(configStr))
	{
		throw std::runtime_error("从字符串加载配置失败");
	}
	std::cout << "从字符串加载配置完成。" << std::endl;
	IDLOG_INFO("配置应用后的日志消息：");
	IDLOG_TRACE("这是一个TRACE级别的测试日志消息。");
	IDLOG_DEBUG("这是一个DEBUG级别的测试日志消息。");
	IDLOG_INFO("这是一个INFO级别的测试日志消息。");
	IDLOG_WARN("这是一个WARN级别的测试日志消息。");
	IDLOG_ERROR("这是一个ERROR级别的测试日志消息。");
	IDLOG_FATAL("这是一个FATAL级别的测试日志消息。");

	auto logger = IDLOG_GET_LOGGER("justTest");
	logger->Info("测试从字符串加载配置的日志消息。");


	std::cout << "从字符串加载配置测试通过。" << std::endl;
}

void TestLoadFromFile()
{
	std::cout << "=== 测试从文件加载配置 ===" << std::endl;
	std::string filename = "test_config2.ini";
	IDLog::Configuration& newConfig = IDLog::Configuration::GetInstance();
	if (!newConfig.LoadFromFile(filename))
	{
		throw std::runtime_error("从文件加载配置失败");
	}
	std::cout << "从文件加载配置完成。" << std::endl;
	auto logger = IDLOG_GET_LOGGER("Yes,Logger2");
	logger->Trace("这是一个TRACE级别的测试日志消息。");
	logger->Debug("这是一个DEBUG级别的测试日志消息。");
	logger->Info("这是一个INFO级别的测试日志消息。");
	logger->Warn("这是一个WARN级别的测试日志消息。");
	logger->Error("这是一个ERROR级别的测试日志消息。");
	logger->Fatal("这是一个FATAL级别的测试日志消息。");
}

void TestAutoReload()
{
	std::cout << "=== 测试自动重新加载配置 ===" << std::endl;
	IDLog::Configuration& config = IDLog::Configuration::GetInstance();
	std::string filename = "test_config2.ini";
	if (!config.LoadFromFile(filename))
	{
		throw std::runtime_error("从文件加载配置失败");
	}
	config.EnableAutoReload(true, 3); // 每3秒检查一次文件变化

	config.RegisterChangeCallback([]() {
		std::cout << "配置文件已更改，重新加载完成。" << std::endl;
		});

	auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	while (true)
	{
		auto logger = IDLOG_GET_LOGGER("Yes,Logger2");
		logger->Trace("这是一个TRACE级别的测试日志消息。");
		logger->Debug("这是一个DEBUG级别的测试日志消息。");
		logger->Info("这是一个INFO级别的测试日志消息。");
		logger->Warn("这是一个WARN级别的测试日志消息。");
		logger->Error("这是一个ERROR级别的测试日志消息。");
		logger->Fatal("这是一个FATAL级别的测试日志消息。");

		IDLOG_TRACE("这是一个TRACE级别的测试日志消息。");
		IDLOG_DEBUG("这是一个DEBUG级别的测试日志消息。");
		IDLOG_INFO("这是一个INFO级别的测试日志消息。");
		IDLOG_WARN("这是一个WARN级别的测试日志消息。");
		IDLOG_ERROR("这是一个ERROR级别的测试日志消息。");
		IDLOG_FATAL("这是一个FATAL级别的测试日志消息。");

		std::this_thread::sleep_for(std::chrono::seconds(1));

		// 超过60s后退出
		if (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - now > 60)
		{
			break;
		}
	}
}

int main()
{
	std::cout << "InverseDarkLog(IDLog) " << IDLOG_VERSION_STRING << " 配置功能测试" << std::endl;
	std::cout << "===============================" << std::endl;

	try
	{
		TestConfiguration();
		TestLoadFromString();
		TestLoadFromFile();
		TestAutoReload();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "测试过程中发生异常: " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}