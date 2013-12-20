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
#ifndef __THE_LEE_NET_IOCP_H_H__
#define __THE_LEE_NET_IOCP_H_H__

#include "ContextList.h"
#include "BufferPool.h"
#include "NetServerDef.h"

class CNetIOCP
{
public:
	CNetIOCP();
	virtual ~CNetIOCP();

public:
	/// ==============================================
	/// @par 功能 
	/// 初始化IOCP网络信息
	/// @param 
	/// [in,out]	<参数名>			<参数说明>
	///	[in]		sttNetInitStruct	网络初始化参数
	/// [in]		pFunRecvData		接收数据回调函数
	/// [in]		pFunSendData		发送数据回调函数
	/// [in]		pFunConection		客户端TCP成功连接回调函数
	/// [in]		pFunDisConection	客户端TCP断开连接回调函数
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	bool e_InitNetModel(T_NetInitStruct& sttNetInitStruct,
		OnRecvDataCallBack pFunRecvData = NULL,
		OnSendDataCallBack pFunSendData = NULL, 
		OnConectionCallBack pFunConection = NULL, 
		OnDisConectionCallBack pFunDisConection = NULL,
		OnPrintLog pFunPrintLog = NULL);

	/// ==============================================
	/// @par 功能 
	/// 启动网络服务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>	
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	bool e_StartNetService();

	/// ==============================================
	/// @par 功能 
	/// 启动网络服务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_StopdNetService();
private:
	/// ==============================================
	/// @par 功能 
	/// 启动运行TCP网络服务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	bool i_RunTCPService();

	/// ==============================================
	/// @par 功能 
	/// 启动运行UDP网络服务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	bool i_RunUDPService();

	/// ==============================================
	/// @par 功能 
	/// TCP连接监听线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pParama			线程参数
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	static UINT WINAPI i_ListenThread(void* pParama);

	/// ==============================================
	/// @par 功能 
	/// 工作线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pParama			线程参数
	/// @return 	-
	/// @note 		by li.xl 2013/06/14 
	/// ==============================================
	static UINT WINAPI i_WorkThread(void* pParama);

	/// ==============================================
	/// @par 功能 
	/// IOCP工作处理函数
	/// @param 
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	void i_IoWorkerLoop();

	/// ==============================================
	/// @par 功能 
	/// 处理新连接操作，投递到IO工作线程
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		sClientSocket	客户端连接套接字
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	bool i_AssIncomClientWithContext(SOCKET sClientSocket);

	/// ==============================================
	/// @par 功能 
	/// 设置新连接的套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		sSocket			要设置的套接字
	/// @return 	-
	/// @note 		by li.xl 2013/06/17 
	/// ==============================================
	bool i_SetNewSocketOption(SOCKET sSocket);

	/// ==============================================
	/// @par 功能 
	/// 投递UDP接收任务到完成端口
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usNum			投递接收的IO数量
	/// @return 	-
	/// @note 		by li.xl 2013/06/17 
	/// ==============================================
	bool i_PostUDPRecvIssue(USHORT usNum);
	
	/// ==============================================
	/// @par 功能 
	/// 投递UDP接收任务到完成端口
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pNetBuffer		缓冲对象
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	bool i_PostUDPRecvIssue(CNetBuffer* pNetBuffer = NULL);

	/// ==============================================
	/// @par 功能 
	/// 投递UDP发送任务到完成端口
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pszClientIP		客户端断开
	/// [in]		usClientPort	客户端端口号错误
	/// [in]		pSendBuffer		发送数据缓冲
	/// @return 	-
	/// @note 		by li.xl 2013/06/17 
	/// ==============================================
	bool i_PostUDPSendIssue(const char* pszClientIP, USHORT usClientPort, CNetBuffer* pSendBuffer);

