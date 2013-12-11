/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 基础线程对象
/// @par 相关文件
/// ThreadBase.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ThreadBase.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/18
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadBase.h"
#include "ThreadPool.h"

CThreadBase::CThreadBase(CThreadPool* pThreadPool)
{
	m_bIsRuning = false;
	m_hTheadHandle = NULL;
	m_hThreadEven = NULL;
	m_pTTask = NULL;
	m_pThreadPool = pThreadPool;
	// 初始化线程运行事件信号
	m_hThreadEven = CreateEvent(NULL, FALSE, FALSE, NULL);
	// 创建线程
	e_Creat();
}

CThreadBase::~CThreadBase()
{
	// 重置数据变量
	m_pTTask = NULL;
	m_pThreadPool = NULL;
	// 释放线程
	e_Destroy();
}

bool CThreadBase::e_Creat()
{
	// 验证线程是否已经启动
	if(NULL == m_hTheadHandle)
	{
		// 启动线程
		m_hTheadHandle = (HANDLE)_beginthreadex(NULL, 0, i_StartThread, this, 0, NULL);
		m_bIsRuning = (NULL != m_hTheadHandle);
	}
	// 返回线程句柄是否成功
	return m_bIsRuning;
}

bool CThreadBase::e_Run(CTTask* pTTask)
{
	// 验证线程任务对象指针
	if(NULL != pTTask)
	{
		m_pTTask = pTTask;
	}
	// 设置线程处理事件信号
	SetEvent(m_hThreadEven);
	return true;
}

bool CThreadBase::e_Destroy()
{
	// 验证线程是否存在
	if(NULL != m_hTheadHandle)
	{
		// 设置停止运行
		m_bIsRuning = false;
		// 设置线程事件信号量
		SetEvent(m_hThreadEven);
		// 获取线程对象句柄信号
		WaitForSingleObject(m_hTheadHandle, INFINITE);
		// 释放句柄
		CloseHandle(m_hTheadHandle);
		m_hTheadHandle = NULL;
		CloseHandle(m_hThreadEven);
		m_hThreadEven = NULL;
	}
	// 删除任务对象
	i_DelTask();
	// 返回释放线程成功
	return true;
}

bool CThreadBase::e_GetRunStatus()
{
	// 返回运行状态
	return m_bIsRuning;
}

void CThreadBase::e_ThreadProcess()
{
	// 处理线程
	while(true == m_bIsRuning)
	{
		/// 等待执行事件
		WaitForSingleObject(m_hThreadEven, INFINITE);
		/// 指定的处理事件不为NULL，回调处理
		if(NULL != m_pTTask)
		{
			// 执行任务线程
			m_pTTask->e_TTaskProcess();
			// 删除任务对象
			i_DelTask();
			// 停止线程池，任务已处理完成
			if(NULL != m_pThreadPool)
			{
				m_pThreadPool->e_Processed(this);
			}
		}
	}
}

bool CThreadBase::i_DelTask()
{
	// 定义返回值
	bool bRet = false;
	if(NULL != m_pTTask)
	{
		// 删除任务对象
		delete m_pTTask;
		m_pTTask = NULL;
		bRet = true;
	}
	return bRet;
}

UINT WINAPI CThreadBase::i_StartThread(LPVOID lpParama)
{
	CThreadBase* pThis = (CThreadBase* )lpParama;
	if(NULL != pThis)
	{
		// 开始启动线程循环
		pThis->e_ThreadProcess();
	}
	return 0;
}