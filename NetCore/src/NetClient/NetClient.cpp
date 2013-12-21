////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 处理接口函数调用
/// @par 相关文件
/// NetClient.h NetTreatMent.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetClient.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "common/CommTypes.h"
#include "NetClient/NetClient.h"
#include "NetTreatMent.h"

#define e_GetNetClient	((CNetTreatment* )(m_pProNetClient))

CNetClient::CNetClient()
{
	m_pProNetClient = new CNetTreatment();
}

CNetClient::~CNetClient()
{
	// 释放数据对象
	if(NULL != e_GetNetClient)
	{
		delete e_GetNetClient;
		m_pProNetClient = NULL;
	}
}

int CNetClient::e_IInitNetClient(T_InitNetClient* psttInitNetClient, 
	OnRecvDataCallBack pfnRecvDataCallBack)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 初始化数据
	e_GetNetClient->e_InitNetClient(psttInitNetClient);
	e_GetNetClient->e_SetReceiveDataCallBack(pfnRecvDataCallBack);
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_IConnectionServer()
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 连接服务器
	if(true == e_GetNetClient->e_ConnectServer())
	{
		nRet = 1;
	}
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_IDisconnectServer()
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	e_GetNetClient->e_StopTCPProcess();
	nRet = 1;
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_ICreatUDPClient()
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 创建UDP客户端
	if(true == e_GetNetClient->e_CreatUDPClient())
	{
		nRet = 1;
	}
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_IDestroyUDPClient()
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	e_GetNetClient->e_StopUDPProcess();
	nRet = 1;
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_ISendTCPStringData(char* pszSendData, UINT unDatalen)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 验证数据合法性
	if(NULL == pszSendData || 0 >= unDatalen)
	{
		END_DEBUG_INFO
		return nRet;
	}

	T_TCPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_TCPBufferHead));
	/// 赋值数据
	sttBufferHead.nTotalLen = sizeof(T_TCPBufferHead) + unDatalen\
		- sizeof(sttBufferHead.nTotalLen);
	sttBufferHead.sDataType = JDT_StringData;
	sttBufferHead.nOBJSize = unDatalen;
	sttBufferHead.sOBJCount = 1;

	if(true == e_GetNetClient->e_CreatSendTCPData(sttBufferHead,
		(BYTE* )pszSendData, unDatalen))
	{
		nRet = 1;
	}
	/// 发送数据
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_ISendTCPBinaryData(UINT unSSDType, UINT unOBJSize,
	USHORT usOBJCount, BYTE* pSendData)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 获取数据长度
	int nDataLen = unOBJSize * usOBJCount;
	/// 验证数据合法性
	if(NULL == pSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return nRet;
	}

	T_TCPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_TCPBufferHead));
	/// 赋值数据
	sttBufferHead.nTotalLen = sizeof(T_TCPBufferHead) + nDataLen\
		- sizeof(sttBufferHead.nTotalLen);
	sttBufferHead.sDataType = JDT_StructData;
	sttBufferHead.nOBJType = unSSDType;
	sttBufferHead.nOBJSize = unOBJSize;
	sttBufferHead.sOBJCount = usOBJCount;
	sttBufferHead.sSNum = 1;
	sttBufferHead.sENum = usOBJCount;
	/// 发送数据
	if(true == e_GetNetClient->e_CreatSendTCPData(sttBufferHead,
		(BYTE* )pSendData, nDataLen))
	{
		nRet = 1;
	}
	END_DEBUG_INFO
	return nRet; 
}

int CNetClient::e_ISendUDPStringData(const char* pRemoteIP, USHORT usRemotePort, 
	char* pszSendData, UINT unDatalen)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 验证数据合法性
	if(NULL == pRemoteIP || 0 >= usRemotePort ||
		NULL == pszSendData || 0 >= unDatalen)
	{
		END_DEBUG_INFO
		return nRet;
	}

	/// 定义UDP发送数据头
	T_UDPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_UDPBufferHead));
	/// 赋值数据
	sttBufferHead.sDataType = JDT_StringData;
	sttBufferHead.nOBJSize = unDatalen;
	sttBufferHead.sOBJCount = 1;
	/// 发送数据
	if(true == e_GetNetClient->e_CreatSendUDPData(pRemoteIP, usRemotePort, sttBufferHead,
		(BYTE* )pszSendData, unDatalen))
	{
		nRet = 1;
	}
	END_DEBUG_INFO
	return nRet;
}

int CNetClient::e_ISendUDPBinaryData(const char* pRemoteIP, USHORT usRemotePort,
	UINT unSSDType, UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = 0;
	/// 获取数据长度
	int nDataLen = unOBJSize * usOBJCount;
	/// 验证数据合法性
	if(NULL == pRemoteIP || 0 >= usRemotePort ||
		NULL == pSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
	}

	T_UDPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_UDPBufferHead));
	/// 赋值数据
	sttBufferHead.sDataType = JDT_StructData;
	sttBufferHead.nOBJType = unSSDType;
	sttBufferHead.nOBJSize = unOBJSize;
	sttBufferHead.sOBJCount = usOBJCount;
	sttBufferHead.sSNum = 1;
	sttBufferHead.sENum = usOBJCount;
	/// 发送数据
	if(true == e_GetNetClient->e_CreatSendUDPData(pRemoteIP,
		usRemotePort, sttBufferHead, pSendData, nDataLen))
	{
		nRet = 1;
	}
	END_DEBUG_INFO
	return nRet;
}