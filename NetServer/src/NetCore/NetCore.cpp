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
#include "NetCore.h"

/// 定义网络服务
CNetIOCP* m_pCNetServer = NULL;

CNetCore::CNetCore()
{
	// 初始化管理对象
	m_pCNetServer = new CNetIOCP();
}

CNetCore::~CNetCore()
{
	// 释放数据对象
	if(NULL != m_pCNetServer)
	{
		delete m_pCNetServer;
		m_pCNetServer = NULL;
	}
}

bool CNetCore::e_IStartServer(T_NetInitStruct* psttNetInitStruct,
	OnRecvDataCallBack pFunRecvData, OnSendDataCallBack pFunSendData, 
	OnConectionCallBack pFunConection, OnDisConectionCallBack pFunDisConection,
	OnPrintLog pFunPrintLog)
{
	START_DEBUG_INFO
	/// 初始化网络模块
	if(true == m_pCNetServer->e_InitNetModel(*psttNetInitStruct, pFunRecvData,
		pFunSendData, pFunConection, pFunDisConection, pFunPrintLog))
	{
		END_DEBUG_INFO
		return m_pCNetServer->e_StartNetService();
	}
	END_DEBUG_INFO
	return false;
}

bool CNetCore::e_IStopdServer()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return m_pCNetServer->e_StopdNetService();
}

bool CNetCore::e_ITCPSendData(ULONG ulContextID, BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
	}
	/// 发送数据
	END_DEBUG_INFO
	return m_pCNetServer->e_SendDataToTCPClient(ulContextID, pSendData, nDataLen);
}

bool CNetCore::e_IUDPSendData(const char* pszClientIP, USHORT usClientPort, BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort || NULL == pSendData || 0 >= nDataLen)
	{
		END_DEBUG_INFO
		return false;
	}
	/// 发送数据
	END_DEBUG_INFO
	return m_pCNetServer->e_SendDataToUDPClient(pszClientIP, usClientPort, pSendData, nDataLen);
}

bool CNetCore::e_ICloseTCPContext(ULONG ulContextID)
{
	START_DEBUG_INFO
	/// 关闭TCP连接
	return m_pCNetServer->e_CloseTCPContext(ulContextID);
	END_DEBUG_INFO
}