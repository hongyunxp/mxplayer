////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// <幸运农场客户端通讯处理>
/// @par 相关文件
/// NetSocket.cpp 
/// @par 功能详细描述
/// <幸运农场客户端通讯处理>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetSocket.h
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/05/25
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FWAYNET_LOTTERYNETSOCKET_H__
#define __FWAYNET_LOTTERYNETSOCKET_H__

#include "common/AutoLock.h"
#include "NetBaseDef.h"

class CNetBase
{
public:
	CNetBase();
	~CNetBase();
public:

	/// 
	/// @par 功能 
	/// 连接服务器
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pszServerIp		连接服务IP
	/// [in]		nServerPort		服务端口号
	/// [in]		nClientType		客户端类型
	/// [in]		nConnectType	连接服务类型(0 = 扫描连接Ip， 1 = 首次连接指定Ip，2 = 只连接指定Ip)
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_ConnectServer(char* pszServerIp, int nServerPort, int nClientType, int nConnectType);

	/// 
	/// @par 功能 
	/// 开始接收数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	int e_StartReceive();

	/// 
	/// @par 功能 
	/// 发送数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	int e_StartSend();

	/// 
	/// @par 功能 
	/// 停止接收线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_StopReceive();

	/// 
	/// @par 功能 
	/// 停止发送线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_StopSend();

	/// 
	/// @par 功能 
	/// 设置接收结构体回调
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_SetReceiveDataCallBack(OnRecvDataCallBack pReceiveStructCallBack);

	/// 
	/// @par 功能 
	/// 发送结构数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		nJobType		工作类型
	/// [in]		nSST			结构类型
	/// @return 	bool			返回成功或者失败
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_SendStructData(int nJobType, int nSST, long lStructLen, void* pStructData);

	/// 
	/// @par 功能 
	/// 打包数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		nJobType		操作类型
	/// [in]		nSST			结构体类型
	/// [in]		lStructLen		结构体大小
	/// [in]		pStructData		结构体数据
	/// [in out]	pBuffer			发送数据
	/// [in]		plSendLen		发送数据大小
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_BuildBuffer(UINT nJobType, UINT nSST, long lStructLen, 
		void* pStructData, char* pBuffer, long* plSendLen);

	/// 
	/// @par 功能 
	/// 发送结构体数据打包
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		s				套接字ID
	/// [in]		SST				结构体类型
	/// [in]		StructLen		结构体数据大小
	/// [in]		pStructData		结构体数据
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_SendStruct(HSOCKET s, UINT nJobType, UINT nSST, long lStructLen, void* pStructData);

	/// 
	/// @par 功能 
	/// 
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		s				套接字ID
	/// [in]		pBuf			要发送的数据
	/// [in]		size			数据大小
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	int e_SendData(HSOCKET s, char* pBuf, UINT nsize);

	/// 
	/// @par 功能 
	/// 接收数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	virtual void e_ReceiveLoop() = 0;

	/// 
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	virtual void e_SendLoop() = 0;

	/// 
	/// @par 功能 
	/// 处理接收到到的数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		byDataType		操作数据包类型
	/// [in]		pData			数据指针
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	virtual void e_ProcessReceiveData(BYTE BtDataType, void* pData) = 0;
		
	/// 
	/// @par 功能 
	/// 验证是否连接到了服务器
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_IsConnect();

	/// 
	/// @par 功能 
	/// 初始化客户端连接
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_InitClient();

	/// 
	/// @par 功能 
	/// 获取本机Ip
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in/out]	pszHostIp		获取本机IP
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_GetHostIP(char* pszHostIp);

