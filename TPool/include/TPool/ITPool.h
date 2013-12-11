////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程池调用接口模块
/// @par 相关文件
/// 引用此文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ITPool.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TPOOL_CITPOOL_H_H__
#define __TPOOL_CITPOOL_H_H__

#include "TPoolDef.h"

#ifdef TPOOL_EXPORTS
	#define IPOOL_API __declspec(dllexport)
#else
	#define IPOOL_API __declspec(dllimport)
#endif /// TPOOL_EXPORTS

class IPOOL_API CITPool
{
public:
	CITPool();
	~CITPool();

	/// ==============================================
	/// @par 功能 
	/// 初始化线程池
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usThreadCount	线程池线程总数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_IInitTPool(USHORT usThreadCount);

	/// ==============================================
	/// @par 功能 
	/// 获取任务ID，全局唯一
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-		
	/// @return 	ULONG		生成的任务ID唯一标识号
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	ULONG e_IGetTTaskID();

	/// ==============================================
	/// @par 功能 
	/// 增加线程任务
	/// @param 
	/// [in,out]	<参数名>			<参数说明>
	///	[in]		pfnThreadProcess	线程回调事件函数指针
	/// [in]		ulTaskID			任务ID
	/// [in]		usPriority			任务优先级
	/// @return 	返回增加线程任务是否成功
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_IAddPoolTask(OnThreadProcess pfnThreadProcess, 
		ULONG ulTaskID, USHORT usPriority = ETTP_NORMAL);

	/// ==============================================
	/// @par 功能 
	/// 删除线程任务[如果线程任务还未处理，可从线程任务列表中删除]
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulTaskID		任务ID
	/// @return 	返回删除线程任务是否成功
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_IDelPoolTask(ULONG ulTaskID);
};

#endif	// __TPOOL_CITPOOL_H_H__