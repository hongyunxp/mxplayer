////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络通讯业务处理
/// @par 相关文件
/// NetTreatment.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         NetTreatMent.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/12/20
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetTreatMent.h"

CNetTreatment::CNetTreatment()
{
	/// 初始化接收数据缓冲
	memset(m_szRecvBuffer, 0x00, MAX_NET_BUFFER_SIZE);
}

CNetTreatment::~CNetTreatment()
{
}

void CNetTreatment::e_SendLoop()
{
	/// 启动线程处理发送数据
	/*while(true)
	{
		/// 验证连接合法性
		if(0 >= (int)m_SendBufferArray.size())
		{
			Sleep(1);
			continue;
		}

		/// 在缓冲中查找要发送的数据
		T_BufferStruct* pBuffer = NULL;
		if(0 < (int)m_SendBufferArray.size())
		{
			CAutoLock lckSendBufferLock(&m_SendBufferLock);
			pBuffer = (T_BufferStruct* )*m_SendBufferArray.begin();
			if(NULL != pBuffer)
			{
				m_SendBufferArray.erase(m_SendBufferArray.begin());
			}
		}

		/// 如果数据存在则发送
		if(NULL != pBuffer)
		{
			e_SendStruct(m_nSocketId, pBuffer->nJobType, pBuffer->nSST, pBuffer->nSize, pBuffer->pBuf);

			if(pBuffer->pBuf)
			{
				delete pBuffer->pBuf;
				pBuffer->pBuf = NULL;
			}
			delete pBuffer;
			pBuffer = NULL;
		}
	}*/
}

void CNetTreatment::e_ReceiveLoop()
{
	BYTE* pBuffer = NULL;
	BYTE* pBufferTmp = NULL;

	/// 数据类型，必须是1个字节大小
	BYTE BtDataType = 0;

	while(true)
	{
		/// 处理TCP接收数据包
		i_ProcessTCPRecData();
		/// 处理UDP接收数据包
		i_ProcessUDPRecData();
	}
}

void CNetTreatment::i_ProcessTCPRecData()
{
	/// 检查TCP是否在线
	if(0 >= e_IsConnect())
	{
		/// 断线重连
		e_ReconnectServer();
		return;
	}

	/// 要读取的数据长度
	int nDataLen = 0;
	/// 已读取的数据长度
	int nReadCount = 0;
	/// 获取数据长度
	nReadCount = e_RecTCPData((char* )&nDataLen, sizeof(UINT));

	/// 没有读取到数据
	if(-1 == nReadCount)
	{
		printf("NetClient Read TCP Data Len Error, ReadCount = %d, DataLen = %d .\r\n",
			nReadCount, nDataLen);
		/// 断开连接
		e_CloseTCPSocket();
		return;
	}

	/// 正确读取到了数据长度
	if(0 < nDataLen)
	{
		/// 除去已经接收了sizeof(DataType)大小，接收数据内容
		nReadCount = e_RecTCPData(m_szRecvBuffer, nDataLen);
		/// 如果读取的数据不正确，则关闭连接
		if(nReadCount != nDataLen)
		{
			printf("NetClient Read TCP Data Content Error, ReadCount = %d, DataLen = %d .\r\n",
				nReadCount, nDataLen);
			/// 断开套接字连接
			e_CloseTCPSocket();
			return;
		}
		
		T_TCPBufferHead sttBufferHead;
		memset(&sttBufferHead, 0x00, sizeof(T_TCPBufferHead));

		mem
	}
}

void CNetTreatment::i_ProcessUDPRecData()
{
	/// 检查TCP是否在线
	if(0 >= e_IsConnect())
	{
		/// 断线重连
		e_ReconnectServer();
		return;
	}
	/// 已读取的数据长度
	int nReadCount = 0;

	string strRemoteIP = "";
	USHORT usRemotePort = 0;

	/// 获取数据长度
	nReadCount = e_RecUDPData((char* )&m_szRecvBuffer, MAX_PAL_BUFFER_SIZE - 1,
		strRemoteIP, usRemotePort);
	if(0 < nReadCount)
	{

	}
}

void CNetTreatment::i_ProcessReceivePackage(USHORT usNetType, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData)
{
	/// 定义接收到的结构体
	T_RSStructData* psttRSStructData = (T_RSStructData* )pData;

	if(NULL == psttRSStructData)
	{
		printf("Get The T_RSStructData Error.\r\n");
		return;
	}

	/// 如果存在回调函数
	if(NULL != m_pReceiveStructCallBack)
	{
		/// 传递数据给回调函数
		m_pReceiveStructCallBack(psttRSStructData->nSRStructType,
		psttRSStructData->nDataCount, psttRSStructData->szData);
	}
}