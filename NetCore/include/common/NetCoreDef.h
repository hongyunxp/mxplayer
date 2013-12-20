///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 网络核心模块定义文件
/// @par 相关文件
/// 引用此文件头的所有文件
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
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET_NETCORE_NETCORE_DEF_H_H__
#define __FWAYNET_NETCORE_NETCORE_DEF_H_H__

/// 最小网络数据缓冲包大小 Sizeof(UINT)
#define MIN_NET_BUFFER_SIZE		4
/// 最大网络数据缓冲包大小 1024 * 32 (32K)
#define MAX_NET_BUFFER_SIZE		32768
/// 最大网络数据包有效数据大小 MAX_NET_BUFFER_SIZE - MIN_NET_BUFFER_SIZE
#define MAX_PAL_BUFFER_SIZE		32764

/// 传输工作类型
enum _eJobDataType
{
	JDT_None = 0,					///< 无工作类型
	JDT_StringData,					///< 字符串类型
	JDT_StructData,					///< 对象/结构体数据
};

/// TCP网络数据头结构体
typedef struct _tTCPBufferHead
{
	int				nTotalLen;		///< 数据总长度(包含数据头和数据内容长度，不包含描述字段本身)
	short			sDataType;		///< 数据类型			参见：_eJobDataType
	int				nOBJType;		///< 对象数据类型		参见：_eCmdOBJType
	int				nOBJSize;		///< 对象数据大小
	short			sOBJCount;		///< 对象数据个数
	short			sSNum;			///< 开始数量
	short			sENum;			///< 结束数量
	int				nReserved;		///< 预留扩展字段
}T_TCPBufferHead, *LP_TCPBufferHead;

/// UDP网络数据头结构体
typedef struct _tUDPBufferHead
{
	short			sDataType;		///< 数据类型			参见：_eJobDataType
	int				nOBJType;		///< 对象数据类型		参见：_eCmdOBJType
	int				nOBJSize;		///< 对象数据大小
	short			sOBJCount;		///< 对象数据个数
	short			sSNum;			///< 开始数量
	short			sENum;			///< 结束数量
	int				nFlag;			///< 标识符(请求/回复)
	int				nReserved;		///< 预留扩展字段
}T_UDPBufferHead, *LP_UDPBufferHead;

#endif	/// __FWAYNET_NETCORE_NETCORE_DEF_H_H__