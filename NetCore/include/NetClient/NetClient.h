////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 处理接口函数调用
/// @par 相关文件
/// NetClient.cpp
/// @par 功能详细描述
/// <这里是本功能的详细描述，内容超过120个半角字符需要加反斜杠N换行接上一行内容>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetClient.h
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/06/08
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_NETCLIENT_OUTINCLUDE_H__INCLUDE__
#define __LEE_NETCLIENT_OUTINCLUDE_H__INCLUDE__

#include "NetClient/NetClientDef.h"
#include "NetBusinessDef.h"

#ifdef	NETCLIENT_EXPORTS
#define NETCLIENT_API __declspec(dllexport)
#else
#define NETCLIENT_API __declspec(dllimport)
#endif /// NETCLIENT_EXPORTS

class NETCLIENT_API CNetClient
{
public:
	CNetClient();
	~CNetClient();
public:
	/// ==============================================
	/// @par 功能 
	/// 初始化客户端网络模块
	/// @param 
	/// [in,out]	<参数名>			<参数说明>
	///	[in]		psttInitNetClient	初始化结构体对象指针
	/// [in]		pfnRecvDataCallBack	接收数据回调函数
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	int e_IInitNetClient(T_InitNetClient* psttInitNetClient,
		OnRecvDataCallBack pfnRecvDataCallBack);

	/// ==============================================
	/// @par 功能 
	/// 连接服务器(支持自动重连)
	/// @param 
	/// [in,out]	<参数名>			<参数说明>
	/// -
	/// @return 	int		1 = 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_IConnectionServer();

	/// ==============================================
	/// @par 功能 
	/// 断开和服务器连接
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	int		1= 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_IDisconnectServer();

	/// ==============================================
	/// @par 功能 
	/// 创建UDP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	int		1= 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	int e_ICreatUDPClient();

	/// ==============================================
	/// @par 功能 
	/// 销毁UDP客户端
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	-
	/// @return 	int		1= 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	int e_IDestroyUDPClient();

	/// ==============================================
	/// @par 功能 
	/// 发送TCP字符串数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszSendData		字符串数据
	/// [in]		unDatalen		字符串长度	
	/// @return 	int		1 = 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_ISendTCPStringData(char* pszSendData, UINT unDatalen);

	/// ==============================================
	/// @par 功能 
	/// 发送二进制数据对象
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		unSSDType		对象/结构的定义类型
	/// [in]		unOBJSize		对象/结构的大小
	/// [in]		usOBJCount		对象/结构的个数
	/// [in]		pSendData		要发送的数据内容
	/// @return 	int		1 = 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_ISendTCPBinaryData(UINT unSSDType, UINT unOBJSize,
		USHORT usOBJCount, BYTE* pSendData);

	/// ==============================================
	/// @par 功能 
	/// 发送UDP字符串数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		pszSendData		字符串数据
	/// [in]		unDatalen		字符串长度	
	/// @return 	int		1 = 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_ISendUDPStringData(const char* pRemoteIP, USHORT usRemotePort, 
		char* pszSendData, UINT unDatalen);

	/// ==============================================
	/// @par 功能 
	/// 发送UDP二进制数据对象
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	/// [in]		unSSDType		对象/结构的定义类型
	/// [in]		unOBJSize		对象/结构的大小
	/// [in]		usOBJCount		对象/结构的个数
	/// [in]		pSendData		要发送的数据内容
	/// @return 	int		1 = 成功，小于等于0表示失败
	/// @note 		Creat By li.xl 2013/12/19 
	/// ==============================================
	int e_ISendUDPBinaryData(const char* pRemoteIP, USHORT usRemotePort, UINT unSSDType,
		UINT unOBJSize, USHORT usOBJCount, BYTE* pSendData);

private:
	/// 客户端网络处理对象
	void*  m_pProNetClient;
};

#endif	/// __LEE_NETCLIENT_OUTINCLUDE_H__INCLUDE__