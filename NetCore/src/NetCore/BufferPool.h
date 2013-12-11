///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 数据缓冲列表管理对象（缓冲池）
/// @par 相关文件
/// BufferPool.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         BufferPool.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/14
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LEE_CONTEST_BUFFER_POOL_MANAGER_H_H__
#define __LEE_CONTEST_BUFFER_POOL_MANAGER_H_H__

#include "NetBuffer.h"

/// 定义缓冲对象数据链表
typedef vector<CNetBuffer* >						CBufferPtrArray;

class CBufferPool
{
public:
	/// ==============================================
	/// @par 功能 
	/// 默认构造函数
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	CBufferPool();

	/// ==============================================
	/// @par 功能 
	/// 创建指定数量的列表构造函数
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usBufCount		指定缓冲数量
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	CBufferPool(USHORT usBufCount);

	/// ==============================================
	/// @par 功能 
	/// 默认析构函数
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	~CBufferPool();

public:

	/// ==============================================
	/// @par 功能 
	/// 设置空闲缓冲数量
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usBufCount		指定构造的数量
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	void e_SetBuffFreeCount(USHORT usBufCount);

	/// ==============================================
	/// @par 功能 
	/// 获取空闲缓冲列表大小
	/// @param 
	/// @return 	USHORT	返回的数量
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	USHORT e_GetFreeListSize();

	/// ==============================================
	/// @par 功能 
	/// 获取使用缓冲列表大小
	/// @param 
	/// @return 	USHORT	返回的数量
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	USHORT e_GetBusyListSize();

	/// ==============================================
	/// @par 功能 
	/// 从列表中获取可用缓冲数据对象信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBuffer		缓冲对象指针
	/// [in]		usIOType		缓冲对象类型
	/// @return 	bool			获取成功或者失败
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool e_AllocateBuffer(CNetBuffer*& pNetBuffer, USHORT usIOType);

	/// ==============================================
	/// @par 功能 
	/// 拆分网络缓冲数据包
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pSrcBuffer		数据缓冲原数据
	/// [in]		pDesBuffer		目标数据缓冲
	/// [in]		unSplitSize		要拆分的数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	bool e_SplitNetBuffer(CNetBuffer*& pSrcBuffer, CNetBuffer*& pDesBuffer, UINT unSplitSize);

	/// ==============================================
	/// @par 功能 
	/// 释放使用的缓冲对象
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBuffer		缓冲对象指针
	/// @return 	bool			释放成功或者失败
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool e_ReleaseBuffer(CNetBuffer*& pNetBuffer);

private:
	/// ==============================================
	/// @par 功能 
	/// 增加缓冲信息对象到空闲列表
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBuffer		缓冲对象指针
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool i_AddBufferToFreeList(CNetBuffer*& pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 清空数据缓冲列表
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	void i_ClearBufferList();

	/// ==============================================
	/// @par 功能 
	/// 生成缓冲ID
	/// @param 
	/// @return 	返回生成成功的生成缓冲的自编号
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	ULONG i_BuildNetBufferID();
private:
	/// 构造连接数量(为 NO_NUM_FREE_LIST 表示无固定数量列表)
	USHORT					m_usFreeBufCount;
	/// 缓冲列表锁
	CLightCritSec			m_CFreeListCrit;
	/// 空闲列表定义链表类型列表
	CBufferPtrArray			m_ArrayFreeBufList;
	/// 缓冲列表锁
	CLightCritSec			m_CBusyListCrit;
	/// 使用列表定义Hash类型列表
	CBufferMapToPtr			m_ArrayBusyBufList;
	/// 连接信息自编号变量
	ULONG					m_ulConstomBufferID;
};

#endif	/// __LEE_CONTEST_BUFFER_POOL_MANAGER_H_H__