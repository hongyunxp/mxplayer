////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 连接数据缓冲对象
/// @par 相关文件
/// CNetBuffer.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         CNetBuffer.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/13
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetBuffer.h"

CNetBuffer::CNetBuffer()
{
	START_DEBUG_INFO
	/// 初始化数据变量
	m_ulBufferID = 0;
	m_ulBuffSeqNum = 0;
	m_unUsed = 0;
	m_usIOProType = IOPT_None;
	memset(m_Buffer, 0x00, MAX_NET_BUFFER_SIZE);
	memset(&m_sttWsaBuf, 0x00, sizeof(WSABUF));	
	memset(&m_sttIOOverLapped, 0x00, sizeof(OVERLAPPED));
	memset(&m_sttUDPAddrInfo, 0x00, sizeof(SOCKADDR_IN));
	m_nAddressLen = sizeof(SOCKADDR_IN);
	END_DEBUG_INFO
}

CNetBuffer::~CNetBuffer()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
}

/// 增加数据到数据缓冲
bool CNetBuffer::e_AddData(const BYTE* pBtData, UINT unSize)
{
	START_DEBUG_INFO
	/// 定义返回值
	bool bRet = false;
	if(NULL == pBtData || 0 == unSize)
	{
		END_DEBUG_INFO
		return bRet;
	}
	/// 验证数据合法性
	if(unSize <= MAX_NET_BUFFER_SIZE - m_unUsed)
	{
		/// 增加数据到缓冲
		memcpy(m_Buffer + m_unUsed, pBtData, unSize);
		m_unUsed += unSize;
		bRet = true;
	}
	END_DEBUG_INFO
	return bRet;
}

/// 从缓冲中获取数据
bool CNetBuffer::e_GetData(BYTE*& pBtData, UINT unSize)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL == pBtData || m_unUsed < unSize)
	{
		END_DEBUG_INFO
		return false;
	}
	/// 获取数据包类型
	memmove(pBtData, m_Buffer, unSize);	
	m_unUsed -= unSize;
	memmove(m_Buffer, m_Buffer + unSize, m_unUsed);
	/// 重置移除的部分数据
	memset(m_Buffer + m_unUsed, 0x00, unSize);
	END_DEBUG_INFO
	return true;
}

/// 从缓冲中获取数据
BYTE* CNetBuffer::e_GetBuffer()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return m_Buffer;
}

/// 从缓冲中移除数据
bool CNetBuffer::e_FlushBuffer(UINT unRemoveSize)
{
	START_DEBUG_INFO
	/// 获取数据包类型, 验证数据大小是否操作使用的数据缓冲
	if(MAX_NET_BUFFER_SIZE < unRemoveSize || m_unUsed < unRemoveSize)
	{
		END_DEBUG_INFO
		return false;
	}
	/// 从内存中移除一定大小的缓冲
	m_unUsed -= unRemoveSize;
	memmove(m_Buffer, m_Buffer + unRemoveSize, m_unUsed);
	/// 重置移除的部分数据
	memset(m_Buffer + m_unUsed, 0x00, unRemoveSize);
	END_DEBUG_INFO
	return true;
}

bool CNetBuffer::e_AddMoveOtherBuffer(CNetBuffer*& pOtherBuffer, UINT usMoveSize)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pOtherBuffer)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 赋值对象缓冲
	if(usMoveSize <= MAX_NET_BUFFER_SIZE - m_unUsed)
	{
		/// 获取其他缓冲的数据
		if(true == e_AddData(pOtherBuffer->e_GetBuffer(), usMoveSize))
		{
			/// 移除交换的数据块
			if(true == pOtherBuffer->e_FlushBuffer(usMoveSize))
			{
				bRet = true;
			}
		}
	}
	END_DEBUG_INFO
	return bRet;
}

/// 获取当前使用的缓冲大小
UINT CNetBuffer::e_GetUsed()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return m_unUsed;	
}

/// 增加当前使用的缓冲大小
UINT CNetBuffer::e_AddUsed(UINT unAddUsed)
{
	START_DEBUG_INFO
	m_unUsed += unAddUsed;
	END_DEBUG_INFO
	return m_unUsed;	
}

/// 设置缓冲序列号
void CNetBuffer::e_SetBufferSeqNum(ULONG ulBuffSeqNum)
{
	START_DEBUG_INFO
	m_ulBuffSeqNum = ulBuffSeqNum;
	END_DEBUG_INFO
}

/// 获取缓冲序列号
ULONG CNetBuffer::e_GetBufferSeqNum()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return m_ulBuffSeqNum;
}

/// 设置缓冲ID
void CNetBuffer::e_SetBufferID(ULONG ulBufferID)
{
	START_DEBUG_INFO
	m_ulBufferID = ulBufferID;
	END_DEBUG_INFO
}

/// 获取缓冲ID
ULONG CNetBuffer::e_GetBufferID()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	return m_ulBufferID;
}

/// 获取WSABuffer
WSABUF*	CNetBuffer::e_GetWSABuffer()
{
	START_DEBUG_INFO
	/// 获取操作类型
	USHORT usIOType = e_GetOperation();
	if(IOPT_TCPRECV == usIOType || IOPT_TRECVED == usIOType || IOPT_UDPRECV == usIOType)
	{
		/// 设置可读内存数据（发送的数据）
		m_sttWsaBuf.buf = reinterpret_cast<char*>(m_Buffer) + m_unUsed;
		m_sttWsaBuf.len = MAX_NET_BUFFER_SIZE - m_unUsed;
	}
	else if(IOPT_TCPSEND == usIOType || IOPT_TSENDED == usIOType || IOPT_UDPSEND == usIOType)
	{
		/// 设置可写内存数据（发送的数据）
		m_sttWsaBuf.buf = reinterpret_cast<char*>(m_Buffer);
		m_sttWsaBuf.len = m_unUsed;
	}
	else
	{
		m_sttWsaBuf.buf = reinterpret_cast<char*>(m_Buffer);
		m_sttWsaBuf.len = 0;
	}
	END_DEBUG_INFO
	return &m_sttWsaBuf;
}

void CNetBuffer::e_SetOperation(USHORT usIOType)
{
	START_DEBUG_INFO
	/// 清空重叠数据
	memset(&m_sttIOOverLapped, 0x00, sizeof(OVERLAPPED));
	/// 设置操作类型
	m_usIOProType = usIOType;
	END_DEBUG_INFO
}

USHORT CNetBuffer::e_GetOperation()
{
	START_DEBUG_INFO
	END_DEBUG_INFO
	/// 返回当前操作类型
	return m_usIOProType;
}

void CNetBuffer::e_ReBuffValue()
{
	START_DEBUG_INFO
	/// 重置数据内容
	memset(m_Buffer, 0x00, e_GetUsed());
	m_unUsed = 0;
	memset(&m_sttWsaBuf, 0x00, sizeof(WSABUF));
	END_DEBUG_INFO
}

/// 重置缓冲
void CNetBuffer::e_ReSetBuffer()
{
	START_DEBUG_INFO
	/// 重置数据变量
	m_ulBufferID = 0;
	m_ulBuffSeqNum = 0;
	memset(m_Buffer, 0x00, e_GetUsed());
	m_unUsed = 0;
	m_usIOProType = IOPT_None;
	memset(&m_sttWsaBuf, 0x00, sizeof(WSABUF));
	memset(&m_sttIOOverLapped, 0x00, sizeof(OVERLAPPED));
	memset(&m_sttUDPAddrInfo, 0x00, sizeof(SOCKADDR_IN));
	END_DEBUG_INFO
}