////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程池调用接口模块
/// @par 相关文件
/// ITPool.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ITPool.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadPool.h"
#include "TPool/ITPool.h"

CThreadPool* m_pThreadPool = NULL;

CITPool::CITPool()
{
	m_pThreadPool = new CThreadPool();
}

CITPool::~CITPool()
{
	if(NULL != m_pThreadPool)
	{
		delete m_pThreadPool;
		m_pThreadPool = NULL;
	}
}

bool CITPool::e_IInitTPool(USHORT usThreadCount)
{
	// 初始化线程池
	return m_pThreadPool->e_InitTPool(usThreadCount);
}

ULONG CITPool::e_IGetTTaskID()
{
	// 获取任务唯一标识
	return m_pThreadPool->e_GetTaskOrderID();
}

bool CITPool::e_IAddPoolTask(OnThreadProcess pfnThreadProcess, 
	ULONG ulTaskID, USHORT usPriority)
{
	// 增加任务信息
	return m_pThreadPool->e_AddTTask(pfnThreadProcess, ulTaskID, usPriority);
}

bool CITPool::e_IDelPoolTask(ULONG ulTaskID)
{
	// 删除任务ID对应的任务
	return m_pThreadPool->e_DelTTask(ulTaskID);
}