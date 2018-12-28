
// CopyToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CopyTool.h"
#include "CopyToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//�ȼ�����
#define IDH_F1 4001
#define IDH_F3 4002
//��Ϣ
#define WM_NOTIFYICON WM_USER+1

//���
static HWND hwndNextViewer;
//��С������
NOTIFYICONDATA gNotifyIcon;
ATOM atom = GlobalAddAtom(L"F1");
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCopyToolDlg �Ի���



CCopyToolDlg::CCopyToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_COPYTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_rowIndex = -1;
	m_Copy = false;
	m_Paste = false;
	m_rowAuto = 0;
	m_Lock = false;
}

void CCopyToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEM, m_list);
	DDX_Control(pDX, IDC_BTN_APPLY, m_apply);
	DDX_Control(pDX, IDC_BTN_STOP, m_stop);
	DDX_Control(pDX, IDC_COMBO_SPLITCHAR, m_comboSpitChar);
	DDX_Control(pDX, IDC_COMBO_APPEND, m_comboAppend);
}

BEGIN_MESSAGE_MAP(CCopyToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CCopyToolDlg::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_STOP, &CCopyToolDlg::OnBnClickedBtnStop)
	ON_WM_DRAWCLIPBOARD()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOCK, &CCopyToolDlg::OnBnClickedBtnLock)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CCopyToolDlg::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CCopyToolDlg::OnBnClickedBtnClear)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ITEM, &CCopyToolDlg::OnClickListItem)
	ON_MESSAGE(WM_NOTIFYICON, OnClickTray)
	ON_COMMAND(ID_TRAY_SHOW, &CCopyToolDlg::OnTrayShow)
	ON_COMMAND(ID_TRAY_EXIT, &CCopyToolDlg::OnTrayExit)
	ON_WM_HOTKEY()
	//ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xffff, &CCopyToolDlg::OnToolTipText)
	ON_BN_CLICKED(ID_BTN_START, &CCopyToolDlg::OnBnClickedBtnApply)
	ON_BN_CLICKED(ID_BTN_STOP, &CCopyToolDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_CHECK_HEAD, &CCopyToolDlg::OnBnClickedCheckHead)
	ON_BN_CLICKED(IDC_CHECK_TAIL, &CCopyToolDlg::OnBnClickedCheckTail)
	ON_BN_CLICKED(IDC_BTN_TEST, &CCopyToolDlg::OnBnClickedBtnTest)
END_MESSAGE_MAP()


// CCopyToolDlg ��Ϣ�������

BOOL CCopyToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	//�Զ��幤��������
	if (!m_wndMyToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndMyToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//ͼ���б��ʼ��
	m_imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 0);
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON_START));
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON_STOP));
	//ͼ���б�͹���������
	m_wndMyToolBar.GetToolBarCtrl().SetImageList(&m_imageList);
	m_wndMyToolBar.ShowWindow(SW_SHOW);
	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_STOP, false);
	//�ؼ�����λ  
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//��ӳ�Ϊ������鿴��
	hwndNextViewer = SetClipboardViewer();

	//�б��ʼ��
	m_list.SetExtendedStyle(m_list.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT
		| LVS_EX_GRIDLINES);
	CRect rt;
	m_list.GetWindowRect(&rt);
	m_list.InsertColumn(0, _T("���"), LVCFMT_CENTER, 40);
	m_list.InsertColumn(1, _T("��������"), LVCFMT_CENTER, rt.Width() - 70);
	//��ť״̬
	m_apply.EnableWindow();
	m_stop.EnableWindow(FALSE);
	//��������ͼ��
	CString sText;
	this->GetWindowText(sText);
	gNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	gNotifyIcon.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	gNotifyIcon.hWnd = m_hWnd;
	lstrcpy(gNotifyIcon.szTip, sText);
	gNotifyIcon.uCallbackMessage = WM_NOTIFYICON;
	gNotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &gNotifyIcon);

	//��ѡ����
	CheckBoxCheck();
	m_comboSpitChar.AddString(L"");
	m_comboSpitChar.AddString(L"�Ʊ��");
	m_comboSpitChar.AddString(L"���з�");
	m_comboAppend.AddString(L"");
	m_comboAppend.AddString(L"�Ʊ��");
	m_comboAppend.AddString(L"���з�");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCopyToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCopyToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCopyToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CCopyToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_HOTKEY == pMsg->message)
	{
		CWnd *pWnd = NULL;
		CString s;
		CTime time = CTime::GetCurrentTime();

		switch (pMsg->wParam)
		{
		case IDH_F1:
			pWnd = (CWnd*)GetDlgItem(IDC_STATIC_TIP);
			pWnd->SetWindowText(time.Format(L"F1 %H:%M:%S"));
			m_Copy = true;
			pWnd = GetActiveWindow();
			if (!pWnd)
				pWnd = GetTopWindow();
			if (pWnd)
			{
				// ģ�ⰴ��ALT+F4
				/*keybd_event(18, 0, 0, 0);
				keybd_event(115, 0, 0, 0);
				keybd_event(18, 0, KEYEVENTF_KEYUP, 0);
				keybd_event(115, 0, KEYEVENTF_KEYUP, 0);*/
				// ģ�ⰴ��CTRL+C
				keybd_event(17, 0, 0, 0);
				keybd_event(67, 0, 0, 0);
				keybd_event(17, 0, KEYEVENTF_KEYUP, 0);
				keybd_event(67, 0, KEYEVENTF_KEYUP, 0);
			}
			//ShowWindow(SW_SHOW);
			ShowTop();
			break;
		case IDH_F3:
				pWnd = (CWnd*)GetDlgItem(IDC_STATIC_TIP);
				pWnd->SetWindowText(time.Format(L"F3 %H:%M:%S"));
				m_Paste = true;
				if (m_stringList.GetCount() > 0) {
					if(m_Lock){
						s = GetListItemAutoMove();
						BindList();
					}
					else
					{
						s = m_stringList.GetHead();
						POSITION pos = m_stringList.GetHeadPosition();
						m_stringList.RemoveAt(pos);
						BindList();
					}
					CopyTextToClipboard(AppendCharactor(s));
				}
			//// ģ�ⰴ��CTRL+V
			//keybd_event(17, 0, 0, 0);
			//keybd_event(86, 0, 0, 0);
			//keybd_event(17, 0, KEYEVENTF_KEYUP, 0);
			//keybd_event(86, 0, KEYEVENTF_KEYUP, 0);
			break;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCopyToolDlg::OnBnClickedBtnApply()
{
	//ע���ȼ�
	RegisterHotKey(this->GetSafeHwnd(), IDH_F1, MOD_NOREPEAT, VK_F1);
	RegisterHotKey(this->GetSafeHwnd(), IDH_F3, MOD_NOREPEAT, VK_F3);
	BOOL bFlag = m_stop.IsWindowEnabled();
	m_stop.EnableWindow(!bFlag);
	m_apply.EnableWindow(bFlag);

	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_START, bFlag);
	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_STOP, !bFlag);
}


