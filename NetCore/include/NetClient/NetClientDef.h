////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络数据命令格式
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

#ifndef __FWAYNET__NETCLIENTDEF_H__
#define __FWAYNET__NETCLIENTDEF_H__

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
typedef void (*OnRecvDataCallBack)(USHORT usNetType, ULONG ulContextID, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

/// 命令对象类型
enum _eCmdOBJType
{
	COBJT_None = 0,
	COBJT_Test = 1,
};

/// 测试结构体
typedef struct _tTestStruct
{
	char	szTime[32];
	char	szAddr[64];
	char	szName[32];
}T_TestStruct, *LP_TestStruct;

#endif /// __FWAYNET__NETCLIENTDEF_H__
