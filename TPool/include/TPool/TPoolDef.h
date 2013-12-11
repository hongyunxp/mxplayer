////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程池定义信息模块
/// @par 相关文件
/// 引用此文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         TPoolDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/18
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TPOOL_THREAD_POOL_DEF_H_H__
#define __TPOOL_THREAD_POOL_DEF_H_H__

// 线程任务优先级
enum _eThreadTaskPriority
{
	ETTP_NORMAL	= 0,		///< 标准优先权
	ETTP_HIGHT	= 1,		///< 高级优先权
	ETTP_END	= 2,
};

/// ==============================================
/// @par 功能 
/// 线程回调事件函数
/// @param 
/// [in,out]	<参数名>		<参数说明>
///	[in]		ulTaskID		任务ID
/// @return 	处理结果状态
/// @note 		Creat By li.xl 2013/11/19 
/// ==============================================
typedef bool (* OnThreadProcess)(ULONG ulTaskID);

#endif	// __TPOOL_THREAD_POOL_DEF_H_H__