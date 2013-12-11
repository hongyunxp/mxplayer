/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// @par 模块名
/// 日志文件操作模块
/// @par 相关文件
/// 引用此文件所有头文件
/// @par 功能详细描述
/// = class CLogFile;//用户定义日志文件名
///	= class CLogFileEx;//有日志文件名自动生成功能 , 可分年月日频率生成文件名, 可指定日志存放的目录
///	= CLogFile gLog("My.Log");
///	= gLog.Log("test", 4);//记录日志
///	= gLog.Log("系统启动");
///	= CLogFileEx gLog(".", CLogFileEx :: YEAR);			//一年生成一个日志文件
///	= CLogFileEx gLog(".\\Log", CLogFileEx :: MONTH);	//一月生成一个日志文件
///	= CLogFileEx gLog(".\\Log", CLogFileEx :: DAY);		//一天生成一个日志文件
/// = 注意日志所属目录创建失败会自动退出， 请注意目录的合法性， 文件生成频率看情况掌握
/// = 24小时运行的程序可以每天生成一个日志文件， 以免内容过多
/// @par 多线程安全性
/// [否，说明]
/// @par 异常时安全性
/// [否，说明]
/// @note         -
/// @file         LogFile.h
/// @brief        -
/// @author       Li.xl
/// @version      1.0
/// @date         2012/12/04
/// @todo         -
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LOG_FILE_H_H__
#define __LOG_FILE_H_H__

#include <assert.h>
#include <time.h>
#include "PublicDef.h"
using namespace PUBLICDEF;

class CLogFile
{
protected:

	CRITICAL_SECTION _csLock;
	char * _szFileName;
	HANDLE _hFile;

	bool OpenFile(char* filePath)//打开文件， 指针到文件尾
	{
		if(IsOpen())
			return true;

		if(!filePath)
			return false;

		_hFile =  CreateFile(
			filePath, 
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL 
			);

		if(!IsOpen() && 2 == GetLastError())
		{
			//打开不成功， 且因为文件不存在， 创建文件
			_hFile =  CreateFile(
				filePath, 
				GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL 
				);
		}

		if(IsOpen())
		{
			SetFilePointer(_hFile, 0, NULL, FILE_END);
		}

		return IsOpen();
	}

	DWORD Write(LPCVOID lpBuffer, DWORD dwLength)
	{
		DWORD dwWriteLength = 0;

		if(IsOpen())
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);

		return dwWriteLength;
	}

	virtual void WriteLog( LPCVOID lpBuffer, DWORD dwLength)//写日志, 可以扩展修改
	{
		/// time_t now;
		/// char temp[32];
		DWORD dwWriteLength;
		if(IsOpen())
		{
			//time(&now);
			//strftime(temp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
			string strTime;
			e_GetSysTime(strTime);
			WriteFile(_hFile, strTime.c_str(), 24, &dwWriteLength, NULL);
			WriteFile(_hFile, "      \xd\xa", 6, &dwWriteLength, NULL);
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
			WriteFile(_hFile, "\xd\xa", 2, &dwWriteLength, NULL);
			printf("%s : %s\r\n\r\n", strTime.c_str(), lpBuffer);
			//此句加在这里极度影响速度
			//FlushFileBuffers(_hFile);
		}
	}

	void Lock()  { ::EnterCriticalSection(&_csLock); }
	void Unlock() { ::LeaveCriticalSection(&_csLock); }

