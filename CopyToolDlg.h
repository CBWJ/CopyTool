
// CopyToolDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CCopyToolDlg �Ի���
class CCopyToolDlg : public CDialogEx
{
// ����
public:
	CCopyToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COPYTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public://�ֶ��������
	CStringList m_stringList;
	int m_rowIndex;	//ѡ���к�
	int m_rowAuto;	//�����к�
	bool m_Copy;	
	bool m_Paste;
	bool m_Lock;	//����
	bool m_bAddHead;	//�Ƿ�����ײ��ַ�
	bool m_bAddTail;	//�Ƿ����β���ַ�
	CToolBar m_wndMyToolBar;	//���һ��CToolBarѡ����
	CImageList m_imageList;	//ͼ���б�
	afx_msg LRESULT OnClickTray(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);  // ��������ʾ  
	//
	void CheckBoxCheck();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnDrawClipboard();
	afx_msg void OnDestroy();
	CListCtrl m_list;
	// //�����Ŀ
	void AddListItem(CString itemString);
	// ɾ����Ŀ
	void DeleteListItem(int index);
	// ����б�
	void ClearList();
	afx_msg void OnBnClickedBtnLock();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnClear();
	
	// //������
	void BindList();
	afx_msg void OnClickListItem(NMHDR *pNMHDR, LRESULT *pResult);
	// �����ı���������
	void CopyTextToClipboard(CString sSource);
	// ���Ƽ������ı�
	CString GetTextFromClipboard();
	// ѭ����ȡ��Ŀ
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
