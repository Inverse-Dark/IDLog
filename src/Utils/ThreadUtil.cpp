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
        static thread_local std::string t_threadIdStr = "";	///< 线程ID字符串缓存
        static thread_local uint64_t t_threadIdNum = 0;	///< 线程ID数字缓存
        static thread_local std::string t_threadName = "";	///< 线程名称缓存

		std::string ThreadUtil::GetThreadId()
		{
            if (t_threadIdStr.empty())
            {
                std::stringstream ss;
                ss << GetThreadIdNum();
                t_threadIdStr = ss.str();
            }
            return t_threadIdStr;
		}
		uint64_t ThreadUtil::GetThreadIdNum()
		{
            if (t_threadIdNum == 0)
            {
#ifdef IDLOG_PLATFORM_WINDOWS
                t_threadIdNum = static_cast<uint64_t>(GetCurrentThreadId());
#else
                t_threadIdNum = static_cast<uint64_t>(syscall(SYS_gettid));
#endif
            }
            return t_threadIdNum;
		}

		std::string ThreadUtil::GetThreadName()
		{
            // 如果缓存为空，才去查询系统
            if (t_threadName.empty())
            {
#ifdef IDLOG_PLATFORM_WINDOWS
                PWSTR threadName;
                // 注意：GetThreadDescription 需要释放内存吗？通常不需要，但这里我们只调一次
                if (SUCCEEDED(GetThreadDescription(GetCurrentThread(), &threadName)))
                {
                    char mbThreadName[256];
                    WideCharToMultiByte(CP_UTF8, 0, threadName, -1, mbThreadName, 256, NULL, NULL);
                    t_threadName = mbThreadName;
                    LocalFree(threadName); // 记得释放！
                }
#else
                char threadName[16] = {0};
                pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
                t_threadName = threadName;
#endif
            }
            return t_threadName;
		}

		void ThreadUtil::SetThreadName(const std::string &name)
		{
			t_threadName = name; // 更新缓存
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
