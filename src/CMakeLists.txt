# 定义库的名称
set(LIBRARY_NAME IDLog)

# 查找线程库（可移植替代直接链接 pthread）
find_package(Threads REQUIRED)

# 收集所有源文件
file(GLOB SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Formatter/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Appender/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Filter/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp
)

if(WIN32)
    # 生成 version.rc 文件
    # @LIBRARY_NAME@ 和 @CMAKE_DEBUG_POSTFIX@ 会在这里被替换
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY
    )
    # 将生成的资源文件添加到源列表中
    list(APPEND SOURCES ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
endif()

# 根据选项构建静态或动态库
if(IDLOG_BUILD_SHARED)
	# 只构建动态库
    message(STATUS "构建动态库")
	# 构建动态库
	add_library(${LIBRARY_NAME} SHARED ${SOURCES})
	# 设置动态库的属性
	set_target_properties(${LIBRARY_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME ${LIBRARY_NAME}
		DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    )
	# 添加宏定义（目标级）
	target_compile_definitions(${LIBRARY_NAME} PRIVATE IDLOG_BUILD_SHARED)
elseif(IDLOG_BUILD_STATIC)
	# 只构建静态库
	message(STATUS "构建静态库")
	# 构建静态库
	add_library(${LIBRARY_NAME} STATIC ${SOURCES})
	# 设置静态库的属性
	set_target_properties(${LIBRARY_NAME} PROPERTIES
		VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME ${LIBRARY_NAME}
		DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
	)
	# 添加宏定义（目标级）
	target_compile_definitions(${LIBRARY_NAME}
		PRIVATE 
			IDLOG_BUILD_STATIC
		INTERFACE
			IDLOG_STATIC_LIB
	)
endif()

# 目标级别：导出版本宏并启用 C++ 标准特性与 PIC
# 这些在库创建后统一设置
if(TARGET ${LIBRARY_NAME})
	# 要求 C++ 标准（现代 CMake 风格）
	target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_17)

	# 生成位置无关代码（适用于共享库和某些静态库场景）
	set_target_properties(${LIBRARY_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

	# 包含目录
	target_include_directories(${LIBRARY_NAME} 
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
	)

	# 链接可移植的线程库
	target_link_libraries(${LIBRARY_NAME} PUBLIC Threads::Threads)

	# 其余公共设置保留（例如其他系统库可在此处添加）
endif()

# 使用生成器表达式安装不同配置的库
install(TARGETS ${LIBRARY_NAME}
		EXPORT ${LIBRARY_NAME}Targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
	)

# 创建别名
add_library(IDLog::IDLog ALIAS ${LIBRARY_NAME})