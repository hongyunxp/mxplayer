////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// DEBUG信息输出模块
/// @par 相关文件
/// Debug.cpp, 其他引用本头文件的文件
/// @par 功能详细描述
/// 实现输出函数的DEBUG信息，包括函数开始、结束及重大问题时的信息。
/// @par 多线程安全性
/// 否
/// @par 异常时安全性
/// 否
/// @par 备注
/// Include本头文件即可
/// 
/// @file Debug.h
/// @brief 调试信息输出
/// @author Lee@Home
/// @version 1.0
/// @date 2010.12.20
/// @todo 
/// 
////////////////////////////////////////////////////////////////////////////////

#ifndef __LIGHT_DEBUG_INFORMATION__
#define __LIGHT_DEBUG_INFORMATION__

// 函数开始宏定义
#define _DEBUG_START		0
// 函数结束宏定义
#define _DEBUG_END			1
// 一般调试信息宏定义
#define LD_INFO				2
// 警告信息宏定义
#define LD_WARN				3
// 错误信息宏定义
#define LD_ERR				4

#define LD_LINE				__LINE__
#define LD_FILE				__FILE__
#define LD_FUNC				(char*)__FUNCTION__

#ifdef WIN32
	#define DLL_EXPORT_API __declspec(dllexport)
#else
	#define DLL_EXPORT_API 
#endif


#ifdef _LDEBUG
		// 函数开始的DEBUG信息输出：行号、文件名、函数名
		#define START_DEBUG_INFO		P_PrintLog(_DEBUG_START, LD_LINE, \
											LD_FUNC, LD_FILE, "%s\n", "函数开始");
		
		// 函数结束的DEBUG信息输出：行号、文件名、函数名
		#define  END_DEBUG_INFO			P_PrintLog(_DEBUG_END, LD_LINE, \
											LD_FUNC, LD_FILE, "%s\n", "函数结束");
		
		/// 一般性信息输出
		#define D_INFO(fmt,...)		P_PrintLog(LD_INFO, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
		
		/// 警告信息输出
		#define D_WARN(fmt,...)		P_PrintLog(LD_WARN, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
		/// 错误信息输出
		#define D_ERROR(fmt,...)	P_PrintLog(LD_ERR, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
#endif

#ifdef _LDEBUG_W
		#define START_DEBUG_INFO		
		#define END_DEBUG_INFO			
		#define D_INFO(fmt,...)			
		/// 警告信息输出
		#define D_WARN(fmt,...)		P_PrintLog(LD_WARN, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
		/// 错误信息输出
		#define D_ERROR(fmt,...)	P_PrintLog(LD_ERR, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
#endif

#ifdef _LDEBUG_E
		#define START_DEBUG_INFO		
		#define   END_DEBUG_INFO		
		#define D_INFO(fmt,...)		
		#define D_WARN(fmt,...)		
		/// 错误信息输出
		#define D_ERROR(fmt,...)	P_PrintLog(LD_ERR, LD_LINE, LD_FUNC, LD_FILE, \
											fmt, __VA_ARGS__);
#endif


/// Windows平台
#ifdef WIN32
#if !defined(_LDEBUG) && !defined(_LDEBUG_W) && !defined(_LDEBUG_E)
		#define START_DEBUG_INFO		
		#define   END_DEBUG_INFO		
		#define D_INFO(fmt,...)		
		/// 未定义_LDEBUG_E或_LDEBUG_W时，不输出本信息
		#define D_WARN(fmt,...)		
		/// 未定义_LDEBUG_E时，不输出本信息
		#define D_ERROR(fmt,...)	
#endif

/// Linux平台
#else
#ifndef _LDEBUG && _LDEBUG_W && _LDEBUG_E
		#define START_DEBUG_INFO		
		#define   END_DEBUG_INFO		
		#define D_INFO(fmt,...)		
		/// 未定义_LDEBUG_E或_LDEBUG_W时，不输出本信息
		#define D_WARN(fmt,...)		
		/// 未定义_LDEBUG_E时，不输出本信息
		#define D_ERROR(fmt,...)	
#endif
#endif// end of WIN32


/// 各种平台下不同API函数的定义
#if defined WIN32
	#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
		#define swprintf        swprintf_s
		#define snprintf        sprintf_s
		#define strncasecmp     _memicmp
	#else
		#define swprintf        _snwprintf
		#define snprintf        _snprintf
	#endif
#endif

/// 
/// @par 功能
/// 输出信息的实体函数
/// @param[in]  nFlag          Flag标志(0:开始、1:结束、2:重大错误)
/// @param[in]  nLineNo        行号
/// @param[in]  szFunctionName 函数名
/// @param[in]  szFileName     文件名
/// @param[in]  szFormat       输出信息的格式
/// @param[in]  ...            可变参数
/// @return 无
/// @note -
/// @par 示例：
/// @code
/// @endcode
/// @see -
/// @deprecated -
/// 
DLL_EXPORT_API 
void P_PrintLog(int nFlag, int nLineNo, char* szFunctionName, 
				char* szFileName, char *szFormat, ...);

/// 
/// @par 功能
/// 打开文件
/// @param[in]  szFileName     文件名
/// @return 打开的文件指针
/// @note -
/// @par 示例：
/// @code
/// @endcode
/// @see -
/// @deprecated -
/// 
FILE* OpenFile(char* szFileName);


#endif// end of __LIGHT_DEBUG_INFORMATION__