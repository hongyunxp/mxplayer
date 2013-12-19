////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 客户端网络通讯业务处理
/// @par 相关文件
/// NetTreatMent.h
/// @par 功能详细描述
/// <控制台应用程序的入口点>
/// @par 多线程安全性
/// <是/否>[否，说明]
/// @par 异常时安全性
/// <是/否>[否，说明]
/// @note         -
/// 
/// @file         NetTreatMent.cpp
/// @brief        <模块功能简述>
/// @author       Li.xl
/// @version      1.0
/// @date         2011/05/25
/// @todo         <将来要作的事情>
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NetTreatMent.h"
#include <fstream>

namespace FWAYNET
{
	CNetTreatment::CNetTreatment()
	{
	}

	CNetTreatment::~CNetTreatment()
	{
	}

	void CNetTreatment::e_SendLoop()
	{
		/// 启动线程处理发送数据
		while(true)
		{
			/// 验证连接合法性
			if(NULL == m_bConnect || 0 >= (int)m_SendBufferArray.size())
			{
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			}

			/// 在缓冲中查找要发送的数据
			T_BufferStruct* pBuffer = NULL;
			if(0 < (int)m_SendBufferArray.size())
			{
				CLightAutoLock lckSendBufferLock(&m_SendBufferLock);
				pBuffer = (T_BufferStruct* )*m_SendBufferArray.begin();
				if(NULL != pBuffer)
				{
					m_SendBufferArray.erase(m_SendBufferArray.begin());
				}
			}
			else
			{
				/// 如果没有找到发送的数据则返回
#ifdef WIN32
				Sleep(1);
#else
				usleep(1000);
#endif
				continue;
			}
			/// 如果数据存在则发送
			if(NULL != pBuffer)
			{
				/// 禁止多线程同时发送数据
				CLightAutoLock lckSend(&m_SendLock);

				e_SendStruct(m_nSocketId, pBuffer->nJobType, pBuffer->nSST, pBuffer->nSize, pBuffer->pBuf);

				if(pBuffer->pBuf)
				{
					delete pBuffer->pBuf;
					pBuffer->pBuf = NULL;
				}
				delete pBuffer;
				pBuffer = NULL;
			}
		}
	}

	void CNetTreatment::e_ReceiveLoop()
	{
		BYTE* pBuffer = NULL;
		BYTE* pBufferTmp = NULL;
		/// 读取获取的数据长度
		long lReadCount = 0;
		/// 数据长度
		long lDataLen = 0;
		/// 数据类型，必须是1个字节大小
		BYTE BtDataType = 0;
		while(true)
		{
			if(0 >= m_nSocketId)
			{
				/// 断线重连
				e_ReconnectServer();
#ifdef WIN32
				Sleep(1000);
#else
				usleep(1000000);
#endif
				continue;
			}

			/// 获取数据长度
			lReadCount = e_ReceiveData(m_nSocketId, (char* )&lDataLen, sizeof(lDataLen));

			/// 没有读取到数据
			if(-1 == lReadCount)
			{
				printf("NetClient Read 1 ReadCount = %d, DataLen = %d This is Error Data\r\n",
					lReadCount, lDataLen);

				/// 断开连接
				e_CloseSocket();
				continue;
			}

			/// 正确读取到了数据长度
			if(0 < lDataLen)
			{
				/// = 先接收一个字节，WINCE下，进行转换结构指针时，地址要按4字节的倍数对齐，
				/// = 因为有个数据头1+4，所以先接收一个字节，后面的转换会按4字节进行对齐了
				/// = 读取数据类型长度
				lReadCount = e_ReceiveData(m_nSocketId, (char* )&BtDataType, sizeof(BtDataType));
				/// = 如果读取到的数据类型长度和本省的数据类型长度不一致则断开连接
				if(lReadCount != sizeof(BtDataType))
				{
					printf("NetClient Read 2 ReadCount = %d, DataLen = %d This is Error Data\r\n",
							lReadCount, sizeof(BtDataType));
					
					/// 断开套接字连接
					e_CloseSocket();
					continue;
				}

				/// 多分配EXTRA_BUFFER_SIZE大小内存
				pBuffer = new BYTE[lDataLen + EXTRA_BUFFER_SIZE];
				if(NULL == pBuffer)
				{
					continue;
				}
				/// = 只对多分配的内存初始化，因为前面的内存一定会被网络传输的数据覆盖；
				/// = 减少memset时间消耗要减去sizeof(DataType)；因为这个是单独接收的，未包含在DataLen内
				memset(&pBuffer[lDataLen - sizeof(BtDataType)], 0x00, EXTRA_BUFFER_SIZE);
			}

			/// 除去已经接收了sizeof(DataType)大小，接收数据内容
			lReadCount = e_ReceiveData(m_nSocketId, (char* )pBuffer, lDataLen-sizeof(BtDataType));
			/// 如果读取的数据不正确，则关闭连接
			if(lReadCount != lDataLen - sizeof(BtDataType))
			{
				printf("NetClient Read 3 ReadCount = %d, DataLen = %d This is Error Data\r\n", 
					lReadCount, lDataLen);
				/// 断开套接字连接
				e_CloseSocket();
				delete[] pBuffer;
				pBuffer = NULL;
				continue;
			}
			/// 数据不正确返回
			if(-1 == lReadCount)
			{
				delete[] pBuffer;
				pBuffer = NULL;
				continue;
			}
			e_ProcessReceiveData(BtDataType, pBuffer);
			if(NULL != pBuffer)
			{
				delete[] pBuffer;
				pBuffer = NULL;
			}
		}
	}

