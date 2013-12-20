////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 数据缓冲列表管理对象（缓冲池）
/// @par 相关文件
/// BufferPool.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         BufferPool.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/14
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BufferPool.h"

CBufferPool::CBufferPool()
{
	START_DEBUG_INFO
	/// 初始化缓冲对象
	m_ulConstomBufferID = 0;
	m_usFreeBufCount = NO_NUM_FREE_LIST;
	m_ArrayFreeBufList.clear();
	m_ArrayBusyBufList.clear();
	END_DEBUG_INFO
}

/// 创建指定数量的列表构造函数
CBufferPool::CBufferPool(USHORT usBufCount)
{
	START_DEBUG_INFO
	/// 初始化缓冲对象变量值
	m_ulConstomBufferID = 0;
	m_ArrayFreeBufList.clear();
	m_ArrayBusyBufList.clear();
	/// 赋值列表固定数量
	m_usFreeBufCount = usBufCount;
	/// 构造数据对象
	for(USHORT i = 0; i < m_usFreeBufCount; i++)
	{
		CNetBuffer* pNetBuffer = new CNetBuffer();
		if(NULL != pNetBuffer)
		{
			/// 增加数据到列表
			m_ArrayFreeBufList.push_back(pNetBuffer);
		}
	}
	END_DEBUG_INFO
}

CBufferPool::~CBufferPool()
{
	START_DEBUG_INFO
	i_ClearBufferList();
	END_DEBUG_INFO
}

/// 设置空闲缓冲数量
void CBufferPool::e_SetBuffFreeCount(USHORT usBufCount)
{
	START_DEBUG_INFO;
	/// 赋值列表固定数量
	m_usFreeBufCount = usBufCount;
	m_CFreeListCrit.e_Lock();
	if(m_usFreeBufCount > m_ArrayFreeBufList.size())
	{
		USHORT usCreatNum = m_usFreeBufCount - m_ArrayFreeBufList.size();
		/// 构造数据
		for(USHORT i = 0; i < usCreatNum; i++)
		{
			CNetBuffer* pNetBuffer = new CNetBuffer();
			if(NULL != pNetBuffer)
			{
				/// 增加数据到列表
				m_ArrayFreeBufList.push_back(pNetBuffer);
			}
		}
	}
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO;
}

/// 获取空闲缓冲列表大小
USHORT CBufferPool::e_GetFreeListSize()
{
	START_DEBUG_INFO
	USHORT usListSize = 0;
	m_CFreeListCrit.e_Lock();
	usListSize = m_ArrayFreeBufList.size();
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO
	return usListSize;
}

/// 获取使用缓冲列表大小
USHORT CBufferPool::e_GetBusyListSize()
{
	START_DEBUG_INFO
	USHORT usListSize = 0;
	m_CBusyListCrit.e_Lock();
	usListSize = m_ArrayBusyBufList.size();
	m_CBusyListCrit.e_Unlock();
	END_DEBUG_INFO
	return usListSize;
}

/// 从列表中获取可用缓冲数据对象信息
bool CBufferPool::e_AllocateBuffer(CNetBuffer*& pNetBuffer, USHORT usIOType)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 初始化
	pNetBuffer = NULL;
	CBufferPtrArray::iterator FreeItemPos;
	/// 获取空闲的连接信息对象
	m_CFreeListCrit.e_Lock();
	if(0 < m_ArrayFreeBufList.size())
	{
		FreeItemPos = m_ArrayFreeBufList.begin();
		pNetBuffer = *FreeItemPos;
		m_ArrayFreeBufList.erase(FreeItemPos);
	}
	else
	{
		pNetBuffer = new CNetBuffer();
	}
	m_CFreeListCrit.e_Unlock();

	/// 增加信息对象到使用连接列表
	if(NULL != pNetBuffer)
	{
		m_CBusyListCrit.e_Lock();
		/// 设置缓冲对象列表ID
		pNetBuffer->e_SetBufferID(i_BuildNetBufferID());
		pNetBuffer->e_SetOperation(usIOType);
		/// 赋值数据变量
		m_ArrayBusyBufList[pNetBuffer->e_GetBufferID()] = pNetBuffer;
		/// 赋值返回值
		bRet = true;
		m_CBusyListCrit.e_Unlock();
	}
	END_DEBUG_INFO
	return bRet;
}

