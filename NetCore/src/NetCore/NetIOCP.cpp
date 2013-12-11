/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// IOCP处理模块
/// @par 相关文件
/// NetIOCP.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetIOCP.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/08
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetIOCP.h"

CNetIOCP::CNetIOCP()
{
	START_DEBUG_INFO
	/// 网络服务状态
	m_bNetRunStatus = false;
	/// 初始化服务端监听线程句柄
	m_hListenThread = NULL;
	/// 初始化服务端监听套接字
	m_sListenTCPSocket = NULL;
	/// 初始化UDP服务套接字
	m_sUDPServerSocket = NULL;
	/// 初始化UDP服务端连接信息
	m_pUDPSeverContext = NULL;
	/// 初始化服务端监听事件句柄
	m_hPostAcceptEvent = NULL;
	/// 初始化完成端口句柄
	m_hCompletionPort = NULL;
	/// 初始化工作线程句柄
	for(int i = 0;  i < MAX_PROCESSER_NUMBERS; i++)
	{
		m_hWorkThread[i] = NULL;
	}
	/// 初始化回调函数指针变量
	m_pFunRecvData = NULL;
	m_pFunSendData = NULL;
	m_pFunConection = NULL;
	m_pFunDisConection = NULL;
	m_pFunPrintLog = NULL;
	/// 初始化连接列表管理对象
	m_ContextManager.e_InitContextList(&m_CBufferManager);
	END_DEBUG_INFO
}

CNetIOCP::~CNetIOCP()
{
	START_DEBUG_INFO
	/// 停止服务
	e_StopdNetService();
	END_DEBUG_INFO
}

/// 初始化网络服务模型
bool CNetIOCP::e_InitNetModel(T_NetInitStruct& sttNetInitStruct,
	OnRecvDataCallBack pFunRecvData,
	OnSendDataCallBack pFunSendData, 
	OnConectionCallBack pFunConection, 
	OnDisConectionCallBack pFunDisConection,
	OnPrintLog pFunPrintLog)
{
	START_DEBUG_INFO
	/// 赋值网络参数和数据回调函数
	memcpy(&m_sttNetInitStruct, &sttNetInitStruct, sizeof(T_NetInitStruct));
	/// 如果连接数量为0, 设置默认空闲连接数量
	if(0 == m_sttNetInitStruct.usFreeConnMaxNum)
	{
		m_sttNetInitStruct.usFreeConnMaxNum = NETCORE_TCP_CONPOOL_NUM;
	}
	/// 如果缓冲数量为0, 设置默认空闲缓冲数量
	if(0 == m_sttNetInitStruct.usFreeBufMaxNum)
	{
		m_sttNetInitStruct.usFreeBufMaxNum = NETCORE_BUFFER_POOL_NUM;
	}
	/// 如果投递事件接收数量为 0
	if(0 == m_sttNetInitStruct.usPendReadsNum)
	{
		/// 赋值为工作线程数量
		m_sttNetInitStruct.usPendReadsNum = m_sttNetInitStruct.usMaxIOWorkers;
	}
	m_pFunRecvData = pFunRecvData;
	m_pFunSendData = pFunSendData;
	m_pFunConection = pFunConection;
	m_pFunDisConection = pFunDisConection;
	m_pFunPrintLog = pFunPrintLog;
	END_DEBUG_INFO
	return true;
}

/// 启动网络服务
bool CNetIOCP::e_StartNetService()
{
	START_DEBUG_INFO
	/// 验证服务是否已在运行
	if(true == m_bNetRunStatus)
	{
		END_DEBUG_INFO;
		return false;
	}
	/// 赋值已启动
	m_bNetRunStatus = true;
	/// 创建完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(NULL == m_hCompletionPort)
	{
		m_bNetRunStatus = false;
		END_DEBUG_INFO
		return false;
	}
	bool bRet = true;
	/// 如果网络服务为TCP服务
	if(NCNT_TCP == m_sttNetInitStruct.usServerNetType ||
		NCNT_Both == m_sttNetInitStruct.usServerNetType)
	{
		bRet &= i_RunTCPService();
	}
	/// 如果网络服务为UDP服务
	if(NCNT_UDP == m_sttNetInitStruct.usServerNetType ||
		NCNT_Both == m_sttNetInitStruct.usServerNetType)
	{
		bRet &= i_RunUDPService();
	}
	/// 如果服务启动成功
	if(true == bRet)
	{
		/// 设置空闲连接数量
		m_ContextManager.e_SetFreeConNum(m_sttNetInitStruct.usFreeConnMaxNum);
		/// 设置空闲缓冲数量
		m_CBufferManager.e_SetBuffFreeCount(m_sttNetInitStruct.usFreeBufMaxNum);
		/// 如果工作线程数为0，获取最佳工作线程数
		if(0 == m_sttNetInitStruct.usMaxIOWorkers)
		{
			/// 最佳线程数量是CPU数量的2倍
			SYSTEM_INFO	sysinfo;
			GetSystemInfo(&sysinfo);
			m_sttNetInitStruct.usMaxIOWorkers = (USHORT)sysinfo.dwNumberOfProcessors * 2 + 2;
			/// 赋值投递接收数量
			m_sttNetInitStruct.usPendReadsNum = m_sttNetInitStruct.usMaxIOWorkers;
		}

		/// 验证是否超过了最大工作线程数量限制
		if(MAX_PROCESSER_NUMBERS < m_sttNetInitStruct.usMaxIOWorkers)
		{
			/// 赋值工作线程数
			m_sttNetInitStruct.usMaxIOWorkers = MAX_PROCESSER_NUMBERS;
		}

		/// 如果线程数大于1，并且投递可读数据大于1，设置必须按顺序读写数据
		if(1 < m_sttNetInitStruct.usMaxIOWorkers && 1 < m_sttNetInitStruct.usPendReadsNum)
		{
			m_sttNetInitStruct.bOrderSend = true;
			m_sttNetInitStruct.bOrderRead = true;
		}

		/// 创建工作线程
		USHORT usIOWorkers = 0;
		while(usIOWorkers != m_sttNetInitStruct.usMaxIOWorkers)
		{
			/// 创建监听线程
			m_hWorkThread[usIOWorkers] = (HANDLE)_beginthreadex(NULL, 0, i_WorkThread, this, 0, NULL);
			if(NULL != m_hWorkThread[usIOWorkers])
			{
				usIOWorkers++;
			}
		}
		/// 启动服务成功
		e_PrintLog("启动网络服务成功...\r\n");
	}
	else
	{
		/// 赋值未启动成功
		m_bNetRunStatus = false;
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::e_StopdNetService()
{
	START_DEBUG_INFO
	/// 停止工作线程以及监听线程
	if(true == m_bNetRunStatus)
	{
		m_bNetRunStatus = false;
		/// 监听线程退出
		if(NULL != m_hListenThread)
		{
			WaitForSingleObject(m_hListenThread, INFINITE);
			CloseHandle(m_hListenThread);
			m_hListenThread = NULL;
		}
		/// 工作线程退出
		int nIONums = m_sttNetInitStruct.usMaxIOWorkers;
		while(0 < nIONums)
		{
			if(TRUE == PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD)NULL, NULL))
			{
				nIONums--;
			}
		}
		/// 等待有信号事件
		WaitForMultipleObjects(m_sttNetInitStruct.usMaxIOWorkers,
			m_hWorkThread, TRUE, INFINITE);
		for(int i = 0;  i < m_sttNetInitStruct.usMaxIOWorkers; i++)
		{
			if(NULL != m_hWorkThread[i])
			{
				CloseHandle(m_hWorkThread[i]);
				m_hWorkThread[i] = NULL;
			}
		}
		/// 释放连接通知事件
		if(NULL != m_hPostAcceptEvent)
		{
			CloseHandle(m_hPostAcceptEvent);
			m_hPostAcceptEvent = NULL;
		}
		/// 停止完成端口操作
		if(NULL != m_hCompletionPort)
		{
			CloseHandle(m_hCompletionPort);
			m_hCompletionPort = NULL;
		}
		/// 释放TCP监听套接字，和UDP服务套接字
		e_CloseSocket(m_sListenTCPSocket);
		e_CloseSocket(m_sUDPServerSocket);
		/// 释放UDP服务连接对象信息
		if(NULL != m_pUDPSeverContext)
		{
			m_ContextManager.e_ReleaseConntext(m_pUDPSeverContext);
			m_pUDPSeverContext = NULL;
		}
		/// 重置服务端监听套接字
		m_sListenTCPSocket = NULL;
		/// 重置UDP服务套接字
		m_sUDPServerSocket = NULL;
		/// 重置服务参数结构
		m_sttNetInitStruct.e_InitStruct();
		/// 停止服务成功
		e_PrintLog("停止网络服务成功...\r\n");
		/// 重置回调函数指针变量
		m_pFunRecvData = NULL;
		m_pFunSendData = NULL;
		m_pFunConection = NULL;
		m_pFunDisConection = NULL;
		m_pFunPrintLog = NULL;
	}
	END_DEBUG_INFO
	return true;
}

