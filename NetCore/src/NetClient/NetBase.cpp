////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// FWAY客户端通讯处理
/// @par 相关文件
/// NetBase.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetBase.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetBase.h"

CNetBase::CNetBase()
{
	START_DEBUG_INFO
	/// 初始化套接字
	m_sClientTCPSocket = 0;
	m_sClientUDPSocket = 0;
	/// 初始化服务器IP
	memset(m_szServerIP, 0x00, sizeof(m_szServerIP));
	/// 初始化服务器端口号
	m_usServerTCPPort = 0;
	m_usClientUDPPort = 0;
	/// 初始化线程句柄
	m_HRecvThreadHandle = NULL;
	m_HSendThreadHandle = NULL;
	/// 初始化线程结束事件
	m_HRecvStopEven = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_HSendStopEven = CreateEvent(NULL, FALSE, FALSE, NULL);
	/// 初始化缓冲数组
	m_SendBufferArray.clear();
	/// 初始化回调函数
	m_pfnRecDateCallBack = NULL;
	/// 初始化连接服务器状态
	m_bConnect = false;
	END_DEBUG_INFO
}

CNetBase::~CNetBase()
{
	START_DEBUG_INFO
	/// 停止发送线程
	e_StopReceive();
	/// 停止接收线程
	e_StopSend();
	/// 释放套接字
	e_CloseSocket();
	/// 清空发送的缓冲数组数据
	i_DestroySendBufferArray();
	END_DEBUG_INFO
}

bool CNetBase::e_ConnectServer(const char* pszServerIP, USHORT usServerPort)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pszServerIP || 0 >= usServerPort)
	{
		return bRet;
	}
	/// 设置服务器IP
	strncpy_s(m_szServerIP, pszServerIP, sizeof(m_szServerIP) - 1);
	/// 设置服务端口号
	m_usServerTCPPort = usServerPort;
	END_DEBUG_INFO
	return e_ReconnectServer();
}

bool CNetBase::e_CreatUDPClient(USHORT usClientUDPPort)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据合法性
	if(0 >= usClientUDPPort)
	{
		return bRet;
	}
	/// 赋值UDP端口号
	m_usClientUDPPort = usClientUDPPort;
	/// 初始化UDP套接字
	bRet = i_InitUDPSocket();
	END_DEBUG_INFO
	return bRet;
}

void CNetBase::e_SetReceiveDataCallBack(OnRecvDataCallBack pfnRecDataCallBack)
{
	START_DEBUG_INFO
	/// 设置回调函数
	m_pfnRecDateCallBack = pfnRecDataCallBack;
	END_DEBUG_INFO
}

int CNetBase::e_SendTCPData(char* pszSendBuf, UINT nSendSize)
{
	START_DEBUG_INFO
	int nRet = -1;
	if(NULL == pszSendBuf || 0 >= nSendSize)
	{
		END_DEBUG_INFO
		return nRet;
	}

	int nSendedlen = 0, nSendedSize = nSendSize;
	int nSeletRet = SELECT_STATE_ERROR;
	/// 直到发送完成或者退出循环
	while(0 < nSendSize)
	{
		nSeletRet = e_SelectProcess(m_sClientTCPSocket, SELECT_MODE_WRITE);

		if(SELECT_STATE_READY == nSeletRet)
		{
			nSendedlen = send(m_sClientTCPSocket, pszSendBuf, nSendSize, 0);
			/// 赋值发送数据大小
			nSendSize -= nSendedlen;
			pszSendBuf += nSendedlen;
		}
		else if(SELECT_STATE_ERROR == nSeletRet)
		{
			printf("e_SendTCPData Error = %d\r\n", GetLastError());
			END_DEBUG_INFO
			return nRet;
		}
		else
		{
			/// 超时
			END_DEBUG_INFO
			continue;
		}
	}
	END_DEBUG_INFO
	return nSendedSize - nSendSize;
}

