// NetDemo.cpp : 定义控制台应用程序的入口点。
//

#include "Common/CommTypes.h"
#include "Common/AutoLock.h"
#include "NetCore.h"
#include "Common/LogFile.h"
#include "common/xml/Markup.h"


/// 获取日志打印数据
CLogFileEx* m_pLogFile = new CLogFileEx("Log/NetDemo", CLogFileEx :: DAY);

CNetCore m_CNetCore;

typedef struct _tTestStruct
{
	USHORT	usSize;
	char	szText[1024 * 7];
}T_TestStruct, *LP_TestStruct;

static void e_PrintLog(const char* pszfmt, ...)
{
	/// 临时字符串
	char szTmp[2048] = {0};
	va_list argptr;
	va_start(argptr, pszfmt);
	memset(szTmp, 0x00, sizeof(szTmp));
	vsnprintf_s(szTmp, sizeof(szTmp) - 1, pszfmt, argptr);
	va_end(argptr);

	if(NULL != m_pLogFile)
	{
		m_pLogFile->Log(szTmp);
	}
	else
	{
		printf("%s\r\n", szTmp);
	}
}


/// 处理接收数据
static void e_OnRecvDataCallBack(USHORT usNetDataType, ULONG ulContextID,
	const char* pszClientIP, USHORT usClientPort, void* pData, int nDatalen)
{
	START_DEBUG_INFO;
	/// 收到TCP数据
	if(NDT_TCPData == usNetDataType)
	{
		//char szTemp[2048];
		//memset(szTemp, 0x00, sizeof(szTemp));
		//memcpy(szTemp, pData, nDatalen);

		char* pszData = NULL;
		pszData = (char* )pData;
		/// pszData[nDatalen] = '\0';
		pszData += 5;
		CMarkup XmlTemp;
		if(false == XmlTemp.SetDoc(pszData))
		{
			e_PrintLog("XML 包不完整【收到TCP数据错误】，ClientIP = %s, ClientPort = %d nDatalen = %d, 异常数据:\r\n\r\n %s",
				pszClientIP, usClientPort, nDatalen, pszData);
		}
		//else
		//{
		//	//e_PrintLog("收到TCP数据，ClientIP = %s, ClientPort = %d\r\n",
		//	//	pszClientIP, usClientPort);

		//	/// e_PrintLog("%s", pszData + 3);
		//}
		/// m_CNetCore.e_ITCPSendData(ulContextID, (PBYTE)pData, nDatalen);
	}
	/// 收到UDP数据
	else if(NDT_UDPData == usNetDataType)
	{
		e_PrintLog("收到UDP数据，ClientIP = %s, ClientPort = %d\r\n",
			pszClientIP, usClientPort);

		/// m_CNetCore.e_IUDPSendData(pszClientIP, usClientPort, (PBYTE)pData, nDatalen);
	}
	END_DEBUG_INFO;
}

/// 处理发送完成数据
static void e_OnSendDataCallBack(USHORT usNetDataType, ULONG ulContextID,
	const char* pszClientIP, USHORT usClientPort, void* pData, int nDatalen)
{
	START_DEBUG_INFO
	/// 发送TCP数据
	/*if(NDT_TCPData == usNetDataType)
	{
		e_PrintLog("发送TCP数据，ClientIP = %s, ClientPort = %d, nDatalen = %d\r\n. \r\n\r\n",
			pszClientIP, usClientPort, nDatalen);
	}
	/// 发送UDP数据
	else if(NDT_UDPData == usNetDataType)
	{
		e_PrintLog("发送UDP数据，ClientIP = %s, ClientPort = %d, nDatalen = %d\r\n. \r\n\r\n",
			pszClientIP, usClientPort, nDatalen);
	}*/
	END_DEBUG_INFO
}

