////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络通讯业务处理
/// @par 相关文件
/// NetTreatment.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetTreatMent.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetTreatMent.h"

CNetTreatment::CNetTreatment()
{
	/// 初始化接收数据缓冲
	memset(m_szTCPRecvBuffer, 0x00, MAX_NET_BUFFER_SIZE);
	memset(m_szUDPRecvBuffer, 0x00, MAX_NET_BUFFER_SIZE);
}

CNetTreatment::~CNetTreatment()
{
}

void CNetTreatment::e_StopTCPProcess()
{
	/// 停止TCP线程
	e_StopTCPReceive();
	e_StopTCPSend();
	/// 释放套接字
	e_CloseTCPSocket();
}

void CNetTreatment::e_StopUDPProcess()
{
	/// 停止UDP线程
	e_StopUDPReceive();
	e_StopUDPSend();
	/// 释放套接字
	e_CloseUDPSocket();
}

void CNetTreatment::e_SendTCPLoop()
{
	START_DEBUG_INFO
	/// 定义事件返回值
	int nWaitRet = 0;
	T_TCPSendBuffer* psttSendBuffer = NULL;
	/// 处理TCP发送数据
	while(true)
	{
		nWaitRet = WaitForSingleObject(m_HSendTCPStopEven, 5);
		/// 停止线程信号，退出线程
		if(WAIT_OBJECT_0 == nWaitRet)
		{
			break;
		}
		/// 在缓冲中查找要发送的数据
		psttSendBuffer = NULL;
		m_SendTCPBufferLock.e_Lock();
		if(0 < (int)m_SendTCPBufferArray.size())
		{			
			psttSendBuffer = (T_TCPSendBuffer* )*m_SendTCPBufferArray.begin();
			if(NULL != psttSendBuffer)
			{
				m_SendTCPBufferArray.erase(m_SendTCPBufferArray.begin());
			}
		}
		m_SendTCPBufferLock.e_Unlock();

		/// 数据合法发送数据
		if(NULL != psttSendBuffer)
		{			
			/// 数据内容是否合法
			if(NULL != psttSendBuffer->pszBuffer)
			{
				/// 发送数据
				e_SendTCPData(psttSendBuffer->pszBuffer, psttSendBuffer->nBufferSize);
				/// 释放数据内容
				delete psttSendBuffer->pszBuffer;
				psttSendBuffer->pszBuffer = NULL;
			}
			delete psttSendBuffer;
			psttSendBuffer = NULL;
		}
	}
	END_DEBUG_INFO;
}

void CNetTreatment::e_ReceiveTCPLoop()
{
	START_DEBUG_INFO
	/// 定义事件返回值
	int nWaitRet = 0;
	/// 处理TCP接收数据
	while(true)
	{
		nWaitRet = WaitForSingleObject(m_HRecvTCPStopEven, 5);
		/// 停止线程信号，退出线程
		if(WAIT_OBJECT_0 == nWaitRet)
		{
			break;
		}
		/// 处理TCP接收数据包
		i_ProcessTCPRecData();
	}
	END_DEBUG_INFO
}

void CNetTreatment::e_SendUDPLoop()
{
	START_DEBUG_INFO
	/// 定义事件返回值
	int nWaitRet = 0;
	T_UDPSendBuffer* psttSendBuffer = NULL;
	/// 处理TCP发送数据
	while(true)
	{
		nWaitRet = WaitForSingleObject(m_HSendUDPStopEven, 5);
		/// 停止线程信号，退出线程
		if(WAIT_OBJECT_0 == nWaitRet)
		{
			break;
		}
		/// 在缓冲中查找要发送的数据
		psttSendBuffer = NULL;
		m_SendUDPBufferLock.e_Lock();
		if(0 < (int)m_SendUDPBufferArray.size())
		{			
			psttSendBuffer = (T_UDPSendBuffer* )*m_SendUDPBufferArray.begin();
			if(NULL != psttSendBuffer)
			{
				m_SendUDPBufferArray.erase(m_SendUDPBufferArray.begin());
			}
		}
		m_SendUDPBufferLock.e_Unlock();

		/// 数据合法发送数据
		if(NULL != psttSendBuffer)
		{			
			/// 数据内容是否合法
			if(NULL != psttSendBuffer->pszBuffer)
			{
				/// 发送数据
				e_SendUDPData(psttSendBuffer->szRemoteIP, psttSendBuffer->szRemotePort, 
					psttSendBuffer->pszBuffer, psttSendBuffer->nBufferSize);
				/// 释放数据内容
				delete psttSendBuffer->pszBuffer;
				psttSendBuffer->pszBuffer = NULL;
			}
			delete psttSendBuffer;
			psttSendBuffer = NULL;
		}
	}
	END_DEBUG_INFO
}