/// 启动TCP网络服务
bool CNetIOCP::i_RunTCPService()
{
	START_DEBUG_INFO
	/// 创建TCP服务端监听套接字
	m_sListenTCPSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == m_sListenTCPSocket)
	{
		END_DEBUG_INFO
		return false;
	}

	///// 把receiveform及sendto设置为异步非阻塞模式,因为用WSAStartup初始化默认是同步
	//ULONG ul = 1;
	//int nErrorCode = ioctlsocket(m_sListenTCPSocket, FIONBIO, &ul);
	//if(SOCKET_ERROR == nErrorCode)
	//{
	//	END_DEBUG_INFO
	//	return false;
	//}

	/// 设置为地址重用，优点在于服务器关闭后可以立即启用		
	char szOptRet = 1;
	int nErrorCode = setsockopt(m_sListenTCPSocket, SOL_SOCKET, SO_REUSEADDR, &szOptRet, sizeof(szOptRet));
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 设置监听端口
	SOCKADDR_IN	sttTcpServerAdd;
	sttTcpServerAdd.sin_family = AF_INET;
	sttTcpServerAdd.sin_addr.s_addr = INADDR_ANY;
	sttTcpServerAdd.sin_port = htons(m_sttNetInitStruct.usServerPort);
	/// 邦定套接字和服务器地址
	nErrorCode = bind(m_sListenTCPSocket, (struct sockaddr* )&sttTcpServerAdd, sizeof(sttTcpServerAdd));
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 创建IO事件
	m_hPostAcceptEvent = WSACreateEvent();
	/// 创建失败则返回
	if(WSA_INVALID_EVENT == m_hPostAcceptEvent)
	{
		/// 关闭套接字
		e_CloseSocket(m_sListenTCPSocket);
		END_DEBUG_INFO
		return false;
	}

	/// = 此事件将由系统触发
	WSAEventSelect(m_sListenTCPSocket, m_hPostAcceptEvent, FD_ACCEPT);
	/// = 设置监听等待句柄
	/// = 第二个参数设置在连接队列里面的等待句柄数目.并发量大的server需要调大此值
	nErrorCode = listen(m_sListenTCPSocket, SOMAXCONN);
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 创建监听线程
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, i_ListenThread, this, 0, NULL);
	if(NULL == m_hListenThread)
	{
		END_DEBUG_INFO
		return false;
	}
	END_DEBUG_INFO
	return true;
}

/// 启动UDP网络服务
bool CNetIOCP::i_RunUDPService()
{
	START_DEBUG_INFO
	/// 创建监听套接字
	m_sUDPServerSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == m_sUDPServerSocket)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 把receiveform及sendto设置为异步非阻塞模式,因为用WSAStartup初始化默认是同步
	//ULONG ul = 1;
	//int nErrorCode = ioctlsocket(m_sUDPServerSocket, FIONBIO, &ul);
	//if(SOCKET_ERROR == nErrorCode)
	//{
	//	END_DEBUG_INFO
	//	return false;
	//}
	
	/// 设置为地址重用，优点在于服务器关闭后可以立即启用		
	char szOptRet = 1;
	int nErrorCode = setsockopt(m_sUDPServerSocket, SOL_SOCKET, SO_REUSEADDR, &szOptRet, sizeof(szOptRet));
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 设置UDP数据报生存周期
	int nCurTTL = 255;
	/// 设置新的TTL值
	setsockopt(m_sUDPServerSocket, IPPROTO_IP, IP_TTL, (char *)&nCurTTL, sizeof(nCurTTL));

	DWORD dwBytesReturned = 0;
	int bNewBehavior = FALSE;
	/// 下面的函数用于解决远端突然关闭会导致WSARecvFrom返回10054错误导致服务器完成队列中没有Receive操作而设置
	nErrorCode  = WSAIoctl(m_sUDPServerSocket, SIO_UDP_CONNRESET, &bNewBehavior,
		sizeof(bNewBehavior), NULL, 0, &dwBytesReturned, NULL, NULL);
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}
	/// 填充服务器地址信息
	SOCKADDR_IN UdpServerAdd;
	UdpServerAdd.sin_family = AF_INET;
	UdpServerAdd.sin_addr.s_addr = INADDR_ANY;
	UdpServerAdd.sin_port = htons(m_sttNetInitStruct.usServerPort);

	/// 邦定套接字和服务器地址
	nErrorCode = bind(m_sUDPServerSocket, (struct sockaddr*)&UdpServerAdd, sizeof(UdpServerAdd));
	if(SOCKET_ERROR == nErrorCode)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 是否加入组播
	if(true == m_sttNetInitStruct.bUDPJoinGroup)
	{
		/// 设置该UDP组播地址
		struct ip_mreq sIpMreqGroup;
		/// 相当于设置组名
		sIpMreqGroup.imr_multiaddr.s_addr = inet_addr(m_sttNetInitStruct.szUDPGroupIP);
		sIpMreqGroup.imr_interface.s_addr = htons(INADDR_ANY);
		nErrorCode = setsockopt(m_sUDPServerSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&sIpMreqGroup, sizeof(sIpMreqGroup));
		/// int nLoopBack = 0;
		/// 取消本地回环发送
		/// setsockopt(m_sUDPServerSocket, IPPROTO_IP, IP_MULTICAST_LOOP, (char* )&nLoopBack, sizeof(int));
		if(SOCKET_ERROR == nErrorCode)
		{
			END_DEBUG_INFO
			return false;
		}
	}

	if(NULL == m_pUDPSeverContext)
	{
		if(true == m_ContextManager.e_AllocateConntext(m_pUDPSeverContext))
		{
			m_pUDPSeverContext->sSocketID = m_sUDPServerSocket;
			/// 把监听线程和完成端口邦定
			HANDLE hHandle = CreateIoCompletionPort((HANDLE)m_pUDPSeverContext->sSocketID,
				m_hCompletionPort, (ULONG)m_pUDPSeverContext, 0);
			if(hHandle != m_hCompletionPort)
			{
				/// 移除连接信息
				m_ContextManager.e_ReleaseConntext(m_pUDPSeverContext);
				/// 赋值对象为空
				m_pUDPSeverContext = NULL;
				/// 释放套接字连接
				e_CloseSocket(m_sUDPServerSocket);
				END_DEBUG_INFO
				return false;
			}
			/// 获取投递个数
			i_PostUDPRecvIssue(m_sttNetInitStruct.usUDPRevcNum);
		}
	}
	END_DEBUG_INFO
	return true;
}

