
// CopyToolDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CCopyToolDlg 对话框
class CCopyToolDlg : public CDialogEx
{
// 构造
public:
	CCopyToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COPYTOOL_DIALOG };
#endif

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
public://手动定义变量
	CStringList m_stringList;
	int m_rowIndex;	//选中行号
	int m_rowAuto;	//数据行号
	bool m_Copy;	
	bool m_Paste;
	bool m_Lock;	//锁定
	bool m_bAddHead;	//是否添加首部字符
	bool m_bAddTail;	//是否添加尾部字符
	CToolBar m_wndMyToolBar;	//添加一个CToolBar选件类
	CImageList m_imageList;	//图像列表
	afx_msg LRESULT OnClickTray(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);  // 工具条提示  
	//
	void CheckBoxCheck();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnDrawClipboard();
	afx_msg void OnDestroy();
	CListCtrl m_list;
	// //添加条目
	void AddListItem(CString itemString);
	// 删除条目
	void DeleteListItem(int index);
	// 清空列表
	void ClearList();
	afx_msg void OnBnClickedBtnLock();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnClear();
	
	// //绑定数据
	void BindList();
	afx_msg void OnClickListItem(NMHDR *pNMHDR, LRESULT *pResult);
	// 设置文本到剪贴板
	void CopyTextToClipboard(CString sSource);
	// 复制剪贴板文本
	CString GetTextFromClipboard();
	// 循环获取条目
	CString GetListItemAutoMove();
	CButton m_apply;
	CButton m_stop;
	afx_msg void OnTrayShow();
	afx_msg void OnTrayExit();
	void ShowTop();
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg void OnBnClickedCheckHead();
	afx_msg void OnBnClickedCheckTail();
	CString AppendCharactor(CString srcString);
	CComboBox m_comboSpitChar;
	afx_msg void OnBnClickedBtnTest();
	void StringListAdd(CString sSource);
	CComboBox m_comboAppend;
};
