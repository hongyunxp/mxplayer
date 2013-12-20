////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端连接列表管理对象
/// @par 相关文件
/// ContextList.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ContextList.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/14
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LEE_CONTEXT_LIST_MANAGER_H_H__
#define __LEE_CONTEXT_LIST_MANAGER_H_H__

#include "BufferPool.h"
#include "NetIOCPDef.h"

/// 定义Hash映射
typedef map<ULONG, T_ClientContext*, greater<ULONG>>		CContextMapULONGToPtr;

typedef vector<T_ClientContext* >							CContextPtrArray;

class CContextList
{
public:
	/// ==============================================
	/// @par 功能 
	/// 不创建连接数量的列表构造函数
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	CContextList();

	/// ==============================================
	/// @par 功能 
	/// 创建指定数量的列表构造函数
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usConFreeNum	指定构造的数量
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	CContextList(USHORT usConFreeNum);

	/// ==============================================
	/// @par 功能 
	/// 析构函数
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	virtual ~CContextList();

public:

	/// ==============================================
	/// @par 功能 
	/// 初始化连接列表
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pBufferPool		数据缓冲管理对象指针
	/// @return 	-
	/// @note 		by li.xl 2013/06/25 
	/// ==============================================
	void e_InitContextList(CBufferPool* pBufferPool);

	/// ==============================================
	/// @par 功能 
	/// 设置空闲连接数量
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usConFreeNum	指定构造的数量
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	void e_SetFreeConNum(USHORT usConFreeNum);

	/// ==============================================
	/// @par 功能 
	/// 获取空闲连接列表大小
	/// @param 
	/// @return 	USHORT	返回的数量
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	USHORT e_GetFreeListSize();

	/// ==============================================
	/// @par 功能 
	/// 获取使用连接列表大小
	/// @param 
	/// @return 	USHORT	返回的数量
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	USHORT e_GetBusyListSize();

	/// ==============================================
	/// @par 功能 
	/// 从列表中获取可用连接信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		psttContext		连接信息对象指针
	/// @return 	bool			获取成功或者失败
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool e_AllocateConntext(T_ClientContext*& psttContext);

	/// ==============================================
	/// @par 功能 
	/// 释放使用连接信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		psttContext		连接信息对象指针
	/// @return 	bool			释放成功或者失败
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool e_ReleaseConntext(T_ClientContext*& psttContext);

	/// ==============================================
	/// @par 功能 
	/// 获取正在使用的客户端连接信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID
	/// @return 	T_ClientContext	返回的连接信息对象
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	T_ClientContext* e_GetConntext(ULONG ulContextID);

private:
	/// ==============================================
	/// @par 功能 
	/// 增加连接信息到空闲列表
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		psttContext		连接信息对象指针
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool i_AddContextToFreeList(T_ClientContext* psttContext);

	/// ==============================================
	/// @par 功能 
	/// 清空连接信息列表
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	void i_ClearContextList();

	/// ==============================================
	/// @par 功能 
	/// 生成连接信息ID
	/// @param 
	/// @return 	返回生成成功的连接信息自编号
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	ULONG i_BuildContextID();

	/// ==============================================
	/// @par 功能 
	/// 释放Buffer缓冲列表
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pBufferMap		缓冲映射列表
	/// @return 	-
	/// @note 		by li.xl 2013/06/25 
	/// ==============================================
	void i_ReleaseBufferMap(CBufferMapToPtr* pBufferMap);
private:
	/// 构造连接数量(为 NO_NUM_FREE_LIST 表示无固定数量列表)
	USHORT					m_usConFreeNum;
	/// 连接列表锁
	CLightCritSec			m_CFreeListCrit;
	/// 连接列表
	CContextPtrArray		m_ContextFreeArray;
	/// 连接列表锁
	CLightCritSec			m_CBusyListCrit;
	/// 连接列表
	CContextMapULONGToPtr	m_ContextBusyArray;
	/// 连接信息自编号变量
	ULONG					m_ulContextID;
	/// 设置数据缓冲管理对象指针
	CBufferPool*			m_pBufferManager;
};

#endif	/// __LEE_CONTEXT_LIST_MANAGER_H_H__