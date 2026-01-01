/** 
 * @Description: IDLog 版本信息源文件
 * @Author: InverseDark
 * @Date: 2026-01-01 15:41:03
 * @LastEditTime: 2026-01-01 15:54:30
 * @LastEditors: InverseDark
 */
#include "IDLog/IDLog.h"

namespace IDLog
{

// 主版本号定义
#ifndef IDLOG_VERSION_MAJOR
#define IDLOG_VERSION_MAJOR 0
#endif // !IDLOG_VERSION_MAJOR
// 次版本号定义
#ifndef IDLOG_VERSION_MINOR
#define IDLOG_VERSION_MINOR 0
#endif // !IDLOG_VERSION_MINOR
// 修订号定义
#ifndef IDLOG_VERSION_PATCH
#define IDLOG_VERSION_PATCH 0
#endif // !IDLOG_VERSION_PATCH

// 版本字符串定义
#ifndef IDLOG_VERSION_STRING
#define IDLOG_VERSION_STRING "0.0.0"
#endif // !IDLOG_VERSION_STRING

	const int VersionMajor = IDLOG_VERSION_MAJOR; ///< 主版本号
	const int VersionMinor = IDLOG_VERSION_MINOR; ///< 次版本号
	const int VersionPatch = IDLOG_VERSION_PATCH; ///< 修订号
	const char VersionString[] = IDLOG_VERSION_STRING; ///< 版本字符串

	const char *IDLog_Version()
	{
		return VersionString;
	}
} // namespace IDLog