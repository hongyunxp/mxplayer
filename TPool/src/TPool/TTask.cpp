////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程任务信息对象
/// @par 相关文件
/// TTask.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         TTask.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TTask.h"

CTTask::CTTask(void* pFunction)
{
	// 初始化任务成员
	m_ulTaskID = 0;
	m_usPriority = ETTP_NORMAL;
	m_pfnThreadProcess = (OnThreadProcess)pFunction;
}

CTTask::~CTTask()
{
	// 重置任务成员信息
	m_ulTaskID = 0;
	m_usPriority = ETTP_NORMAL;
	m_pfnThreadProcess = NULL;
}

ULONG CTTask::e_GetTaskID()
{
	return m_ulTaskID;
}

void CTTask::e_SetTaskID(ULONG ulTaskID)
{
	m_ulTaskID = ulTaskID;
}

USHORT CTTask::e_GetPriority()
{
	return m_usPriority;
}

void CTTask::e_SetPriority(USHORT usPriority)
{
	m_usPriority = usPriority;
}

bool CTTask::e_TTaskProcess()
{
	// 定义返回值
	bool bRet = false;
	// 验证函数指针正确性
	if (NULL != m_pfnThreadProcess)
	{
		// 回调事件函数处理
		bRet = m_pfnThreadProcess(m_ulTaskID);
	}
	// 返回处理结果
	return bRet;
}