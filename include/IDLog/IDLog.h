/** 
 * @Description: IDLog 日志库主头文件
 * @Author: InverseDark
 * @Date: 2025-12-18 23:23:17
 * @LastEditTime: 2025-12-21 14:43:42
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_IDLOG_H
#define IDLOG_IDLOG_H

// 包含核心组件头文件
#include "IDLog/Core/LogLevel.h"
#include "IDLog/Core/LogEvent.h"
#include "IDLog/Core/Logger.h"
#include "IDLog/Core/LoggerManager.h"
#include "IDLog/Core/Configuration.h"
#include "IDLog/Core/LogFactory.h"

// 包含输出器头文件
#include "IDLog/Appender/LogAppender.h"
#include "IDLog/Appender/ConsoleAppender.h"
#include "IDLog/Appender/FileAppender.h"

// 包含格式化器头文件
#include "IDLog/Formatter/Formatter.h"
#include "IDLog/Formatter/PatternFormatter.h"

// 包含过滤器头文件
#include "IDLog/Filter/Filter.h"
#include "IDLog/Filter/LevelFilter.h"

// 包含工具头文件
#include "IDLog/Utils/StringUtil.h"
#include "IDLog/Utils/ThreadUtil.h"
#include "IDLog/Utils/ConfigParseUtil.h"

#endif // !IDLOG_IDLOG_H