	/// ==============================================
	/// @par 功能 
	/// 处理IO消息处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pClientContext	连接信息
	///	[in]		psttIOHand		IO重叠句柄信息
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/17 
	/// ==============================================
	bool i_ProcessIOMessage(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理TCP新连接事件处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnTCPAccept(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理TCP准备接收事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnTCPRecv(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 处理TCP接收完成事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnTCPRecved(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理TCP准备发送事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnTCPSend(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理TCP发送完成事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnTCPSended(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理UDP准备接收事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnUDPRecv(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 处理UDP准备发送事件
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		nDataLen		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_OnUDPSend(T_ClientContext* pClientContext, CNetBuffer* pNetBuffer, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 发送一个处理信息到完成端口
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		usIOType		IO操作类型
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	bool i_AIOProcess(T_ClientContext* pClientContext, USHORT usIOType, CNetBuffer* pNetBuffer = NULL);

	/// ==============================================
	/// @par 功能 
	/// 使IO顺序读取操作
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_MakeOrderdRead(T_ClientContext* pClientContext, CNetBuffer*& pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 按顺序获取读操作
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	CNetBuffer		返回要处理的缓冲对象
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	CNetBuffer* i_GetNextReadBuffer(T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer = NULL);

	/// ==============================================
	/// @par 功能 
	/// 按顺序获取写操作
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	CNetBuffer		返回要处理的缓冲对象
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	CNetBuffer* i_GetNextSendBuffer(T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer = NULL);

	/// ==============================================
	/// @par 功能 
	/// 处理接收包拆分
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	void i_ProcessRecvPackage(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 处理前一次未接收完的数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	bool			false == 还有未处理完成的数据, true == 已经处理完成
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	bool i_ProcessPreviouRecv(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 处理当前接收的数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	连接信息
	/// [in]		pNetBuffer		缓冲对象
	/// @return 	bool			false == 还有未处理完成的数据, true == 已经处理完成
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	void i_ProcessCurrentRecv(T_ClientContext*& pContextStruct, CNetBuffer*& pNetBuffer);

protected:

	/// ==============================================
	/// @par 功能 
	/// 通知客户端新连接到来
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	客户端连接信息
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	bool e_NotifyClientConntext(T_ClientContext* pClientContext);

	/// ==============================================
	/// @par 功能 
	/// 通知客户端断开连接
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pClientContext	客户端连接信息
	/// @return 	-
	/// @note 		by li.xl 2013/06/19 
	/// ==============================================
	bool e_DisConectionConntext(T_ClientContext*& pClientContext);

	/// ==============================================
	/// @par 功能 
	/// 通知接收到数据包
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pContextStruct	接收的到数据
	/// [in]		pNetBuffer		缓冲对象
	/// [in]		unDataSize		数据大小
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	void e_NotifyReceivedPackage(USHORT usNetType, T_ClientContext* pContextStruct, CNetBuffer* pNetBuffer);

	/// ==============================================
	/// @par 功能 
	/// 通知发送数据包完成
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usNetType		发送和接收数据的网络类型 参见_eNetTransfType
	/// [in]		ulContextID		若是TCP连接此参数大于0，UDP连接此参数为0
	/// [in]		pszClientIP		客户端IP
	/// [in]		usClientPort	客户端端口号
	/// [in]		sDataType		数据类型
	/// [in]		nOBJType		对象/结构类型
	/// [in]		sOBJCount		对象/结构总数
	/// [in]		sSNum			对象/结构数组开始数量(数据列表拆包时与sOBJCount配合使用)
	/// [in]		sENum			对象/结构数组结束数量(数据列表拆包是与sOBJCount配合使用)
	/// [in]		nDatalen		数据长度
	/// [in]		pData			数据内容
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	void e_NotifyWriteCompleted(USHORT usNetType, ULONG ulContextID, const char* pszClientIP,
		USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount,
		SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

	/// ==============================================
	/// @par 功能 
	/// 获取IP和端口信息
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		sClientSocket	客户端套接字
	/// [in]		pszClientIP		客户端IP
	/// [in]		usStrSize		字符串长度
	/// [in]		usClientPort	客户端端口
	/// @return 	-
	/// @note 		by li.xl 2013/06/20 
	/// ==============================================
	bool e_GetIPAddrBySocket(SOCKET sClientSocket, char* pszClientIP, USHORT usStrSize, USHORT& usClientPort);

	/// ==============================================
	/// @par 功能 
	/// 释放套接字
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		sSocket			要释放的套接字
	/// @return 	-
	/// @note 		by li.xl 2013/06/18 
	/// ==============================================
	void e_CloseSocket(SOCKET& sSocket);
public:

	/// ==============================================
	/// @par 功能 
	/// 发送数据到TCP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID
	/// [in]		sttBufferHead	TCP数据头
	/// [in]		pSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/21 
	/// ==============================================
	bool e_SendDataToTCPClient(ULONG ulContextID, T_TCPBufferHead& sttBufferHead,
		BYTE* pSendData, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 发送数据到UDP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszClientIP		客户端IP
	///	[in]		usClientPort	客户端端口号
	/// [in]		sttBufferHead	UDP数据头
	/// [in]		pSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_SendDataToUDPClient(const char* pszClientIP, USHORT usClientPort,
		T_UDPBufferHead& sttBufferHead, BYTE* pSendData, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// 关闭TCP连接
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID	
	/// @return 	-
	/// @note 		by li.xl 2013/06/28 
	/// ==============================================
	bool e_CloseTCPContext(ULONG ulContextID);

	/// ==============================================
	/// @par 功能 
	/// 打印日志
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		pszLog			要打印的日志信息
	/// @return 	-
	/// @note 		by li.xl 2013/07/23 
	/// ==============================================
	void e_PrintLog(const char* pszLog, ...);

private:
	/// 服务启动状态
	bool					m_bNetRunStatus;
	/// TCP服务监听线程句柄
	HANDLE					m_hListenThread;
	/// TCP服务监听套接字
	SOCKET					m_sListenTCPSocket;
	/// TCP监听套接字事件句柄
	HANDLE					m_hPostAcceptEvent;
	/// IOCP完成端口实例句柄
	HANDLE					m_hCompletionPort;
	/// 网络服务初始化信息结构
	T_NetInitStruct			m_sttNetInitStruct;
	/// 工作线程句柄数组
	HANDLE					m_hWorkThread[MAX_PROCESSER_NUMBERS];
	/// 连接信息列表管理对象
	CContextList			m_ContextManager;
	/// 缓冲数据管理对象
	CBufferPool				m_CBufferManager;
	/// UDP服务套接字
	SOCKET					m_sUDPServerSocket;
	/// UDP服务端使用的连接系想你
	T_ClientContext*		m_pUDPSeverContext;
	/// 接收数据回调函数
	OnRecvDataCallBack		m_pFunRecvData;
	/// 发送数据回调函数
	OnSendDataCallBack		m_pFunSendData;
	/// 客户端TCP成功连接回调函数
	OnConectionCallBack		m_pFunConection;
	/// 客户端TCP断开连接回调函数
	OnDisConectionCallBack	m_pFunDisConection;
	/// 打印日志回调函数
	OnPrintLog				m_pFunPrintLog;
};

#endif	/// __THE_LEE_NET_IOCP_H_H__