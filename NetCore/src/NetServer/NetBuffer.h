////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 连接数据缓冲对象
/// @par 相关文件
/// CNetBuffer.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         CNetBuffer.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/13
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_CONTEXT_BUFFER_H_H__
#define __LEE_CONTEXT_BUFFER_H_H__

#include "Common\CommTypes.h"
#include "Common\AutoLock.h"
#include "NetIOCPDef.h"

class CNetBuffer
{
public:
	CNetBuffer();
	virtual ~CNetBuffer();

	/// ==============================================
	/// @par 功能 
	/// 增加数据到数据缓冲
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pBtData			要增加的数据
	/// [in]		nSize			要增加的数据缓冲大小
	/// @return 	
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	bool e_AddData(const BYTE* pBtData, UINT unSize);

	/// ==============================================
	/// @par 功能 
	/// 从缓冲中获取数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in/out]	pBtData			要获取的数据变量
	/// [in]		nSize			要获取的数据缓冲大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	bool e_GetData(BYTE*& pBtData, UINT unSize);

	/// ==============================================
	/// @par 功能 
	/// 获取数据缓冲
	/// @param 
	/// @return 	返回缓冲对象指针
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	BYTE* e_GetBuffer();

	/// ==============================================
	/// @par 功能 
	/// 移除定长的数据包
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		unRemoveSize	移除的数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	bool e_FlushBuffer(UINT unRemoveSize);

	/// ==============================================
	/// @par 功能 
	/// 增加其他的Buffer对象到数据缓冲
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pOtherBuffer	其他缓冲对象
	/// [in]		usMoveSize		要移动的数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	bool e_AddMoveOtherBuffer(CNetBuffer*& pOtherBuffer, UINT usMoveSize);
 
	/// ==============================================
	/// @par 功能 
	/// 获取当前使用的缓冲大小
	/// @param 
	/// @return 	UINT			增加后的使用缓冲大小
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	UINT e_GetUsed();

	/// ==============================================
	/// @par 功能 
	/// 增加当前数据使用大小
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		unAddUsed		要增加的数量
	/// @return 	UINT			增加后的使用缓冲大小
	/// @note 		by li.xl 2013/06/24 
	/// ==============================================
	UINT e_AddUsed(UINT unAddUsed);

	/// ==============================================
	/// @par 功能 
	/// 设置缓冲序列号
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulBuffSeqNum	缓冲序列号
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	void e_SetBufferSeqNum(ULONG ulBuffSeqNum);

	/// ==============================================
	/// @par 功能 
	/// 获取缓冲序列号
	/// @return 	ULONG			返回缓冲序列号
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	ULONG e_GetBufferSeqNum();

	 /// ==============================================
	 /// @par 功能 
	 /// 设置缓冲ID
	 /// @param 
	 /// [in,out]	<参数名>		<参数说明>
	 /// [in]		ulBufferID		缓冲ID
	 /// @return 	-
	 /// @note 		by li.xl 2013/06/19 
	 /// ==============================================
	void e_SetBufferID(ULONG ulBufferID);

	/// ==============================================
	/// @par 功能 
	/// 获取缓冲ID
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	ulBufferID		缓冲ID
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	ULONG e_GetBufferID();

	/// ==============================================
	/// @par 功能 
	/// 获取WASBuffer 数据
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	WSABUF*	e_GetWSABuffer();

	/// ==============================================
	/// @par 功能 
	/// 设置缓冲类型
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usIOType		缓冲操作类型
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	void e_SetOperation(USHORT usIOType);

	/// ==============================================
	/// @par 功能 
	/// 获取缓冲类型
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	USHORT			缓冲操作类型
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	USHORT e_GetOperation();

	/// ==============================================
	/// @par 功能 
	/// 重置数据内容
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	-
	/// @note 		by li.xl 2013/07/01 
	/// ==============================================
	void e_ReBuffValue();

	/// ==============================================
	/// @par 功能 
	/// 重置缓冲
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ============================================== 
	void e_ReSetBuffer();

private:
	/// 定义数据缓冲ID, 便于列表检索
	ULONG		m_ulBufferID;
	/// 定义序列号
	ULONG		m_ulBuffSeqNum;
	/// 定义缓冲数据
	BYTE		m_Buffer[MAX_NET_BUFFER_SIZE];
	/// 定义使用的数据大小
	UINT		m_unUsed;
	/// 用于存放发送和接收的缓冲数据
	WSABUF		m_sttWsaBuf;
	/// IO操作类型
	USHORT		m_usIOProType;
public:
	/// 投递的重叠对象结构体
	WSAOVERLAPPED m_sttIOOverLapped;
	/// 存放UDP客户端地址信息
	SOCKADDR_IN	  m_sttUDPAddrInfo;
	/// = INT 存放 SOCKADDR_IN 长度大小,
	/// = 此值在UDP接收函数投递WSARecvFrom时，必须是【In、Out】
	/// = 参数形式存在，只有当在完成端口取出数据后，该数据参数才能被释放，
	/// = 所以此值作为NetBuffer的成员变量
	int			  m_nAddressLen;
};

#endif	/// __LEE_CONTEXT_BUFFER_H_H__