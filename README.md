<!--
 * @Description: 
 * @Author: InverseDark
 * @Date: 2025-12-18 23:51:14
 * @LastEditTime: 2025-12-18 23:51:17
 * @LastEditors: InverseDark
-->
# InverseDarkLog (IDLog)

项目概述
----------
IDLog 是一个轻量、可扩展的 C++ 日志库，提供多级别日志、可插拔的输出器、格式化器与过滤器，适用于多线程场景与多种后端集成需求。

目标：提供简单易用的日志 API（含宏）、良好的并发性和灵活的扩展能力以适配不同输出后端。

功能特性
--------
- 日志级别：支持 TRACE / DEBUG / INFO / WARN / ERROR / FATAL / OFF，并提供 LevelToString / StringToLevel 互转。
- 日志事件：`LogEvent` 包含级别、日志器名、`SourceLocation`（文件/函数/行）、时间戳（含毫秒）、线程ID/名称、消息与 `ToString()` 序列化。
- 记录器：`Logger` 提供按级别记录方法（`Trace`/`Debug`/`Info`/`Warn`/`Error`/`Fatal`）、格式化接口（`*Fmt`）、延迟消息（lambda）和级别配置。
- 宏与便捷 API：提供 `IDLOG_*` 系列宏与 `IDLOG_GET_LOGGER` 等快捷使用方式。
- 输出器（Appender）：抽象 `LogAppender`，包含 `ConsoleAppender`（支持彩色输出开关、可设置 `Formatter`）。
- 格式化器：`Formatter` 接口与 `PatternFormatter`，支持时间模板与可定制日志格式（如 `%d`、`%p`、`%c`、`%m`）。
- 过滤器：`Filter` 基类及实现（`LevelFilter`、`LevelRangeFilter`、`LevelThresholdFilter`），支持动态添加/移除以控制输出。
- Logger 管理：`LoggerManager` 负责命名日志器的创建/查找/移除，并支持父子继承策略。
- 线程工具与并发：`Utils::ThreadUtil` 支持设置/获取线程名，设计用于并发场景并记录线程元数据。
- 可扩展性：支持自定义 `Formatter` / `Appender` / `Filter`，便于集成文件、网络或其他后端。

快速开始（构建与运行）
------------------
- 使用 CMake 构建（Windows / Linux / macOS）：

```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

- 运行测试/示例：

```powershell
ctest -C Release --output-on-failure
# 或直接运行构建生成的示例可执行文件
.\examples\simple_example.exe
```

示例用法（简要）
----------------
- 获取 logger 并记录：

```cpp
auto logger = IDLOG_GET_LOGGER("MyApp");
logger->SetLevel(IDLog::LogLevel::INFO);
logger->Info("应用启动");
logger->DebugFmt("值: %d", 42);
```

- 使用宏：

```cpp
IDLOG_INFO("处理开始");
IDLOG_ERROR_FMT("处理失败: %s", errmsg.c_str());
```

- 自定义格式化器与控制台输出器：

```cpp
auto fmt = std::make_shared<IDLog::PatternFormatter>("%d{%Y-%m-%d %H:%M:%S} [%p] %c - %m%n");
auto appender = std::make_shared<IDLog::ConsoleAppender>();
appender->SetFormatter(fmt);
logger->ClearAppenders();
logger->AddAppender(appender);
```

测试与示例
-----------
- 示例/测试入口：请参阅 `tests/test_basic.cpp` 中的用法示例（级别互转、事件、记录器、宏、格式化器、多线程、彩色输出、过滤器等）。
- 示例程序：查看 `examples/simple_example.cpp` 了解最小集成方式。

项目结构（主要文件映射）
-----------------------
- 公共头：`include/IDLog/IDLog.h`
- Appender：`include/IDLog/Appender/LogAppender.h`、`include/IDLog/Appender/ConsoleAppender.h`（实现在 `src/Appender/`）
- Core：`include/IDLog/Core/*`（`LogEvent.h`、`Logger.h`、`LoggerManager.h`、`LogLevel.h`、`Macro.h`；实现在 `src/Core/`）
- Filter：`include/IDLog/Filter/*`（实现在 `src/Filter/`）
- Formatter：`include/IDLog/Formatter/*`（实现在 `src/Formatter/`）
- Utils：`include/IDLog/Utils/*`（实现在 `src/Utils/`）

扩展与集成建议
----------------
- 接入文件/网络/自定义后端：继承 `LogAppender` 并实现 `Append()`；可复用 `PatternFormatter` 或实现自定义 `Formatter`。
- 灵活过滤：实现 `Filter` 子类并通过 `Logger::AddFilter` 动态添加。
- 多线程场景：使用 `Utils::ThreadUtil` 设置线程名以便日志定位。