/// TCP网络服务监听线程
UINT WINAPI CNetIOCP::i_ListenThread(void* pParama)
{
	START_DEBUG_INFO
	CNetIOCP* pThis = (CNetIOCP* )pParama;
	/// 如果对象指针为空返回，或者服务端TCP监听套接字不存在，退出线程
	if(NULL == pThis || NULL == pThis->m_sListenTCPSocket)
	{
		END_DEBUG_INFO
		return 0; 
	}

	ULONG	ulRet = 0;
	USHORT	usPostAcceptNum = 1;
	int nAddrSize = sizeof(SOCKADDR_IN);
	/// 验证服务状态
	while(true == pThis->m_bNetRunStatus)
	{
		ulRet = WSAWaitForMultipleEvents(1, &pThis->m_hPostAcceptEvent, FALSE, 100, FALSE);
		/// 验证服务状态
		if(false == pThis->m_bNetRunStatus)
		{
			break;
		}

		if(WSA_WAIT_FAILED == ulRet || WSA_WAIT_TIMEOUT == ulRet)
		{
			continue;
		}
		else  
		{
			WSANETWORKEVENTS NetWorkEvents;
			/// 检测所指定的套接口上网络事件的发生。
			int nRet = WSAEnumNetworkEvents(pThis->m_sListenTCPSocket,
				pThis->m_hPostAcceptEvent, &NetWorkEvents);
			/// 如果出现错误,跳出循环
			if(SOCKET_ERROR == nRet)
			{
				break;
			}

			/// 位运算为真, 即为连接事件
			if(NetWorkEvents.lNetworkEvents & FD_ACCEPT)
			{
				/// 如果获取到事件无错误
				if(0 == NetWorkEvents.iErrorCode[FD_ACCEPT_BIT] && true == pThis->m_bNetRunStatus)
				{
					SOCKET sClientSocket = INVALID_SOCKET;
					/// 获取新套接字描述
					sClientSocket = WSAAccept(pThis->m_sListenTCPSocket, NULL, &nAddrSize, 0, 0);
					/// 套接字错误
					if(SOCKET_ERROR == sClientSocket)
					{
						/// 获取错误编号
						nRet = WSAGetLastError();
						/// 缓冲数据没有满情况
						if(WSAEWOULDBLOCK != nRet)
						{
							pThis->e_PrintLog("i_ListenThread WASAccept() Error.\r\n");
						}
					}
					else
					{
						/// 新的连接到来，为工作线程做准备
						pThis->i_AssIncomClientWithContext(sClientSocket);
					}
				}
			}
		}
	}
	END_DEBUG_INFO
	return 0;
}

bool CNetIOCP::i_AssIncomClientWithContext(SOCKET sClientSocket)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证合法性
	if(INVALID_SOCKET == sClientSocket)
	{
		return bRet;
	}
	/// 定义连接信息指针
	T_ClientContext* pContextStruct = NULL;
	/// 获取一个新的连接信息
	if(true == m_ContextManager.e_AllocateConntext(pContextStruct))
	{
		/// 依据套接字初始化连接
		pContextStruct->sSocketID = sClientSocket;
		/// 设置压缩算法
		const char chOpt = 1;
		int nErrCode = setsockopt(pContextStruct->sSocketID, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
		/// 设置连接套接字失败
		if(-1 == nErrCode)
		{
			/// 移除连接信息
			m_ContextManager.e_ReleaseConntext(pContextStruct);
			/// 释放套接字连接
			e_CloseSocket(sClientSocket);
			END_DEBUG_INFO;
			return bRet;
		}

		/// 根据套接字创建对应完成端口
		HANDLE hHandle = CreateIoCompletionPort((HANDLE)pContextStruct->sSocketID, m_hCompletionPort, (ULONG)pContextStruct, 0);
		if(hHandle != m_hCompletionPort)
		{
			/// 移除连接信息
			m_ContextManager.e_ReleaseConntext(pContextStruct);
			/// 释放套接字连接
			e_CloseSocket(sClientSocket);
			END_DEBUG_INFO;
			return bRet;
		}

		/// 缓冲对象合法
		CNetBuffer* pNetBuffer = NULL;
		if(true == m_CBufferManager.e_AllocateBuffer(pNetBuffer, IOPT_TACCEPT))
		{
			/// 指定完成端口m_hCompletionPort发送终止线程
			BOOL bSuccess = PostQueuedCompletionStatus(m_hCompletionPort, 0, (DWORD)pContextStruct, &pNetBuffer->m_sttIOOverLapped);
			/// 结束失败发送终止线程
			if(FALSE == bSuccess && ERROR_IO_PENDING != GetLastError())
			{
				/// 释放连接信息
				m_ContextManager.e_ReleaseConntext(pContextStruct);
				/// 释放缓冲数据
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				END_DEBUG_INFO;
				return FALSE;
			}
		}
		else
		{
			/// 移除连接信息
			m_ContextManager.e_ReleaseConntext(pContextStruct);
			/// 释放套接字连接
			e_CloseSocket(sClientSocket);
		}
	}
	return TRUE;
}

/// 网络服务工作线程
UINT WINAPI CNetIOCP::i_WorkThread(void* pParama)
{
	START_DEBUG_INFO
	CNetIOCP* pThis = (CNetIOCP* )pParama;
	/// 如果对象指针为空返回，退出线程
	if(NULL != pThis)
	{
		pThis->i_IoWorkerLoop();
	}
	END_DEBUG_INFO
	return 0;
}

