
// CopyTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCopyToolApp: 
// �йش����ʵ�֣������ CopyTool.cpp
//

class CCopyToolApp : public CWinApp
{
public:
	CCopyToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCopyToolApp theApp;