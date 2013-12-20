////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// <幸运农场客户端通讯处理>
/// @par 相关文件
/// NetSocket.h 
/// @par 功能详细描述
/// <幸运农场客户端通讯处理>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetSocket.cpp
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/05/25
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "NetSocket.h"

namespace FWAYNET
{
	/// 声明静态调用的全局类变量
	CNetBase* m_pCCNetSocket = NULL;
	CNetBase::CNetBase()
	{
		START_DEBUG_INFO
#ifdef WIN32
		/// 定义Windows Sockets 初始化数据
		WSADATA wsaData;
		/// 初始化套接字Sockets获取初始化状态
		m_nWSAInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		/// 初始化接收线程ID
		m_DRecvThreadId = NULL;
		/// 初始化发送线程ID
		m_DSendThreadId = NULL;
#endif
		/// 初始化套接字ID
		m_nSocketId = 0;
		/// 初始化停止接收标志
		m_bStopRecvTag = false;
		/// 初始化发送接收标志
		m_bStopSendTag = false;
		/// 初始化服务器IP
		memset(m_szServerIp, 0x00, sizeof(m_szServerIp));
		/// 初始化服务器端口号
		m_nServerPort = 0;
		/// 初始化连接服务器状态
		m_bConnect = false;
		/// 初始化回调函数
		m_pReceiveStructCallBack = NULL;
		/// 初始化缓冲数组
		m_SendBufferArray.clear();
		/// 初始化客户端类型
		m_nClientType = CT_NoneError;
		/// 赋值全局类变量
		m_pCCNetSocket = this;
		/// 初始化连接类型
		m_nConnectType = 0;
		END_DEBUG_INFO
	}

	CNetBase::~CNetBase()
	{
		START_DEBUG_INFO
	#ifdef WIN32
		/// 套接字状态为错误，则断开套接字
		if(NO_ERROR == m_nWSAInitResult)
		{
			WSACleanup();
		}
	#endif
		/// 停止套接字连接，停止发送和接收线程
		e_StopReceive();
		e_StopSend();
		/// 清空发送的缓冲数组数据
		e_DestroySendBufferArray();
		END_DEBUG_INFO
	}

	bool CNetBase::e_IniSocket()
	{
		START_DEBUG_INFO
		/// 关闭套接字
		e_CloseSocket();
		/// 获取新的套接字
		m_nSocketId = socket(AF_INET,SOCK_STREAM, 0);
		/// 如果获取套接字成功,则处理
		if(-1 != m_nSocketId)
		{
			/// 套接字相关设置操作在此处理
			e_SetSocket(m_nSocketId);
		}
		else
		{
			m_nSocketId = 0;
		}
		END_DEBUG_INFO
		return (0 < m_nSocketId);
	}

	void CNetBase::e_SetSocket(int nSocketId)
	{
		START_DEBUG_INFO
		/// 如果套接字不存在直接返回
		if(0 >= nSocketId)
		{
			END_DEBUG_INFO
			return;
		}

		BOOL bSopt = TRUE;
		int ret = 0;

		int nBufferSize = 1024 * 64;
		/// 立即发送数据
		setsockopt(nSocketId, IPPROTO_TCP, TCP_NODELAY, (char *)&bSopt, sizeof(BOOL));
#ifdef WIN32
		/// 设置阻塞超时参数[60*1000] = 60s
		int timeout = 60000;
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
		/// 设置发送和接收套接字的缓冲大小
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_SNDBUF, (const char *)&nBufferSize, sizeof(int));
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_RCVBUF, (const char *)&nBufferSize, sizeof(int));
#else
		/// 设置阻塞超时参数60s
		struct timeval timeout = {60, 0};
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
		nBufferSize = 1024 * 64;
		/// 设置发送和接收套接字的缓冲大小
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_SNDBUF, &nBufferSize, sizeof(int));
		ret = setsockopt(nSocketId, SOL_SOCKET, SO_RCVBUF, &nBufferSize, sizeof(int));
