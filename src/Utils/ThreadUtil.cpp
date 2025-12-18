/**
 * @Description: 线程工具源文件
 * @Author: InverseDark
 * @Date: 2025-12-18 19:03:14
 * @LastEditTime: 2025-12-18 19:03:14
 * @LastEditors: InverseDark
 */
#include "IDLog/Utils/ThreadUtil.h"

#include <sstream>
#include <chrono>

#ifdef IDLOG_PLATFORM_WINDOWS
#include <windows.h>
#include <processthreadsapi.h>
#pragma comment(lib, "kernel32.lib")
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

namespace IDLog
{
	namespace Utils
	{
		std::string ThreadUtil::GetThreadId()
		{
			std::stringstream ss;
			ss << GetThreadIdNum();
			return ss.str();
		}
		uint64_t ThreadUtil::GetThreadIdNum()
		{
#ifdef IDLOG_PLATFORM_WINDOWS
			return static_cast<uint64_t>(GetCurrentThreadId());
#else
			return static_cast<uint64_t>(syscall(SYS_gettid));
#endif
		}

		std::string ThreadUtil::GetThreadName()
		{
#ifdef IDLOG_PLATFORM_WINDOWS
			PWSTR threadName[MAX_PATH];
			GetThreadDescription(GetCurrentThread(), threadName);

			// 宽字符转多字节
			char mbThreadName[MAX_PATH];
			WideCharToMultiByte(CP_UTF8, 0, *threadName, -1, mbThreadName, MAX_PATH, NULL, NULL);
			return std::string(mbThreadName);
#else
			char threadName[16] = {0};
			pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
			return std::string(threadName);
#endif
		}

		void ThreadUtil::SetThreadName(const std::string &name)
		{
#ifdef IDLOG_PLATFORM_WINDOWS
			// Windows需要宽字符
			wchar_t wname[256];
			MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, wname, 256);
			SetThreadDescription(GetCurrentThread(), wname);
#else
			pthread_setname_np(pthread_self(), name.c_str());
#endif
		}

		void ThreadUtil::Sleep(uint64_t milliseconds)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
		}

		unsigned int ThreadUtil::GetCpuCount()
		{
#ifdef IDLOG_PLATFORM_WINDOWS
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			return sysInfo.dwNumberOfProcessors;
#else
			return sysconf(_SC_NPROCESSORS_ONLN);
#endif
		}
	} // namespace Utils
} // namespace IDLog
