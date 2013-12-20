////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// FWAY客户端通讯处理
/// @par 相关文件
/// NetBase.cpp 
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetBase.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET_NETCLIENT_NETBASE_H__
#define __FWAYNET_NETCLIENT_NETBASE_H__

#include "common/AutoLock.h"
#include "NetBaseDef.h"
#include "NetClient/NetClientDef.h"

class CNetBase
{
public:
	CNetBase();
	~CNetBase();
public:
	/// ==============================================
	/// @par 功能 
	/// 连接服务器
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszServerIp		连接服务IP
	/// [in]		usServerPort	服务端口号
	/// @return 	bool			连接是否成功
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_ConnectServer(const char* pszServerIP, USHORT usServerPort);

	/// ==============================================
	/// @par 功能 
	/// 创建UDP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usClientUDPPort	UDP端口号
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_CreatUDPClient(USHORT usClientUDPPort);

	/// ==============================================
	/// @par 功能 
	/// 设置接收结构体回调
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_SetReceiveDataCallBack(OnRecvDataCallBack pfnRecDataCallBack);

	/// ==============================================
	/// @par 功能 
	/// 发送数据到客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszSendBuf		要发送的数据
	/// [in]		nSendSize		数据大小
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_SendTCPData(char* pszSendBuf, UINT nSendSize);

	/// ==============================================
	/// @par 功能 
	/// 发送数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pszRemoteIP		远程数据IP
	/// [in]		usRemotePort	远程端口
	/// [in]		pszSendBuf		要发送的数据
	/// [in]		nSendSize		数据大小		
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_SendUDPData(const char* pszRemoteIP, USHORT usRemotePort, char* pszSendBuf, UINT nSendSize);


protected:

	/// ==============================================
	/// @par 功能 
	/// 重新连接服务器
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_ReconnectServer();

	/// ==============================================
	/// @par 功能 
	/// 开始接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_StartReceive();

	/// ==============================================
	/// @par 功能 
	/// 停止接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopReceive();

	/// ==============================================
	/// @par 功能 
	/// 开始发送数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_StartSend();

	/// ==============================================
	/// @par 功能 
	/// 停止发送线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopSend();

	/// ==============================================
	/// @par 功能 
	/// 接收数据线程（虚函数）
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_ReceiveLoop() = 0;

	/// ==============================================
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_SendLoop() = 0;
		
	/// ==============================================
	/// @par 功能 
	/// 验证是否连接到了服务器
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	bool			返回是否连接成功
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_IsConnect();

	/// ==============================================
	/// @par 功能 
	/// 关闭套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_CloseSocket();

	/// ==============================================
	/// @par 功能 
	/// 关闭TCP连接套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_CloseTCPSocket();

	/// ==============================================
	/// @par 功能 
	/// 关闭UDP连接套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_CloseUDPSocket();

	/// ==============================================
	/// @par 功能 
	/// 处理接收TCP数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszRevBuffer	接收到的数据Buffer
	/// [in]		nRevLength		收到的数据长度
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_RecTCPData(char* pszRevBuffer, int nRevLength);

	/// ==============================================
	/// @par 功能 
	/// 处理接收UDP数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszRevBuffer	接收数据缓冲
	/// [in]		nRevLength		收到的数据长度
	///	[in]		strRemoteIP		远程IP
	/// [in]		usRemotePort	远程端口
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	int e_RecUDPData(char* pszRevBuffer, int nRevLength, string& strRemoteIP, USHORT& usRemotePort);

	/// ==============================================
	/// @par 功能 
	/// 验证套接字是否可读写
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		sClientSocket	客户端套接字
	///	[in]		usMode			读写模式验证
	/// [in]		usProcessTimeOut超时时间
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	int e_SelectProcess(HSOCKET sClientSocket, USHORT usMode, USHORT usProcessTimeOut = NET_REV_SEND_TIME_OUT);

private:
	/// ==============================================
	/// @par 功能 
	/// 接收数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	static DWORD WINAPI i_ReceiveThread(void* pNetBase);

	/// ==============================================
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	////	[in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	static DWORD WINAPI i_SendThread(void* pNetBase);

	/// ==============================================
	/// @par 功能 
	/// 初始化套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool i_InitTCPSocket();

	/// ==============================================
	/// @par 功能 
	/// 初始化UDP套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	bool i_InitUDPSocket();

	/// ==============================================
	/// @par 功能 
	/// 释放发送缓冲数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void i_DestroySendBufferArray();

protected:
	/// TCP套接字ID
	HSOCKET					m_sClientTCPSocket;
	/// UDP套接字
	HSOCKET					m_sClientUDPSocket;
	/// 服务器IP地址
	char					m_szServerIP[16];
	/// 服务器端口号
	USHORT					m_usServerTCPPort;
	/// UDP端口号
	USHORT					m_usClientUDPPort;

protected:
	/// 接收数据线程句柄
	HANDLE					m_HRecvThreadHandle;
	/// 发送数据线程句柄
	HANDLE					m_HSendThreadHandle;
	/// 接收停止事件
	HANDLE					m_HRecvStopEven;
	/// 发送停止事件
	HANDLE					m_HSendStopEven;
	/// 发送缓冲锁
	CLightCritSec			m_SendBufferLock;
	/// 发送缓冲数组
	CPublicPtrArray			m_SendBufferArray;
	/// 接收数据回调函数定义
	OnRecvDataCallBack		m_pfnRecDateCallBack;
	/// 是否连接成功状态
	bool					m_bConnect;
};

#endif	/// __FWAYNET_NETCLIENT_NETBASE_H__