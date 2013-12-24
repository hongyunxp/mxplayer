////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 业务处理信息定义模块
/// @par 相关文件
/// 引用此文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetBusinessDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/21
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAY_NETCORE_NETBUSSINESS_H_H__
#define __FWAY_NETCORE_NETBUSSINESS_H_H__

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

#endif	/// __FWAY_NETCORE_NETBUSSINESS_H_H__