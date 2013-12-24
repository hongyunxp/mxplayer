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
	/// 初始化客户端网络信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		psttNetClient	客户端网络信息结构体
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	bool e_InitNetClient(T_InitNetClient* psttNetClient);

	/// ==============================================
	/// @par 功能 
	/// 连接服务器
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	bool			连接是否成功
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_ConnectServer();

	/// ==============================================
	/// @par 功能 
	/// 创建UDP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_CreatUDPClient();

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
	/// 验证是否连接到了服务器
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	bool			返回是否连接成功
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_IsTCPConnect();

	/// ==============================================
	/// @par 功能 
	/// 验证UDP客户端是否已经创建
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	bool e_IsUDPCreated();

	/// ==============================================
	/// @par 功能 
	/// 开始接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_StartTCPReceive();

	/// ==============================================
	/// @par 功能 
	/// 开始发送数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_StartTCPSend();

	/// ==============================================
	/// @par 功能 
	/// 停止接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopTCPReceive();

	/// ==============================================
	/// @par 功能 
	/// 停止发送线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopTCPSend();

	/// ==============================================
	/// @par 功能 
	/// 接收数据线程（虚函数）
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_ReceiveTCPLoop() = 0;

	/// ==============================================
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_SendTCPLoop() = 0;

	/// ==============================================
	/// @par 功能 
	/// 开始接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_StartUDPReceive();

	/// ==============================================
	/// @par 功能 
	/// 开始发送数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	bool e_StartUDPSend();

	/// ==============================================
	/// @par 功能 
	/// 停止接收数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopUDPReceive();

	/// ==============================================
	/// @par 功能 
	/// 停止发送线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void e_StopUDPSend();

	/// ==============================================
	/// @par 功能 
	/// 接收数据线程（虚函数）
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_ReceiveUDPLoop() = 0;

	/// ==============================================
	/// @par 功能 
	/// 发送数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_SendUDPLoop() = 0;

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
	/// 接收TCP数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	static DWORD WINAPI i_RecvTCPThread(void* pNetBase);

	/// ==============================================
	/// @par 功能 
	/// 发送TCP数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	static DWORD WINAPI i_SendTCPThread(void* pNetBase);

	/// ==============================================
	/// @par 功能 
	/// 接收UDP数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	static DWORD WINAPI i_RecvUDPThread(void* pNetBase);

	/// ==============================================
	/// @par 功能 
	/// 发送UDP数据线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pNetBase		对象参数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	static DWORD WINAPI i_SendUDPThread(void* pNetBase);

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
	/// 释放事件参数
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	void i_CancelEvens();

	/// ==============================================
	/// @par 功能 
	/// 释放TCP发送缓冲数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	void i_DestroyTCPSendBufferArray();

	/// ==============================================
	/// @par 功能 
	/// 释放UDP发送缓冲
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	void i_DestroyUDPSendBufferArray();

private:
	/// TCP套接字ID
	HSOCKET					m_sClientTCPSocket;
	/// UDP套接字
	HSOCKET					m_sClientUDPSocket;
	/// 发送TCP缓冲锁
	CLightCritSec			m_CLCSConnSeverLock;
	/// TCP是否连接成功状态
	bool					m_bIsTCPConnect;
	/// UDP是否创建成功
	bool					m_bIsUDPCreated;
protected:
	/// 初始化网络客户端信息
	T_InitNetClient			m_sttInitNetClient;
	/// 接收TCP数据线程句柄
	HANDLE					m_HRecvTCPThreadHandle;
	/// 发送TCP数据线程句柄
	HANDLE					m_HSendTCPThreadHandle;
	/// 接收TCP停止事件
	HANDLE					m_HRecvTCPStopEven;
	/// 发送TCP停止事件
	HANDLE					m_HSendTCPStopEven;

	/// 接收UDP数据线程句柄
	HANDLE					m_HRecvUDPThreadHandle;
	/// 发送UDP数据线程句柄
	HANDLE					m_HSendUDPThreadHandle;
	/// 接收UDP停止事件
	HANDLE					m_HRecvUDPStopEven;
	/// 发送UDP停止事件
	HANDLE					m_HSendUDPStopEven;

	/// 发送TCP缓冲锁
	CLightCritSec			m_SendTCPBufferLock;
	/// 发送缓冲数组
	CPublicPtrArray			m_SendTCPBufferArray;
	/// 发送UDP缓冲锁
	CLightCritSec			m_SendUDPBufferLock;
	/// 发送数据缓冲数组
	CPublicPtrArray			m_SendUDPBufferArray;
	/// 接收数据回调函数定义
	OnRecvDataCallBack		m_pfnRecDataCallBack;
};

#endif	/// __FWAYNET_NETCLIENT_NETBASE_H__