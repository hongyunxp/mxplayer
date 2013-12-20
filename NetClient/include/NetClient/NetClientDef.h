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

#ifndef __FWAYNET_LOTTERY_NETCLIENTDEF_H__
#define __FWAYNET_LOTTERY_NETCLIENTDEF_H__

/// 命令对象类型
enum _eCmdOBJType
{
	COBJT_None = 0,
	COBJT_Test = 1,
};

/// 接收结构数据回调函数
typedef void (*OnRecvDataCallBack)(UINT nSttType, int nCount, void* pData);

/// 测试结构体
typedef struct _tTestStruct
{
	char	szTime[32];
	char	szAddr[64];
	char	szName[32];
}T_TestStruct, *LP_TestStruct;

#endif /// __FWAYNET_LOTTERY_NETCLIENTDEF_H__
