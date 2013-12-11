////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程池管理对象
/// @par 相关文件
/// CThreadPool.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ThreadPool.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadPool.h"

CThreadPool::CThreadPool()
{
	// 初始化数据成员
	m_usThreadCount = 0;
	m_TBusyList.clear();
	m_TFreeList.clear();
}

CThreadPool::~CThreadPool()
{
	// 销毁线程池
	i_DestroyPool();
	// 重置数据成员
	m_usThreadCount = 0;
}

bool CThreadPool::e_InitTPool(USHORT usThreadCount)
{
	// 定义返回值
	bool bRet = false;
	// 判断线程格式是否超限
	if(MAX_THREAD_POOL < usThreadCount)
	{
		usThreadCount = MAX_THREAD_POOL;
	}

	// 定义线程对象指针
	CThreadBase* pThread = NULL;
	// 创建线程
	for(; m_usThreadCount < usThreadCount; m_usThreadCount++)
	{
		pThread = new CThreadBase(this);
		if(NULL != pThread)
		{
			i_PushToFreeList(pThread);
		}
	}
	// 验证线程是否完全创建
	bRet = (m_usThreadCount == usThreadCount);
	return bRet;
}

ULONG CThreadPool::e_GetTaskOrderID()
{
	// 获取任务ID
	return m_CTaskList.e_CreatTaskID();
}

bool CThreadPool::e_AddTTask(OnThreadProcess pfnThreadProcess,
	ULONG ulTaskID, USHORT usPriority)
{
	// 定义返回值
	bool bRet = false;
	// 验证回调线程方法是否合法
	if(NULL != pfnThreadProcess)
	{
		// 增加到任务列表
		bRet = m_CTaskList.e_AddTaskList(pfnThreadProcess,
			ulTaskID, usPriority);
		if(true == bRet)
		{
			// 处理线程任务
			e_ProcessTask();
		}
	}
	return bRet;
}

bool CThreadPool::e_DelTTask(ULONG ulTaskID)
{
	// 定义返回值
	bool bRet = false;
	// 删除任务信息对象
	bRet = m_CTaskList.e_DelTask(ulTaskID);
	// 返回数据
	return bRet;
}

bool CThreadPool::e_ProcessTask(CThreadBase* pThread)
{
	// 定义返回值
	bool bRet = false;
	// 验证线程对象是否合法
	if(NULL == pThread)
	{
		// 获取空闲列表
		pThread = i_PopFromFreeList();
	}

	// 线程是否存在
	if(NULL != pThread)
	{
		// 获取要处理的任务
		CTTask* pTTask = m_CTaskList.e_PopTask();
		if(NULL != pTTask)
		{
			// 运行线程
			pThread->e_Run(pTTask);
			// 增加到繁忙列表
			i_AddToBusyList(pThread);
			bRet = true;
		}
		else
		{
			// 如果没有任务处理, 从使用列表中移除，加入空闲线程列表
			i_DelFromBusyList(pThread);
		}
	}
	return bRet;
}

bool CThreadPool::e_Processed(CThreadBase* pThread)
{
	// 定义返回值
	bool bRet = false;
	// 验证线程对象合法性
	if(NULL != pThread)
	{
		// 继续处理未处理的任务
		bRet = e_ProcessTask(pThread);
	}
	return bRet;
}

bool CThreadPool::i_AddToBusyList(CThreadBase* pThread)
{
	// 定义返回值
	bool bRet = false;
	// 验证线程对象指针
	if(NULL != pThread)
	{
		// 加锁列表
		CAutoLock AutoLock(&m_CsTBusyLock);
		// 增加数据到列表
		if(m_TBusyList.end() == find(m_TBusyList.begin(),
			m_TBusyList.end(), pThread))
		{
			m_TBusyList.push_back(pThread);
		}		
		bRet = true;
	}
	return bRet;
}

bool CThreadPool::i_DelFromBusyList(CThreadBase* pThread)
{
	// 定义返回值
	bool bRet = false;
	// 验证线程对象指针
	if(NULL != pThread)
	{
		// 加锁列表
		m_CsTBusyLock.e_Lock();
		m_TBusyList.remove(pThread);
		m_CsTBusyLock.e_Unlock();
		// 增加到空闲列表
		i_PushToFreeList(pThread);
		bRet = true;
	}
	return bRet;
}

bool CThreadPool::i_PushToFreeList(CThreadBase* pThread)
{
	// 定义返回值
	bool bRet = false;
	// 验证指针合法性
	if(NULL != pThread)
	{
		// 加入到空闲指针对象
		CAutoLock AutoLock(&m_CsTFreeLock);
		// 增加数据到列表
		if(m_TFreeList.end() == find(m_TFreeList.begin(),
			m_TFreeList.end(), pThread))
		{
			m_TFreeList.push_back(pThread);
		}
		bRet = true;
	}
	return bRet;
}

CThreadBase* CThreadPool::i_PopFromFreeList()
{
	CThreadBase* pThread = NULL;
	// 加锁列表
	CAutoLock AutoLock(&m_CsTFreeLock);
	// 验证列表是否为空
	if(false == m_TFreeList.empty())
	{
		// 获取线程池中线程对象
		pThread = m_TFreeList.front();
		m_TFreeList.pop_front();
	}
	// 返回获取到线程对象指针
	return pThread;
}

void CThreadPool::i_DestroyPool()
{
	ThreadList::iterator iterBusyPos;
	ThreadQueue::iterator iterFreePos;
	CThreadBase* pThread = NULL;
	m_CsTBusyLock.e_Lock();
	for(iterBusyPos = m_TBusyList.begin(); 
		iterBusyPos != m_TBusyList.end();
		iterBusyPos++)
	{
		// 删除队列中对象
		pThread = (*iterBusyPos);
		if(NULL != pThread)
		{
			delete pThread;
			pThread = NULL;
		}
	}
	// 清空列表
	m_TBusyList.clear();
	m_CsTBusyLock.e_Unlock();

	m_CsTFreeLock.e_Lock();
	for(iterFreePos = m_TFreeList.begin(); 
		iterFreePos != m_TFreeList.end();
		iterFreePos++)
	{
		// 删除队列中对象
		pThread = (*iterFreePos);
		if(NULL != pThread)
		{
			delete pThread;
			pThread = NULL;
		}
	}
	// 清空列表
	m_TFreeList.clear();
	m_CsTFreeLock.e_Unlock();
}