void CCopyToolDlg::OnBnClickedBtnStop()
{
	//ж���ȼ�
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F1);
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F3);
	BOOL bFlag = m_apply.IsWindowEnabled();
	m_apply.EnableWindow(!bFlag);
	m_stop.EnableWindow(bFlag);

	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_START, !bFlag);
	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_STOP, bFlag);
}


void CCopyToolDlg::OnDrawClipboard()
{
	CDialogEx::OnDrawClipboard();
	if (m_Paste)
	{
		// ģ�ⰴ��CTRL+V
		keybd_event(17, 0, 0, 0);
		keybd_event(86, 0, 0, 0);
		keybd_event(17, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(86, 0, KEYEVENTF_KEYUP, 0);
		m_Paste = false;
	}
	if (m_Copy)
	{
		CString s = GetTextFromClipboard();
		//m_stringList.AddTail(s);
		//�ж��Ƿ���Ҫ�ָ��ַ�
		StringListAdd(s);
		BindList();
		m_Copy = false;
		//ShowTop();
	}
}


void CCopyToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	//ж���ȼ�
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F1);
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F3);
}


// //�����Ŀ
void CCopyToolDlg::AddListItem(CString itemString)
{
	int count = m_list.GetItemCount();
	CString sTemp;
	if(m_Lock){
		if(count == m_rowAuto)
			sTemp.Format(L"%d >", count + 1);
		else {
			sTemp.Format(L"%d", count + 1);
		}
	}
	else {
		sTemp.Format(L"%d", count + 1);
	}
	m_list.InsertItem(count, sTemp);
	m_list.SetItemText(count, 1, itemString);

	int nCount = m_list.GetItemCount();
	if (nCount > 0)
		m_list.EnsureVisible(nCount - 1, FALSE);
}


// ɾ����Ŀ
void CCopyToolDlg::DeleteListItem(int index)
{
	if(index < m_list.GetItemCount())
	{
		m_list.DeleteItem(index);
	}
}


// ����б�
void CCopyToolDlg::ClearList()
{
	m_list.DeleteAllItems();
}


void CCopyToolDlg::OnBnClickedBtnLock()
{
	m_Lock = !m_Lock;
	CButton* btn = (CButton*)GetDlgItem(IDC_BTN_LOCK);
	if (m_Lock)
	{
		btn->SetWindowText(L"������");
		BindList();
	}
	else
		btn->SetWindowText(L"����");
}


