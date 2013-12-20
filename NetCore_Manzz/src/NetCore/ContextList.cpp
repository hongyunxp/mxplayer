////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端连接列表管理对象
/// @par 相关文件
/// ContextList.h
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         ContextList.cpp
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/06/14
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ContextList.h"

CContextList::CContextList()
{
	START_DEBUG_INFO
	m_ulContextID = 0;
	m_usConFreeNum = NO_NUM_FREE_LIST;
	m_ContextFreeArray.clear();
	m_ContextBusyArray.clear();
	m_pBufferManager = NULL;
	END_DEBUG_INFO
}

/// 创建指定数量的列表构造函数
CContextList::CContextList(USHORT usConFreeNum)
{
	START_DEBUG_INFO
	m_ulContextID = 0;
	m_ContextFreeArray.clear();
	m_ContextBusyArray.clear();
	m_pBufferManager = NULL;
	/// 赋值列表固定数量
	m_usConFreeNum = usConFreeNum;
	/// 构造数据
	for(USHORT i = 0; i < m_usConFreeNum; i++)
	{
		T_ClientContext* psttContext = new T_ClientContext();
		if(NULL != psttContext)
		{
			/// 增加数据到列表
			m_ContextFreeArray.push_back(psttContext);
		}
	}
	END_DEBUG_INFO
}

CContextList::~CContextList()
{
	START_DEBUG_INFO
	/// 释放数据列表
	i_ClearContextList();
	m_pBufferManager = NULL;
	END_DEBUG_INFO
}

/// 初始化连接信息
void CContextList::e_InitContextList(CBufferPool* pBufferPool)
{
	START_DEBUG_INFO
	/// 验证数据合法性
	if(NULL != pBufferPool)
	{
		m_pBufferManager = pBufferPool;
	}
	END_DEBUG_INFO
}

/// 设置空闲连接数量
void CContextList::e_SetFreeConNum(USHORT usConFreeNum)
{
	START_DEBUG_INFO;
	/// 赋值列表固定数量
	m_usConFreeNum = usConFreeNum;
	m_CFreeListCrit.e_Lock();
	if(m_usConFreeNum > m_ContextFreeArray.size())
	{
		USHORT usCreatNum = m_usConFreeNum - m_ContextFreeArray.size();
		/// 构造数据
		for(USHORT i = 0; i < usCreatNum; i++)
		{
			T_ClientContext* psttContext = new T_ClientContext();
			if(NULL != psttContext)
			{
				/// 增加数据到列表
				m_ContextFreeArray.push_back(psttContext);
			}
		}
	}
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO;
}

/// 获取空闲连接列表大小
USHORT CContextList::e_GetFreeListSize()
{
	START_DEBUG_INFO
	USHORT usListSize = 0;
	m_CFreeListCrit.e_Lock();
	usListSize = m_ContextFreeArray.size();
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO
	return usListSize;
}

/// 获取使用连接列表大小
USHORT CContextList::e_GetBusyListSize()
{
	START_DEBUG_INFO
	USHORT usListSize = 0;
	m_CBusyListCrit.e_Lock();
	usListSize = m_ContextBusyArray.size();
	m_CBusyListCrit.e_Unlock();
	END_DEBUG_INFO
	return usListSize;
}

/// 从列表中获取可用连接信息
bool CContextList::e_AllocateConntext(T_ClientContext*& psttContext)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 初始化
	psttContext = NULL;
	CContextPtrArray::iterator	ItemFreePos;
	/// 获取空闲的连接信息对象
	m_CFreeListCrit.e_Lock();
	if(0 < m_ContextFreeArray.size())
	{
		ItemFreePos = m_ContextFreeArray.begin();
		psttContext = *ItemFreePos;
		m_ContextFreeArray.erase(ItemFreePos);
	}
	else
	{
		psttContext = new T_ClientContext();
	}
	m_CFreeListCrit.e_Unlock();

	/// 增加信息对象到使用连接列表
	if(NULL != psttContext)
	{
		m_CBusyListCrit.e_Lock();
		/// 获取连接ID
		psttContext->ulContextID = i_BuildContextID();
		/// 增加引用计数
		psttContext->e_EnterRefNum();
		/// 赋值数据变量
		m_ContextBusyArray[psttContext->ulContextID] = psttContext;
		/// 赋值返回值
		bRet = true;
		m_CBusyListCrit.e_Unlock();
	}
	END_DEBUG_INFO
	return bRet;
}

