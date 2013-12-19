////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// <幸运农场客户端所需结构、和枚举定义>
/// @par 相关文件
/// CommTypes.h 
/// @par 功能详细描述
/// <所需结构，和枚举定义>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetClientDef.h
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/05/25
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET_LOTTERY_NETCLIENTDEF_H__
#define __FWAYNET_LOTTERY_NETCLIENTDEF_H__

namespace FWAYNET
{
	/// 接收结构数据回调函数
	typedef void (*OnRecvDataCallBack)(UINT nSttType, int nCount, void* pData);

	/// 测试结构体
	typedef struct _tTestStruct
	{
		char	szTime[32];
		char	szAddr[64];
		char	szName[32];
	}T_TestStruct, *LP_TestStruct;
}

#endif /// __FWAYNET_LOTTERY_NETCLIENTDEF_H__
