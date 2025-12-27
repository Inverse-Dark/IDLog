# 核心功能测试
add_executable(test_core test_core.cpp)
target_link_libraries(test_core PRIVATE IDLog)
add_test(NAME IDLog_CoreTest COMMAND test_core)

# 格式化器测试
add_executable(test_formatter test_formatter.cpp)
target_link_libraries(test_formatter PRIVATE IDLog)
add_test(NAME IDLog_FormatterTest COMMAND test_formatter)

# 输出器测试
add_executable(test_appender test_appender.cpp)
target_link_libraries(test_appender PRIVATE IDLog)
add_test(NAME IDLog_AppenderTest COMMAND test_appender)

# 过滤器测试
add_executable(test_filter test_filter.cpp)
target_link_libraries(test_filter PRIVATE IDLog)
add_test(NAME IDLog_FilterTest COMMAND test_filter)

# 异步功能测试
add_executable(test_async test_async.cpp)
target_link_libraries(test_async PRIVATE IDLog)
add_test(NAME IDLog_AsyncTest COMMAND test_async)

# 配置加载测试
add_executable(test_config test_config.cpp)
target_link_libraries(test_config PRIVATE IDLog)
add_test(NAME IDLog_ConfigTest COMMAND test_config)

# 性能基准测试
add_executable(test_benchmark test_benchmark.cpp)
target_link_libraries(test_benchmark PRIVATE IDLog)
add_test(NAME IDLog_BenchmarkTest COMMAND test_benchmark)