/// 释放使用连接信息
bool CContextList::e_ReleaseConntext(T_ClientContext*& psttContext)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证指针对象
	if(NULL == psttContext)
	{
		END_DEBUG_INFO
		return bRet;
	}

	/// 加锁使用列表
	m_CBusyListCrit.e_Lock();
	CContextMapULONGToPtr::iterator ItemPos;
	ItemPos = m_ContextBusyArray.find(psttContext->ulContextID);
	if(ItemPos != m_ContextBusyArray.end())
	{
		if(0 == psttContext->e_ExitRefNum())
		{
			/// 验证数据合法性
			if(NULL != m_pBufferManager)
			{
				m_pBufferManager->e_ReleaseBuffer(psttContext->pCurrentBuffer);
			}
			psttContext->pCurrentBuffer = NULL;
			/// 释放列表
			i_ReleaseBufferMap(&psttContext->CReadBufferMap);
			psttContext->CReadBufferMap.clear();
			i_ReleaseBufferMap(&psttContext->CSendBufferMap);
			psttContext->CSendBufferMap.clear();
			///移除列表，释放数据
			m_ContextBusyArray.erase(ItemPos);
			bRet = true;
		}
	}
	m_CBusyListCrit.e_Unlock();

	/// 如果从使用列表中移除对象成功
	if(true == bRet)
	{
		/// 加入空闲列表
		if(false == i_AddContextToFreeList(psttContext))
		{
			/// 删除对象
			delete psttContext;
			psttContext = NULL;
		}
	}
	END_DEBUG_INFO
	return bRet;
}

/// 从列表中获取可用连接信息
T_ClientContext* CContextList::e_GetConntext(ULONG ulContextID)
{
	START_DEBUG_INFO
	/// 初始化
	T_ClientContext* psttContext = NULL;
	CContextMapULONGToPtr::iterator ItemPos;
	/// 加锁列表
	m_CBusyListCrit.e_Lock();
	ItemPos = m_ContextBusyArray.find(ulContextID);
	if(ItemPos != m_ContextBusyArray.end())
	{
		/// 赋值数据
		psttContext = ItemPos->second;
		/// 增加引用计数
		psttContext->e_EnterRefNum();
	}
	m_CBusyListCrit.e_Unlock();
	END_DEBUG_INFO
	return psttContext;
}

/// 增加连接信息到空闲列表
bool CContextList::i_AddContextToFreeList(T_ClientContext* psttContext)
{
	START_DEBUG_INFO
	bool bRet = false;
	/// 验证数据合法性
	if(NULL == psttContext)
	{
		END_DEBUG_INFO
		return bRet;
	}
	/// 加入列表
	m_CFreeListCrit.e_Lock();
	/// 如果无固定列表数量，或者未达到固定数据
	if(NO_NUM_FREE_LIST == m_usConFreeNum || m_usConFreeNum > m_ContextFreeArray.size())
	{
		/// 重置数据信息结构
		psttContext->e_InitStruct();
		m_ContextFreeArray.push_back(psttContext);
		bRet = true;
	}
	m_CFreeListCrit.e_Unlock();
	END_DEBUG_INFO
	return bRet;
}

/// 清空连接信息列表
void CContextList::i_ClearContextList()
{
	START_DEBUG_INFO
	T_ClientContext* psttContext = NULL;
	CContextMapULONGToPtr::iterator iterPos;
	m_CFreeListCrit.e_Lock();
	/// 获取列表大小
	USHORT usFreeSize = m_ContextFreeArray.size();
	/// 循环释放数据
	for(USHORT i = 0; i < usFreeSize; i++)
	{
		/// 获取数据对象
		psttContext = m_ContextFreeArray.at(i);
		if(NULL != psttContext)
		{
			delete psttContext;
			psttContext = NULL;
		}
	}
	m_ContextFreeArray.clear();
	m_CFreeListCrit.e_Unlock();

	m_CBusyListCrit.e_Lock();
	/// 循环释放数据
	for(iterPos = m_ContextBusyArray.begin(); iterPos != m_ContextBusyArray.end(); iterPos++)
	{
		/// 获取数据对象
		psttContext = iterPos->second;
		if(NULL != psttContext)
		{
			delete psttContext;
			psttContext = NULL;
		}
	}
	m_ContextBusyArray.clear();
	m_CBusyListCrit.e_Unlock();
	END_DEBUG_INFO
}

/// 生成连接信息ID
ULONG CContextList::i_BuildContextID()
{
	START_DEBUG_INFO
	if(0 == ++m_ulContextID)
	{
		m_ulContextID++;
	}
	/// 返回生成的连接自编号
	END_DEBUG_INFO
	return m_ulContextID;
}

void CContextList::i_ReleaseBufferMap(CBufferMapToPtr* pBufferMap)
{
	START_DEBUG_INFO;
	/// 验证数据合法性
	if(NULL == m_pBufferManager || NULL == pBufferMap)
	{		
		END_DEBUG_INFO
		return;
	}

	CNetBuffer* pNetBuffer = NULL;
	CBufferMapToPtr::iterator ItemPos;
	for(ItemPos = pBufferMap->begin(); ItemPos != pBufferMap->end(); ItemPos++)
	{
		pNetBuffer = ItemPos->second;
		if(NULL != pNetBuffer)
		{
			/// 释放数据缓冲对象
			m_pBufferManager->e_ReleaseBuffer(pNetBuffer);
		}
	}
	END_DEBUG_INFO;
}