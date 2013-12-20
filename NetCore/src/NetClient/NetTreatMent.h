////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络通讯业务处理
/// @par 相关文件
/// NetTreatment.cpp
/// @par 功能详细描述
/// <控制台应用程序的入口点>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetTreatment.h
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/05/25
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FWAYNET_LOTTERY_NETTREATMENT_H__
#define __FWAYNET_LOTTERY_NETTREATMENT_H__

#include "NetBase.h"

namespace FWAYNET
{
	////////////////////////////////////////////////////////////////////////////////
	/// 
	/// @par 	异常类
	/// <异常类说明>
	/// @par 	备注
	/// <处理客户端业务通讯类>
	/// @par 	典型用法
	/// @code
	/// <这里是典型用法的示例代码>
	/// @endcode
	/// 
	/// @brief
	/// @author	Li.xl
	/// 
	////////////////////////////////////////////////////////////////////////////////
	class CNetTreatment: public CNetBase
	{
	public:
		/// 
		/// @par 功能 
		/// 构造函数
		/// @param 
		///	[in,out]	<参数名>		<参数说明>
		/// 
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		CNetTreatment();

		/// 
		/// @par 功能 
		/// 析构函数
		/// @param 
		///	[in,out]	<参数名>		<参数说明>
		/// 
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		~CNetTreatment();

		/// 
		/// @par 功能 
		/// 发送线程处理
		/// @param 
		///	[in,out]	<参数名>		<参数说明>
		/// 
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		void e_SendLoop();

		/// 
		/// @par 功能 
		/// 接收线程处理
		/// @param 
		///	[in,out]	<参数名>		<参数说明>
		/// 
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		void e_ReceiveLoop();

		/// 
		/// @par 功能 
		/// 处理接收到的数据
		/// @param 
		///	[in,out]	<参数名>		<参数说明>
		/// [in]		BtDataType		发送数据类型
		/// [in]		pData			发送的数据
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		virtual void e_ProcessReceiveData(BYTE BtDataType, void* pData);

		/// 
		/// @par 功能 
		/// 处理接收到的结构体数据
		/// @param 
		///	[in,out]		<参数名>		<参数说明>
		/// [in]			nSendStructType	发送过来的结构体类型
		/// [in]			pData			发送的数据
		/// @return 		-
		/// @note 
		/// <函数说明>
		/// @par 示例：
		/// @code 
		/// <调用本函数的示例代码>
		/// @endcode 
		/// @see		<参见内容> 
		/// @deprecated	<过时说明> 
		/// 
		void e_PackageReceiveStruct(UINT nSendStructType, void* pData);

		/// 
		/// @par 功能 
		/// 获取当前运行目录路径
		/// @note 		-
		/// @par 示例：
		/// @code 		-
		/// @endcode 
		/// @see		-
		/// @deprecated	- 
		/// 
		string e_GetFilePath();

		/// 
		/// @par 功能 
		/// 获取配置文件中指定连接网关服务的IP
		/// @param[in/out]	pszServiceIp;	返回参数,要获取的服务器IP
		/// @param[in]		nSizeIp;		返回参数字符串长度
		/// @return 	bool	获取成功或者失败
		/// @note 		-
		/// @par 示例：
		/// @code 		-
		/// @endcode 
		/// @see		-
		/// @deprecated	- 
		/// 
		bool e_GetNetServiceIp(char* pszServiceIp, int nSizeIp);
	public:
		/// 数据发送时间
		time_t		m_tSendTime;
	};
}

#endif /// __FWAYNET_LOTTERY_NETTREATMENT_H__