bool CBufferPool::e_SplitNetBuffer(CNetBuffer*& pSrcBuffer, CNetBuffer*& pDesBuffer, UINT unSplitSize)
{
	START_DEBUG_INFO
	if(NULL == pSrcBuffer || unSplitSize > pSrcBuffer->e_GetUsed())
	{
		END_DEBUG_INFO
		return false;
	}
	/// 获取缓冲数据对象
	e_AllocateBuffer(pDesBuffer, pSrcBuffer->e_GetOperation());
	/// 如果对象不存在则返回
	if(NULL == pDesBuffer)
	{
		END_DEBUG_INFO
		return false;
	}

	/// 设置缓冲序列号
	pDesBuffer->e_SetBufferSeqNum(pSrcBuffer->e_GetBufferSeqNum());
	/// 增加一定大小的缓冲数据到声明的缓冲变量
	if(false == pDesBuffer->e_AddData(pSrcBuffer->e_GetBuffer(), unSplitSize))
	{
		/// 增加失败，则删除
		e_ReleaseBuffer(pDesBuffer);
		/// 返回NULL
		END_DEBUG_INFO
		return false;
	}
	/// 释放一定大小的原有数据
	if(false == pSrcBuffer->e_FlushBuffer(unSplitSize))
	{
		/// 释放一定大小原有数据失败则删除声明的临时缓冲对象
		e_ReleaseBuffer(pDesBuffer);
		/// 返回NULL
		END_DEBUG_INFO
		return false;
	}
	/// 返回拆分后的数据缓冲对象
	END_DEBUG_INFO
	return true;
}

/// 释放使用的缓冲对象
bool CBufferPool::e_ReleaseBuffer(CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证指针对象
	if(NULL == pNetBuffer)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 定义MAP迭代
	CBufferMapToPtr::iterator ItemPos;
	/// 加锁使用列表
	m_CBusyListCrit.e_Lock();
	ItemPos = m_ArrayBusyBufList.find(pNetBuffer->e_GetBufferID());
	if(ItemPos != m_ArrayBusyBufList.end())
	{
		///移除列表，释放数据
		ItemPos = m_ArrayBusyBufList.erase(ItemPos);
		bRet = true;
	}
	m_CBusyListCrit.e_Unlock();

	/// 如果从使用列表中移除对象成功
	if(true == bRet)
	{
		/// 加入空闲列表
		if(false == i_AddBufferToFreeList(pNetBuffer))
		{
			/// 删除对象
			delete pNetBuffer;
			pNetBuffer = NULL;
		}
	}
	END_DEBUG_INFO
	return bRet;
}

bool CBufferPool::i_AddBufferToFreeList(CNetBuffer*& pNetBuffer)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == pNetBuffer)
	{
		END_DEBUG_INFO
		return bRet;
	}
	/// 加入列表
	m_CFreeListCrit.e_Lock();
	/// 如果无固定列表数量，或者未达到固定数据
	if(NO_NUM_FREE_LIST == m_usFreeBufCount || m_usFreeBufCount > m_ArrayFreeBufList.size())
	{
		pNetBuffer->e_ReSetBuffer();
		m_ArrayFreeBufList.push_back(pNetBuffer);
		bRet = true;
	}
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO
	return bRet;
}

/// 清空数据缓冲列表
void CBufferPool::i_ClearBufferList()
{
	START_DEBUG_INFO
	CNetBuffer* pNetBuffer = NULL;
	CBufferMapToPtr::iterator iterPos;
	m_CFreeListCrit.e_Lock();
	/// 循环释放数据
	int nBufferSize = (int)m_ArrayFreeBufList.size();
	for(int i = 0; i < nBufferSize; i++)
	{
		pNetBuffer = m_ArrayFreeBufList.at(i);
		if(NULL != pNetBuffer)
		{
			delete pNetBuffer;
			pNetBuffer = NULL;
		}
	}
	m_ArrayFreeBufList.clear();
	m_CFreeListCrit.e_Unlock();

	m_CBusyListCrit.e_Lock();
	/// 循环释放数据
	for(iterPos = m_ArrayBusyBufList.begin(); iterPos != m_ArrayBusyBufList.end(); iterPos++)
	{
		/// 获取数据对象
		pNetBuffer = iterPos->second;
		if(NULL != pNetBuffer)
		{
			delete pNetBuffer;
			pNetBuffer = NULL;
		}
	}
	m_ArrayBusyBufList.clear();
	m_CBusyListCrit.e_Unlock();
	END_DEBUG_INFO
}

/// 生成缓冲ID
ULONG CBufferPool::i_BuildNetBufferID()
{
	START_DEBUG_INFO
	m_ulConstomBufferID++;
	/// 返回生成的连接自编号
	END_DEBUG_INFO
	return m_ulConstomBufferID;
}