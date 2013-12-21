////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络调用预定
/// @par 相关文件
/// 引用此文件的所有文件头
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetClientDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET__NETCLIENTDEF_H_H__
#define __FWAYNET__NETCLIENTDEF_H_H__

/// ==============================================
/// @par 功能 
/// 处理接收到的数据
/// @param 
/// [in,out]	<参数名>		<参数说明>
///	[in]		usNetType		发送和接收数据的网络类型 参见_eNetTransfType
/// [in]		pszClientIP		客户端IP
/// [in]		usClientPort	客户端端口号
/// [in]		sDataType		数据类型
/// [in]		nOBJType		对象/结构类型
/// [in]		sOBJCount		对象/结构总数
/// [in]		sSNum			对象/结构数组开始数量(数据列表拆包时与sOBJCount配合使用)
/// [in]		sENum			对象/结构数组结束数量(数据列表拆包是与sOBJCount配合使用)
/// [in]		nDatalen		数据长度
/// @return 	-
/// @note 		Creat By li.xl 2013/12/21 
/// ==============================================
typedef void (*OnRecvDataCallBack)(USHORT usNetType, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

/// 客户端初始化信息结构
typedef struct _tInitNetClient
{
	char	szTCPServerIP[16];		///< TCP服务器IP
	USHORT	usTCPServerPort;		///< TCP服务器端口
	USHORT	usLocalUDPPort;			///< 本地UDP端口号
	USHORT	usUDPJoinGroup;			///< 是否加入UDP组播
	char	szUDPGroupIP[16];		///< 组播绑定IP

	_tInitNetClient()
	{
		e_InitStruct();
	}

	void e_InitStruct()
	{
		memset(szTCPServerIP, 0x00, sizeof(szTCPServerIP));
		memset(szUDPGroupIP, 0x00, sizeof(szUDPGroupIP));
		usTCPServerPort = 0;
		usLocalUDPPort = 0;
		usUDPJoinGroup = 0;
	}
}T_InitNetClient, *LP_InitNetClient;

#endif /// __FWAYNET__NETCLIENTDEF_H_H__