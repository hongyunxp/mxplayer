/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 导出结构和数据类型定义
/// @par 相关文件
/// 应用此文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetCoreDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/14
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LEE_NET_CORE_DEF_H_H__
#define __LEE_NET_CORE_DEF_H_H__

/// ==============================================
/// @par 功能 
/// 接收数据完成回调函数
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
/// @note 		by li.xl 2013/06/14 
/// ==============================================
typedef void (*OnRecvDataCallBack)(USHORT usNetType, ULONG ulContextID, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

/// ==============================================
/// @par 功能 
/// 发送数据完成回调函数
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
/// @note 		by li.xl 2013/06/14 
/// ==============================================
typedef void (*OnSendDataCallBack)(USHORT usNetType, ULONG ulContextID, const char* pszClientIP,
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount,
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

/// ==============================================
/// @par 功能 
/// TCP客户端成功连接通知
/// @param 
/// [in,out]	<参数名>		<参数说明>
/// [in]		ulContextID		客户端连接ID
/// [in]		pszClientIP		客户端IP
/// [in]		usClientPort	客户端端口号
/// @return 	-
/// @note 		by li.xl 2013/06/14 
/// ==============================================
typedef void (*OnConectionCallBack)(ULONG ulContextID, const char* pszClientIP, USHORT usClientPort);

/// ==============================================
/// @par 功能 
/// TCP客户端断开连接通知
/// @param 
/// [in,out]	<参数名>		<参数说明>
/// [in]		ulContextID		客户端连接ID
/// [in]		pszClientIP		客户端IP
/// [in]		usClientPort	客户端端口号
/// @return 	-
/// @note 		by li.xl 2013/06/14 
/// ==============================================
typedef void (*OnDisConectionCallBack)(ULONG ulContextID, const char* pszClientIP, USHORT usClientPort);

/// ==============================================
/// @par 功能 
/// 打印日志回调函数定义
/// @param 
/// [in,out]	<参数名>		<参数说明>
///	[in]		pszfmt			要打印的日志信息
/// @return 	-
/// @note 		by li.xl 2013/07/23 
/// ==============================================
typedef void (*OnPrintLog)(const char* pszfmt, ...);

/// 网络传输类型
enum _eNetTransfType
{
	NTT_None		= 0,	///< 未知
	NTT_TCPData		= 1,	///< TCP数据
	NTT_UDPData		= 2,	///< UDP数据
};

/// 网络服务类型
enum _eNetServerNetType
{
	NSNT_None	= 0,		///< 无，未知
	NSNT_TCP	= 1,		///< TCP 网络服务
	NSNT_UDP	= 2,		///< UDP 网络服务
	NSNT_Both	= 3,		///< 兼容 UDP 和 TCP
};

/// 网络服务初始化结构体信息
typedef struct _tNetInitStruct
{
	USHORT				usServerPort;					///< 服务端口号
	USHORT				usServerNetType;				///< 服务端网络类型			(参见 _eNetCoreNetType)
	USHORT				usMaxIOWorkers;					///< 最大工作线程			(此参数为0，系统则为您选择最优的工作线程数)
	USHORT				usPendReadsNum;					///< 投递读取数据的数量
	USHORT				usFreeBufMaxNum;				///< 最大空闲缓冲数			(此参数为0，默认空闲缓冲数量2048)
	USHORT				usFreeConnMaxNum;				///< 最大空闲连接数			(此参数为0，默认空闲连接数量1024)
	bool				bOrderSend;						///< 是否顺序发送
	bool				bOrderRead;						///< 是否顺序读取
	bool				bUDPJoinGroup;					///< 是否加入组播
	char				szUDPGroupIP[16];				///< 加入组播的地址
	USHORT				usUDPRevcNum;					///< 投递UDP接收的数据并发个数

	/// 构造初始化
	_tNetInitStruct()
	{
		e_InitStruct();
	}

	/// 初始化结构体
	void e_InitStruct()
	{
		usServerPort = 0;
		usServerNetType = NSNT_None;
		usMaxIOWorkers = 0;
		usPendReadsNum = 4;
		usFreeBufMaxNum = 0;
		usFreeConnMaxNum = 0;
		bOrderSend = true;
		bOrderRead = true;
		bUDPJoinGroup = false;
		memset(szUDPGroupIP, 0x00, sizeof(szUDPGroupIP));
		usUDPRevcNum = 32;
	}

}T_NetInitStruct, *LP_NetInitStruct;

#endif /// __LEE_NET_CORE_DEF_H_H__