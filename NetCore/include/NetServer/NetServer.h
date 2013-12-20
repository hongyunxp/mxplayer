/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 网络核心模块
/// @par 相关文件
/// 引用才文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetCore.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/13
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_NET_CORE_H_H__
#define __LEE_NET_CORE_H_H__

#include "NetServerDef.h"

#ifdef NETSERVER_EXPORTS
	#define NETSERVER_API __declspec(dllexport)
#else
	#define NETSERVER_API __declspec(dllimport)
#endif /// NETSERVER_EXPORTS

class NETSERVER_API CNetServer
{
public:
	CNetServer();
	virtual ~CNetServer();

	/// ==============================================
	/// @par 功能 
	/// 启动服务
	/// @param 
	/// [in,out]	<参数名>			<参数说明>
	///	[in]		psttNetInitStruct	网络初始化参数
	/// [in]		pFunRecvData		接收数据回调函数
	/// [in]		pFunSendData		发送数据回调函数
	/// [in]		pFunConection		客户端TCP成功连接回调函数
	/// [in]		pFunDisConection	客户端TCP断开连接回调函数
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_IStartServer(T_NetInitStruct* psttNetInitStruct,
		OnRecvDataCallBack pFunRecvData = NULL,
		OnSendDataCallBack pFunSendData = NULL, 
		OnConectionCallBack pFunConection = NULL, 
		OnDisConectionCallBack pFunDisConection = NULL,
		OnPrintLog pFunPrintLog = NULL);

	/// ==============================================
	/// @par 功能 
	/// 停止服务
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// @return 	bool			返回停止成功或者失败
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_IStopdServer();

	/// ==============================================
	/// @par 功能 
	/// TCP发送字符串数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID，0表示发送信息到所有客户端
	/// [in]		pszSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_ITCPSendStringData(ULONG ulContextID, char* pszSendData, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// TCP发送二进制数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID，0表示发送信息到所有客户端
	/// [in]		unSSDType		对象/结构类型
	/// [in]		unOBJSize		对象数据大小
	/// [in]		usOBJCount		对象数据个数
	/// [in]		pSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_ITCPSendBinaryData(ULONG ulContextID, UINT unSSDType, 
		UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData);

	/// ==============================================
	/// @par 功能 
	/// UDP发送字符串数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszClientIP		客户端IP
	///	[in]		usClientPort	客户端端口号
	/// [in]		pszSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_IUDPSendStringData(const char* pszClientIP, USHORT usClientPort,
		char* pszSendData, int nDataLen);

	/// ==============================================
	/// @par 功能 
	/// UDP发送二进制数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszClientIP		客户端IP
	///	[in]		usClientPort	客户端端口号
	/// [in]		unSSDType		对象/结构类型
	/// [in]		unOBJSize		对象数据大小
	/// [in]		usOBJCount		对象数据个数
	/// [in]		pSendData		发送的数据
	/// [in]		nDataLen		数据长度
	/// @return 	-
	/// @note 		by li.xl 2013/06/22 
	/// ==============================================
	bool e_IUDPSendBinaryData(const char* pszClientIP, USHORT usClientPort, 
		UINT unSSDType, UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData);

	/// ==============================================
	/// @par 功能 
	/// 关闭TCP连接
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		ulContextID		连接信息ID
	/// @return 	-
	/// @note 		by li.xl 2013/06/28 
	/// ==============================================
	bool e_ICloseTCPContext(ULONG ulContextID);

private:
	/// 定义网络服务对象指针
	void* m_pCNetProcess;
};

#endif	/// __LEE_NET_CORE_H_H__