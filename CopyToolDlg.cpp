
// CopyToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CopyTool.h"
#include "CopyToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//热键定义
#define IDH_F1 4001
#define IDH_F3 4002
//消息
#define WM_NOTIFYICON WM_USER+1

//句柄
static HWND hwndNextViewer;
//最小化托盘
NOTIFYICONDATA gNotifyIcon;
ATOM atom = GlobalAddAtom(L"F1");
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CCopyToolDlg 对话框



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


// CCopyToolDlg 消息处理程序

BOOL CCopyToolDlg::OnInitDialog()
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
	//自定义工具栏加载
	if (!m_wndMyToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndMyToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	//图像列表初始化
	m_imageList.Create(32, 32, ILC_COLOR32 | ILC_MASK, 0, 0);
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON_START));
	m_imageList.Add(AfxGetApp()->LoadIconW(IDI_ICON_STOP));
	//图像列表和工具栏关联
	m_wndMyToolBar.GetToolBarCtrl().SetImageList(&m_imageList);
	m_wndMyToolBar.ShowWindow(SW_SHOW);
	m_wndMyToolBar.GetToolBarCtrl().EnableButton(ID_BTN_STOP, false);
	//控件条定位  
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//添加成为剪贴板查看器
	hwndNextViewer = SetClipboardViewer();

	//列表初始化
	m_list.SetExtendedStyle(m_list.GetExtendedStyle()
		| LVS_EX_FULLROWSELECT
		| LVS_EX_GRIDLINES);
	CRect rt;
	m_list.GetWindowRect(&rt);
	m_list.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 40);
	m_list.InsertColumn(1, _T("复制内容"), LVCFMT_CENTER, rt.Width() - 70);
	//按钮状态
	m_apply.EnableWindow();
	m_stop.EnableWindow(FALSE);
	//创建托盘图标
	CString sText;
	this->GetWindowText(sText);
	gNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	gNotifyIcon.hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);
	gNotifyIcon.hWnd = m_hWnd;
	lstrcpy(gNotifyIcon.szTip, sText);
	gNotifyIcon.uCallbackMessage = WM_NOTIFYICON;
	gNotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	Shell_NotifyIcon(NIM_ADD, &gNotifyIcon);

	//复选框检查
	CheckBoxCheck();
	m_comboSpitChar.AddString(L"");
	m_comboSpitChar.AddString(L"制表符");
	m_comboSpitChar.AddString(L"换行符");
	m_comboAppend.AddString(L"");
	m_comboAppend.AddString(L"制表符");
	m_comboAppend.AddString(L"换行符");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCopyToolDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
				// 模拟按键ALT+F4
				/*keybd_event(18, 0, 0, 0);
				keybd_event(115, 0, 0, 0);
				keybd_event(18, 0, KEYEVENTF_KEYUP, 0);
				keybd_event(115, 0, KEYEVENTF_KEYUP, 0);*/
				// 模拟按键CTRL+C
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
			//// 模拟按键CTRL+V
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
	//注册热键
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
	//卸载热键
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
		// 模拟按键CTRL+V
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
		//判断是否需要分割字符
		StringListAdd(s);
		BindList();
		m_Copy = false;
		//ShowTop();
	}
}


void CCopyToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	//卸载热键
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F1);
	UnregisterHotKey(this->GetSafeHwnd(), IDH_F3);
}


// //添加条目
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


// 删除条目
void CCopyToolDlg::DeleteListItem(int index)
{
	if(index < m_list.GetItemCount())
	{
		m_list.DeleteItem(index);
	}
}


// 清空列表
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
		btn->SetWindowText(L"已锁定");
		BindList();
	}
	else
		btn->SetWindowText(L"锁定");
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


// //绑定数据
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