/// 处理连接成功通知
static void e_OnConectionCallBack(ULONG ulContextID, const char* pszClientIP, USHORT usClientPort)
{
	START_DEBUG_INFO
	/// 客户端连接成功
	e_PrintLog("客户端连接成功：ClientIP = %s, ClientPort = %d\r\n", pszClientIP, usClientPort);

	/// 测试发送数据
	T_TestStruct sttTest;
	
	strcpy_s(sttTest.szText, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
		"<MANZAIVideo>\r\n"\
		"<Version>1.0</Version>\r\n"\
		"<Sequence>64</Sequence>\r\n"\
		"<CommandType>ASK</CommandType>\r\n"\
		"<Command>LOGIN</Command>\r\n"\
		"<ClientID>ec9e8bf5-b462-4af3-be51-88bbbccd5527</ClientID>\r\n"\
		"</MANZAIVideo>\r\n");
	sttTest.usSize = strlen(sttTest.szText) + 3;

	BYTE BtTemp = 1;

	char szTemp[1024];
	memset(szTemp, 0x00, sizeof(szTemp));
	memcpy(szTemp, &sttTest.usSize, sizeof(USHORT));
	memcpy(szTemp + sizeof(USHORT), &sttTest.usSize, sizeof(USHORT));
	memcpy(szTemp + sizeof(USHORT) * 2, &BtTemp, sizeof(BYTE));
	memcpy(szTemp + sizeof(USHORT) * 2 + 1, sttTest.szText, strlen(sttTest.szText));
	int nSendlen = sttTest.usSize + 2;
	m_CNetCore.e_ITCPSendData(ulContextID, (PBYTE)szTemp, nSendlen);
	/// 释放连接
	/// m_CNetCore.e_ICloseTCPContext(ulContextID);
	END_DEBUG_INFO
}

/// 处理断开连接通知
static void e_OnDisConectionCallBack(ULONG ulContextID, const char* pszClientIP, USHORT usClientPort)
{
	START_DEBUG_INFO
	/// 客户端断开连接
	e_PrintLog("客户端断开连接：ClientIP = %s, ClientPort = %d\r\n", pszClientIP, usClientPort);
	END_DEBUG_INFO
}

int main(int argc, char* argv[])
{
	/// 初始化 Windows 网络模块
	WSADATA wsData;
	if(0 != WSAStartup(MAKEWORD(2, 2), &wsData))
	{
		printf("初始化sockte库错误.", GetLastError());
	}

	T_NetInitStruct sttNetInit;
	sttNetInit.usServerPort = 6880;
	sttNetInit.usServerNetType = NCNT_Both;
	sttNetInit.usMaxIOWorkers = 6;
	sttNetInit.usPendReadsNum = 6;
	sttNetInit.bOrderSend = true;
	sttNetInit.bOrderRead = true;
	sttNetInit.usFreeConnMaxNum = 512;
	sttNetInit.usFreeBufMaxNum = 2048;
	sttNetInit.usUDPRevcNum = 256;
	sttNetInit.bUDPJoinGroup = true;
	strcpy(sttNetInit.szUDPGroupIP, "239.255.0.1");
	/// 启动服务
	m_CNetCore.e_IStartServer(&sttNetInit, e_OnRecvDataCallBack, e_OnSendDataCallBack,
		e_OnConectionCallBack, e_OnDisConectionCallBack, e_PrintLog);

	char szChar;
	while(szChar = getchar())
	{
		if('0' == szChar)
		{
			m_CNetCore.e_IStopdServer();
			break;
		}
		else if('1' == szChar)
		{
			/// 测试大量发包
			ULONG ulNums = 0;
			char szData[2048];
			/// while(0 <= ulNums)
			{
				memset(szData, 0x00, sizeof(szData));
				snprintf(szData, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"\
					"<MANZAIVideo>\r\n"\
					"<Version>1.0</Version>\r\n"\
					"<Sequence>%lu</Sequence>\r\n"\
					"<CommandType>ASK</CommandType>\r\n"\
					"<Command>LOGIN</Command>\r\n"\
					"<ClientID>ec9e8bf5-b462-4af3-be51-88bbbccd5527</ClientID>\r\n"\
					"</MANZAIVideo>\r\n", ++ulNums);

				m_CNetCore.e_IUDPSendData("239.255.0.1", 6880, (PBYTE)szData, 2048);

				/// Sleep(5);
			}
		}
	}

	if(NULL != m_pLogFile)
	{
		delete m_pLogFile;
		m_pLogFile = NULL;
	}
	/// 终止Winsock 2 DLL (Ws2_32.dll) 的使用
	WSACleanup();
	return 0;
}