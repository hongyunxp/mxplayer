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
	JDT_None = 0,						///< 无工作类型
	JDT_TextData,						///< 文本类型
	JDT_StructData,						///< 结构体数据
	JDT_ArrStructData,					///< 结构体数组数据
};

/// 缓冲数据结构体
typedef struct _tBufferStruct
{
	int					nJobType;		///< 操作类型
	int					nSST;			///< 发送数据类型
	int					nSize;			///< 发送数据大小
	char*				pBuf;			///< 缓冲区数据
}T_BufferStruct, *LP_BufferStruct;

/// = 用于接收和发送的结构信息
/// = 通过 nJobDataType，nClientType，nSRStructType，等参数验证
/// = 通过 szData 取得数据
typedef struct _tRSStructData
{
	UINT				nJobDataType;	///< 工作类型
	UINT				nClientType;	///< 客户端类型
	UINT				nSRStructType;	///< 数据区结构体处理类型
	UINT				nDataSize;		///< 数据大小指szData指向的数据区域大小
	UINT				nDataCount;		///< 数据区数据项数，即szData中有效的结构体的个数
	UINT				nErrorCode;		///< 错误信息编码
	char				szData[1];		///< 数据区有效数据首地址；必须为最后一个字段
}T_RSStructData, *LP_RSStructData;

#endif	/// __FWAYNET_NETCLIENTD_NETBASEDEF_H_H__