void CCopyToolDlg::OnBnClickedBtnDelete()
{
	if(m_rowIndex != -1)
	{
		POSITION pos;
		pos = m_stringList.FindIndex(m_rowIndex);
		m_stringList.RemoveAt(pos);
		BindList();
	}
}


void CCopyToolDlg::OnBnClickedBtnClear()
{
	m_stringList.RemoveAll();
	BindList();
}


// //������
void CCopyToolDlg::BindList()
{
	int count = m_stringList.GetCount();
	int i, index;
	POSITION pos = NULL;
	CString sItem;

	ClearList();
	for (i = 0; i < count; ++i)
	{
		pos = m_stringList.FindIndex(i);
		sItem = m_stringList.GetAt(pos);
		AddListItem(sItem);
	}
}


void CCopyToolDlg::OnClickListItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	NMLISTVIEW *pList = (NMLISTVIEW*)pNMHDR;
	int row;
	row = pList->iItem;
	if(row != -1)
	{
		m_rowIndex = row;
	}
	*pResult = 0;
}


// �����ı���������
void CCopyToolDlg::CopyTextToClipboard(CString sSource)
{
	//if (OpenClipboard())	//�򿪼�����
	//{
	//	EmptyClipboard();//��ռ��а�,�õ�ǰ���ڽ���ӵ�м��а�
	//	HANDLE hClip;
	//	TCHAR *pBuf;
	//	//�����ڴ���󷵻ص�ַ(�ú�����ϵͳ��ȫ�ֶ��з����ڴ�)
	//	hClip = GlobalAlloc(GMEM_MOVEABLE, (sSource.GetLength() + 1)*sizeof(TCHAR));
	//	pBuf = (TCHAR*)GlobalLock(hClip);//����ȫ���ڴ���ָ�����ڴ�飬������һ����ֵַ������ָ���ڴ�����ʼ��
	//	//strcpy_s(pBuf,sSource.GetLength()+1,_T("aa"));//��Str�����е�����Copy���ڴ�ռ���
	//	//wcscpy_s(pBuf,sSource.GetLength()+1,sSource);
	//	memcpy(pBuf, sSource, sSource.GetLength() + 1);
	//	GlobalUnlock(hClip);//����ȫ���ڴ��
	//	SetClipboardData(CF_UNICODETEXT, hClip);//���ü���������
	//	CloseClipboard();//�ر�
	//}
	if (this->OpenClipboard())   //����ܴ򿪼�����  
	{
		::EmptyClipboard();  //��ռ����壬ʹ�ô��ڳ�Ϊ�������ӵ����   
		HGLOBAL hClip;
		hClip = ::GlobalAlloc(GMEM_MOVEABLE, (sSource.GetLength() * 2) + 2); //�ж�Ҫ���ı����ݣ������ڴ�ʱ�����һ���ַ�  
		TCHAR *pBuf;
		pBuf = (TCHAR *)::GlobalLock(hClip);//����������  
		lstrcpy(pBuf, sSource);//��CStringת��  
		::GlobalUnlock(hClip);//�������������  
		::SetClipboardData(CF_UNICODETEXT, hClip);//���ı����ݷ��͵�������  CF_UNICODETEXTΪUnicode����  
		::CloseClipboard();//�رռ�����  
	}
}


// ���Ƽ������ı�
CString CCopyToolDlg::GetTextFromClipboard()
{
	//���Ƽ����������
	PTSTR pText = NULL, pGlobal = NULL;
	HGLOBAL hGlobal;
	//1.�򿪼�����
	if (OpenClipboard())
	{
		//2.ȡ��ָ���ı���ȫ���ڴ��ľ��,û��ָ����ʽ��������ΪNULL
		hGlobal = GetClipboardData(CF_UNICODETEXT);
		if (hGlobal)
		{
			//3.�����þ�����õ�һ��ָ��ü��������ݿ��ָ��
			pGlobal = (PTSTR)GlobalLock(hGlobal);
			//4.�����ڴ�
			pText = (PTSTR)malloc(GlobalSize(hGlobal));
			//5.��������
			lstrcpy(pText, pGlobal);
			//6.�����ڴ��
			GlobalUnlock(hGlobal);
			CloseClipboard();
		}
	}
	return CString(pText);
}


// ѭ����ȡ��Ŀ
CString CCopyToolDlg::GetListItemAutoMove()
{
	POSITION pos = m_stringList.FindIndex(m_rowAuto);
	CString s = m_stringList.GetAt(pos);
	m_rowAuto++;
	if (m_rowAuto == m_stringList.GetCount())
		m_rowAuto = 0;
	return s;
}

