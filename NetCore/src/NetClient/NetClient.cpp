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

/// 定义通讯连接对象
CNetTreatment m_CNetTreatment;

CNetClient::CNetClient()
{
}

CNetClient::~CNetClient()
{
}

int CNetClient::e_ConnectionServer(UINT nServerPort, int nClientType, 
	OnRecvDataCallBack pReceiveDataCallBack, UINT nConnectType, char* pszServerIp)
{
	START_DEBUG_INFO

	/// 如果回调函数不存在，则返回
	if(NULL == pReceiveDataCallBack)
	{
		END_DEBUG_INFO
		return 0;
	}

	/// 如果配置文件中配置了IP连接指定配置的IP
	m_CNetTreatment.e_ConnectServer(szServiceIp, nServerPort, nClientType, 2);		
	/// 设置回调函数
	m_CNetTreatment.e_SetReceiveDataCallBack(pReceiveDataCallBack);
	/// 启动发送线程
	m_CNetTreatment.e_StartSend();
	/// 启动接收线程
	m_CNetTreatment.e_StartReceive();

	END_DEBUG_INFO
	return 1;
}

int CNetClient::e_DisconnectServer()
{
	START_DEBUG_INFO
	/// 断开接收线程
	m_CNetTreatment.e_StopReceive();
	/// 断开发送线程
	m_CNetTreatment.e_StopSend();
	END_DEBUG_INFO
	return 1;
}

int CNetClient::e_SendDataByStruct(UINT nSSDType, UINT nStructSize, int nCount, void* pData)
{
	START_DEBUG_INFO
	/// 声明发送接收临时变量
	T_RSStructData* pRecvClient = NULL;
	/// 获取结构体数据大小
	long lAllStructlen = 0;
	/// 获取数据操作类型
	UINT nJobDataType = JDT_StructData;
	if(nCount > 0)
	{
		lAllStructlen = nStructSize * nCount;
		if(nCount > 1)
		{
			nJobDataType = JDT_ArrStructData;
		}
	}
	/// 获取数据包大小
	long lAllSize = 0;
	if(nCount > 0)
	{
		/// 如果有数据内容
		lAllSize = sizeof(T_RSStructData) + lAllStructlen - 1;
	}
	else
	{
		/// 如果没有数据内容
		lAllSize = sizeof(T_RSStructData);
	}
	/// 获取缓冲数据
	pRecvClient = (T_RSStructData*)new char[lAllSize];
	/// 赋值数据包
	pRecvClient->nClientType = m_CNetTreatment.m_nClientType;
	pRecvClient->nJobDataType = nJobDataType;
	pRecvClient->nSRStructType = nSSDType;
	pRecvClient->nDataSize = nStructSize;
	pRecvClient->nDataCount = nCount;
	pRecvClient->nErrorCode = ECT_None;
	/// 赋值数据
	if(0 < lAllStructlen)
	{
		memcpy(pRecvClient->szData, pData, lAllStructlen);
	}
	/// 发送数据
	m_CNetTreatment.m_tSendTime = time(NULL);
	if(false == m_CNetTreatment.e_SendStructData(nJobDataType, nSSDType, lAllSize, pRecvClient))
	{
		m_CNetTreatment.m_tSendTime = 0;
		END_DEBUG_INFO
		return 0;
	}
	END_DEBUG_INFO
	return 1;
}