/// IOCP工作处理函数
void CNetIOCP::i_IoWorkerLoop()
{
	START_DEBUG_INFO
	try
	{
		/// 定义声明临时变量
		DWORD dwIoSize;
		bool bError = false;
		BOOL bIORet = FALSE;
		DWORD dwIOError = 0;
		LPOVERLAPPED pOverlapped = NULL;
		T_ClientContext* pContextStruct = NULL;
		CNetBuffer* pNetBuffer = NULL;
		/// 获取IO队列
		while(false == bError && true == m_bNetRunStatus)
		{
			/// 初始化临时变量
			pOverlapped = NULL;
			pContextStruct = NULL;
			/// 获取一个完成端口队列
			bIORet = GetQueuedCompletionStatus(m_hCompletionPort, &dwIoSize,
				(LPDWORD)&pContextStruct, &pOverlapped, INFINITE);

			/// 如果获取IOCP序列对象失败
			if(FALSE == bIORet)
			{
				/// 如果不为连接超时
				if(WAIT_TIMEOUT != WSAGetLastError())
				{
					/// 如果连接存在
					if(NULL != pContextStruct)
					{
						/// OVERLAPPED合法
						if(NULL != pOverlapped)
						{
							/// 获取IOCP机制所引用的OVERLAPPED
							if(NULL != (pNetBuffer = CONTAINING_RECORD(pOverlapped, CNetBuffer, m_sttIOOverLapped)))
							{
								/// 如果为UDP接收错误，提交信息的接收到完成端口
								if(IOPT_UDPRECV == pNetBuffer->e_GetOperation())
								{
									///// 释放数据缓冲
									//m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
									///// 投递新胡缓冲数据到完成端口
									//i_PostUDPRecvIssue();
									/// 释放数据缓冲
									pNetBuffer->e_ReBuffValue();
									/// 投递接收数据到客户端
									i_PostUDPRecvIssue(pNetBuffer);
									continue;
								}
								/// 释放相间
								m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
							}
							/// 重置数据指针
							pOverlapped = NULL;
						}
						/// 释放连接
						e_DisConectionConntext(pContextStruct);
					}
				}

				/// OVERLAPPED合法
				if(NULL != pOverlapped)
				{
					/// 获取IOCP机制所引用的OVERLAPPED
					if(NULL != (pNetBuffer = CONTAINING_RECORD(pOverlapped, CNetBuffer, m_sttIOOverLapped)))
					{
						m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
					}
				}
				/// 结束本次循环继续下次循环
				continue;
			}

			/// 连接都正常
			if(NULL != pContextStruct && NULL != pOverlapped)
			{
				/// 数据包对象合法
				if(NULL != (pNetBuffer = CONTAINING_RECORD(pOverlapped, CNetBuffer, m_sttIOOverLapped)))
				{
					/// 处理IO消息
					i_ProcessIOMessage(pContextStruct, pNetBuffer, dwIoSize);
				}
			}

			/// 如果连接不正常获取IOCP已停止
			if((NULL == pContextStruct && NULL == pNetBuffer) || false == m_bNetRunStatus)
			{
				/// 赋值停止循环标志
				bError = true;
			}
		}
	}
	catch(...)
	{
		e_PrintLog("此处有异常...\r\n");
	}
	END_DEBUG_INFO
}