public:

	CLogFile(const char *szFileName = "Log.log")//设定日志文件名
	{
		_szFileName = NULL;
		_hFile = INVALID_HANDLE_VALUE;
		::InitializeCriticalSection(&_csLock);
		//InitializeCriticalSectionAndSpinCount(&_csLock,4000);  

		SetFileName(szFileName);
	}

	virtual ~CLogFile()
	{
		::DeleteCriticalSection(&_csLock);
		Close();

		if(_szFileName)
		{
			delete []_szFileName;
			_szFileName = NULL;
		}

	}

	const char * GetFileName()
	{
		return _szFileName;
	}

	void SetFileName(const char *szName)//修改文件名， 同时关闭上一个日志文件
	{
		assert(szName);

		if(_szFileName)
		{
			delete []_szFileName;
			_szFileName = NULL;
		}

		Close();

		_szFileName = new char[strlen(szName)+1];
		assert(_szFileName);
		strcpy(_szFileName, szName);
	}

	bool IsOpen()
	{
		return _hFile != INVALID_HANDLE_VALUE;
	}

	void Close()
	{
		if(IsOpen())
		{
			FlushFileBuffers(_hFile);
			CloseHandle(_hFile);
			_hFile = INVALID_HANDLE_VALUE;
		}
	}

	void Log(LPCVOID lpBuffer, DWORD dwLength)//追加日志内容
	{
		assert(lpBuffer);
		__try
		{
			Lock();

			if(!OpenFile(_szFileName))
				return;

			WriteLog(lpBuffer, dwLength);
		}
		__finally
		{
			Unlock();
		} 
	}

	void Log(const char *szText)
	{
		Log(szText,(DWORD)strlen(szText));
	}

private://屏蔽函数

	CLogFile(const CLogFile&);
	CLogFile&operator = (const CLogFile&);
};

class CLogFileEx : public CLogFile
{
protected:

	char *_szPath;
	char m_szPath[256];
	char _szLastDate[11];
	int _iType;

	void SetPath(const char* szPath)
	{
		assert(szPath);
		char temp[MAX_PATH + 1] = {0};
		strcpy(temp, szPath);
		/// 目录不存在
		if(-1 == CreatDir(temp))
		{
			printf("The Class CLogFileEx Creat Log Path error.\r\n");
			exit(1);
		}
		else
		{
			GetFullPathName(szPath, MAX_PATH, temp, NULL);
			_szPath = new char[strlen(temp) + 256];
			assert(_szPath);
			strcpy(_szPath, temp);
		}
	}

public:

	enum LOG_TYPE{YEAR = 0, MONTH = 1, DAY = 2};

	CLogFileEx(const char* szPath = ".", LOG_TYPE iType = MONTH)
	{
		_szPath = NULL;
		memset(m_szPath, 0x00, sizeof(m_szPath));
		strncpy(m_szPath, szPath, sizeof(m_szPath) - 1);
		SetPath(m_szPath);
		_iType = iType;
		memset(_szLastDate, 0x00, sizeof(_szLastDate));
	}

	~CLogFileEx()
	{
		if(_szPath)
			delete []_szPath;
	}

	const char * GetPath()
	{
		return _szPath;
	}

	virtual void Log(LPCVOID lpBuffer, DWORD dwLength)
	{
		assert(lpBuffer);

		char szTemp[20];
		static const char format[3][20] = {"%Y", "%Y-%m", "%Y-%m-%d %H-%M-%S"};

		__try
		{
			Lock();

			time_t now = time(NULL);

			strftime(szTemp, 20, format[_iType], localtime(&now));

			if(strncmp(_szLastDate, szTemp, sizeof(_szLastDate) - 1) != 0)//更换文件名
			{
				/// 赋值最后一次时间切换值
				strncpy(_szLastDate, szTemp, sizeof(_szLastDate) - 1);
				strncpy(_szPath, m_szPath, sizeof(m_szPath) - 1);
				strcat(_szPath,"\\");
				strcat(_szPath, szTemp);
				strcat(_szPath,".log");
				Close();
			}

			if(!OpenFile(_szPath))
			{
				return;
			}

			WriteLog(lpBuffer, dwLength);
		}
		__finally
		{
			Unlock();
		}
	}

	virtual void Log(const char *szText)
	{
		Log(szText, (DWORD)strlen(szText));
	}

	void LogEx(char *fmt, ...)
	{
		char s[512];
		va_list argptr;

		va_start(argptr, fmt);
		vsprintf(s, fmt, argptr);
		va_end(argptr);

		Log(s, (DWORD)strlen(s));
	}

private://屏蔽函数

	CLogFileEx(const CLogFileEx&);
	CLogFileEx&operator = (const CLogFileEx&);

};

#endif /// __LOG_FILE_H_H__