void CNetTreatment::e_ReceiveUDPLoop()
{
	/// 定义事件返回值
	int nWaitRet = 0;
	/// 处理UDP接收数据
	while(true)
	{
		nWaitRet = WaitForSingleObject(m_HRecvUDPStopEven, 5);
		/// 停止线程信号，退出线程
		if(WAIT_OBJECT_0 == nWaitRet)
		{
			break;
		}
		/// 处理UDP接收数据包
		i_ProcessUDPRecData();
	}
}

void CNetTreatment::i_ProcessTCPRecData()
{
	START_DEBUG_INFO
	/// 检查TCP是否在线
	if(false == e_IsTCPConnect())
	{
		/// 断线重连
		e_ReconnectServer();
		END_DEBUG_INFO
		return;
	}

	/// 要读取的数据长度
	int nDataLen = 0;
	/// 已读取的数据长度
	int nReadCount = 0;
	/// 获取数据长度
	nReadCount = e_RecTCPData((char* )&nDataLen, sizeof(UINT));
	/// 没有读取到数据，或者有效长度错误
	if(-1 == nReadCount || nDataLen < sizeof(T_TCPBufferHead) - sizeof(INT) ||
		nDataLen > MAX_PAL_BUFFER_SIZE)
	{
		printf("NetClient Read TCP Data Len Error, ReadCount = %d, DataLen = %d .\r\n",
			nReadCount, nDataLen);
		/// 断开连接
		e_CloseTCPSocket();
		END_DEBUG_INFO
		return;
	}

	/// 正确读取到了数据长度
	if(0 < nDataLen)
	{
		/// 重置接收数据变量
		memset(m_szTCPRecvBuffer, 0x00, sizeof(MAX_NET_BUFFER_SIZE));
		/// 接收数据长度
		memcpy(m_szTCPRecvBuffer, &nDataLen, sizeof(INT));
		/// 已经接收了sizeof(nDataLen)大小，接收数据内容
		char* pszRecvBuffer = m_szTCPRecvBuffer + sizeof(INT);
		/// 接收数据内容
		nReadCount = e_RecTCPData(pszRecvBuffer, nDataLen);
		/// 如果读取的数据不正确，则关闭连接
		if(nReadCount != nDataLen)
		{
			printf("NetClient Read TCP Data Content Error, ReadCount = %d, DataLen = %d .\r\n",
				nReadCount, nDataLen);
			/// 断开套接字连接
			e_CloseTCPSocket();
			END_DEBUG_INFO
			return;
		}
		
		/// = 收到完整的数据包
		/// = 获取数据头
		T_TCPBufferHead sttBufferHead;
		memset(&sttBufferHead, 0x00, sizeof(T_TCPBufferHead));
		memcpy(&sttBufferHead, m_szTCPRecvBuffer, sizeof(T_TCPBufferHead));
		/// = 提取数据部分
		int nDataSize = nDataLen - sizeof(T_TCPBufferHead);
		memmove(m_szTCPRecvBuffer, m_szTCPRecvBuffer + sizeof(T_TCPBufferHead), nDataSize);
		memset(m_szTCPRecvBuffer + nDataLen, 0x00, sizeof(T_TCPBufferHead));
		/// 处理收到完整数据包
		i_ProcessReceivePackage(NTT_TCPData, m_sttInitNetClient.szTCPServerIP,
			m_sttInitNetClient.usTCPServerPort, sttBufferHead.sDataType,
			sttBufferHead.nOBJType, sttBufferHead.sOBJCount, sttBufferHead.sSNum, 
			sttBufferHead.sENum, nDataSize, m_szTCPRecvBuffer);
	}
	END_DEBUG_INFO
}

void CNetTreatment::i_ProcessUDPRecData()
{
	START_DEBUG_INFO
	if(false == e_IsUDPCreated())
	{
		END_DEBUG_INFO
		return;
	}
	/// 远程IP和端口定义
	string strRemoteIP = "";
	USHORT usRemotePort = 0;
	/// 已读取的数据长度
	int nReadCount = 0;
	/// 初始化接收缓存
	memset(m_szUDPRecvBuffer, 0x00, sizeof(MAX_NET_BUFFER_SIZE));
	/// 获取数据长度
	nReadCount = e_RecUDPData((char* )&m_szUDPRecvBuffer, MAX_NET_BUFFER_SIZE,
		strRemoteIP, usRemotePort);
	/// 验证数据合法性
	if(0 >= nReadCount || nReadCount < sizeof(T_UDPBufferHead) ||
		nReadCount > MAX_NET_BUFFER_SIZE)
	{
		printf("NetClient Read UDP Data Content Error, ReadCount = %d.\r\n",
			nReadCount);
		END_DEBUG_INFO
		return;
	}

	/// = 收到完整的数据包
	/// = 获取数据头
	T_UDPBufferHead sttBufferHead;
	memset(&sttBufferHead, 0x00, sizeof(T_UDPBufferHead));
	memcpy(&sttBufferHead, m_szUDPRecvBuffer, sizeof(T_UDPBufferHead));
	/// = 提取数据部分
	int nDataSize = nReadCount - sizeof(T_UDPBufferHead);
	memmove(m_szUDPRecvBuffer, m_szUDPRecvBuffer + sizeof(T_UDPBufferHead), nDataSize);
	memset(m_szUDPRecvBuffer + nReadCount, 0x00, sizeof(T_UDPBufferHead));
	/// 处理收到完整数据包
	i_ProcessReceivePackage(NTT_UDPData, strRemoteIP.c_str(), usRemotePort,
		sttBufferHead.sDataType, sttBufferHead.nOBJType, sttBufferHead.sOBJCount,
		sttBufferHead.sSNum, sttBufferHead.sENum, nDataSize, m_szUDPRecvBuffer);
	END_DEBUG_INFO
}

