/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 环形缓冲池
/// @par 相关文件
/// 引用此文件所有文件头
/// @par 功能详细描述
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         RingBuf.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2013/01/17
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __COMM_RING_BUFFER_H_H__
#define __COMM_RING_BUFFER_H_H__
#include "CommTypes.h"
#include "AutoLock.h"

template <class T>
class CRingBuf
{
public:
	CRingBuf(UINT unSize = TRANSF_STREAM_BUF_SIZE);
	~CRingBuf();
public:
	/// 获取缓冲池可读数据块个数
	UINT i_GetBufSize();
	/// 获取缓冲池可写数据块个数
	UINT i_GetBufSpace();
public:
	/// 读取数据块
	bool e_PutData(T *data, UINT nNum);
	/// 读取数据块
	UINT e_GetData(T *data, UINT nNum, bool bIsTakeAway = true);
	/// 查看数据块
	UINT e_PeekData(T *data, UINT nNum);
	/// 移动可读数据块位置
	bool e_SeekData(UINT nNum);
private:
	/// 读写内存池指针
	T* m_rb;
	/// 指向当前应该读取位置
	UINT m_unRPos;
	/// 指向当前应该写入位置
	UINT m_unWPos;
	/// 缓冲池内存块对象个数
	UINT m_unSize;
	/// 加锁
	CLightCritSec m_LockCrit;
};

template <class T>
CRingBuf<T>::CRingBuf(UINT unSize)
{
	m_unRPos = 0;
	m_unWPos = 0;
	m_unSize = unSize;
	m_rb = new T[m_unSize];
	memset(m_rb, 0x00, m_unSize * sizeof(T));
}

template <class T>
CRingBuf<T>::~CRingBuf()
{
	delete[] m_rb;
	m_rb = NULL;
}

// 数据可读长度
template <class T>
UINT CRingBuf<T>::i_GetBufSize()
{
	CAutoLock AutoLock(&m_LockCrit);
	if((0 <= m_unRPos && m_unRPos < m_unSize) && (0 <= m_unWPos && m_unWPos < m_unSize))
	{
		if(m_unWPos >= m_unRPos)
		{
			return m_unWPos - m_unRPos;
		}
		else
		{
			return m_unSize - m_unRPos + m_unWPos;
		}
	}
	else
	{
		return 0;
	}
}

// 剩余可写空间
template <class T>
UINT CRingBuf<T>::i_GetBufSpace()
{
	return m_unSize - 1 - i_GetBufSize();
}

// 往缓冲区写数据
template <class T>
bool CRingBuf<T>::e_PutData(T* data, UINT unNum)
{
	if(NULL == data || 0 >= unNum)
	{
		return false;
	}

	// 保证有足够的空间
	if(unNum > i_GetBufSpace())
	{
		return false;
	}

	CAutoLock AutoLock(&m_LockCrit);
	if(unNum <= m_unSize - m_unWPos)
	{
		// 缓冲区末尾有足够空间
		memcpy(m_rb + m_unWPos, data, unNum * sizeof(T));
		m_unWPos = (m_unWPos + unNum) % m_unSize;
	}
	else
	{
		// 缓冲区末尾空间不够，分两次复制
		memcpy(m_rb + m_unWPos, data, (m_unSize - m_unWPos) * sizeof(T));
		memcpy(m_rb, data + m_unSize - m_unWPos, (unNum - (m_unSize - m_unWPos)) * sizeof(T));
		m_unWPos = unNum - (m_unSize - m_unWPos);
	}
	return true;
}

// 从缓冲区读数据
template <class T>
UINT CRingBuf<T>::e_GetData(T* data, UINT unNum, bool bIsTakeAway)
{
	/// 定义返回值
	UINT unRet = 0;
	if(NULL == data || 0 >= unNum)
	{
		return unRet;
	}

	CAutoLock AutoLock(&m_LockCrit);
	/// 获取缓冲数量
	unRet = i_GetBufSize();
	if(unNum > unRet)
	{
		return 0;
	}

	/// 赋值数据
	if(m_unWPos >= m_unRPos || unNum <= m_unSize - m_unRPos)
	{
		// 数据连续，或不连续但末尾有足够数据
		memcpy(data, m_rb + m_unRPos, unNum * sizeof(T));
		if(bIsTakeAway)
		{
			m_unRPos = (m_unRPos + unNum) % m_unSize;
		}
	}
	else
	{
		// 数据不连续，且末尾没有足够数据，分两次复制
		memcpy(data, m_rb + m_unRPos, (m_unSize - m_unRPos) * sizeof(T));
		memcpy(data + m_unSize - m_unRPos, m_rb, (unNum - (m_unSize - m_unRPos)) * sizeof(T));
		if(bIsTakeAway)
		{
			m_unRPos = unNum - (m_unSize - m_unRPos);
		}
	}
	return unRet;
}

// 查看缓冲区，不取出
template <class T>
UINT CRingBuf<T>::e_PeekData(T* data, UINT unNum)
{
	return e_GetData(data, unNum, false);
}

// 移动读取的位置
template <class T>
bool CRingBuf<T>::e_SeekData(UINT unNum)
{
	if(0 >= unNum)
	{
		return false;
	}

	CAutoLock AutoLock(&m_LockCrit);
	/// 获取缓冲数量
	UINT unTemp = i_GetBufSize();
	if(unNum > unTemp)
	{
		return false;
	}
	/// 移动可读数据块位置
	if(m_unWPos >= m_unRPos || unNum <= m_unSize - m_unRPos)
	{
		m_unRPos = (m_unRPos + unNum) % m_unSize;
	}
	else
	{
		m_unRPos = unNum - (m_unSize - m_unRPos);
	}
	return true;
}

#endif __COMM_RING_BUFFER_H_H__