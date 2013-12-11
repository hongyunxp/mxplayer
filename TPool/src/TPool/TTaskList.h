////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程任务列表管理对象
/// @par 相关文件
/// TTaskList.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         TTaskList.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TPOOL_TTASK_LIST_H_H__
#define __TPOOL_TTASK_LIST_H_H__

#include "common/CommTypes.h"
#include "common/AutoLock.h"
#include "TTask.h"

///< 任务队列定义
typedef deque<CTTask* >	TQueue;

class CTTaskList
{
public:
	CTTaskList();
	virtual ~CTTaskList();

	/// ==============================================
	/// @par 功能 
	/// 创建任务ID
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	返回新创建任务ID
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	ULONG e_CreatTaskID();

	/// ==============================================
	/// @par 功能 
	/// 增加线程任务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pfnThreadProcess	线程回调事件函数指针
	/// [in]		ulTaskID			任务ID
	/// [in]		usPriority			任务优先级
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_AddTaskList(OnThreadProcess pfnThreadProcess, ULONG ulTaskID, USHORT usPriority);

	/// ==============================================
	/// @par 功能 
	/// 追加数据任务处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pfnThreadProcess线程处理回调函数
	/// [in]		ulTaskID		任务ID
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_PushTask(OnThreadProcess pfnThreadProcess, ULONG ulTaskID);

	/// ==============================================
	/// @par 功能 
	/// 在数据列表头增加任务处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pfnThreadProcess线程处理回调函数
	/// [in]		ulTaskID		任务ID
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_FrontTask(OnThreadProcess pfnThreadProcess, ULONG ulTaskID);

	/// ==============================================
	/// @par 功能 
	/// 从队列中移除任务信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	CTTask	任务信息对象指针
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	CTTask* e_PopTask();

	/// ==============================================
	/// @par 功能 
	/// 删除任务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulTaskID		任务ID
	/// @return 	bool		删除任务对象是否成功
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_DelTask(ULONG ulTaskID);

	/// ==============================================
	/// @par 功能 
	/// 清除任务队列
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	清除是否成功
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_ClearList();

private:
	// 当前任务序列锁
	CLightCritSec	m_CsOrderLock;
	// 当前任务序列
	ULONG			m_ulCurTaskOrder;
	// 任务队列锁
	CLightCritSec	m_CsTaskLock;
	// 任务队列
	TQueue			m_CTaskQueue;
};

#endif	// __TPOOL_TASK_LIST_H_H__