	void CNetTreatment::e_ProcessReceiveData(BYTE BtDataType, void* pData)
	{
		UINT nStructType = 0;
		BYTE* pBtDataTmp = (BYTE* )pData;
		
		switch((_eJobDataType)BtDataType)
		{
		case JDT_TextData:
			{
				/// 处理文本信息
				break;
			}
		case JDT_StructData:
		case JDT_ArrStructData:
			{
				/// 重置发送数据时间
				m_tSendTime = 0;

				memcpy(&nStructType, pBtDataTmp, sizeof(nStructType));
				/// 数据指针跳过结构体类型数据，指向数据内容
				pBtDataTmp += sizeof(nStructType);
				/// = DataLen为数据包数据大小，减去BYTE DataType数据包类型，减去sizeof(StructType)结构体类型，
				/// = 结果为数据内容实际大小
				e_PackageReceiveStruct(nStructType, pBtDataTmp);
				break;
			}
		}
	}

	void CNetTreatment::e_PackageReceiveStruct(UINT nSendStructType, void* pData)
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

	/// 得到当前运行目录路径
	string CNetTreatment::e_GetFilePath()
	{
		/// 定义获取字符字符串
		char szExePath[512];
		memset(szExePath, 0x00, sizeof(szExePath));
		/// 获取定义的地址长度
		int nPathLen = sizeof(szExePath);
		/// 定义获取返回的变量
		DWORD ldwReturn = 0;
		/// 定义字符指针
		char* pPos = NULL;
		/// 定义路径间隔目录字符
		char cInchar = '\\';

		/// 获取路径
#ifndef WIN32
		ldwReturn = readlink( "/proc/self/exe", szExePath, nPathLen);
		cInchar = '/';
#else
		ldwReturn = GetModuleFileNameA(NULL, szExePath, nPathLen);
		cInchar = '\\';
#endif

		if(0 < (int)ldwReturn && nPathLen > (int)ldwReturn)
		{
			pPos = strrchr(szExePath, cInchar);
			if(NULL != pPos)
			{
				*++pPos = '\0';
			}
		}
		/// 处理返回获取到的路径字符串
		string strExePath = szExePath;
		return strExePath;
	}

	/// 获取配置文件中指定连接的Ip
	bool CNetTreatment::e_GetNetServiceIp(char* pszServiceIp, int nSizeIp)
	{
		START_DEBUG_INFO;
		/// 获取交易Ip
		if(NULL == pszServiceIp)
		{
			END_DEBUG_INFO
			return false;
		}
		/// 获取路径
		string strFilePath = e_GetFilePath();

		strFilePath += "ClientConfig.ini";
		/// 打开配置文件准备读取
		ifstream fsln(strFilePath.c_str());

		/// 定义获取配置数据一行的字符串
		string strLine = "";
		strLine.clear();
		/// 定义获取配置的中间变量字符串
		string strTemp,strTempThe;
		strTemp.clear();
		strTempThe.clear();

		int nstrIndex = 0;
		if(NULL != fsln)
		{
			while(getline(fsln, strLine))
			{
				if(0 < strLine.length())
				{
					nstrIndex = 0;
					if(0 >= strlen(pszServiceIp))
					{
						nstrIndex = strLine.find("NetSerciceIP:");
						/// 如果找到字符结尾都没有找到
						if(nstrIndex != string::npos)
						{
							strTempThe = "NetSerciceIP:";
							nstrIndex += strTempThe.length();
							strTemp = strLine.substr(nstrIndex, strLine.length());
							/// 去掉字符串首尾的空格
							strTemp.erase(strTemp.find_last_not_of(" ") + 1);
							strTemp.erase(0, strTemp.find_first_not_of(" "));
							snprintf(pszServiceIp, nSizeIp, "%s", strTemp.c_str());
							continue;
						}
					}
				}
			}
		}
		else
		{
			printf("\r\n读取连接配置文件失败...\r\n");
			END_DEBUG_INFO;
			return false;
		}

		if('\0' == pszServiceIp[0] || '\r' == pszServiceIp[0] || '\n' == pszServiceIp[0])
		{
			END_DEBUG_INFO;
			return false;
		}


		END_DEBUG_INFO;
		return true;
	}
}