/** 
 * @Description: IDLog 版本信息头文件
 * @Author: InverseDark
 * @Date: 2026-01-01 15:48:26
 * @LastEditTime: 2026-01-01 15:58:40
 * @LastEditors: InverseDark
 */
#ifndef IDLOG_CORE_VERSION_H
#define IDLOG_CORE_VERSION_H

#include "IDLog/Core/Macro.h"

namespace IDLog
{
	IDLOG_API extern const int VersionMajor; ///< 主版本号
	IDLOG_API extern const int VersionMinor; ///< 次版本号
	IDLOG_API extern const int VersionPatch; ///< 修订号
	
	IDLOG_API extern const char VersionString[]; ///< 版本字符串

	/// @brief 获取IDLog库的版本字符串
	/// @return 版本字符串，格式为 "主版本号.次版本号.修订号"
	IDLOG_API const char *IDLog_Version();
} // namespace IDLog

#endif // !IDLOG_CORE_VERSION_H