int CNetBase::e_SendUDPData(const char* pszRemoteIP, USHORT usRemotePort,
	char* pszSendBuf, UINT nSendSize)
{
	/// 定义返回值
	int nRet = -1;
	/// 验证数据数据合法性
	if(NULL == pszRemoteIP || 0 >= usRemotePort || NULL != pszSendBuf || 0 >= nSendSize)
	{
		return nRet;
	}

	int nSelectState = e_SelectProcess(m_sClientUDPSocket, SELECT_MODE_WRITE);
	/// 超时返回
	if(SELECT_STATE_TIMEOUT == nSelectState || SELECT_STATE_ABORTED == nSelectState)
	{
		return 0;
	}
	else if(SELECT_STATE_READY == nSelectState)
	{
		/// 开始发送数据
		sockaddr_in ClientAddr;
		int nAddrSize = sizeof(ClientAddr);
		ClientAddr.sin_family = AF_INET;
		ClientAddr.sin_port = htons(usRemotePort);
		ClientAddr.sin_addr.S_un.S_addr = inet_addr(pszRemoteIP);
		nRet = sendto(m_sClientUDPSocket, pszSendBuf, nSendSize, 0,
			(sockaddr *)&ClientAddr, nAddrSize);
		nRet = (0 >= nRet) ? -1 : nRet;
	}
	return nRet;
}

void CNetBase::e_CloseSocket()
{
	/// 释放套接字
	e_CloseTCPSocket();
	e_CloseUDPSocket();
}

void CNetBase::e_CloseTCPSocket()
{
	START_DEBUG_INFO
	/// 如果连接存在关闭连接
	if(0 < m_sClientTCPSocket)
	{
		closesocket(m_sClientTCPSocket);
	}
	/// 套接字ID初始化为0
	m_sClientTCPSocket = 0;
	/// 连接状态设置为未连接
	m_bConnect = false;
	END_DEBUG_INFO
}

void CNetBase::e_CloseUDPSocket()
{
	START_DEBUG_INFO
	/// 如果UDP套接字存在则关闭套接字
	if(0 < m_sClientUDPSocket)
	{
		closesocket(m_sClientUDPSocket);
	}
	/// 套接字ID初始化为0
	m_sClientUDPSocket = 0;
	END_DEBUG_INFO
}

