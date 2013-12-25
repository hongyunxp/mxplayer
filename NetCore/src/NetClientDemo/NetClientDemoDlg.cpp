
// NetClientDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NetClientDemo.h"
#include "NetClientDemoDlg.h"
#include "afxdialogex.h"
#include "common/CommTypes.h"
#include "common/NetCoreDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNetClientDemoDlg 对话框




CNetClientDemoDlg::CNetClientDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CNetClientDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	/// 初始化 Windows 网络模块
	WSADATA wsData;
	if(0 != WSAStartup(MAKEWORD(2, 2), &wsData))
	{
		printf("初始化sockte库错误.", GetLastError());
	}

	
	T_InitNetClient sttNetClient;
	strncpy_s(sttNetClient.szTCPServerIP, "127.0.0.1",
		sizeof(sttNetClient.szTCPServerIP) - 1);
	strncpy_s(sttNetClient.szUDPGroupIP, "239.255.0.2", 
		sizeof(sttNetClient.szUDPGroupIP) - 1);
	sttNetClient.usTCPServerPort = 6880;
	sttNetClient.usLocalUDPPort = 15008;
	sttNetClient.usUDPJoinGroup = 1;
	m_CNetClient.e_IInitNetClient(&sttNetClient, i_OnRecvDataCallBack);
}

void CNetClientDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetClientDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDBTNCONN, &CNetClientDemoDlg::OnBnClickedBtnconn)
	ON_BN_CLICKED(IDBTNDISCONN, &CNetClientDemoDlg::OnBnClickedBtndisconn)
	ON_BN_CLICKED(IDBTNUDPCREAT, &CNetClientDemoDlg::OnBnClickedBtnudpcreat)
	ON_BN_CLICKED(IDBTNUDPDESTORY, &CNetClientDemoDlg::OnBnClickedBtnudpdestory)
	ON_BN_CLICKED(IDBTNCONN2, &CNetClientDemoDlg::OnBnClickedBtnSendTCPString)
	ON_BN_CLICKED(IDBTNCONN4, &CNetClientDemoDlg::OnBnClickedBtnSendTCPBary)
	ON_BN_CLICKED(IDBTNCONN3, &CNetClientDemoDlg::OnBnClickedBtnSendUDPString)
	ON_BN_CLICKED(IDBTNCONN5, &CNetClientDemoDlg::OnBnClickedBtnSendUDPBary)
END_MESSAGE_MAP()


// CNetClientDemoDlg 消息处理程序

BOOL CNetClientDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNetClientDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetClientDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CNetClientDemoDlg::i_OnRecvDataCallBack(USHORT usNetType, const char* pszClientIP, 
	USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
	SHORT sSNum, SHORT sENum, int nDatalen, void* pData)
{
	/// 验证数据合法性
	if(NULL == pszClientIP || 0 >= usClientPort || 0 >= nDatalen)
	{
		return;
	}
	/// 打印日志
	TRACE("NetType = %d, ClientIP = %s, ClientPort = %d, DataType = %d, OBJType = %d, OBJCount = %d, "\
		"SNum = %d, ENum = %d, Datalen = %d", usNetType, pszClientIP, usClientPort, sDataType, nOBJType,
		sOBJCount, sSNum, sENum, nDatalen);

	if(JDT_StringData == sDataType)
	{
		TRACE("收到, String Data ---> %s\r\n", (char* )pData);
	}
	else
	{
		if(COBJT_Test == nOBJType)
		{
			T_TestStruct* pTestStruct = (T_TestStruct* )pData;
			TRACE("COBJT_Test ---> Time = %s, Addr = %s, Name = %s\r\n", pTestStruct->szTime,
				pTestStruct->szAddr, pTestStruct->szName);
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetClientDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CNetClientDemoDlg::OnBnClickedBtnconn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CNetClient.e_IConnectionServer();
}


void CNetClientDemoDlg::OnBnClickedBtndisconn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CNetClient.e_IDisconnectServer();
}


void CNetClientDemoDlg::OnBnClickedBtnudpcreat()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CNetClient.e_ICreatUDPClient();
}


void CNetClientDemoDlg::OnBnClickedBtnudpdestory()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CNetClient.e_IDestroyUDPClient();
}


void CNetClientDemoDlg::OnBnClickedBtnSendTCPString()
{
	// TODO: 在此添加控件通知处理程序代码
	string strSend = "Hello This Is Fway Client Sending TCP String Data.";

	m_CNetClient.e_ISendTCPStringData((char* )strSend.c_str(), strSend.length());
}


void CNetClientDemoDlg::OnBnClickedBtnSendTCPBary()
{
	// TODO: 在此添加控件通知处理程序代码

	T_TestStruct sttTestStruct;
	strncpy_s(sttTestStruct.szTime, "2013-12-12 00:00:00", sizeof(sttTestStruct.szTime) - 1);
	strncpy_s(sttTestStruct.szName, "FWAY TCP", sizeof(sttTestStruct.szName) - 1);
	strncpy_s(sttTestStruct.szAddr, "At Home", sizeof(sttTestStruct.szAddr) - 1);

	m_CNetClient.e_ISendTCPBinaryData(COBJT_Test, sizeof(T_TestStruct), 1, (BYTE* )&sttTestStruct);
}


void CNetClientDemoDlg::OnBnClickedBtnSendUDPString()
{
	// TODO: 在此添加控件通知处理程序代码
	string strSend = "Hello This Is Fway Client Sending UDP String Data.";
	/// UDP地址发送的是组播地址
	m_CNetClient.e_ISendUDPStringData("127.0.0.1", 6880, (char* )strSend.c_str(), strSend.length());
}


void CNetClientDemoDlg::OnBnClickedBtnSendUDPBary()
{
	// TODO: 在此添加控件通知处理程序代码
	T_TestStruct sttTestStruct;
	strncpy_s(sttTestStruct.szTime, "2013-12-12 00:00:00", sizeof(sttTestStruct.szTime) - 1);
	strncpy_s(sttTestStruct.szName, "FWAY UDP", sizeof(sttTestStruct.szName) - 1);
	strncpy_s(sttTestStruct.szAddr, "At Home", sizeof(sttTestStruct.szAddr) - 1);

	m_CNetClient.e_ISendUDPBinaryData("239.255.0.1", 6880, COBJT_Test, sizeof(T_TestStruct), 1, (BYTE* )&sttTestStruct);
}
