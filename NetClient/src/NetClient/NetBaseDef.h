////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络模块基础数据定义
/// @par 相关文件
/// 引用此文件头的所有文件
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetBaseDef.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/19
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET_NETCLIENTD_NETBASEDEF_H_H__
#define __FWAYNET_NETCLIENTD_NETBASEDEF_H_H__

#include "NetClient/NetClientDef.h"

#define MAXDATALEN

/// 允许最多发送的缓冲数组长度
#define MAXSENDBUFARR		1000			
/// 接收网络数据时，多分配100字节数据，避免通信双方的结构不一样大，造成内存访问越界，并对多分配的内存初始化
#define	EXTRA_BUFFER_SIZE	100
/// 连接服务器时扫描的IP开始地址
#define NETSERVERSTARIP		1
/// 连接服务器时扫描的IP结束地址
#define NETSERVERSTOPIP		20

/// 传输工作类型
enum _eJobDataType
{
	JDT_None = 0,					///< 无工作类型
	JDT_StringData,					///< 字符串类型
	JDT_StructData,					///< 对象/结构体数据
};

/// 网络数据头结构体
typedef struct _tBufferHead
{
	int				nTotalLen;		///< 数据总长度(包含数据头和数据内容长度)
	short			sDataType;		///< 数据类型			参见：_eJobDataType
	int				nOBJType;		///< 对象数据类型		参见：_eCmdOBJType
	int				nOBJSize;		///< 对象数据大小		
	short			sOBJCount;		///< 对象数据个数
	short			sSNum;			///< 开始数量
	short			sENum;			///< 结束数量
	int				nFlag;			///< 标识符(请求/回复)
	int				nReserved;		///< 预留扩展字段
	char			cBuf;			///< 缓冲区数据(存放数据得一个字节)
}T_BufferHead, *LP_BufferHead;

#endif	/// __FWAYNET_NETCLIENTD_NETBASEDEF_H_H__