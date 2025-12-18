# 定义库的名称
set(LIBRARY_NAME IDLog)

# 收集所有源文件
file(GLOB SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Formatter/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Appender/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Filter/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp
)

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
        DEFINE_SYMBOL "IDLOG_BUILD_SHARED"
    )
	# 添加宏定义
	target_compile_definitions(${LIBRARY_NAME} PRIVATE IDLOG_BUILD_SHARED)
	# 包含目录
	target_include_directories(${LIBRARY_NAME}
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
	)
	# 安装动态库
	install(TARGETS ${LIBRARY_NAME}
		EXPORT ${LIBRARY_NAME}Targets
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		RUNTIME DESTINATION bin
	)
	# 创建别名
	add_library(IDLog::IDLog ALIAS ${LIBRARY_NAME})
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
		DEFINE_SYMBOL "IDLOG_BUILD_STATIC"
	)
	# 添加宏定义
	target_compile_definitions(${LIBRARY_NAME} 
		PRIVATE 
			IDLOG_BUILD_STATIC
		INTERFACE
			IDLOG_STATIC_LIB
	)
	# 包含目录
	target_include_directories(${LIBRARY_NAME} 
		PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}
	)
	# 安装静态库
	install(TARGETS ${LIBRARY_NAME}
		EXPORT ${LIBRARY_NAME}Targets
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
	)
	# 创建别名
	add_library(IDLog::IDLog ALIAS ${LIBRARY_NAME})
endif()

# 设置库的属性(公共部分)
if(TARGET ${LIBRARY_NAME})
	# 链接系统库
	if(UNIX AND NOT APPLE)
		target_link_libraries(${LIBRARY_NAME} PUBLIC pthread)
	endif()
endif()