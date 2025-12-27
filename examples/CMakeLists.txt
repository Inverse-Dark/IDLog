# 基础用法示例
add_executable(example_basic example_basic.cpp)
target_link_libraries(example_basic PRIVATE IDLog)

# 配置文件示例
add_executable(example_config example_config.cpp)
target_link_libraries(example_config PRIVATE IDLog)

# 异步日志示例
add_executable(example_async example_async.cpp)
target_link_libraries(example_async PRIVATE IDLog)

# 综合场景示例
add_executable(example_full example_full.cpp)
target_link_libraries(example_full PRIVATE IDLog)