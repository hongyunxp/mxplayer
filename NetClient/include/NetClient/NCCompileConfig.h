////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 编译配置文件
/// @par 相关文件
/// NetClient.h <与此文件相关的其它文件>
/// @par 功能详细描述
/// <这里是本功能的详细描述，内容超过120个半角字符需要加反斜杠N换行接上一行内容>
/// @par 多线程安全性
/// <是/否>[，说明]
/// @par 异常时安全性
/// <是/否>[，说明]
/// @note         - 
/// 
/// @file         NCCompileConfig.h
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/06/09
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_NETCLIENT_COMPILE_CONFIG_H__INCLUDE__
#define __LEE_NETCLIENT_COMPILE_CONFIG_H__INCLUDE__

/// GUIScript SDK Version
#define LOTTERYNETCLIENT_VERSION_MAJOR 1
#define LOTTERYNETCLIENT_VERSION_MINOR 0
#define LOTTERYNETCLIENT_VERSION_REVISION 1

/// 定义SVN服务器上的版本信息
#define LOTTERYNETCLIENT_VERSION_SVN  10011
/// 定义SDK版本号
#define LOTTERYNETCLIENT_SDK_VERSION "1.0.1"


/// WIN32 for Windowsint32
/// WIN64 for Windowsint64
/// 定义WINDOWS平台
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define _NC_WINDOWS_PLATFORM_
#define _NC_WINDOWS_API_
#endif

/// WINCE is a very restricted environment for mobile devices
/// 定义WINDOWS CE平台
#if defined(_WIN32_WCE)
#define _NC_WINDOWS_PLATFORM_
#define _NC_WINDOWS_API_
#define _NC_WINDOWS_CE_PLATFORM_
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1300)
#  error "Only Microsoft Visual Studio 7.0 and later are supported."
#endif

/// 定义LINUX系统平台
#if !defined(_NC_WINDOWS_PLATFORM_)
#define _NC_LINUX_PLATFORM_
#define _NC_POSIX_API_
#endif

/// WINDOWS平台下，相关宏定义
#ifdef _NC_WINDOWS_API_
/// 定义动态库编译宏
#ifndef _NC_STATIC_LIB_
#ifdef	LOTTERYNETCLIENT_EXPORTS
#define LOTTERYNETCLIENT_API __declspec(dllexport)
#else
#define LOTTERYNETCLIENT_API __declspec(dllimport)
#endif /// LOTTERYNETCLIENT_API_EXPORTS
#else/// 静态库编译宏定义
#define LOTTERYNETCLIENT_API
#endif /// _NC_STATIC_LIB_

/// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define NCCALLCONV __stdcall
#else
#define NCCALLCONV __cdecl
#endif /// STDCALL_SUPPORTED

/// 定义WIN32_LEAN_AND_MEAN宏，屏蔽掉MFC、winsock.h等，
/// 加快编译，优化应用程序
#define WIN32_LEAN_AND_MEAN

#else /// _NC_WINDOWS_API_
/// 非WINDOWS平台下的动态库、静态库编译宏
/// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_NC_STATIC_LIB_) \
	&& defined(LOTTERYNETCLIENT_EXPORTS)
#define LOTTERYNETCLIENT_API __attribute__ ((visibility("default")))
#else
#define LOTTERYNETCLIENT_API
#endif

#define NCCALLCONV

#endif /// _NC_WINDOWS_API_

#if defined(__sparc__) || defined(__sun__)
#define __BIG_ENDIAN__
#endif

#endif /// __LEE_NETCLIENT_COMPILE_CONFIG_H__INCLUDE__