/// 设置新连接套接字
bool CNetIOCP::i_SetNewSocketOption(SOCKET sSocket)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(INVALID_SOCKET == sSocket)
	{
		END_DEBUG_INFO
		return bRet;
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::i_PostUDPRecvIssue(USHORT usNum)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证
	if(0 == usNum)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 验证数据合法性
	if(INVALID_SOCKET != m_sUDPServerSocket && NULL != m_pUDPSeverContext)
	{
		do 
		{
			/// 投递接收到IO完成端口
			if(true == i_PostUDPRecvIssue())
			{
				/// 减少
				usNum--;
			}
		}while(0 < usNum);
		bRet = true;
	}

	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::i_PostUDPRecvIssue(CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pNetBuffer)
	{
		m_CBufferManager.e_AllocateBuffer(pNetBuffer, IOPT_UDPRECV);
	}

	if(NULL != pNetBuffer)
	{
		/// 验证数据合法性
		if(INVALID_SOCKET != m_sUDPServerSocket && NULL != m_pUDPSeverContext)
		{
			ULONG ulBytes = 0;
			ULONG ulFlags = 0;
			memset(&pNetBuffer->m_sttUDPAddrInfo, 0x00, sizeof(struct sockaddr_in));
			memset(&pNetBuffer->m_sttIOOverLapped, 0x00, sizeof(WSAOVERLAPPED));
			/// = 发送接收operation到完成端口队列中,此处一定是异步操作，
			/// = 此处的 lpFromLen 参数 即：pNetBuffer->m_nAddressLen 参数必须是在队列完成IO后才能释放，
			/// = 所以此参数定义为 NetBuffer 的成员变量
			int nErrorCode = WSARecvFrom(
				m_sUDPServerSocket, 
				pNetBuffer->e_GetWSABuffer(),
				1,
				&ulBytes,
				&ulFlags,
				(struct sockaddr*)&pNetBuffer->m_sttUDPAddrInfo,
				&pNetBuffer->m_nAddressLen,
				&pNetBuffer->m_sttIOOverLapped,
				NULL);
			/// 赋值返回值
			bRet = true;
			/// 如果投递错误
			if(SOCKET_ERROR == nErrorCode && WSA_IO_PENDING != WSAGetLastError())
			{
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				bRet = false;
			}
		}
		else
		{
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
		}
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::i_PostUDPSendIssue(const char* pszClientIP, USHORT usClientPort, CNetBuffer* pSendBuffer)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(INVALID_SOCKET != m_sUDPServerSocket && NULL != m_pUDPSeverContext)
	{
		ULONG ulBytes = 0;
		ULONG ulFlags = 0;
		pSendBuffer->m_sttUDPAddrInfo.sin_family = AF_INET;
		pSendBuffer->m_sttUDPAddrInfo.sin_addr.s_addr = inet_addr(pszClientIP);
		pSendBuffer->m_sttUDPAddrInfo.sin_port = htons(usClientPort);
		memset(&pSendBuffer->m_sttIOOverLapped, 0x00, sizeof(WSAOVERLAPPED));
		/// 发送接收operation到完成端口队列中,此处一定是异步操作
		int nErrorCode = WSASendTo(
			m_sUDPServerSocket, 
			pSendBuffer->e_GetWSABuffer(),
			1,
			&ulBytes,
			ulFlags,
			(struct sockaddr*)&pSendBuffer->m_sttUDPAddrInfo,
			pSendBuffer->m_nAddressLen,
			&pSendBuffer->m_sttIOOverLapped,
			NULL);
		/// 赋值返回值
		bRet = true;
		/// 如果投递错误
		if(SOCKET_ERROR == nErrorCode && WSA_IO_PENDING != WSAGetLastError())
		{
			m_CBufferManager.e_ReleaseBuffer(pSendBuffer);
			bRet = false;
		}
	}
	else
	{
		m_CBufferManager.e_ReleaseBuffer(pSendBuffer);
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::i_ProcessIOMessage(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 获取IO消息类型
	USHORT usIOType = pNetBuffer->e_GetOperation();
	/// 消息类型对应处理
	switch(usIOType)
	{
	case IOPT_TACCEPT:
		{
			/// 处理初始化数据
			i_OnTCPAccept(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	case IOPT_TCPRECV:
		{
			/// 处理TCP接收准备
			i_OnTCPRecv(pClientContext, pNetBuffer);
			break;
		}
	case IOPT_TRECVED:
		{
			/// 处理TCP接收完成
			i_OnTCPRecved(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	case IOPT_TCPSEND:
		{
			/// 处理TCP发送准备
			i_OnTCPSend(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	case IOPT_TSENDED:
		{
			/// 处理TCP发送完成			
			i_OnTCPSended(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	case IOPT_UDPRECV:
		{
			/// 处理UDP接收准备
			i_OnUDPRecv(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	case IOPT_UDPSEND:
		{
			/// 处理UDP接收准备
			i_OnUDPSend(pClientContext, pNetBuffer, nDataLen);
			break;
		}
	default:
		{
			/// 释放缓冲数据对象
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
			/// 释放连接信息对象
			m_ContextManager.e_ReleaseConntext(pClientContext);
			break;
		}
	}
	END_DEBUG_INFO
	return bRet;
}

void CNetIOCP::i_OnTCPAccept(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 处理新连接
	e_NotifyClientConntext(pClientContext);
	/// 投递接收数据IO操作到完成端口
	for(int i = 0; i < m_sttNetInitStruct.usPendReadsNum; i++)
	{
		/// 获取连接计数，投递接收消息到完成端口
		i_AIOProcess(m_ContextManager.e_GetConntext(pClientContext->ulContextID), IOPT_TCPRECV);
	}
	/// 释放一个连接计数
	m_ContextManager.e_ReleaseConntext(pClientContext);
	/// 释放连接缓冲对象
	m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
	END_DEBUG_INFO
}

void CNetIOCP::i_OnTCPRecv(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO
	/// 设置缓冲数据类型
	pNetBuffer->e_SetOperation(IOPT_TRECVED);
	/// 如果按顺序读取状态为真
	if(TRUE == m_sttNetInitStruct.bOrderRead)
	{
		i_MakeOrderdRead(pClientContext, pNetBuffer);
	}
	else
	{
		/// 必要参数
		DWORD dwIoSize = 0;
		ULONG ulFlags = MSG_PARTIAL;
		/// 设置接收数据
		UINT nRetVal = WSARecv(pClientContext->sSocketID, 
			pNetBuffer->e_GetWSABuffer(), 1,
			&dwIoSize, &ulFlags,
			&pNetBuffer->m_sttIOOverLapped, NULL);
		/// 设置接收数据失败
		if(SOCKET_ERROR == nRetVal && WSA_IO_PENDING != WSAGetLastError()) 
		{
			/// 释放缓冲
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
			/// 端口连接
			e_DisConectionConntext(pClientContext);
		}
	}
	END_DEBUG_INFO
}

void CNetIOCP::i_OnTCPRecved(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 如果接收到数据为0, 断开连接
	if(0 == nDataLen)
	{
		m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
		/// 断开连接对象
		e_DisConectionConntext(pClientContext);
		return;
	}
	/// 增加可使用的缓冲数据
	pNetBuffer->e_AddUsed(nDataLen);
	/// 接收序列状态
	if(true == m_sttNetInitStruct.bOrderRead)
	{
		/// 获取数据缓冲对象
		pNetBuffer = i_GetNextReadBuffer(pClientContext, pNetBuffer);
	}

	/// 数据对象存在
	while(NULL != pNetBuffer)
	{
		/// 处理接收数据包
		i_ProcessRecvPackage(pClientContext, pNetBuffer);
		/// 设置接收业务当前序列递增
		pClientContext->e_InCreaseCurReadSeqNum();
		/// 重置缓冲数据
		pNetBuffer = NULL;
		/// 接收序列状态
		if(true == m_sttNetInitStruct.bOrderRead)
		{
			/// 获取数据缓冲对象
			pNetBuffer = i_GetNextReadBuffer(pClientContext);
		}
	}

	if(NULL != pClientContext)
	{
		/// 在完成端口做一个异步信息读取
		i_AIOProcess(pClientContext, IOPT_TCPRECV);
	}
	END_DEBUG_INFO
}

CNetBuffer* CNetIOCP::i_GetNextReadBuffer(T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO
	/// 验证连接合法性
	if(NULL == pContextStruct)
	{
		END_DEBUG_INFO
		return NULL;
	}
	/// 定义Hash迭代
	CBufferMapToPtr::iterator ItemPos;
	/// 连接信息加锁
	CAutoLock AutoLock(&pContextStruct->ContextCritSec);
	/// 定义返回临时缓冲变量
	CNetBuffer* pRetBuffer = NULL;
	/// 缓冲数据存在
	if(NULL != pNetBuffer)
	{
		/// 获取缓冲序列号
		ULONG ulBufferSeqNum = pNetBuffer->e_GetBufferSeqNum();
		/// 缓冲序列号为当前连接缓冲序列号
		if(ulBufferSeqNum == pContextStruct->usCurReadSeqNum)
		{
			/// 返回当前缓冲
			END_DEBUG_INFO
			return pNetBuffer;
		}
		/// 如果接收到的缓冲列表中不存在该序列对象的缓冲对象
		ItemPos = pContextStruct->CReadBufferMap.find(ulBufferSeqNum);
		if(ItemPos == pContextStruct->CReadBufferMap.end())
		{
			/// 如果不在接收到的缓冲列表中加入该对象到列表
			pContextStruct->CReadBufferMap[ulBufferSeqNum] = pNetBuffer;
		}
	}

	/// 如果接收到的缓冲列表中不存在该序列对象的缓冲对象
	ItemPos = pContextStruct->CReadBufferMap.find(pContextStruct->usCurReadSeqNum);
	if(ItemPos != pContextStruct->CReadBufferMap.end())
	{
		pRetBuffer = ItemPos->second;
		pContextStruct->CReadBufferMap.erase(ItemPos);
	}
	/// 返回当前缓冲数据
	END_DEBUG_INFO
	return pRetBuffer;
}

CNetBuffer* CNetIOCP::i_GetNextSendBuffer(T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO;
	/// 验证连接合法性
	if(NULL == pContextStruct)
	{
		END_DEBUG_INFO
		return NULL;
	}
	/// 定义Hash迭代
	CBufferMapToPtr::iterator ItemPos;
	/// 加锁该连接
	CAutoLock AutoLock(&pContextStruct->ContextCritSec);
	/// 定义临时缓冲
	CNetBuffer* pRetBuffer = NULL;
	/// 如果缓冲不为空
	if(NULL != pNetBuffer)
	{ 
		/// 获取缓冲区的序列号
		ULONG ulBufferSeqNum = pNetBuffer->e_GetBufferSeqNum();
		/// 验证该序列号
		if(ulBufferSeqNum == pContextStruct->usCurSendSeqNum)
		{
			/// 返回缓冲
			END_DEBUG_INFO
			return pNetBuffer;
		}
		/// 如果接收到的缓冲列表中不存在该序列对象的缓冲对象
		ItemPos = pContextStruct->CSendBufferMap.find(ulBufferSeqNum);
		if(ItemPos == pContextStruct->CSendBufferMap.end())
		{
			/// 如果不在发送的缓冲列表中加入该对象到列表
			pContextStruct->CSendBufferMap[ulBufferSeqNum] = pNetBuffer;
		}
	}

	/// 如果接收到的缓冲列表中不存在该序列对象的缓冲对象
	ItemPos = pContextStruct->CSendBufferMap.find(pContextStruct->usCurSendSeqNum);
	if(ItemPos != pContextStruct->CSendBufferMap.end())
	{
		pRetBuffer = ItemPos->second;
		pContextStruct->CSendBufferMap.erase(ItemPos);
	}
	/// 返回当前缓冲数据
	END_DEBUG_INFO
	return pRetBuffer;
}

void CNetIOCP::i_ProcessRecvPackage(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	/// 处理上次未处理完成的数据接收
	if(false == i_ProcessPreviouRecv(pContextStruct, pNetBuffer))
	{
		/// 验证连接对象和缓冲对象合法性
		if(NULL != pContextStruct && NULL != pNetBuffer)
		{
			/// 处理此次数据接收
			i_ProcessCurrentRecv(pContextStruct, pNetBuffer);
		}
	}
	END_DEBUG_INFO
}

bool CNetIOCP::i_ProcessPreviouRecv(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 声明临时变量存放数据缓冲对象
	CNetBuffer* pTempBuffer = pContextStruct->pCurrentBuffer;
	/// 定义需要的Buffer大小
	UINT unHowMuchIsNeeded = 0;
	UINT unUsedBuffer = 0;
	/// 定义获取大小变量
	UINT unDataSize = 0;
	/// 验证数据包合法性
	if(NULL != pTempBuffer)
	{
		/// 获取缓冲对象的使用大小
		unUsedBuffer = pTempBuffer->e_GetUsed();
		/// 验证使用数据大小（小于最小包数据）
		if(MIN_NET_BUFFER_SIZE > unUsedBuffer)
		{
			/// 获取需要的数据大小
			unHowMuchIsNeeded = MIN_NET_BUFFER_SIZE - unUsedBuffer;
			/// 根据使用的大小来处理缓冲数据
			if(unHowMuchIsNeeded > pNetBuffer->e_GetUsed())
			{ 
				/// 如果数据太小, 移动缓冲到连接对象缓冲后，释放对象
				pTempBuffer->e_AddMoveOtherBuffer(pNetBuffer, pNetBuffer->e_GetUsed());
				/// 如果对象存在，释放没有被使用的缓冲
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				/// 返回等待更多的数据
				bRet = true;
				END_DEBUG_INFO
				return bRet;
			}
			else
			{
				/// 移动缓冲数据到连接对象缓冲
				pTempBuffer->e_AddMoveOtherBuffer(pNetBuffer, unHowMuchIsNeeded);
			}
		}

		/// 现在数据已经有数据头长度大小的数据了，才是获取整包
		unUsedBuffer = pTempBuffer->e_GetUsed();
		/// 移动数据
		memmove(&unDataSize, pTempBuffer->e_GetBuffer(), MIN_NET_BUFFER_SIZE);
		/// 数据大小如果小于等于最大有效包数据
		if(MAX_PAL_BUFFER_SIZE >= unDataSize)
		{
			/// 获取需要的数据大小
			unHowMuchIsNeeded = unDataSize - (unUsedBuffer - MIN_NET_BUFFER_SIZE);
			/// 如果我们只需要一点数据，就增加我们需要的小数据
			if(unHowMuchIsNeeded <= pNetBuffer->e_GetUsed())
			{
				/// 增加一定大小的数据到连接缓冲
				pTempBuffer->e_AddMoveOtherBuffer(pNetBuffer, unHowMuchIsNeeded);
				/// 处理接收包数据
				e_NotifyReceivedPackage(NDT_TCPData, pContextStruct, pTempBuffer);
				/// 释放数据缓冲
				m_CBufferManager.e_ReleaseBuffer(pContextStruct->pCurrentBuffer);
				/// 重置当前处理的缓冲
				pContextStruct->pCurrentBuffer = NULL;
				/// 如果数据缓冲中数据包刚刚等于所需数据大小
				if(0 == pNetBuffer->e_GetUsed())
				{
					/// 释放数据缓冲
					m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
					/// 已处理完成
					bRet = true;
				}
			}
			else
			{
				/// 增加一定大小的数据
				pTempBuffer->e_AddMoveOtherBuffer(pNetBuffer, pNetBuffer->e_GetUsed());
				/// 如果对象存在，释放没有被使用的缓冲
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				/// 返回等待更多的数据
				bRet = true;
				END_DEBUG_INFO
				return bRet;
			}
		}
		else
		{
			/// 释放数据缓冲
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
			m_CBufferManager.e_ReleaseBuffer(pContextStruct->pCurrentBuffer);
			pContextStruct->pCurrentBuffer = NULL;
			/// 断开连接
			e_DisConectionConntext(pContextStruct);
			/// 直接返回
			bRet = true;
			END_DEBUG_INFO
			return bRet;
		}
	}
	END_DEBUG_INFO
	return bRet;
}

void CNetIOCP::i_ProcessCurrentRecv(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	/// 声明临时变量存放数据缓冲对象
	CNetBuffer* pTempBuffer = NULL;
	/// 定义需要的Buffer大小
	UINT unHowMuchIsNeeded = 0;
	UINT unUsedBuffer = 0;
	/// 定义获取大小变量
	UINT unDataSize = 0;
	/// 声明循环变量
	bool bDone = false;
	/// 开始循环获取数据
	do 
	{
		/// 赋值循环变量
		bDone = true;
		/// 获取数据使用大小
		unUsedBuffer = pNetBuffer->e_GetUsed();
		/// 缓冲大小
		if(MIN_NET_BUFFER_SIZE <= unUsedBuffer)
		{
			/// 获取数据包大小
			memmove(&unDataSize, pNetBuffer->e_GetBuffer(), MIN_NET_BUFFER_SIZE);
			/// 如果是完整的包
			if(unDataSize == unUsedBuffer - MIN_NET_BUFFER_SIZE)
			{
				/// 处理接收数据包
				e_NotifyReceivedPackage(NDT_TCPData, pContextStruct, pNetBuffer);
				/// 释放数据对象
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				/// 赋值循环变量, 退出循环
				bDone = true;
			}
			else if(unDataSize < unUsedBuffer - MIN_NET_BUFFER_SIZE)
			{
				/// 已使用的数据大于要设置的数据大小 则拆分数据
				if(true == m_CBufferManager.e_SplitNetBuffer(pNetBuffer, pTempBuffer, unDataSize + MIN_NET_BUFFER_SIZE))
				{
					/// 处理数据包
					e_NotifyReceivedPackage(NDT_TCPData, pContextStruct, pTempBuffer);
					/// 释放数据
					m_CBufferManager.e_ReleaseBuffer(pTempBuffer);
					/// 继续循环获取数据，因为还有未被拆分出来的数据没有接收
					bDone = false;
				}
			}
			else if(unDataSize > unUsedBuffer - MIN_NET_BUFFER_SIZE && MAX_PAL_BUFFER_SIZE > unDataSize)
			{
				/// 数据缓冲大小在 使用数据和最大数据包之间
				/// 赋值数据包对象到连接数据缓冲对象
				pContextStruct->pCurrentBuffer = pNetBuffer;
				/// 重置缓冲数据包
				pNetBuffer = NULL;
				/// 赋值循环变量, 退出循环
				bDone = true;
			}
			else
			{
				/// = 此处只有一种情况：数据大小大于缓冲区预设的大小，
				/// = 视为不合法 (unDataSize > MAX_PAL_BUFFER_SIZE)
				/// 释放缓冲数据对象
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				/// 断开连接
				e_DisConectionConntext(pContextStruct);
				/// 赋值循环变量, 退出循环
				bDone = true;
			}
		}
		else if(0 < unUsedBuffer)
		{
			/// 数据太小赋值缓冲数据到连接缓冲对象
			pContextStruct->pCurrentBuffer = pNetBuffer;
			/// 重置传入的缓冲对象
			pNetBuffer = NULL;
			/// 赋值循环标志，停止循环
			bDone = true;
		}
	}while(false == bDone);
	END_DEBUG_INFO
}

void CNetIOCP::i_OnTCPSend(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证连接合法性
	if(NULL != pClientContext && INVALID_SOCKET != pClientContext->sSocketID)
	{
		/// 设置发送状态为真
		if(true == m_sttNetInitStruct.bOrderSend)
		{
			/// 获取发送缓冲对象
			pNetBuffer = i_GetNextSendBuffer(pClientContext, pNetBuffer);
		}

		/// 加锁数据连接信息
		pClientContext->ContextCritSec.e_Lock();
		/// 缓冲对象存在
		while(NULL != pNetBuffer)
		{
			/// 设置操作状态
			pNetBuffer->e_SetOperation(IOPT_TSENDED);
			/// 定义必须变量
			ULONG ulFlags = MSG_PARTIAL;
			DWORD dwSendNumBytes = 0;

			/// 设置发送状态
			int nRetVal = WSASend(pClientContext->sSocketID,
				pNetBuffer->e_GetWSABuffer(),
				1,
				&dwSendNumBytes, 
				ulFlags,
				&pNetBuffer->m_sttIOOverLapped,
				NULL);
			/// 增加发送当前处理的序列号
			pClientContext->e_InCreaseCurSendSeqNum();
			/// 设置状态失败
			if(SOCKET_ERROR == nRetVal && WSA_IO_PENDING != WSAGetLastError())
			{
				/// 释放缓冲数据
				m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
				/// 重置数据缓冲
				pNetBuffer = NULL;
				/// 断开连接
				e_DisConectionConntext(pClientContext);
			}
			else
			{
				/// 重置数据缓冲
				pNetBuffer = NULL;
				/// 验证发送状态
				if(TRUE == m_sttNetInitStruct.bOrderSend)
				{
					/// 获取发送的缓冲对象
					pNetBuffer = i_GetNextSendBuffer(pClientContext);
				}
			}
		}
		/// 连接信息存在，解锁对象
		if(NULL != pClientContext)
		{
			pClientContext->ContextCritSec.e_Unlock();
		}
	}
	else
	{
		/// 释放缓冲对象
		m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
		/// 释放连接对象
		m_ContextManager.e_ReleaseConntext(pClientContext);
	}
	END_DEBUG_INFO
}

void CNetIOCP::i_OnTCPSended(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 连接信息合法
	if(NULL != pClientContext)
	{
		/// 缓冲数据对象合法
		if(NULL != pNetBuffer)
		{
			/// 缓冲对象的使用大小等于端口获取的大小
			if(nDataLen == pNetBuffer->e_GetUsed())
			{
				/// 处理发送完成
				e_NotifyWriteCompleted(NDT_TCPData, pClientContext->ulContextID,
					pClientContext->szClientIP, pClientContext->usClientPort, 
					pNetBuffer->e_GetBuffer(), nDataLen);
			}
			/// 释放缓冲数据
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
		}
		/// 释放连接信息
		m_ContextManager.e_ReleaseConntext(pClientContext);
	}
	END_DEBUG_INFO
}

void CNetIOCP::i_OnUDPRecv(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(0 >= nDataLen)
	{
		END_DEBUG_INFO
		return;
	}
	/// 如果缓冲数据存在
	if(NULL != pNetBuffer)
	{
		/// 增加使用数据大小
		pNetBuffer->e_AddUsed(nDataLen);
		/// 执行回调函数
		if(NULL != m_pFunRecvData)
		{
			m_pFunRecvData(NDT_UDPData, 0/*pClientContext->ulContextID*/,
				inet_ntoa(pNetBuffer->m_sttUDPAddrInfo.sin_addr),
				ntohs(pNetBuffer->m_sttUDPAddrInfo.sin_port),
				pNetBuffer->e_GetBuffer(), nDataLen);
		}
		/// 释放数据缓冲
		pNetBuffer->e_ReBuffValue();
		/// 投递接收数据到客户端
		i_PostUDPRecvIssue(pNetBuffer);
		/// 释放数据缓冲
		/// m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
	}
	/// 投递接收到完成端口
	/// i_PostUDPRecvIssue();
	END_DEBUG_INFO
}

void CNetIOCP::i_OnUDPSend(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen)
{
	START_DEBUG_INFO
	/// 如果缓冲数据存在
	if(NULL != pNetBuffer)
	{
		/// 发送数据完成通知
		e_NotifyWriteCompleted(NDT_UDPData, pClientContext->ulContextID,
			inet_ntoa(pNetBuffer->m_sttUDPAddrInfo.sin_addr),
			ntohs(pNetBuffer->m_sttUDPAddrInfo.sin_port),
			pNetBuffer->e_GetBuffer(), nDataLen);
		
		/// 释放接收数据
		m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
	}
	END_DEBUG_INFO
}

bool CNetIOCP::i_AIOProcess(T_ClientContext* pClientContext, USHORT usIOType, CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 验证数据是否合法
	if(NULL == pNetBuffer)
	{
		m_CBufferManager.e_AllocateBuffer(pNetBuffer, usIOType);
	}
	if(NULL != pNetBuffer)
	{
		bRet = true;
		/// 投递IO事件到完成端口
		BOOL bSuccess = PostQueuedCompletionStatus(m_hCompletionPort, 0,
			(DWORD)pClientContext, &pNetBuffer->m_sttIOOverLapped);
		/// 验证正确性
		if(FALSE == bSuccess && WSA_IO_PENDING != WSAGetLastError())
		{
			bRet = false;
			/// 移除数据缓冲
			m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
			/// 移除客户端连接信息
			e_DisConectionConntext(pClientContext);
		}
	}
	else
	{
		/// 移除客户端连接信息
		m_ContextManager.e_ReleaseConntext(pClientContext);
	}
	END_DEBUG_INFO
	return bRet;
}

void CNetIOCP::i_MakeOrderdRead(T_ClientContext* pClientContext, CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	/// 加锁列表，因为要保证生成的顺序号正确
	pClientContext->ContextCritSec.e_Lock();
	/// 必要参数
	DWORD dwIoSize = 0;
	ULONG ulFlags = MSG_PARTIAL;
	/// 设置接收业务序列号到缓冲对象
	pNetBuffer->e_SetBufferSeqNum(pClientContext->usReadSeqNum);
	/// 设置接收数据
	UINT nRetVal = WSARecv(pClientContext->sSocketID, 
		pNetBuffer->e_GetWSABuffer(), 1,
		&dwIoSize, &ulFlags,
		&pNetBuffer->m_sttIOOverLapped, NULL);
	/// 设置接收数据失败
	if(SOCKET_ERROR == nRetVal && WSA_IO_PENDING != WSAGetLastError()) 
	{
		/// 释放缓冲
		m_CBufferManager.e_ReleaseBuffer(pNetBuffer);
		/// 端口连接
		e_DisConectionConntext(pClientContext);
	}
	else
	{
		/// 增加接收业务序号
		pClientContext->e_AddReadSeqNum();
	}

	/// 验证数据合法性
	if(NULL != pClientContext)
	{
		/// 解锁列表
		pClientContext->ContextCritSec.e_Unlock();
	}
	END_DEBUG_INFO
}

bool CNetIOCP::e_NotifyClientConntext(T_ClientContext* pClientContext)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pClientContext)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 回调通知客户端成功连接
	if(NULL != m_pFunConection)
	{
		/// 获取客户端IP和端口信息
		if(true == e_GetIPAddrBySocket(pClientContext->sSocketID, pClientContext->szClientIP, 
			sizeof(pClientContext->szClientIP), pClientContext->usClientPort))
		{
			m_pFunConection(pClientContext->ulContextID, pClientContext->szClientIP,
				pClientContext->usClientPort);
		}
	}
	END_DEBUG_INFO
	return true;
}

bool CNetIOCP::e_DisConectionConntext(T_ClientContext*& pClientContext)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pClientContext)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 列表加锁
	pClientContext->ContextCritSec.e_Lock();
	/// 关闭套接字连接
	if(INVALID_SOCKET != pClientContext->sSocketID && NULL != pClientContext->sSocketID)
	{
		/// 回调通知客户端断开连接
		if(NULL != m_pFunDisConection)
		{
			m_pFunDisConection(pClientContext->ulContextID, pClientContext->szClientIP,
				pClientContext->usClientPort);
		}
		/// 释放套接字
		e_CloseSocket(pClientContext->sSocketID);
	}
	pClientContext->ContextCritSec.e_Unlock();
	/// 释放数据连接
	m_ContextManager.e_ReleaseConntext(pClientContext);
	END_DEBUG_INFO
	return true;
}

void CNetIOCP::e_NotifyReceivedPackage(USHORT usNetType, T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pContextStruct || NULL == pNetBuffer)
	{
		END_DEBUG_INFO;
		return;
	}

	if(NULL != m_pFunRecvData)
	{
		/// 接收数据到客户端
		m_pFunRecvData(usNetType, pContextStruct->ulContextID,
			pContextStruct->szClientIP, pContextStruct->usClientPort,
			pNetBuffer->e_GetBuffer(), pNetBuffer->e_GetUsed());
	}
	END_DEBUG_INFO
}

void CNetIOCP::e_NotifyWriteCompleted(USHORT usNetType, ULONG ulContextID, const char* pszClientIP,
	USHORT usClientPort, PBYTE pSendBuffer, UINT ulDataLen)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(0 >= usNetType || 0 >= ulContextID || NULL == pszClientIP || 
		0 >= usClientPort || NULL == pSendBuffer || 0 >= ulDataLen)
	{
		END_DEBUG_INFO;
		return;
	}

	if(NULL != m_pFunSendData)
	{
		/// 发送完成通知客户端
		m_pFunSendData(usNetType, ulContextID, pszClientIP,
			usClientPort, pSendBuffer, ulDataLen);
	}
	END_DEBUG_INFO
}

bool CNetIOCP::e_GetIPAddrBySocket(SOCKET sClientSocket, char* pszClientIP, USHORT usStrSize, USHORT& usClientPort)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(INVALID_SOCKET == sClientSocket || NULL == pszClientIP || 0 >= usStrSize)
	{
		END_DEBUG_INFO
		return bRet; 
	}
	/// 获取指定客户端IP地址
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	/// 获取返回值
	int nResult = getpeername(sClientSocket, (struct sockaddr *)&sockAddr, &nSockAddrLen);
	/// 如果获取成功
	if(0 == nResult)
	{
		/// 赋值IP
		strncpy_s(pszClientIP, usStrSize, inet_ntoa(sockAddr.sin_addr), 16);
		/// 赋值端口号
		usClientPort = ntohs(sockAddr.sin_port);
		/// 赋值返回
		bRet = true;
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::e_SendDataToTCPClient(ULONG ulContextID, BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证发送数据合法性
	if(NULL == pSendData || 0 >= nDataLen || MAX_NET_BUFFER_SIZE < nDataLen)
	{
		END_DEBUG_INFO
		return bRet;
	}
	/// 获取客户端连接信息
	T_ClientContext* pClientContext = m_ContextManager.e_GetConntext(ulContextID);
	if(NULL != pClientContext)
	{
		if(INVALID_SOCKET != pClientContext->sSocketID && NULL != pClientContext->sSocketID)
		{
			CNetBuffer* pNetBuffer = NULL;
			/// 获取
			if(true == m_CBufferManager.e_AllocateBuffer(pNetBuffer, IOPT_TCPSEND))
			{
				/// 增加数据到缓冲
				pNetBuffer->e_AddData(pSendData, nDataLen);
				/// 加锁数据对象
				pClientContext->ContextCritSec.e_Lock();
				/// 设置发送缓冲对象的序列号
				pNetBuffer->e_SetBufferSeqNum(pClientContext->usSendSeqNum);
				/// 增加发送业务序列号
				pClientContext->e_AddSendSeqNum();
				/// 解锁数据对象
				pClientContext->ContextCritSec.e_Unlock();
				/// 投递一个发送操作到IO
				bRet = i_AIOProcess(pClientContext, IOPT_TCPSEND, pNetBuffer);
			}
			else
			{
				/// 如果获取缓冲错误，释放连接
				m_ContextManager.e_ReleaseConntext(pClientContext);
			}
		}
		else
		{
			m_ContextManager.e_ReleaseConntext(pClientContext);
		}
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::e_SendDataToUDPClient(const char* pszClientIP, USHORT usClientPort, BYTE* pSendData, int nDataLen)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort || NULL == pSendData 
		|| 0 >= nDataLen || MAX_NET_BUFFER_SIZE < nDataLen)
	{
		END_DEBUG_INFO
		return bRet;
	}

	CNetBuffer* pNetBuffer = NULL;
	if(true == m_CBufferManager.e_AllocateBuffer(pNetBuffer, IOPT_UDPSEND))
	{
		/// 增加数据到缓冲对象
		pNetBuffer->e_AddData(pSendData, nDataLen);
		/// 发送UDP信息到客户但
		bRet = i_PostUDPSendIssue(pszClientIP, usClientPort, pNetBuffer);
	}
	END_DEBUG_INFO
	return bRet;
}

bool CNetIOCP::e_CloseTCPContext(ULONG ulContextID)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	/// 获取连接信息
	T_ClientContext* pClientContext = m_ContextManager.e_GetConntext(ulContextID);
	if(NULL != pClientContext)
	{
		/// 断开连接
		bRet = e_DisConectionConntext(pClientContext);
	}
	END_DEBUG_INFO
	return bRet;
}

void CNetIOCP::e_CloseSocket(SOCKET& sSocket)
{
	START_DEBUG_INFO
	/// 验证数据的合法性
	if(INVALID_SOCKET == sSocket)
	{
		return;
	}
	/// 取消IO上套接字操作, 避免套接字重用发生错误。
	CancelIo((HANDLE)sSocket);
	/// 停止数据
	shutdown(sSocket, SD_BOTH);
	/// 释放套接字连接
	closesocket(sSocket);
	/// 重置套接字
	sSocket = SOCKET_ERROR;
	END_DEBUG_INFO
}

void CNetIOCP::e_PrintLog(const char* pszLog, ...)
{
	/// 临时字符串
	char szTmp[1024] = {0};
	va_list argptr;
	va_start(argptr, pszLog);
	memset(szTmp, 0x00, sizeof(szTmp));
	vsnprintf_s(szTmp, sizeof(szTmp) - 1, pszLog, argptr);
	va_end(argptr);
	/// 回调打印日志
	if(NULL != m_pFunPrintLog)
	{
		m_pFunPrintLog("%s", szTmp);
	}
	else
	{
		printf("%s", szTmp);
	}	
}