////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程任务列表管理对象
/// @par 相关文件
/// TTaskList.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         TTaskList.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TTaskList.h"

CTTaskList::CTTaskList()
{
	// 初始化任务列表成员信息
	m_ulCurTaskOrder = 0;
	m_CTaskQueue.clear();
}

CTTaskList::~CTTaskList()
{
	// 初始化任务列表成员信息
	m_ulCurTaskOrder = 0;
	// 清空列表
	e_ClearList();
}

ULONG CTTaskList::e_CreatTaskID()
{
	// 增加任务序号
	CAutoLock AutoLock(&m_CsOrderLock);
	if(0 == ++m_ulCurTaskOrder)
	{
		m_ulCurTaskOrder++;
	}
	return m_ulCurTaskOrder;
}

bool CTTaskList::e_AddTaskList(OnThreadProcess pfnThreadProcess, ULONG ulTaskID, USHORT usPriority)
{
	bool bRet = false;
	// 普通权限
	if(ETTP_NORMAL == usPriority)
	{
		bRet = e_PushTask(pfnThreadProcess, ulTaskID);
	}
	else
	{
		bRet = e_FrontTask(pfnThreadProcess, ulTaskID);
	}
	return bRet;
}

bool CTTaskList::e_PushTask(OnThreadProcess pfnThreadProcess, ULONG ulTaskID)
{
	bool bRet = false;
	// 增加任务信息, 普通优先级
	CTTask* pTTask = new CTTask(pfnThreadProcess);
	if(NULL != pTTask)
	{
		pTTask->e_SetTaskID(ulTaskID);
		// 增加任务到队列尾部
		CAutoLock AutoLock(&m_CsTaskLock);
		m_CTaskQueue.push_back(pTTask);
		bRet = true;
	}
	return bRet;
}

bool CTTaskList::e_FrontTask(OnThreadProcess pfnThreadProcess, ULONG ulTaskID)
{
	bool bRet = false;
	// 增加任务信息, 高级优先级
	CTTask* pTTask = new CTTask(pfnThreadProcess);
	if(NULL != pTTask)
	{
		pTTask->e_SetTaskID(ulTaskID);
		pTTask->e_SetPriority(ETTP_HIGHT);
		// 增加任务到队列头部
		CAutoLock AutoLock(&m_CsTaskLock);
		m_CTaskQueue.push_front(pTTask);
		bRet = true;
	}
	return bRet;
}

CTTask* CTTaskList::e_PopTask()
{
	CTTask* pTTask = NULL;
	// 从队列中获取任务对象
	CAutoLock AutoLock(&m_CsTaskLock);
	// 判断队列是否为空
	if(false == m_CTaskQueue.empty())
	{
		// 取出队列头数据
		pTTask = m_CTaskQueue.front();
		// 移除队列头
		m_CTaskQueue.pop_front();
	}
	// 返回任务信息
	return pTTask;
}

bool CTTaskList::e_DelTask(ULONG ulTaskID)
{
	// 定义返回值
	bool bRet = false;
	TQueue::iterator iterPos;
	CTTask* pTTask = NULL;
	CAutoLock AutoLock(&m_CsTaskLock);
	// 查找对应的数据
	for(iterPos = m_CTaskQueue.begin(); 
		iterPos != m_CTaskQueue.end();
		iterPos++)
	{
		pTTask = (*iterPos);
		if(NULL != pTTask && ulTaskID == pTTask->e_GetTaskID())
		{
			m_CTaskQueue.erase(iterPos);
			// 删除任务对象
			delete pTTask;
			pTTask = NULL;
			bRet = true;
			break;
		}
	}
	return bRet;
}

bool CTTaskList::e_ClearList()
{
	TQueue::iterator iterPos;
	CTTask* pTTask = NULL;
	CAutoLock AutoLock(&m_CsTaskLock);
	for(iterPos = m_CTaskQueue.begin(); 
		iterPos != m_CTaskQueue.end();
		iterPos++)
	{
		// 删除队列中对象
		pTTask = (*iterPos);
		if(NULL != pTTask)
		{
			delete pTTask;
			pTTask = NULL;
		}
	}
	// 清空列表
	m_CTaskQueue.clear();
	return true;
}