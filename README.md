# InverseDarkLog (IDLog)

## 项目概述
IDLog 是一个高性能、轻量级、可扩展的 C++ 日志库。它专为高并发场景设计，支持同步/异步模式、多级日志、丰富的格式化选项、灵活的过滤器以及基于文件的配置系统。

**核心目标**：在保持简单易用的 API（类 Log4j/Logback）的同时，提供极致的性能表现（异步模式下 QPS 可达 170万+）。

## 核心特性

### 🚀 高性能
- **异步日志 (Async Logging)**：基于无锁设计思想（条件通知优化）的异步队列，将 I/O 操作从业务线程剥离。
- **极致优化**：
  - **I/O 缓冲**：`FileAppender` 内置 64KB 写缓冲区，大幅减少系统调用。
  - **时间缓存**：秒级时间格式化缓存，消除高频日志下的 CPU 热点。
  - **零拷贝思想**：在关键路径上减少内存分配与拷贝。
- **基准测试**：单线程同步写入 56万+ QPS，异步写入 170万+ QPS（Ryzen 7 环境）。

### 🛠 功能丰富
- **多级日志**：TRACE / DEBUG / INFO / WARN / ERROR / FATAL。
- **多种输出器 (Appenders)**：
  - `ConsoleAppender`：支持彩色输出（自动识别 TTY）。
  - `FileAppender`：支持多种滚动策略（按大小、按天/小时/分钟滚动）。
  - `AsyncAppender`：异步包装器，可将任意 Appender 变为异步模式。
- **灵活配置**：
  - 支持 `.ini` 配置文件加载，热更友好。
  - 支持父子 Logger 继承关系（Additivity）。
- **格式化**：强大的 `PatternFormatter`，支持类似 Log4j 的格式字符串（如 `%d{%H:%M:%S} [%t] %-5p %c - %m%n`）。
- **过滤器**：支持按级别、范围、阈值过滤日志。

### 📦 易于集成
- **Header-only 友好**：核心接口简洁，依赖少。
- **CMake 支持**：标准的 CMake 构建系统，易于引入。
- **跨平台**：支持 Windows / Linux / macOS。

## 快速开始

### 1. 构建项目

```powershell
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### 2. 运行示例

我们提供了丰富的示例代码，位于 `examples` 目录下：

- **基础用法** (`example_basic`)：展示宏、Logger 对象的基本操作。
- **配置文件** (`example_config`)：展示如何从 `.ini` 文件加载配置。
- **异步日志** (`example_async`)：展示如何手动组装异步 Appender。
- **综合场景** (`example_full`)：模拟真实业务场景（多线程、多模块、异步配置）。

```powershell
# 运行综合示例
.\examples\Release\example_full.exe
```

## 代码示例

### 基础用法 (使用宏)

```cpp
#include "IDLog/IDLog.h"

int main() {
    // 初始化（可选）
    auto logger = IDLOG_GET_ROOT_LOGGER();
    logger->SetLevel(IDLog::LogLevel::INFO);

    // 使用宏记录（推荐）
    IDLOG_INFO("系统启动成功");
    IDLOG_WARN_FMT("磁盘空间不足: %d%%", 85);
    
    // 显式关闭（确保异步日志落盘）
    IDLOG_SHUTDOWN();
    return 0;
}
```

### 异步日志配置 (代码方式)

```cpp
// 1. 创建文件后端
auto fileAppender = std::make_shared<IDLog::FileAppender>(
    "app.log", nullptr, IDLog::FileAppender::RollPolicy::DAILY);

// 2. 创建异步包装器
auto asyncAppender = std::make_shared<IDLog::AsyncAppender>(
    fileAppender, 
    100000, // 队列容量
    100,    // 批处理大小
    1000,   // 刷新间隔(ms)
    IDLog::AsyncAppender::OverflowPolicy::BLOCK // 队列满时阻塞
);
asyncAppender->Start();

// 3. 挂载到 Logger
auto logger = IDLOG_GET_LOGGER("AsyncLogger");
logger->AddAppender(asyncAppender);
```

### 配置文件示例 (`log_config.ini`)

```ini
[global]
rootLevel=INFO

[logger.ROOT]
level=INFO
appenders=AsyncFile,Console

[logger.Network]
level=DEBUG
appenders=AsyncFile
additivity=false

# 异步文件输出器配置
[appender.AsyncFile]
type=async
queueCapacity=50000
batchSize=50
flushIntervalMs=2000
backendType=file
backend.formatter=Detailed
backend.filename=server.log
backend.rollPolicy=daily

[appender.Console]
type=console
formatter=Simple
useColor=true

# 简单格式化器
[formatter.Simple]
type=pattern
pattern=%d{%H:%M:%S} %-5p %c - %m%n

# 详细格式化器
[formatter.Detailed]
type=pattern
pattern=%d{%Y-%m-%d %H:%M:%S.%ms} [%t] %-5p %c - %m%n
```

## 性能基准 (Benchmark)

运行 `tests/test_benchmark` 可在你的机器上进行压测。以下为参考数据（单线程 50万条日志）：

| 模式 | 总耗时 | QPS (条/秒) | 说明 |
| :--- | :--- | :--- | :--- |
| **同步 (Sync)** | 849 ms | **588928** | 适合低频、关键路径 |
| **异步 (Async)** | 281 ms | **1,779,359** | 适合高并发、生产环境 |

## 项目结构

- `include/IDLog`
  - `Core/`: 核心逻辑 (Logger, Event, Manager)
  - `Appender/`: 输出器实现 (Console, File, Async)
  - `Formatter/`: 格式化器
  - `Filter/`: 过滤器
  - `Utils/`: 工具类 (Thread, Time, String)
- `src`: 源代码实现
- `examples`: 使用示例
- `tests`: 单元测试与基准测试

## 最佳实践

1.  **生产环境首选异步**：对于高吞吐应用，务必使用 `AsyncAppender` 以避免阻塞业务线程。
2.  **显式关闭**：在 `main` 函数退出前调用 `IDLOG_SHUTDOWN()`，防止日志丢失。
3.  **合理设置缓冲区**：异步队列不宜过大（占用内存），也不宜过小（容易阻塞）。推荐 5w-10w。
4.  **使用宏**：`IDLOG_INFO` 等宏会自动捕获文件名和行号，且在日志级别不满足时几乎无开销。