bool CNetTreatment::i_ProcessReceivePackage(USHORT usNetType, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort 
		|| 0 >= nDatalen || NULL == pData)
	{
		printf("i_ProcessReceivePackage Get The Data Error.\r\n");
		END_DEBUG_INFO
		return bRet;
	}

	/// 如果存在回调函数
	if(NULL != m_pfnRecDataCallBack)
	{
		/// 传递数据给回调函数
		m_pfnRecDataCallBack(usNetType, pszClientIP, usClientPort, sDataType,
			nOBJType, sOBJCount, sSNum, sENum, nDatalen, pData);
		bRet = true;
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetTreatment::e_CreatSendTCPData(T_TCPBufferHead& sttBufferHead,
	BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据合法性, 如果未连接服务器不发送数据
	if(NULL != pSendData || 0 >= nDataLen || nDataLen > \
		MAX_NET_BUFFER_SIZE - sizeof(T_TCPBufferHead) || 
		false == e_IsTCPConnect())
	{
		END_DEBUG_INFO
		return bRet;
	}
	/// 要发送的数据长度
	int nSendLen = sizeof(T_TCPBufferHead) + nDataLen;
	/// 申请TCP发送结构
	T_TCPSendBuffer* psttSendBuffer = new T_TCPSendBuffer();
	if(NULL == psttSendBuffer)
	{
		END_DEBUG_INFO
		return bRet;
	}
	psttSendBuffer->nBufferSize = nSendLen;
	psttSendBuffer->pszBuffer = new char[nSendLen + EXTRA_BUFFER_SIZE];
	if(NULL == psttSendBuffer->pszBuffer)
	{
		delete psttSendBuffer;
		psttSendBuffer = NULL;
		END_DEBUG_INFO
		return bRet;
	}

	/// 拷贝数据
	memcpy(psttSendBuffer->pszBuffer, &sttBufferHead, sizeof(T_TCPBufferHead));
	memcpy(psttSendBuffer->pszBuffer + sizeof(T_TCPBufferHead), pSendData, nDataLen);
	/// 加入发送数据到列表
	CAutoLock AutoLock(&m_SendTCPBufferLock);
	m_SendTCPBufferArray.push_back(psttSendBuffer);
	bRet = true;
	END_DEBUG_INFO
	return bRet;
}

bool CNetTreatment::e_CreatSendUDPData(const char* pszRemoteIP, USHORT usRemotePort,
	T_UDPBufferHead& sttBufferHead, BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据合法性, 如果UDP客户端未创建，创建客户端
	if(NULL != pszRemoteIP || 0 >= usRemotePort || 
		NULL != pSendData || 0 >= nDataLen || 
		false == e_IsUDPCreated())
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 要发送的数据长度
	int nSendLen = sizeof(T_UDPBufferHead) + nDataLen;
	/// 申请TCP发送结构
	T_UDPSendBuffer* psttSendBuffer = new T_UDPSendBuffer();
	if(NULL == psttSendBuffer)
	{
		END_DEBUG_INFO
		return bRet;
	}
	strncpy_s(psttSendBuffer->szRemoteIP, pszRemoteIP, sizeof(psttSendBuffer->szRemoteIP) - 1);
	psttSendBuffer->szRemotePort = usRemotePort;
	psttSendBuffer->nBufferSize = nSendLen;
	psttSendBuffer->pszBuffer = new char[nSendLen + EXTRA_BUFFER_SIZE];
	if(NULL == psttSendBuffer->pszBuffer)
	{
		delete psttSendBuffer;
		psttSendBuffer = NULL;
		END_DEBUG_INFO
		return bRet;
	}

	/// 拷贝数据
	memcpy(psttSendBuffer->pszBuffer, &sttBufferHead, sizeof(T_UDPBufferHead));
	memcpy(psttSendBuffer->pszBuffer + sizeof(T_UDPBufferHead), pSendData, nDataLen);
	/// 加入发送数据到列表
	CAutoLock AutoLock(&m_SendUDPBufferLock);
	m_SendUDPBufferArray.push_back(psttSendBuffer);
	bRet = true;
	END_DEBUG_INFO
	return bRet;
}