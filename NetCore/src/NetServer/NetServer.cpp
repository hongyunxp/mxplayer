/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// IOCP处理模块
/// @par 相关文件
/// NetIOCP.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetIOCP.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/08
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetIOCP.h"
#include "NetServer.h"

#define e_GetNetServer	((CNetIOCP* )(m_pCNetProcess))

CNetServer::CNetServer()
{
	// 初始化管理对象
	m_pCNetProcess = new CNetIOCP();
}

CNetServer::~CNetServer()
{
	// 释放数据对象
	if(NULL != e_GetNetServer)
	{
		delete e_GetNetServer;
		m_pCNetProcess = NULL;
	}
}

bool CNetServer::e_IStartServer(T_NetInitStruct* psttNetInitStruct,
	OnRecvDataCallBack pFunRecvData, OnSendDataCallBack pFunSendData, 
	OnConectionCallBack pFunConection, OnDisConectionCallBack pFunDisConection,
	OnPrintLog pFunPrintLog)
{
	START_DEBUG_INFO
	/// 初始化网络模块
	if(true == e_GetNetServer->e_InitNetModel(*psttNetInitStruct, pFunRecvData,
		pFunSendData, pFunConection, pFunDisConection, pFunPrintLog))
	{
		END_DEBUG_INFO
		return e_GetNetServer->e_StartNetService();
	}
	END_DEBUG_INFO
	return false;
}

bool CNetServer::e_IStopdServer()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return e_GetNetServer->e_StopdNetService();
}

bool CNetServer::e_ITCPSendStringData(ULONG ulContextID, char* pszSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pszSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
	}

	T_TCPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_TCPBufferHead));
	/// 赋值数据
	sttBufferHead.nTotalLen = sizeof(T_TCPBufferHead) + nDataLen\
		- sizeof(sttBufferHead.nTotalLen);
	sttBufferHead.sDataType = JDT_StringData;
	sttBufferHead.nOBJSize = nDataLen;
	sttBufferHead.sOBJCount = 1;
	/// 发送数据
	END_DEBUG_INFO
	return e_GetNetServer->e_SendDataToTCPClient(ulContextID, 
	sttBufferHead, (BYTE* )pszSendData, nDataLen);
}

bool CNetServer::e_ITCPSendBinaryData(ULONG ulContextID, UINT unSSDType, 
	UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData)
{
	START_DEBUG_INFO
	/// 获取数据长度
	int nDataLen = unOBJSize * usOBJCount;
	/// 验证数据合法性
	if(NULL == pSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
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
	END_DEBUG_INFO
	return e_GetNetServer->e_SendDataToTCPClient(ulContextID,
	sttBufferHead, pSendData, nDataLen);
}

bool CNetServer::e_IUDPSendStringData(const char* pszClientIP, 
	USHORT usClientPort, char* pszSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort ||
		NULL == pszSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
	}

	T_UDPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_UDPBufferHead));
	/// 赋值数据
	sttBufferHead.sDataType = JDT_StringData;
	sttBufferHead.nOBJSize = nDataLen;
	sttBufferHead.sOBJCount = 1;
	/// 发送数据
	END_DEBUG_INFO
	return e_GetNetServer->e_SendDataToUDPClient(pszClientIP,
	usClientPort, sttBufferHead, (BYTE* )pszSendData, nDataLen);
}

bool CNetServer::e_IUDPSendBinaryData(const char* pszClientIP, USHORT usClientPort, 
	UINT unSSDType, UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData)
{
	START_DEBUG_INFO
	/// 获取数据长度
	int nDataLen = unOBJSize * usOBJCount;
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort ||
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
	END_DEBUG_INFO
	return e_GetNetServer->e_SendDataToUDPClient(pszClientIP,
		usClientPort, sttBufferHead, pSendData, nDataLen);
}

bool CNetServer::e_ICloseTCPContext(ULONG ulContextID)
{
	START_DEBUG_INFO
	/// 关闭TCP连接
	return e_GetNetServer->e_CloseTCPContext(ulContextID);
	END_DEBUG_INFO
}