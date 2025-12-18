# 设置最低的CMake版本要求
cmake_minimum_required(VERSION 3.10)
# 设置项目名称和版本号
project(IDLog VERSION 1.0.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)	# 使用C++17标准
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 强制使用C++17标准
set(CMAKE_CXX_EXTENSIONS OFF) # 禁用编译器特定的扩展，确保标准兼容性

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加版本宏定义
add_definitions(-DIDLOG_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
add_definitions(-DIDLOG_VERSION_MINOR=${PROJECT_VERSION_MINOR})
add_definitions(-DIDLOG_VERSION_PATCH=${PROJECT_VERSION_PATCH})
add_definitions(-DIDLOG_VERSION_STRING="${PROJECT_VERSION}")

# 添加平台宏定义
if(WIN32)
	add_definitions(-DIDLOG_PLATFORM_WINDOWS)
elseif(UNIX)
	add_definitions(-DIDLOG_PLATFORM_UNIX)
elseif(APPLE)
	add_definitions(-DIDLOG_PLATFORM_APPLE)
endif()

# 设置库类型选项
option(IDLOG_BUILD_SHARED "Build IDLog as shared library" ON)
option(IDLOG_BUILD_STATIC "Build IDLog as static library" OFF)

# 确保至少选择一种库类型
if(NOT IDLOG_BUILD_SHARED AND NOT IDLOG_BUILD_STATIC)
    message(WARNING "没有选择库类型，默认使用动态库")
	set(IDLOG_BUILD_SHARED ON CACHE BOOL "Build IDLog as shared library" FORCE)
	set(IDLOG_BUILD_STATIC OFF CACHE BOOL "Build IDLog as static library" FORCE)
endif()

# 确保只能选择一种库类型
if(IDLOG_BUILD_SHARED AND IDLOG_BUILD_STATIC)
	message(WARNING "同时选择了静态库和动态库，默认使用动态库")
	set(IDLOG_BUILD_STATIC OFF CACHE BOOL "Build IDLog as static library" FORCE)
endif()

# 编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-Wall -Wextra -Wpedantic -Werror -pthread)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-g -O0 -DDEBUG -DIDLOG_BUILD_DEBUG)
	else()
		add_compile_options(-O2 -DNDEBUG -DIDLOG_BUILD_RELEASE)
	endif()

elseif(MSVC)
	add_compile_options(/WX /MP)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(/Zi /Od /DDEBUG /DIDLOG_BUILD_DEBUG)
	else()
		add_compile_options(/O2 /DNDEBUG /DIDLOG_BUILD_RELEASE)
	endif()

endif()

# 添加子目录
add_subdirectory(src)

# 如果启用测试
option(IDLOG_BUILD_TESTS "Build tests for IDLog" ON)
if(IDLOG_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# 如果启用示例
option(IDLOG_BUILD_EXAMPLES "Build examples for IDLog" ON)
if(IDLOG_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# 项目描述
message(STATUS "项目名称: ${PROJECT_NAME}")	# 输出项目名称
message(STATUS "项目版本: ${PROJECT_VERSION}") # 输出项目版本
message(STATUS "C++ 标准: C++${CMAKE_CXX_STANDARD}") # 输出C++标准
message(STATUS "CMake 版本: ${CMAKE_VERSION}") # 输出CMake版本
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}") # 输出构建类型
message(STATUS "源代码目录: ${CMAKE_SOURCE_DIR}") # 输出源代码目录
message(STATUS "二进制目录: ${CMAKE_BINARY_DIR}") # 输出二进制目录
message(STATUS "编译器ID: ${CMAKE_CXX_COMPILER_ID}") # 输出编译器ID
message(STATUS "编译器版本: ${CMAKE_CXX_COMPILER_VERSION}") # 输出编译器版本
message(STATUS "编译器标志: ${CMAKE_CXX_FLAGS}") # 输出编译器标志
message(STATUS "系统名称: ${CMAKE_SYSTEM_NAME}") # 输出系统名称
message(STATUS "系统版本: ${CMAKE_SYSTEM_VERSION}") # 输出系统版本
message(STATUS "系统处理器: ${CMAKE_SYSTEM_PROCESSOR}") # 输出系统处理器
message(STATUS "CMake构建工具: ${CMAKE_BUILD_TOOL}") # 输出CMake构建工具
message(STATUS "CMake生成器: ${CMAKE_GENERATOR}") # 输出CMake生成器
message(STATUS "CMake安装前缀: ${CMAKE_INSTALL_PREFIX}") # 输出CMake安装前缀
message(STATUS "CMake缓存文件目录: ${CMAKE_CACHEFILE_DIR}") # 输出CMake缓存文件目录
message(STATUS "CMake系统库路径: ${CMAKE_SYSTEM_LIBRARY_PATH}") # 输出CMake系统库路径