// 设置文本到剪贴板
void CCopyToolDlg::CopyTextToClipboard(CString sSource)
{
	//if (OpenClipboard())	//打开剪贴板
	//{
	//	EmptyClipboard();//清空剪切板,让当前窗口进程拥有剪切板
	//	HANDLE hClip;
	//	TCHAR *pBuf;
	//	//分配内存对象返回地址(该函数是系统从全局堆中分配内存)
	//	hClip = GlobalAlloc(GMEM_MOVEABLE, (sSource.GetLength() + 1)*sizeof(TCHAR));
	//	pBuf = (TCHAR*)GlobalLock(hClip);//锁定全局内存中指定的内存块，并返回一个地址值，令其指向内存块的起始处
	//	//strcpy_s(pBuf,sSource.GetLength()+1,_T("aa"));//将Str对象中的数据Copy到内存空间中
	//	//wcscpy_s(pBuf,sSource.GetLength()+1,sSource);
	//	memcpy(pBuf, sSource, sSource.GetLength() + 1);
	//	GlobalUnlock(hClip);//解锁全局内存块
	//	SetClipboardData(CF_UNICODETEXT, hClip);//设置剪贴板数据
	//	CloseClipboard();//关闭
	//}
	if (this->OpenClipboard())   //如果能打开剪贴板  
	{
		::EmptyClipboard();  //清空剪贴板，使该窗口成为剪贴板的拥有者   
		HGLOBAL hClip;
		hClip = ::GlobalAlloc(GMEM_MOVEABLE, (sSource.GetLength() * 2) + 2); //判断要是文本数据，分配内存时多分配一个字符  
		TCHAR *pBuf;
		pBuf = (TCHAR *)::GlobalLock(hClip);//锁定剪贴板  
		lstrcpy(pBuf, sSource);//把CString转换  
		::GlobalUnlock(hClip);//解除锁定剪贴板  
		::SetClipboardData(CF_UNICODETEXT, hClip);//把文本数据发送到剪贴板  CF_UNICODETEXT为Unicode编码  
		::CloseClipboard();//关闭剪贴板  
	}
}


// 复制剪贴板文本
CString CCopyToolDlg::GetTextFromClipboard()
{
	//复制剪贴板的内容
	PTSTR pText = NULL, pGlobal = NULL;
	HGLOBAL hGlobal;
	//1.打开剪贴板
	if (OpenClipboard())
	{
		//2.取得指向文本的全局内存块的句柄,没有指定格式的数据则为NULL
		hGlobal = GetClipboardData(CF_UNICODETEXT);
		if (hGlobal)
		{
			//3.锁定该句柄，得到一个指向该剪贴板数据块的指针
			pGlobal = (PTSTR)GlobalLock(hGlobal);
			//4.分配内存
			pText = (PTSTR)malloc(GlobalSize(hGlobal));
			//5.复制数据
			lstrcpy(pText, pGlobal);
			//6.解锁内存块
			GlobalUnlock(hGlobal);
			CloseClipboard();
		}
	}
	return CString(pText);
}


// 循环获取条目
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
		//获取子菜单
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

BOOL CCopyToolDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{

	TOOLTIPTEXT   *pTTT = (TOOLTIPTEXT*)pNMHDR;
	UINT  uID = pNMHDR->idFrom;     // 相当于原WM_COMMAND传递方式的wParam（low-order）, 在wParam中放的则是控件的ID。  

	if (pTTT->uFlags  &  TTF_IDISHWND)
		uID = ::GetDlgCtrlID((HWND)uID);
	if (uID == NULL)
		return   FALSE;
	switch (uID)
	{
	case ID_BTN_START:
		AfxMessageBox(L"开始");
		break;
	case ID_BTN_STOP:
		AfxMessageBox(L"STOP");
		break;
	}

	return TRUE;
}

//首部字符复选框点击事件
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
	//首部检查
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
	//尾部检查
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
	if (sText == "换行符")
	{
		sAppend = L"\r\n";
	}
	else if (sText == "制表符")
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
	CString sTemp = L"abc\nadaf\n哈哈哈\n";
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
	int i;	//分隔符长度
	m_comboSpitChar.GetWindowTextW(sText);
	if (sText != "")
	{
		if (sText == "换行符")
		{
			i = 2;
			sSplit = L"\r\n";
		}
		else if (sText == "制表符")
		{
			i = 1;
			sSplit = L"\t";
		}	
		else
		{
			sSplit = sText;
			i = sSplit.GetLength();
		}
		//分割字符并添加
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
