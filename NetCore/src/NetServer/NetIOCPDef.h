/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 网络处理模块
/// @par 相关文件
/// 引用此文件头的所有文件头
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetIOCPDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/08
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_NET_IOCP_DEF_H_H__
#define __LEE_NET_IOCP_DEF_H_H__

#include "Common/CommTypes.h"
#include "Common/AutoLock.h"
#include "common/NetCoreDef.h"

class CNetBuffer;
typedef map<ULONG, CNetBuffer*, greater<ULONG>>		CBufferMapToPtr;

/// 最大工作线程数量
#define MAX_PROCESSER_NUMBERS	20
/// 缓冲无固定空闲数量的列表
#define	NO_NUM_FREE_LIST		0
/// 连接池数量
#define NETCORE_TCP_CONPOOL_NUM  1024
/// 缓冲池数量
#define NETCORE_BUFFER_POOL_NUM	 2048

/// IO工作线程操作类型
enum _eIOProcessType
{
	IOPT_None			= 0,						///< 未知的操作类型
	IOPT_TACCEPT		= 1,						///< TCP成功连接
	IOPT_TCPRECV		= 2,						///< TCP准备接收数据
	IOPT_TRECVED		= 3,						///< TCP接收数据完成
	IOPT_TCPSEND		= 4,						///< TCP准备发送数据
	IOPT_TSENDED		= 5,						///< TCP发送数据完成
	IOPT_UDPRECV		= 6,						///< UDP接收数据完成
	IOPT_UDPSEND		= 7,						///< UDP发送数据完成
};

/// 客户端连接结构体
typedef struct _tClientContext
{	
	ULONG					ulContextID;			///< 连接信息ID
	SOCKET					sSocketID;				///< 连接套接字	
	char					szClientIP[16];			///< 客户端连接IP
	USHORT					usClientPort;			///< 客户端端口号
	SHORT					sNumOfIORef;			///< IO引用计数
	USHORT					usReadSeqNum;			///< 接收的序列号变量
	USHORT					usSendSeqNum;			///< 发送的序列号变量
	USHORT					usCurReadSeqNum;		///< 当前接收的序列号
	USHORT					usCurSendSeqNum;		///< 当前接收的序列号
	CLightCritSec			ContextCritSec;			///< 连接锁(修改或者读取时使用)
	CBufferMapToPtr			CReadBufferMap;			///< 接收的缓冲乱序集合
	CBufferMapToPtr			CSendBufferMap;			///< 接收的缓冲乱序集合
	CNetBuffer*				pCurrentBuffer;			///< 当前处理缓冲数据

	/// 构造初始化
	_tClientContext()
	{
		e_InitStruct();
	}

	/// 初始化数据
	void e_InitStruct()
	{
		ulContextID = 0;
		sSocketID = NULL;		
		memset(szClientIP, 0x00, sizeof(szClientIP));
		usClientPort = 0;
		sNumOfIORef = 0;
		usReadSeqNum = 0;
		usSendSeqNum = 0;
		usCurReadSeqNum = 0;
		usCurSendSeqNum = 0;
		CReadBufferMap.clear();
		CSendBufferMap.clear();
		pCurrentBuffer = NULL;
	}

	/// 增加接收数据序列
	void e_AddReadSeqNum()
	{
		ContextCritSec.e_Lock();
		usReadSeqNum++;
		ContextCritSec.e_Unlock();
	}

	/// 增加发送数据序列
	void e_AddSendSeqNum()
	{
		ContextCritSec.e_Lock();
		usSendSeqNum++;
		ContextCritSec.e_Unlock();
	}

	/// 增加当前接收数据的序列号
	void e_InCreaseCurReadSeqNum()
	{
		ContextCritSec.e_Lock();
		usCurReadSeqNum++;
		ContextCritSec.e_Unlock();
	}

	/// 增加当前发送数据的序列号
	void e_InCreaseCurSendSeqNum()
	{
		ContextCritSec.e_Lock();
		usCurSendSeqNum++;
		ContextCritSec.e_Unlock();
	}

	/// 增加IO引用计数
	SHORT e_EnterRefNum()
	{
		return ++sNumOfIORef;
	}

	/// 减少IO引用计数
	SHORT e_ExitRefNum()
	{
		return --sNumOfIORef;
	}

}T_ClientContext, *LP_ClientContext;

#endif	/// __LEE_NET_IOCP_DEF_H_H__