private:
#ifdef WIN32
	/// 
	/// @par 功能 
	/// 接收数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pBaseSocket		套接字数据
	/// @return 		-
	/// @note 
	/// 适用于Windows
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	static DWORD WINAPI i_ReceiveThread(void* pBaseSocket);

	/// 
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pBaseSocket		套接字数据
	/// @return 		-
	/// @note 
	/// 适用于Windows
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	static DWORD WINAPI i_SendThread(void* pBaseSocket);
#else
	/// 
	/// @par 功能 
	/// 接收数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pBaseSocket		套接字数据
	/// @return 		-
	/// @note 
	/// 适用于Linux
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	static void *i_ReceiveThread(void *pBaseSocket);

	/// 
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pBaseSocket		套接字数据
	/// @return 		-
	/// @note 
	/// 适用于Linux
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	static void *i_SendThread(void *pBaseSocket);
#endif

protected:
	/// 
	/// @par 功能 
	/// 初始化套接字
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// 主要用户重连机制
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_IniSocket();

	/// 
	/// @par 功能 
	/// 设置当前使用的套接字连接
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		nSocketId		套接字ID
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_SetSocket(int nSocketId);

	/// 
	/// @par 功能 
	/// 关闭套接字
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_CloseSocket();

	/// 
	/// @par 功能 
	/// 重新连接服务器
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_ReconnectServer();

	/// 
	/// @par 功能 
	/// 扫描连接服务器
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 	bool			连接成功或者失败
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_ScanConnectServer();

	/// 
	/// @par 功能 
	/// 测试连接扫描的服务器Ip
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		pServerIp		连接服务器的Ip
	/// [in]		nConnectTimeOut	设置连接超时时间
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	bool e_ConnectTest(char* pServerIp, int nConnectTimeOut);

	/// 
	/// @par 功能 
	/// 处理接收数据
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// [in]		nSocketId		套接字ID
	/// [in]		pszRevBuffer	接收数据缓冲地址
	/// [in]		lngRevLength	接收数据长度
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	long e_ReceiveData(int nSocketId, char* pszRevBuffer, long lRevLength);

	/// 
	/// @par 功能 
	/// 情况缓冲数据内容
	/// @param 
	///	[in,out]	<参数名>		<参数说明>
	/// 
	/// @return 		-
	/// @note 
	/// <函数说明>
	/// @par 示例：
	/// @code 
	/// <调用本函数的示例代码>
	/// @endcode 
	/// @see		<参见内容> 
	/// @deprecated	<过时说明> 
	/// 
	void e_DestroySendBufferArray();

protected:
#ifdef WIN32

	/// 初始化套接字返回值
	int						m_nWSAInitResult;
	/// 接收数据线程ID
	DWORD					m_DRecvThreadId;
	/// 接收数据线程句柄
	HANDLE					m_HRecvThreadHandle;
	/// 发送数据线程ID
	DWORD					m_DSendThreadId;
	/// 发送数据线程句柄
	HANDLE					m_HSendThreadHandle;
#else
	/// 接收数据线程ID 
	pthread_t				m_RecvThreadTaskId;
	/// 接收数据线程句柄
	pthread_mutex_t			m_RecvThreadMutex;
	/// 发送数据线程ID
	pthread_t				m_SendThreadTaskId;
	/// 发送数据线程句柄
	pthread_mutex_t			m_SendThreadMutex;
#endif
	/// 套接字ID
	int						m_nSocketId;
	/// 停止接收标志
	bool					m_bStopRecvTag;
	/// 停止发送标志
	bool					m_bStopSendTag;
	/// 多线程发送数据锁
	CLightCritSec			m_SendLock;
	/// 发送缓冲锁
	CLightCritSec			m_SendBufferLock;
	/// 发送缓冲数组
	CPublicPtrArray			m_SendBufferArray;
	/// 接收数据回调函数定义
	OnRecvDataCallBack		m_pReceiveStructCallBack;
	/// 是否连接成功状态
	bool					m_bConnect;
	/// 服务器IP地址
	char					m_szServerIp[15];
	/// 服务器端口号
	int						m_nServerPort;
	/// 连接服务类型(0 = 扫描连接Ip， 1 = 首次连接指定Ip，2 = 只连接指定Ip)
	int						m_nConnectType;
public:
	/// 客户端类型
	int						m_nClientType;
};

#endif	/// __FWAYNET_LOTTERYNETSOCKET_H__