LRESULT CCopyToolDlg::OnClickTray(WPARAM wParam, LPARAM IParam)
{
	if (IParam == WM_LBUTTONDBLCLK)
	{
		ModifyStyleEx(0, WS_EX_TOPMOST);
		ShowWindow(SW_SHOW);
	}
	else if (IParam == WM_RBUTTONDOWN)
	{
		CMenu popupMenu;
		popupMenu.LoadMenuW(IDR_MENU_TRAY);
		//��ȡ�Ӳ˵�
		CMenu *pSubMenu = popupMenu.GetSubMenu(0);
		CPoint pt;
		GetCursorPos(&pt);
		pSubMenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
	return 0;
}

void CCopyToolDlg::OnTrayShow()
{
	ShowWindow(SW_SHOW);
}


void CCopyToolDlg::OnTrayExit()
{
	OnCancel();
}


void CCopyToolDlg::ShowTop()
{
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}


void CCopyToolDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

BOOL CCopyToolDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{

	TOOLTIPTEXT   *pTTT = (TOOLTIPTEXT*)pNMHDR;
	UINT  uID = pNMHDR->idFrom;     // �൱��ԭWM_COMMAND���ݷ�ʽ��wParam��low-order��, ��wParam�зŵ����ǿؼ���ID��  

	if (pTTT->uFlags  &  TTF_IDISHWND)
		uID = ::GetDlgCtrlID((HWND)uID);
	if (uID == NULL)
		return   FALSE;
	switch (uID)
	{
	case ID_BTN_START:
		AfxMessageBox(L"��ʼ");
		break;
	case ID_BTN_STOP:
		AfxMessageBox(L"STOP");
		break;
	}

	return TRUE;
}

//�ײ��ַ���ѡ�����¼�
void CCopyToolDlg::OnBnClickedCheckHead()
{
	CheckBoxCheck();
}


void CCopyToolDlg::OnBnClickedCheckTail()
{
	CheckBoxCheck();
}

void CCopyToolDlg::CheckBoxCheck()
{
	//�ײ����
	CButton *pHead = (CButton*)GetDlgItem(IDC_CHECK_HEAD);
	if (pHead->GetCheck())
	{
		m_bAddHead = true;
		GetDlgItem(IDC_EDIT_HEAD)->EnableWindow(true);
	}
	else
	{
		m_bAddHead = false;
		GetDlgItem(IDC_EDIT_HEAD)->EnableWindow(false);
	}
	//β�����
	CButton *pTail = (CButton*)GetDlgItem(IDC_CHECK_TAIL);
	if (pTail->GetCheck())
	{
		m_bAddTail = true;
		GetDlgItem(IDC_EDIT_TAIL)->EnableWindow(true);
	}
	else
	{
		m_bAddTail = false;
		GetDlgItem(IDC_EDIT_TAIL)->EnableWindow(false);
	}
}


CString CCopyToolDlg::AppendCharactor(CString srcString)
{
	CString sTemp, sHead, sTail, sAppend, sText;
	if (m_bAddHead)
		GetDlgItem(IDC_EDIT_HEAD)->GetWindowTextW(sHead);
	if (m_bAddTail)
		GetDlgItem(IDC_EDIT_TAIL)->GetWindowText(sTail);
	m_comboAppend.GetWindowTextW(sText);
	if (sText == "���з�")
	{
		sAppend = L"\r\n";
	}
	else if (sText == "�Ʊ��")
	{
		sAppend = L"\t";
	}
	else
	{
		sAppend = "";
	}
	sTemp = sHead;
	sTemp += srcString;
	sTemp += sTail;
	sTemp += sAppend;
	return sTemp;
}


void CCopyToolDlg::OnBnClickedBtnTest()
{
	CString s;
	m_comboSpitChar.GetWindowTextW(s);
	CString sTemp = L"abc\nadaf\n������\n";
	int pos = sTemp.Find(L"\n", 0);;
	while (pos >= 0)
	{
		s = sTemp.Left(pos);
		sTemp = sTemp.Right(sTemp.GetLength() - pos - 1);
		pos = sTemp.Find(L"\n", 0);
		AfxMessageBox(s);
	}
}


void CCopyToolDlg::StringListAdd(CString sSource)
{
	CString sSplit, sText;
	int i;	//�ָ�������
	m_comboSpitChar.GetWindowTextW(sText);
	if (sText != "")
	{
		if (sText == "���з�")
		{
			i = 2;
			sSplit = L"\r\n";
		}
		else if (sText == "�Ʊ��")
		{
			i = 1;
			sSplit = L"\t";
		}	
		else
		{
			sSplit = sText;
			i = sSplit.GetLength();
		}
		//�ָ��ַ������
		CString sTemp;
		int pos = sSource.Find(sSplit, 0);
		while (pos >= 0)
		{
			sTemp = sSource.Left(pos);
			sSource = sSource.Right(sSource.GetLength() - pos - i);
			m_stringList.AddTail(sTemp);
			pos = sSource.Find(sSplit, 0);
		}
		if(sSource!="")
			m_stringList.AddTail(sSource);
	}
	else
	{
		m_stringList.AddTail(sSource);
	}
}