#endif
		END_DEBUG_INFO
	}

	void CNetBase::e_CloseSocket()
	{
		START_DEBUG_INFO
		/// 如果连接存在关闭连接
#ifdef WIN32
		if(0 < m_nSocketId)
		{
			closesocket(m_nSocketId);
		}
#else
		if(0 < m_nSocketId)
		{
			close(m_nSocketId);
		}
#endif
		/// 套接字ID初始化为0
		m_nSocketId = 0;
		/// 连接状态设置为未连接
		m_bConnect = false;
		END_DEBUG_INFO
	}

	bool CNetBase::e_ConnectServer(char* pszServerIp, int nServerPort, int nClientType, int nConnectType)
	{
		START_DEBUG_INFO
		/// 设置服务器IP
		snprintf(m_szServerIp, sizeof(m_szServerIp), "%s", pszServerIp);
		/// 设置服务端口号
		m_nServerPort = nServerPort;
		/// 设置客户端连接类型
		m_nClientType = nClientType;
		/// 设置连接类型
		m_nConnectType = nConnectType;
		/// 连接服务，返回连接状态
		END_DEBUG_INFO
		return e_ReconnectServer();
	}

	bool CNetBase::e_ReconnectServer()
	{
		START_DEBUG_INFO
		/// 端口号未设置返回
		if(0 == m_nServerPort)
		{
			END_DEBUG_INFO
			return false;
		}
		/// 检查连接状态
		if(true == m_bConnect)
		{
			/// 关闭套接字
			e_CloseSocket();
		}
		/// 清空发送缓冲数组数据
		e_DestroySendBufferArray();

		/// 扫描连接网关服务
		if(0 == m_nConnectType)
		{
			/// 扫描连接服务器
			if(false == e_ScanConnectServer())
			{
				/// 扫描失败返回
				printf("扫描连接服务器失败\r\n");
				END_DEBUG_INFO
				return false;
			}
		}

		/// 连接服务Ip为空则返回
		if(0 == strlen(m_szServerIp))
		{
			return false;
		}

		struct sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(m_szServerIp);
		serverAddress.sin_port = htons(m_nServerPort);
		if(0 == m_nSocketId)
		{
			if(false == e_IniSocket())
			{
				END_DEBUG_INFO
				return false;
			}
		}
		/// 睡眠20毫秒
#ifdef WIN32
		Sleep(20);
#else
		usleep(20000);
#endif
		/// 连接服务器并返回连接状态(-1 = 连接不成功)
		m_bConnect = (connect(m_nSocketId, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != -1);
		if(false == m_bConnect)
		{
			static int ntick = 0;
			ntick++;
			if(10 < ntick || 2 != m_nConnectType)
			{
				ntick = 0;
				printf("连接服务器IP = %s Port = %d 失败\r\n", m_szServerIp, m_nServerPort);
			}
			/// 首次连接指定Ip失败，自动赋值为扫描连接服务
			if(1 == m_nConnectType)
			{
				m_nConnectType = 0;
			}
			/// 连接失败，关闭套接字
			e_CloseSocket();
		}
		else
		{
			if(true == e_InitClient())
			{
				printf("连接服务器IP = %s Port = %d 成功\r\n", m_szServerIp, m_nServerPort);
			}
			else
			{
				/// 首次连接指定Ip失败，自动赋值为扫描连接服务
				if(1 == m_nConnectType)
				{
					m_nConnectType = 0;
				}
				/// 设置连接状态
				m_bConnect = false;
				/// 连接失败，关闭套接字
				e_CloseSocket();
			}
		}
		END_DEBUG_INFO
		return m_bConnect;
	}

	bool CNetBase::e_ScanConnectServer()
	{
		START_DEBUG_INFO
		/// 获取网段地址
		char szLocalIp[16];
		memset(szLocalIp, 0x00, sizeof(szLocalIp));
		/// 获取本机Ip
		e_GetHostIP(szLocalIp);
		/// 本机Ip失败
		if(0 == strlen(szLocalIp))
		{
			END_DEBUG_INFO
			return false;
		}

		/// 获取Ip地址头
		char* ptr = NULL;
		ptr = strrchr(szLocalIp, '.');
		if(NULL == ptr)
		{
			/// Ip地址错误，返回假
			END_DEBUG_INFO
			return false;
		}

		/// 声明获取Ip头
		char szBeginPartIp[16];

		/// 声明扫描的Ip地址段
		memset(szBeginPartIp, 0x00, sizeof(szBeginPartIp));
		/// 赋值IP地址头
		memcpy(szBeginPartIp, szLocalIp, ptr - szLocalIp + 1);

		/// 定义获取扫描到的Ip
		char szServerIp[16];

		/// 获取服务器Ip扫描端
		int nScanBegIp = NETSERVERSTARIP;
		int nScanEndIp = NETSERVERSTOPIP;

		/// 循环Ip连接
		for(int i = nScanBegIp; i <= nScanEndIp; i++)
		{
			memset(szServerIp, 0x00, sizeof(szServerIp));
			/// 赋值扫描到的Ip
			snprintf(szServerIp, sizeof(szServerIp), "%s%d", szBeginPartIp, i);

			if(true == e_ConnectTest(szServerIp, 100000))
			{
				/// 赋值服务器Ip
				snprintf(m_szServerIp, sizeof(m_szServerIp), "%s", szServerIp);
				return true;
			}
		}
		/// 返回连接状态
		return false;
	}

	bool CNetBase::e_ConnectTest(char * pServerIp, int nConnectTimeOut)
	{
		START_DEBUG_INFO
		/// 定义返回信息
		bool	bReturn = false;
		/// 定义测试连接的套接字Id
		UINT	nTestSocketID = 0;
		/// 定义获取的错误编码
		int		nError = -1;

		/// 设置连接结构体
		struct sockaddr_in saddr; 
		saddr.sin_family = AF_INET; 
		saddr.sin_addr.s_addr = inet_addr(pServerIp);
		saddr.sin_port = htons(m_nServerPort); 
		/// 获取连接套接字
		nTestSocketID = socket(AF_INET, SOCK_STREAM, 0);
		/// 设置套接字错误
		if(0 >= nTestSocketID)
		{
			END_DEBUG_INFO
			return false; 
		}
		//设置非阻塞方式连接
#ifdef WIN32
		/// 设置连接方式
		unsigned long lSetType = 1;
		nError = ioctlsocket(nTestSocketID, FIONBIO, (unsigned long* )&lSetType);
		/// 设置阻塞连接失败返回
		if(SOCKET_ERROR == nError)
		{
			END_DEBUG_INFO
			return false;
		}
#else
		int flags = fcntl(nTestSocketID, F_GETFL, 0);
		fcntl(nTestSocketID, F_SETFL, flags|O_NONBLOCK); 
#endif
		/// 测试扫描连接服务器
		nError = connect(nTestSocketID, (struct sockaddr*)&saddr, sizeof(saddr));
		if(-1 == nError)
		{
#ifndef WIN32
			/// Linux下如果错误为EINPROGRESS 可能没有错误，只是连接还未完成
			if(errno == EINPROGRESS)
#endif
			{
				/// 设置处理连接超时的时间
				struct timeval tv; 
				fd_set writefds; 
				tv.tv_sec = 0;
				tv.tv_usec = nConnectTimeOut; 
				FD_ZERO(&writefds); 
				FD_SET(nTestSocketID, &writefds); 
				if(0 < select(nTestSocketID + 1, NULL, &writefds, NULL, &tv))
				{
#ifndef WIN32
					/// 下面的一句一定要，主要针对Linux防火墙
					int nlen = sizeof(int);
					getsockopt(nTestSocketID, SOL_SOCKET, SO_ERROR, &nError, (socklen_t *)&nlen);
					if(0 == nError)
					{
						/// 连接成功
						bReturn = true;
					}
					else
					{
						/// 连接失败
						bReturn = false;
					}
#else
					/// 设置成功返回真
					bReturn = true;
#endif
				}
				else
				{
					bReturn = false;
				}
			}
#ifndef WIN32
			else
			{
				/// 如果不是EINPROGRESS 错误，返回假
				bReturn = false; 
			}
#endif
		} 
		else
		{
			/// 连接成功
			bReturn = true;
		}
		/// 关闭测试连接套接字
#ifdef WIN32
		closesocket(nTestSocketID); 
#else
		close(nTestSocketID); 
#endif
		return bReturn; 
	}

	bool CNetBase::e_IsConnect()
	{
		START_DEBUG_INFO
		/// 返回连接状态
		END_DEBUG_INFO
		return m_bConnect;
	}

	bool CNetBase::e_InitClient()
	{
		START_DEBUG_INFO
		/// 定义返回变量
		bool bReturn = false;
		/// 定义客户端数据发送给网关服务
		T_ClientPack sttClientPack;
		/// 初始化结构体数据
		memset(&sttClientPack, 0x00, sizeof(T_ClientPack));
		/// 赋值客户端类型
		sttClientPack.nClientType = m_nClientType;
		/// 赋值套接字ID
		sttClientPack.nClientID = 0;
		/// 赋值客户端IP
		e_GetHostIP(sttClientPack.szClientIp);
		/// 声明发送接收临时变量
		T_RSStructData* pRecvClient = NULL;

		/// 获取结构体大小
		UINT nStructSize = sizeof(T_ClientPack);
		/// 获取数据包大小
		UINT nAllSize = sizeof(T_RSStructData) + nStructSize - 1;
		/// 获取缓冲数据
		pRecvClient = (T_RSStructData*)new char[nAllSize];
		/// 赋值数据包
		pRecvClient->nClientType = m_nClientType;
		pRecvClient->nJobDataType = JDT_StructData;
		pRecvClient->nSRStructType = RST_ClientPack;
		pRecvClient->nDataSize = nStructSize;
		pRecvClient->nDataCount = 1;
		pRecvClient->nErrorCode = ECT_None;
		memcpy(pRecvClient->szData, &sttClientPack, nStructSize);
		/// 发送数据
		bReturn = e_SendStructData(JDT_StructData, RST_ClientPack, nAllSize, pRecvClient);
		if(NULL != pRecvClient)
		{
			delete pRecvClient;
		}
		pRecvClient = NULL;

		END_DEBUG_INFO
		return bReturn;
	}

	int CNetBase::e_StartReceive()
	{
		START_DEBUG_INFO
		/// 返回数据
		int nResult = 0;
		/// 申请接收线程
#ifdef WIN32
		m_HRecvThreadHandle = CreateThread(NULL, 0, &i_ReceiveThread, this, 0, &m_DRecvThreadId);
#else
		pthread_mutex_init(&m_pCCNetSocket->m_RecvThreadMutex, 0);
		nResult = pthread_create(&m_pCCNetSocket->m_RecvThreadTaskId, 0, i_ReceiveThread, this);
#endif
		END_DEBUG_INFO
		return nResult;
	}

	int CNetBase::e_StartSend()
	{
		int nResult = 0;
#ifdef WIN32
		m_HSendThreadHandle = CreateThread(NULL, 0, &i_SendThread, this, 0, &m_DSendThreadId);
#else
		pthread_mutex_init(&m_pCCNetSocket->m_SendThreadMutex, 0);
		nResult = pthread_create(&m_pCCNetSocket->m_SendThreadTaskId, 0, i_SendThread, this);
#endif
		END_DEBUG_INFO
		return nResult;
	}

	void CNetBase::e_StopReceive()
	{
		START_DEBUG_INFO
		/// 赋值结束接收标志
		m_bStopRecvTag = true;
		/// 关闭套接字连接
		e_CloseSocket();

		/// 关闭接收数据线程
#ifdef WIN32
		WaitForMultipleObjects(1, &m_HRecvThreadHandle, TRUE, INFINITE);
		CloseHandle(m_HRecvThreadHandle);
#else
		int	nTemp = 0;
		usleep(200);
		while(true)
		{
			if(EBUSY == pthread_mutex_trylock(&m_pCCNetSocket->m_RecvThreadMutex))
			{
				nTemp++;
				if(10 == nTemp)
				{
					pthread_cancel(m_RecvThreadTaskId);
					break;
				}
				usleep(200);
			}
			else
			{
				break;
			}
		}
		pthread_join(m_RecvThreadTaskId, 0);
		pthread_mutex_destroy(&m_pCCNetSocket->m_RecvThreadMutex);
#endif
		END_DEBUG_INFO
	}

	void CNetBase::e_StopSend()
	{
		START_DEBUG_INFO
		/// 赋值结束发送标志
		m_bStopSendTag = true;
		/// 关闭套接字连接
		e_CloseSocket();
		/// 关闭发送数据线程
#ifdef WIN32
		WaitForMultipleObjects(1, &m_HSendThreadHandle, TRUE, INFINITE);
		CloseHandle(m_HSendThreadHandle);
#else
		int	nTemp = 0;
		usleep(200);
		while(true)
		{
			if(EBUSY == pthread_mutex_trylock(&m_pCCNetSocket->m_SendThreadMutex))
			{
				nTemp++;
				if(10 == nTemp)
				{
					pthread_cancel(m_SendThreadTaskId);
					break;
				}
				usleep(200);
			}
			else
			{
				break;
			}
		}
		pthread_join(m_SendThreadTaskId, 0);
		pthread_mutex_destroy(&m_pCCNetSocket->m_SendThreadMutex);
#endif
		END_DEBUG_INFO
	}

#ifdef WIN32
	DWORD WINAPI CNetBase::i_ReceiveThread(void* pBaseSocket)
	{
#else
	void *CNetBase::i_ReceiveThread(void* pBaseSocket)
	{
#endif
		START_DEBUG_INFO
		/// 启动接收线程
		CNetBase* pSocket = (CNetBase* )pBaseSocket;
#ifndef WIN32
		pthread_mutex_lock(&m_pCCNetSocket->m_RecvThreadMutex);
#endif
		pSocket->e_ReceiveLoop();
#ifndef WIN32
		pthread_mutex_unlock(&m_pCCNetSocket->m_RecvThreadMutex);
#endif
		END_DEBUG_INFO
		return 0;
	}

#ifdef WIN32
	DWORD WINAPI CNetBase::i_SendThread(void* pBaseSocket)
	{
#else
	void *CNetBase::i_SendThread(void* pBaseSocket)
	{
#endif
		START_DEBUG_INFO
		/// 启动发送线程
		CNetBase* pSocket = (CNetBase* )pBaseSocket;
#ifndef WIN32
		pthread_mutex_lock(&m_pCCNetSocket->m_SendThreadMutex);
#endif
		pSocket->e_SendLoop();
#ifndef WIN32
		pthread_mutex_unlock(&m_pCCNetSocket->m_SendThreadMutex);
#endif
		END_DEBUG_INFO
		return 0;
	}

	bool CNetBase::e_SendStructData(int nJobType, int nSST, long lStructLen, void* pStructData)
	{
		START_DEBUG_INFO
		/// 检查连接
		if(false == m_bConnect)
		{
			printf("服务器断开连接\n");
			END_DEBUG_INFO
			return false;
		}

		/// 发送数据加锁
		CLightAutoLock lckSendBufferLock(&m_SendBufferLock);

		if(MAXSENDBUFARR < (int)m_SendBufferArray.size())
		{
			printf("发送缓冲区数据太多\n");
			END_DEBUG_INFO
			return false;
		}

		/// 请求一个缓冲数据
		T_BufferStruct* pBuffer = new T_BufferStruct();
		if(NULL == pBuffer)
		{
			END_DEBUG_INFO
			return false;
		}
		/// 初始化缓冲数据
		memset(pBuffer, 0x00, sizeof(T_BufferStruct));

		/// 缓冲数据设置
		pBuffer->nJobType = nJobType;
		pBuffer->nSST = nSST;
		pBuffer->nSize = lStructLen;
		pBuffer->pBuf = new char[lStructLen + 1024];
		if(NULL == pBuffer->pBuf)
		{
			delete pBuffer;
			END_DEBUG_INFO
			return false;
		}
		/// 赋值发送数据
		memcpy(pBuffer->pBuf, pStructData, lStructLen);
		m_SendBufferArray.push_back(pBuffer);
		END_DEBUG_INFO
		return true;
	}

	void CNetBase::e_BuildBuffer(UINT nJobType, UINT nSST, long lStructLen,
		void* pStructData, char* pBuffer, long* plSendLen)
	{
		START_DEBUG_INFO
		long DataLen = 0;
		long lngTmp = 0;
		long lngSizeOfDataLen = 0;
		DataLen = sizeof(char) + sizeof(lStructLen) + lStructLen;
		/// 加入数据长度
		memcpy(pBuffer, &DataLen, sizeof(DataLen));
		lngSizeOfDataLen = sizeof(DataLen);
		lngTmp = nJobType;
		/// 加入业务类型
		pBuffer[lngSizeOfDataLen] = (char)lngTmp;
		lngTmp = nSST;
		/// 加入结构类型
		memcpy(&pBuffer[lngSizeOfDataLen + sizeof(char)], &lngTmp, sizeof(lngTmp));
		if(NULL != pStructData)
		{
			/// 加入结构数据
			memcpy(&pBuffer[lngSizeOfDataLen + sizeof(char) + sizeof(lngTmp)], pStructData, lStructLen);
		}
		*plSendLen = lngSizeOfDataLen + sizeof(char) + sizeof(lngTmp) + lStructLen;
		END_DEBUG_INFO
	}

	bool CNetBase::e_SendStruct(HSOCKET s, UINT nJobType, UINT nSST, long lStructLen, void* pStructData)
	{
		START_DEBUG_INFO
		long lSendLen = 0;
		/// 发送的BYTE大小
		int nNumBytes;
		char* pBuffer = NULL;
		if(0 < s)
		{
			pBuffer = new char[lStructLen + 1024];
			if(NULL != pBuffer)
			{
				memset(pBuffer, 0x00, lStructLen + 1024);
				/// 数据打包
				e_BuildBuffer(nJobType, nSST, lStructLen, pStructData, pBuffer, &lSendLen);
				if(-1 == (nNumBytes = e_SendData(s, pBuffer, lSendLen)))
				{
					if(NULL != pBuffer)
					{
						delete[] pBuffer;
						pBuffer = NULL;
					}
					END_DEBUG_INFO
					return false;
				}
				if(NULL != pBuffer)
				{
					delete[] pBuffer;
					pBuffer = NULL;
				}
				END_DEBUG_INFO
				return true;
			}
		}
		END_DEBUG_INFO
		return false;
	}

	int CNetBase::e_SendData(HSOCKET s, char* pszBuf, UINT nsize)
	{
		START_DEBUG_INFO
		if(NULL == pszBuf)
		{
			END_DEBUG_INFO
			return -1;
		}
		/// 重试次数
		int nTryCount = 0;
		/// 失败重试次数
		int nTryDoCount = 0;
		int nsize1, nlen;
		nsize1 = nsize;
		while(0 < nsize) 
		{
			nlen = send(s, pszBuf, nsize, 0);
			if(0 > nlen) 
			{
#ifndef WIN32
				int err = errno;
#else
				int err = GetLastError();
#endif
				if(10055 == err)
				{
#ifndef WIN32
					usleep(1000);
#else
					Sleep(10);
#endif
					printf("缓冲已满\n");
					nTryDoCount++;
					if(10 < nTryDoCount)
					{
						END_DEBUG_INFO
						return -1;
					}
					continue;
				}
				else if(10060 == err)
				{
					/// 发送超时，断线重连
					e_CloseSocket();
					END_DEBUG_INFO
					return -1;
				}
				else if(EINTR == err)
				{
#ifndef WIN32
					usleep(1000);
#else
					Sleep(10);
#endif
					printf("Socket EINTR\n");
					nTryDoCount++;
					if(10 < nTryDoCount)
					{
						END_DEBUG_INFO
						return -1;
					}
					continue;
				}
				else if(EAGAIN == err)
				{
					nTryCount++;
					printf("发送数据失败重试次数 = %i\n", nTryCount);
					if(2 <= nTryCount)
					{
						END_DEBUG_INFO
						return -1;
					}
					else
					{
						continue;
					}
				}
				else 
				{
					printf("发送套接字数据错误 = %i\n", err);
					END_DEBUG_INFO
					return -1;
				}
			}
			nsize -= nlen;
			pszBuf += nlen;
			nTryDoCount = 0;
		}
		END_DEBUG_INFO
		return nsize1-nsize;
	}
	
	long CNetBase::e_ReceiveData(int nSocketId, char* pReceiveBuffer, long lRevLength)
	{
		START_DEBUG_INFO
		if(NULL == pReceiveBuffer)
		{
			END_DEBUG_INFO
			return -1;
		}
		/// 数据长度
		int nRecvLen = lRevLength;
		/// 重试次数
		int nTryCount = 0;
		/// 失败重试次数
		int nTryDoCount = 0;
		while(0 < lRevLength)
		{
			int nlen = recv(nSocketId, pReceiveBuffer, lRevLength, 0);
			if(0 >= nlen)
			{
#ifndef WIN32
				int nErr = errno;
#else
				int nErr = GetLastError();
#endif
				if(10055 == nErr)
				{
#ifndef WIN32
					usleep(1000);
#else
					Sleep(5);
#endif
					printf("缓冲已满\n");
					if(nTryDoCount++ > 10)
					{
						END_DEBUG_INFO
						return -1;
					}
					continue;
				}
				else if(EINTR == nErr)
				{
#ifndef WIN32
					usleep(1000);
#else
					Sleep(5);
#endif
					printf("Socket EINTR\n");
					nTryDoCount++;
					if(10 < nTryDoCount)
					{
						END_DEBUG_INFO
						return -1;
					}
					continue;
				}
				else if(EAGAIN == nErr)
				{
					nTryCount++;
					printf("接收数据失败重试次数=%i\n", nTryCount);
					if(2 <= nTryCount)
					{
						END_DEBUG_INFO
						return -1;
					}
					continue;
				}
				else 
				{
					printf("接收数据错误[%i]\n", nErr);
					END_DEBUG_INFO
					return -1;
				}
			}
			else 
			{
				lRevLength -= nlen;
				pReceiveBuffer += nlen;
				nTryDoCount = 0;
			}
		}
		END_DEBUG_INFO
		return nRecvLen;
	}

	void CNetBase::e_DestroySendBufferArray()
	{
		START_DEBUG_INFO
		/// 加锁发送数组缓冲
		CLightAutoLock lckSendBuffer(&m_SendBufferLock);

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

	void CNetBase::e_GetHostIP(char* pszHostIp)
	{
		START_DEBUG_INFO
#ifdef _NC_WINDOWS_PLATFORM_
		char  szName[256];
		memset(szName, 0x00, sizeof(szName));
		/// 获取主机名称
		if(0 == gethostname(szName, 256))
		{
			PHOSTENT  ptHostInfo;
			/// 根据主机名获取Ip地址
			if(NULL != (ptHostInfo = gethostbyname(szName)))
			{
				LPCSTR pcIP = inet_ntoa(*(struct in_addr *)*ptHostInfo->h_addr_list);
				snprintf(pszHostIp, 16, "%s", pcIP);
			}
		}
#else
		int nFd, nIntrface, nRetn = 0;
		struct ifreq  tBuf[16];
		struct ifconf tIfc;
		if((nFd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 
		{
			tIfc.ifc_len = sizeof tBuf;
			tIfc.ifc_buf = (caddr_t)tBuf;
			if(0 == ioctl(nFd, SIOCGIFCONF, (char* )&tIfc)) 
			{
				nIntrface = tIfc.ifc_len / sizeof(struct ifreq);
				while(nIntrface-- > 0)
				{
					/// 操作读取到的网络设备
					if(!(ioctl(nFd, SIOCGIFFLAGS, (char *) &tBuf[nIntrface]))) 
					{
						if(tBuf[nIntrface].ifr_flags & IFF_PROMISC) 
						{
							nRetn++;
						}
					} 
					else 
					{
						char szTemp[256];
						sprintf(szTemp, "Ioctl device %s出错！", tBuf[nIntrface].ifr_name);
						//D_ERROR(szTemp);
					}
					/// 是否等到了网卡状态
					if(tBuf[nIntrface].ifr_flags & IFF_UP) 
					{
					}
					/// 从网卡中获取IP地址
					if(0 == (ioctl(nFd, SIOCGIFADDR, (char *)&tBuf[nIntrface])))
					{
						/// 获取Ip地址
						char* pcIpData = inet_ntoa(((struct sockaddr_in*)
							(&tBuf[nIntrface].ifr_addr))->sin_addr);
						if(0 != strcmp(pcIpData, "127.0.0.1"))
						{
							/// 过滤本地地址ip信息
							strcpy(pszHostIp, pcIpData);
							printf("pszHostIp = %s\r\n", pszHostIp);
						}
					}
					else 
					{
						char szTemp[256];
						sprintf(szTemp, "Ioctl device %s出错！", tBuf[nIntrface].ifr_name);
						//D_ERROR(szTemp);
					}
				}
			} 
			else
			{
				//D_ERROR("Ioctl出错！");
			}
		}
		else
		{
			//D_ERROR("Socket出错！");
		}
		close (nFd);
#endif
		END_DEBUG_INFO
	}

	void CNetBase::e_SetReceiveDataCallBack(OnRecvDataCallBack pReceiveStructCallBack)
	{
		START_DEBUG_INFO
		/// 设置回调函数
		m_pReceiveStructCallBack = pReceiveStructCallBack;
		END_DEBUG_INFO
	}
}