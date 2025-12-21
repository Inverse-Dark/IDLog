# 基础测试
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic PRIVATE IDLog)
add_test(NAME IDLog_BasicTest COMMAND test_basic)

# 配置测试
add_executable(test_config test_config.cpp)
target_link_libraries(test_config PRIVATE IDLog)
add_test(NAME IDLog_ConfigTest COMMAND test_config)