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

#include "common/NetCoreDef.h"

/// 允许最多发送的缓冲数组长度
#define MAXSENDBUFARR		1000			
/// 接收网络数据时，多分配100字节数据，避免通信双方的结构不一样大，造成内存访问越界，并对多分配的内存初始化
#define	EXTRA_BUFFER_SIZE	100
/// 连接服务器时扫描的IP开始地址
#define NETSERVERSTARIP		1
/// 连接服务器时扫描的IP结束地址
#define NETSERVERSTOPIP		20

#endif	/// __FWAYNET_NETCLIENTD_NETBASEDEF_H_H__