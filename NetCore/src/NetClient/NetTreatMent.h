////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络通讯业务处理
/// @par 相关文件
/// NetTreatment.cpp
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetTreatMent.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FWAYNET_NETCLIENT_NETTREATMENT_H__
#define __FWAYNET_NETCLIENT_NETTREATMENT_H__

#include "NetBase.h"

class CNetTreatment: public CNetBase
{
public:
	CNetTreatment();
	~CNetTreatment();

protected:
	/// ==============================================
	/// @par 功能 
	/// 发送线程处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_SendLoop();

	/// ==============================================
	/// @par 功能 
	/// 接收线程处理
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/20 
	/// ==============================================
	virtual void e_ReceiveLoop();

private:
	/// ==============================================
	/// @par 功能 
	/// 
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	void i_ProcessTCPRecData();

	/// ==============================================
	/// @par 功能 
	/// 
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	void i_ProcessUDPRecData();

	/// ==============================================
	/// @par 功能 
	/// 处理接收到的结构体数据
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
	void i_ProcessReceivePackage(USHORT usNetType, const char* pszClientIP, 
		USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
		SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

private:
	/// 接收数据缓存
	char  m_szRecvBuffer[MAX_NET_BUFFER_SIZE];
};

#endif /// __FWAYNET_NETCLIENT_NETTREATMENT_H__