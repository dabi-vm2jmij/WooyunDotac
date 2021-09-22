
// TestMFC.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TestMFC.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CTestMFCApp

BEGIN_MESSAGE_MAP(CTestMFCApp, CWinApp)
END_MESSAGE_MAP()

// CTestMFCApp 构造

CTestMFCApp::CTestMFCApp()
{
	// TODO:  将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	// 为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("TestMFC.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CTestMFCApp 对象

CTestMFCApp theApp;

// CTestMFCApp 初始化

BOOL CTestMFCApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	// 则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	// SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	// 具体用法参考 TestLib.cpp
	wchar_t szSkinPath[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, szSkinPath);
	PathAppendW(szSkinPath, L"..\\TestLib\\res\\Skin");
	UILib::LoadSkin(szSkinPath);

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建并加载框架及其资源
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL, NULL);

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

int CTestMFCApp::ExitInstance()
{
	// TODO:  处理可能已添加的附加资源
	return CWinApp::ExitInstance();
}

// CTestMFCApp 消息处理程序
