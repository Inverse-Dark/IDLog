/**
 * @Description: 基础测试
 * @Author: InverseDark
 * @Date: 2025-12-18 19:09:24
 * @LastEditTime: 2025-12-18 19:13:04
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"

#include <iostream>
#include <functional>
#include <Windows.h>

void TestLogLevel()
{
	std::cout << "=== 测试日志级别转换 ===" << std::endl;

	// 测试LevelToString
	for (int i = 0; i <= 6; ++i)
	{
		IDLog::LogLevel level = static_cast<IDLog::LogLevel>(i);
		std::cout << "日志级别 " << i << " 转换为字符串: " << IDLog::LevelToString(level) << std::endl;
	}

	// 测试StringToLevel
	std::string testLevels[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "OFF", "UNKNOWN" };
	for (const auto& str : testLevels)
	{
		IDLog::LogLevel level = IDLog::StringToLevel(str);
		std::cout << "字符串 \"" << str << "\" 转换为日志级别: " << static_cast<int>(level) << std::endl;
	}
}

void TestLogEvent()
{
	std::cout << "=== 测试日志事件 ===" << std::endl;

	IDLog::SourceLocation loc(__FILE__, __FUNCTION__, __LINE__);
	IDLog::LogEvent event(IDLog::LogLevel::INFO, "TestLogger", loc);
	event.SetLogMessage("这是一个测试日志消息。");

	std::cout << "日志事件详情:" << std::endl;
	std::cout << "级别: " << IDLog::LevelToString(event.GetLevel()) << std::endl;
	std::cout << "日志器名称: " << event.GetLoggerName() << std::endl;
	std::cout << "源文件位置: " << event.GetSourceLocation().ToString() << std::endl;
	std::cout << "时间: " << event.GetFormattedTime() << "." << event.GetMilliseconds() << std::endl;
	std::cout << "线程ID: " << event.GetThreadId() << std::endl;
	std::cout << "线程名称: " << event.GetThreadName() << std::endl;
	std::cout << "消息: " << event.GetLogMessage() << std::endl;

	std::cout << "完整日志事件字符串: " << event.ToString() << std::endl;

}

void TestLogger()
{
	std::cout << "=== 测试日志记录器 ===" << std::endl;

	IDLog::Logger logger("TestLogger", IDLog::LogLevel::TRACE);
	logger.Trace("这是一条TRACE级别的日志消息。");
	logger.Debug("这是一条DEBUG级别的日志消息。");
	logger.Info("这是一条INFO级别的日志消息。");
	logger.Warn("这是一条WARN级别的日志消息。");
	logger.Error("这是一条ERROR级别的日志消息。");
	logger.Fatal("这是一条FATAL级别的日志消息。");

	logger.TraceFmt("格式化的TRACE日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	logger.DebugFmt("格式化的DEBUG日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	logger.InfoFmt("格式化的INFO日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	logger.WarnFmt("格式化的WARN日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	logger.ErrorFmt("格式化的ERROR日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	logger.FatalFmt("格式化的FATAL日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);

	auto lazyMessage = []() {
		return "这是一个延迟计算的日志消息。";
		};
	logger.Log(IDLog::LogLevel::INFO, lazyMessage);
}

void TestBasicLogging()
{
	std::cout << "=== 测试基础日志记录宏 ===" << std::endl;

	IDLOG_TRACE("这是一条TRACE消息");
	IDLOG_DEBUG("这是一条DEBUG消息");
	IDLOG_INFO("这是一条INFO消息");
	IDLOG_WARN("这是一条WARN消息");
	IDLOG_ERROR("这是一条ERROR消息");
	IDLOG_FATAL("这是一条FATAL消息");

	IDLOG_TRACE_FMT("格式化的TRACE日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	IDLOG_DEBUG_FMT("格式化的DEBUG日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	IDLOG_INFO_FMT("格式化的INFO日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	IDLOG_WARN_FMT("格式化的WARN日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	IDLOG_ERROR_FMT("格式化的ERROR日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);
	IDLOG_FATAL_FMT("格式化的FATAL日志，信息：%s, 数字：%d, 浮点数：%.2f",
		"测试字符串", 42, 3.14159);

	IDLog::Logger::Pointer logger = IDLOG_GET_LOGGER("BasicLogger");
	logger->SetLevel(IDLog::LogLevel::ERR);

	logger->Info("基础日志记录测试开始。");
	logger->Trace("这是一个跟踪级别的日志消息。");
	logger->Debug("这是一个调试级别的日志消息。");
	logger->Warn("这是一个警告级别的日志消息。");
	logger->Error("这是一个错误级别的日志消息。");
	logger->Fatal("这是一个致命级别的日志消息。");
	logger->Info("基础日志记录测试结束。");

	IDLog::Logger::Pointer logger2 = IDLOG_GET_LOGGER("BasicLogger.SubLogger");
	logger2->Info("子日志器记录一条INFO消息。");
	logger2->Error("子日志器记录一条ERROR消息。");

	IDLog::LoggerManager::GetInstance().RemoveLogger("BasicLogger");

	IDLog::Logger::Pointer logger3 = IDLOG_GET_LOGGER("BasicLogger.SubLogger2");
	logger3->Info("新创建的子日志器记录一条INFO消息，应继承根日志器级别。");
	logger3->Error("新创建的子日志器记录一条ERROR消息，应继承根日志器级别。");
}

void TestCustomFormatter()
{
	std::cout << "=== 测试自定义日志格式化器 ===" << std::endl;

	// 创建自定义格式化器
	auto customFormatter = std::make_shared<IDLog::PatternFormatter>("%d{%Y-%m-%d %H:%M:%S} [%p] %c - %m%n");

	// 创建控制台输出器并设置自定义格式化器
	auto consoleAppender = std::make_shared<IDLog::ConsoleAppender>();
	consoleAppender->SetFormatter(customFormatter);

	// 获取日志器
	auto logger = IDLOG_GET_LOGGER("CustomFormatterLogger");

	// 清除默认输出器，添加自定义输出器
	logger->ClearAppenders();
	logger->AddAppender(consoleAppender);

	// 记录日志以测试自定义格式化器
	logger->Info("这是一条使用自定义格式化器的INFO日志消息。");
	logger->Error("这是一条使用自定义格式化器的ERROR日志消息。");
}

void TestMultiThread()
{
	std::cout << "=== 测试多线程日志记录 ===" << std::endl;

	auto logger = IDLOG_GET_LOGGER("MultiThreadLogger");
	logger->SetLevel(IDLog::LogLevel::DBG);

	auto customFormatter = std::make_shared<IDLog::PatternFormatter>("%d{%Y-%m-%d %H:%M:%S} [%t](%T) [%p] %c - %m%n");
	logger->GetAppenders()[0]->SetFormatter(customFormatter);

	const int numThreads = 4;
	std::vector<std::thread> threads;
	for (int i = 0; i < numThreads; ++i)
	{
		threads.emplace_back([logger, i]() {
			for (int j = 0; j < 5; ++j)
			{
				IDLog::Utils::ThreadUtil::SetThreadName("WorkerThread-" + std::to_string(i));

				auto threadName = IDLog::Utils::ThreadUtil::GetThreadName();

				logger->DebugFmt("线程 %d - DEBUG 消息 %d", i, j);
				logger->InfoFmt("线程 %d - INFO 消息 %d", i, j);
				Sleep(10); // 模拟一些工作
			}
			});
	}

	for (auto& t : threads)
	{
		t.join();
	}

	std::cout << "多线程日志记录测试完成。" << std::endl;
}

void TestColorOutput() {
	std::cout << "=== 测试彩色输出 ===" << std::endl;

	auto consoleAppender = std::make_shared<IDLog::ConsoleAppender>();
	consoleAppender->SetUseColor(true);

	auto logger = IDLOG_GET_LOGGER("ColorLogger");
	logger->SetLevel(IDLog::LogLevel::TRACE);
	logger->ClearAppenders();
	logger->AddAppender(consoleAppender);

	logger->Trace("TRACE - 白色");
	logger->Debug("DEBUG - 青色");
	logger->Info("INFO - 绿色");
	logger->Warn("WARN - 黄色");
	logger->Error("ERROR - 红色");
	logger->Fatal("FATAL - 洋红色");


	auto consoleAppender2 = std::make_shared<IDLog::ConsoleAppender>();
	consoleAppender2->SetUseColor(false);
	auto logger2 = IDLOG_GET_LOGGER("NoColorLogger");
	logger2->SetLevel(IDLog::LogLevel::TRACE);
	logger2->ClearAppenders();
	logger2->AddAppender(consoleAppender2);

	logger2->Trace("TRACE - 无颜色");
	logger2->Debug("DEBUG - 无颜色");
	logger2->Info("INFO - 无颜色");
	logger2->Warn("WARN - 无颜色");
	logger2->Error("ERROR - 无颜色");
	logger2->Fatal("FATAL - 无颜色");

	std::cout << std::endl;
}

void TestFilter()
{
	std::cout << "=== 测试日志过滤器 ===" << std::endl;

	auto logger = IDLOG_GET_LOGGER("FilterLogger");
	logger->SetLevel(IDLog::LogLevel::TRACE);

	// 创建并添加级别过滤器，只允许WARN及以上级别的日志
	auto levelFilter = std::make_shared<IDLog::LevelFilter>(IDLog::LogLevel::WARN);
	logger->AddFilter(levelFilter);

	logger->Trace("这是一条TRACE消息，应该被过滤掉。");
	logger->Debug("这是一条DEBUG消息，应该被过滤掉。");
	logger->Info("这是一条INFO消息，应该被过滤掉。");
	logger->Warn("这是一条WARN消息，应该被记录。");
	logger->Error("这是一条ERROR消息，应该被记录。");
	logger->Fatal("这是一条FATAL消息，应该被记录。");

	// 移除过滤器
	logger->ClearFilters();

	logger->Info("移除过滤器后，这是一条INFO消息，应该被记录。");
	logger->Warn("移除过滤器后，这是一条WARN消息，应该被记录。");

	auto levelRangeFilter = std::make_shared<IDLog::LevelRangeFilter>(IDLog::LogLevel::DBG, IDLog::LogLevel::ERR);
	logger->AddFilter(levelRangeFilter);
	logger->Trace("这是一条TRACE消息，应该被过滤掉。");
	logger->Debug("这是一条DEBUG消息，应该被记录。");
	logger->Info("这是一条INFO消息，应该被记录。");
	logger->Warn("这是一条WARN消息，应该被记录。");
	logger->Error("这是一条ERROR消息，应该被记录。");
	logger->Fatal("这是一条FATAL消息，应该被过滤掉。");

	logger->ClearFilters();
	logger->Trace("清除所有过滤器后，这是一条TRACE消息，应该被记录。");
	logger->Fatal("清除所有过滤器后，这是一条FATAL消息，应该被记录。");

	auto levelThresholdFilter = std::make_shared<IDLog::LevelThresholdFilter>(IDLog::LogLevel::INFO);
	logger->AddFilter(levelThresholdFilter);
	logger->Debug("这是一条DEBUG消息，应该被过滤掉。");
	logger->Info("这是一条INFO消息，应该被记录。");
	logger->Error("这是一条ERROR消息，应该被记录。");
	logger->Fatal("这是一条FATAL消息，应该被记录。");
	logger->ClearFilters();
	logger->Debug("清除所有过滤器后，这是一条DEBUG消息，应该被记录。");
	
}

int main()
{
	std::cout << "InverseDarkLog(IDLog) " << IDLOG_VERSION_STRING << " 基础功能测试" << std::endl;
	std::cout << "===============================" << std::endl;

	try
	{
		TestLogLevel();
		TestLogEvent();
		TestLogger();
		TestBasicLogging();
		TestCustomFormatter();
		TestMultiThread();
		TestColorOutput();
		TestFilter();
	}
	catch (const std::exception& ex)
	{
		std::cerr << "测试过程中发生异常: " << ex.what() << std::endl;
		return -1;
	}
}