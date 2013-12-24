
// NetClientDemoDlg.h : 头文件
//

#pragma once

#include "NetClient/NetClient.h"


// CNetClientDemoDlg 对话框
class CNetClientDemoDlg : public CDialogEx
{
// 构造
public:
	CNetClientDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_NETCLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnconn();
	afx_msg void OnBnClickedBtndisconn();
	afx_msg void OnBnClickedBtnudpcreat();
	afx_msg void OnBnClickedBtnudpdestory();
	afx_msg void OnBnClickedBtnSendTCPString();
	afx_msg void OnBnClickedBtnSendTCPBary();
	afx_msg void OnBnClickedBtnSendUDPString();
	afx_msg void OnBnClickedBtnSendUDPBary();

private:
	/// ==============================================
	/// @par 功能 
	/// 处理接收到的数据
	/// @param 
	/// [in,out]	<参数名>		<参数说明>
	///	[in]		usNetType		发送和接收数据的网络类型 参见_eNetTransfType
	/// [in]		pszClientIP		客户端IP
	/// [in]		usClientPort	客户端端口号
	/// [in]		sDataType		数据类型
	/// [in]		nOBJType		对象/结构类型
	/// [in]		sOBJCount		对象/结构总数
	/// [in]		sSNum			对象/结构数组开始数量(数据列表拆包时与sOBJCount配合使用)
	/// [in]		sENum			对象/结构数组结束数量(数据列表拆包是与sOBJCount配合使用)
	/// [in]		nDatalen		数据长度
	/// @return 	-
	/// @note 		Creat By li.xl 2013/12/21 
	/// ==============================================
	static void i_OnRecvDataCallBack(USHORT usNetType, const char* pszClientIP, 
		USHORT usClientPort, SHORT sDataType, int nOBJType, SHORT sOBJCount, 
		SHORT sSNum, SHORT sENum, int nDatalen, void* pData);

private:
	/// 获取客户端网络模块
	CNetClient m_CNetClient;
};
