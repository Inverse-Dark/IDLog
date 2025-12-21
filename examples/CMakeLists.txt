# 简单示例
add_executable(simple_example simple_example.cpp)
target_link_libraries(simple_example PRIVATE IDLog)

# 配置示例
add_executable(config_example config_example.cpp)
target_link_libraries(config_example PRIVATE IDLog)