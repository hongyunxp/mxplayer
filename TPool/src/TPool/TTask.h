////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 线程任务信息对象
/// @par 相关文件
/// TTask.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         TTask.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/11/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TPOOL_CTTTASK_H_H__
#define __TPOOL_CTTTASK_H_H__

#include "common/CommTypes.h"
#include "TPool/TPoolDef.h"

class CTTask
{
public:
	CTTask(void* pFunction =  NULL);
	virtual ~CTTask();

	/// ==============================================
	/// @par 功能 
	/// 获取任务ID
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	任务ID
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	ULONG e_GetTaskID();

	/// ==============================================
	/// @par 功能 
	/// 设置任务ID
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulTaskID		任务ID
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	void e_SetTaskID(ULONG ulTaskID);

	/// ==============================================
	/// @par 功能 
	/// 获取任务优先级
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	任务优先级，参见_eThreadTaskPriority
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	USHORT e_GetPriority();

	/// ==============================================
	/// @par 功能 
	/// 设备任务优先级
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usPriority		优先级信息
	/// = 参见 _eThreadTaskPriority
	/// @return 	-
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	void e_SetPriority(USHORT usPriority);

	/// ==============================================
	/// @par 功能 
	/// 线程任务处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	处理完成的状态
	/// @note 		Creat By li.xl 2013/11/19 
	/// ==============================================
	virtual bool e_TTaskProcess();

private:
	// 任务ID
	ULONG	m_ulTaskID;
	// 任务优先级
	USHORT	m_usPriority;
protected:
	// 线程任务回调事件函数
	OnThreadProcess m_pfnThreadProcess;
};

#endif	// __TPOOL_CTTTASK_H_H__