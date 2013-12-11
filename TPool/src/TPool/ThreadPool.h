////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程池管理对象
/// @par 相关文件
/// CThreadPool.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ThreadPool.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TPOOL_CTHREAD_POOL_H_H__
#define __TPOOL_CTHREAD_POOL_H_H__

#include "ThreadBase.h"
#include "TTaskList.h"

///< 定义线程队列
typedef list<CThreadBase *>		ThreadList;
typedef deque<CThreadBase *>	ThreadQueue;

/// 线程池线程最大数量
#define MAX_THREAD_POOL		500

class CThreadPool
{
public:
	CThreadPool();
	virtual ~CThreadPool();

	/// ==============================================
	/// @par 功能 
	/// 初始化线程池
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usThreadCount	线程总数
	/// @return 	初始化成功与否
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_InitTPool(USHORT usThreadCount);

	/// ==============================================
	/// @par 功能 
	/// 获取任务唯一序列号
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	ULONG			任务ID唯一标识
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	ULONG e_GetTaskOrderID();

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
	bool e_AddTTask(OnThreadProcess pfnThreadProcess, 
		ULONG ulTaskID, USHORT usPriority = ETTP_NORMAL);

	/// ==============================================
	/// @par 功能 
	/// 删除线程任务[如果线程任务还未处理，可从线程任务列表中删除]
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulTaskID		任务ID
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_DelTTask(ULONG ulTaskID);

	/// ==============================================
	/// @par 功能 
	/// 处理任务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pThread			线程对象指针
	/// = 如果线程存在，则使用该线程对象处理任务，
	/// = 如果线程不存在，则使用空闲线程列表中的某线程处理任务
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_ProcessTask(CThreadBase* pThread = NULL);

	/// ==============================================
	/// @par 功能 
	/// 线程处理完成
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pThread			线程对象指针
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool e_Processed(CThreadBase* pThread);

private:
	/// ==============================================
	/// @par 功能 
	/// 增加到使用线程列表
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pThread			线程对象指针
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool i_AddToBusyList(CThreadBase* pThread);

	/// ==============================================
	/// @par 功能 
	/// 从使用线程列表中删除
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pThread			线程对象指针
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool i_DelFromBusyList(CThreadBase* pThread);

	/// ==============================================
	/// @par 功能 
	/// 增加到空闲列表尾部
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pThread			线程对象指针
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	bool i_PushToFreeList(CThreadBase* pThread);

	/// ==============================================
	/// @par 功能 
	/// 从空闲列表头部获取线程对象指针
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	 -
	/// @return 	CThreadBase*	线程对象指针
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	CThreadBase* i_PopFromFreeList();

	/// ==============================================
	/// @par 功能 
	/// 销毁线程池
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	void i_DestroyPool();

private:
	// 任务队列
	CTTaskList		m_CTaskList;
	// 使用线程列表锁
	CLightCritSec	m_CsTBusyLock;
	// 使用线程列表
	ThreadList		m_TBusyList;
	// 空闲线程列表锁
	CLightCritSec	m_CsTFreeLock;
	// 空闲线程列表
	ThreadQueue		m_TFreeList;
	// 线程总数
	USHORT			m_usThreadCount;
};

#endif	// __TPOOL_CTHREAD_POOL_H_H__