bool CNetBase::e_ReconnectServer()
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 检查连接状态
	if(true == m_bConnect)
	{
		/// 关闭套接字
		e_CloseTCPSocket();
	}
	/// 清空发送缓冲数组数据
	i_DestroySendBufferArray();

	/// 连接服务Ip为空则返回
	if(0 == strlen(m_szServerIP))
	{
		END_DEBUG_INFO
		return bRet;
	}

	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(m_szServerIP);
	serverAddress.sin_port = htons(m_usServerTCPPort);
	/// 如果TCP套接字无效
	if(INVALID_SOCKET == m_sClientTCPSocket)
	{
		if(false == i_InitTCPSocket())
		{
			END_DEBUG_INFO
			return bRet;
		}
	}
	/// 连接服务器并返回连接状态(-1 = 连接不成功)
	bRet = m_bConnect = (-1 != connect(m_sClientTCPSocket, (struct sockaddr *)&serverAddress,
		sizeof(serverAddress)));
	if(false == m_bConnect)
	{
		/// 连接失败，关闭套接字
		e_CloseTCPSocket();
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetBase::e_IsConnect()
{
	START_DEBUG_INFO
	/// 返回连接状态
	END_DEBUG_INFO
	return m_bConnect;
}

int CNetBase::e_StartReceive()
{
	START_DEBUG_INFO
	/// 返回数据
	int nResult = 0;
	/// 申请接收线程
	m_HRecvThreadHandle = CreateThread(NULL, 0, &i_ReceiveThread, this, 0, NULL);
	END_DEBUG_INFO
	return nResult;
}

int CNetBase::e_StartSend()
{
	START_DEBUG_INFO
	int nResult = 0;
	m_HSendThreadHandle = CreateThread(NULL, 0, &i_SendThread, this, 0, NULL);

	END_DEBUG_INFO
	return nResult;
}

void CNetBase::e_StopReceive()
{
	START_DEBUG_INFO
	WaitForMultipleObjects(1, &m_HRecvThreadHandle, TRUE, INFINITE);
	CloseHandle(m_HRecvThreadHandle);
	END_DEBUG_INFO
}

void CNetBase::e_StopSend()
{
	START_DEBUG_INFO
	/// 关闭发送数据线程
	WaitForMultipleObjects(1, &m_HSendThreadHandle, TRUE, INFINITE);
	CloseHandle(m_HSendThreadHandle);
	END_DEBUG_INFO
}

DWORD WINAPI CNetBase::i_ReceiveThread(void* pBaseSocket)
{
	START_DEBUG_INFO
	/// 启动接收线程
	CNetBase* pThis = (CNetBase* )pBaseSocket;
	pThis->e_ReceiveLoop();
	END_DEBUG_INFO
	return 0;
}

DWORD WINAPI CNetBase::i_SendThread(void* pBaseSocket)
{
	START_DEBUG_INFO
	/// 启动发送线程
	CNetBase* pThis = (CNetBase* )pBaseSocket;
	pThis->e_SendLoop();
	END_DEBUG_INFO
	return 0;
}

int CNetBase::e_RecTCPData(char* pszRevBuffer, int nRevLength)
{
	START_DEBUG_INFO
	/// 定义返回值
	int nRet = -1;
	/// 验证数据合法性
	if(NULL == pszRevBuffer || 0 >= nRevLength)
	{
		END_DEBUG_INFO
		return nRet;
	}
	int nSeletRet = SELECT_STATE_ERROR;
	/// 数据长度
	int nRecvedlen = 0, nCurrlen = 0;
	while(0 < nRevLength)
	{
		nSeletRet = e_SelectProcess(m_sClientTCPSocket, SELECT_MODE_READY);
		if(SELECT_STATE_READY == nSeletRet)
		{
			nCurrlen = recv(m_sClientTCPSocket, pszRevBuffer + nRecvedlen, nRevLength, 0);
			if(0 <= nCurrlen)
			{
				/// 赋值接收数据大小
				nRecvedlen += nCurrlen;
				nRevLength -= nCurrlen;
			}
			else
			{
				printf("e_RecTCPData Rev Error = %d\r\n", GetLastError());
				END_DEBUG_INFO
				return nRet;
			}
		}
		else if(SELECT_STATE_ERROR == nSeletRet)
		{
			printf("e_RecTCPData Select Error = %d\r\n", GetLastError());
			END_DEBUG_INFO
			return nRet;
		}
		else
		{
			/// 超时
			END_DEBUG_INFO
			continue;
		}
	}
	END_DEBUG_INFO
	return nRecvedlen;
}

int CNetBase::e_RecUDPData(char* pszRevBuffer, int nRevLength,
	string& strRemoteIP, USHORT& usRemotePort)
{
	START_DEBUG_INFO
	int nRet = -1;
	/// 验证数据合法性
	if(INVALID_SOCKET == m_sClientUDPSocket || NULL == pszRevBuffer || 0 >= nRevLength)
	{
		return nRet;
	}

	int nSelectState = e_SelectProcess(m_sClientUDPSocket, SELECT_MODE_READY);
	/// 超时返回
	if(SELECT_STATE_TIMEOUT == nSelectState || SELECT_STATE_ABORTED == nSelectState)
	{
		nRet = 0;
		return nRet;
	}
	else if(SELECT_STATE_READY == nSelectState)
	{
		sockaddr_in ClientAddr;
		int nAddrSize = sizeof(ClientAddr);
		/// 开始接收数据
		nRet = recvfrom(m_sClientUDPSocket, pszRevBuffer, nRevLength, 
			0, (sockaddr* )&ClientAddr, &nAddrSize);
		nRet = (0 > nRet) ? -1 : nRet;
		/// 如果存在数据
		if(0 < nRet)
		{
			strRemoteIP = inet_ntoa(ClientAddr.sin_addr);
			usRemotePort = ntohs(ClientAddr.sin_port);
		}
	}
	return nRet;
}

int CNetBase::e_SelectProcess(HSOCKET sClientSocket, USHORT usMode, USHORT usProcessTimeOut)
{
	if(INVALID_SOCKET == sClientSocket)
	{
		return SELECT_STATE_ERROR;
	}
	fd_set fdSet;
	fd_set *readSet, *writeSet;
	/// 超时时间测试
	timeval selectTimeout;
	selectTimeout.tv_sec  = usProcessTimeOut;
	selectTimeout.tv_usec = 0;
	FD_ZERO(&fdSet);
	FD_SET(sClientSocket, &fdSet);
	readSet = (usMode & SELECT_MODE_READY) ? &fdSet : NULL;
	writeSet = (usMode & SELECT_MODE_WRITE) ? &fdSet : NULL;

	int nRet = select((int)sClientSocket + 1, readSet, writeSet, NULL, &selectTimeout);

	if(1 == nRet)
	{
		return SELECT_STATE_READY;
	}
	else if(SOCKET_ERROR == nRet)
	{
		if(errno == EINTR)
		{
			return SELECT_STATE_ABORTED;
		}
		return SELECT_STATE_ERROR;
	} 
	return SELECT_STATE_TIMEOUT;
}

void CNetBase::i_DestroySendBufferArray()
{
	START_DEBUG_INFO
	/// 加锁发送数组缓冲
	CAutoLock lckSendBuffer(&m_SendBufferLock);

	/// 清空缓冲数组数据
	T_BufferStruct* pBuffer = NULL;
	for(int i = 0; i < (int)m_SendBufferArray.size(); i++)
	{
		pBuffer = (T_BufferStruct* )m_SendBufferArray.at(i);
		if(NULL != pBuffer)
		{
			if(pBuffer->pBuf)
			{
				delete pBuffer->pBuf;
			}
			delete pBuffer;
			pBuffer = NULL;
		}
	}
	m_SendBufferArray.clear();
	END_DEBUG_INFO
}

bool CNetBase::i_InitTCPSocket()
{
	START_DEBUG_INFO
	/// 关闭TCP套接字
	e_CloseTCPSocket();
	/// 获取新的套接字
	m_sClientTCPSocket = socket(AF_INET,SOCK_STREAM, 0);
	/// 如果获取套接字成功,则处理
	if(INVALID_SOCKET != m_sClientTCPSocket)
	{		
		///设置立即发送数据
		int nRetLay = 1;
		setsockopt(m_sClientTCPSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&nRetLay, sizeof(int));
		/// 缓冲区大小
		int nBufferSize = 1024 * 64;
		/// 设置发送和接收套接字的缓冲大小
		setsockopt(m_sClientTCPSocket, SOL_SOCKET, SO_SNDBUF, (const char *)&nBufferSize, sizeof(int));
		setsockopt(m_sClientTCPSocket, SOL_SOCKET, SO_RCVBUF, (const char *)&nBufferSize, sizeof(int));
	}
	else
	{
		m_sClientTCPSocket = 0;
	}
	END_DEBUG_INFO
	return (0 < m_sClientTCPSocket);
}

bool CNetBase::i_InitUDPSocket()
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 创建套接字
	m_sClientUDPSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if(INVALID_SOCKET == m_sClientUDPSocket)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 设置为socket重用,防止服务器崩溃后端口能够尽快再次使用或共其他的进程使用
	int nOpt = 1;
	int nError = setsockopt(m_sClientUDPSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nOpt, sizeof(int));
	if(SOCKET_ERROR == nError)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// = 设置接收数据缓冲
	/// = 2M Byte 1000Mbps的network在0.01秒内最高可以接收到1.25MB数据
	int nSendBufVal = (int)(1024 * 1024 * 1.25);
	nError = setsockopt(m_sClientUDPSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nSendBufVal, sizeof(nSendBufVal));
	if(nError == SOCKET_ERROR)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// = 设置发送数据缓冲
	nError = setsockopt(m_sClientUDPSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBufVal, sizeof(nSendBufVal));
	if(nError == SOCKET_ERROR)
	{
		END_DEBUG_INFO
		return bRet;
	}

	ULONG nonBlock = 1;
	if (ioctlsocket(m_sClientUDPSocket, FIONBIO, &nonBlock)) 
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 绑定套接口
	SOCKADDR_IN AddrLocal;
	AddrLocal.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	AddrLocal.sin_family = AF_INET;
	AddrLocal.sin_port = htons(m_usClientUDPPort);
	nError = bind(m_sClientUDPSocket, (struct sockaddr *)&AddrLocal, sizeof(SOCKADDR_IN));
	if(SOCKET_ERROR == nError)
	{
		END_DEBUG_INFO
		return bRet;
	}
	return true;
}