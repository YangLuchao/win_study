/*------------------------------------------------------------------------
 070.c 编程达人win32 API每日一练
	 第70个例子：通用控件-Richedit富文本编辑控件
	 FINDREPLACE查找对话框结构
	 EDITSTREAM富文本控件消息结构
	 CHARFORMAT Rich Edit 控件中字符格式结构
	 LoadLibrary函数
	 WM_ACTIVATE激活消息
	 WM_INITMENU初始化菜单消息
 (c) www.bcdaren.com 编程达人
-----------------------------------------------------------------------*/
#include <Windows.h>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")//通用控件库
#include "resource.h"
#include <richedit.h>//富文本控件

HWND hWinMain, hWinEdit, hFindDialog;
TCHAR szCaptionMain[] = TEXT("记事本"); // 窗口窗口标题
int idFindMessage;
TCHAR szFindText[100];
HANDLE hFile;
HMENU hMenu;
FINDREPLACE stFind = { sizeof(FINDREPLACE), 0, 0, FR_DOWN, szFindText,
	0, sizeof(szFindText), 0, 0, 0, 0 };//初始化查找对话框，默认向下查找

void _SaveFile();
void _OpenFile();
DWORD CALLBACK _ProcStream(DWORD _dwCookie, LPVOID _lpBuffer, DWORD _dwBytes, LPDWORD _lpBytes);
void _Init();
void _Quit();
void _SetStatus();
void _FindText();
BOOL _CheckModify();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)//默认填写
{
	static TCHAR szClassName[] = TEXT("Wordpad"); // 类名
	static TCHAR szDllEdit[] = TEXT("RichEd20.dll"); // Richedit 2.0版本
	MSG msg;      //MSG 结构变量
	WNDCLASSEX wndclass; //WNDCLASSEX结构变量
	HACCEL hAccelerator;
	HMODULE hRichEdit;
	//动态加载DLL链接库
	hRichEdit = LoadLibrary(szDllEdit);

	//初始化窗口类
	wndclass.style = CS_HREDRAW | CS_VREDRAW;//重叠窗口
	wndclass.lpfnWndProc = WndProc;//回调函数指针
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(COLOR_BTNFACE + 1);//窗口背景色
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wndclass.lpszMenuName = NULL;//菜单
	wndclass.hInstance = hInstance;//实例句柄
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.hIconSm = NULL;
	wndclass.lpszClassName = szClassName;
	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(0, TEXT("This program requires Windows NT!"),
			szClassName, MB_ICONERROR);
		return 0;
	}
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	hWinMain = CreateWindowEx(WS_EX_CLIENTEDGE,
		szClassName,//窗口类名
		szCaptionMain,//窗口标题
		WS_OVERLAPPEDWINDOW,     //窗口样式
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
		NULL, // 父窗口句柄
		hMenu, // 窗口菜单句柄
		hInstance, // 程序实例句柄 
		NULL); // 创建参数

	ShowWindow(hWinMain, nShowCmd); //显示窗口
	UpdateWindow(hWinMain);

	hAccelerator = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDA_MAIN));
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (TranslateAccelerator(hWinMain, hAccelerator, &msg) == 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	FreeLibrary(hRichEdit);//释放动态链接库
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR szClass[] = TEXT("EDIT"); // 类名
	static TCHAR szCaption[] = TEXT("命令消息"); // 窗口窗口标题
	static TCHAR szFormat[] = TEXT("收到 WM_COMMAND 消息，命令ID：%d");
	static TCHAR szBuffer[128];
	CHARRANGE stRange;	//头文件richedit.h
	RECT rect;
	HWND tmp;

	switch (message)
	{
	case WM_CREATE:
		hWinMain = hWnd;
		_Init();
		return 0;

	case WM_SIZE:
		GetClientRect(hWinMain, &rect);
		MoveWindow(hWinEdit, 0, 0, rect.right, rect.bottom, TRUE);
		return 0;
		/*方法1：
			case WM_INITMENUPOPUP:
				switch (lParam)
				{
				case 0:	//文件菜单
					EnableMenuItem((HMENU)wParam, IDM_SAVE, //启用，禁用或显示指定的菜单项
						SendMessage(hWinEdit, EM_GETMODIFY, 0, 0L) ?	//获取编辑控件的修改标志的状态
						MF_ENABLED : MF_GRAYED);
					break;

				case 1:	//编辑菜单
					SendMessage(hWinEdit, EM_EXGETSEL, 0, (LPARAM)&stRange);
					EnableMenuItem(hMenu, IDM_COPY, stRange.cpMin == stRange.cpMax ? MF_GRAYED : MF_ENABLED);
					EnableMenuItem(hMenu, IDM_CUT, MF_ENABLED);

					EnableMenuItem(hMenu, IDM_PASTE,
						SendMessage(hWinEdit, EM_CANPASTE, 0, 0) ? MF_ENABLED : MF_GRAYED);

					EnableMenuItem(hMenu, IDM_REDO,
						SendMessage(hWinEdit, EM_CANREDO, 0, 0) ? MF_ENABLED : MF_GRAYED);

					EnableMenuItem(hMenu, IDM_UNDO,
						SendMessage(hWinEdit, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);

					EnableMenuItem(hMenu, IDM_SELALL,
						GetWindowTextLength(hWinEdit) ? MF_ENABLED : MF_GRAYED);

					EnableMenuItem(hMenu, IDM_FINDNEXT, szFindText[0] ? MF_ENABLED : MF_GRAYED);
					EnableMenuItem(hMenu, IDM_FINDPREV, szFindText[0] ? MF_ENABLED : MF_GRAYED);

					break;
				}
				return 0;
		*/
		/*方法2：*/
	case WM_INITMENU:
		_SetStatus();
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
			DestroyWindow(hWinMain);
			PostQuitMessage(0);
			break;
		case IDM_OPEN:
			if (_CheckModify())
				_OpenFile();
			break;
		case IDM_SAVE:
			_SaveFile();
			break;
		case IDM_UNDO:
			SendMessage(hWinEdit, EM_UNDO, 0, 0);
			break;
		case IDM_REDO:
			SendMessage(hWinEdit, EM_REDO, 0, 0);
			break;
		case IDM_SELALL:
			//如果cpMin和cpMax成员相等，则范围为空。如果cpMin为0并且cpMax为–1，则该范围包括所有内容
			stRange.cpMin = 0;
			stRange.cpMax = -1;
			SendMessage(hWinEdit, EM_EXSETSEL, 0, (LPARAM)&stRange);
			break;
		case IDM_COPY:
			SendMessage(hWinEdit, WM_COPY, 0, 0);
			break;
		case IDM_CUT:
			SendMessage(hWinEdit, WM_CUT, 0, 0);
			break;
		case IDM_PASTE:
			SendMessage(hWinEdit, WM_PASTE, 0, 0);
			break;
		case IDM_FIND:
			stFind.Flags &= (~FR_DIALOGTERM);//设置除FR_DIALOGTERM之外的所有标志
			tmp = FindText(&stFind);//创建查找对话框，返回对话框句柄
			if (tmp)
				hFindDialog = tmp;
			break;
		case IDM_FINDPREV:
			stFind.Flags &= (~FR_DOWN);//向上查找
			_FindText();
			break;
		case IDM_FINDNEXT:
			stFind.Flags |= FR_DOWN;//向下查找
			_FindText();
			break;
		}
		return 0;
		//激活消息：发送到正在激活的窗口和正在停用的窗口
	case WM_ACTIVATE:	//鼠标激活或者鼠标以外激活
		if ((LOWORD(wParam)) == WA_CLICKACTIVE || (LOWORD(wParam)) == WA_ACTIVE)//
			SetFocus(hWinEdit);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		if (message == idFindMessage)
		{
			if (stFind.Flags & FR_DIALOGTERM)//检测到对话框关闭
				hFindDialog = 0;
			else
				_FindText();
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void _SaveFile()
{
	EDITSTREAM stES;	//富文本控件消息结构

	SetFilePointer(hFile, 0, 0, FILE_BEGIN);//移动文件指针到开头
	SetEndOfFile(hFile);//截断文件，将指定文件的物理文件大小设置为文件指针的当前位置
	stES.dwCookie = FALSE;//传递给回调函数的标记值，FALSE写操作
	stES.pfnCallback = (EDITSTREAMCALLBACK)_ProcStream;//回调函数
	SendMessage(hWinEdit, EM_STREAMOUT, SF_TEXT, (LPARAM)&stES);//指示富文本控件将文本内容传递给回调函数用于输出
	SendMessage(hWinEdit, EM_SETMODIFY, FALSE, 0);//设置或清除编辑控件的修改标志，TRUE表示文本已修改，FALSE表示未修改该文本
}
void _OpenFile()
{
	OPENFILENAME stOF;
	EDITSTREAM stES;//富编辑控件的信息。Rich Edit 控件使用该信息将数据流传输到控件中或从控件中传出。
	static TCHAR szFileName[MAX_PATH];
	static TCHAR szDefaultExt[] = TEXT("txt");
	static TCHAR szErrOpenFile[] = TEXT("无法打开文件!");
	static TCHAR szFilter[] = TEXT("Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0");

	//显示打开文件对话框
	RtlZeroMemory(&stOF, sizeof(stOF));
	stOF.lStructSize = sizeof(stOF);
	stOF.hwndOwner = hWinMain;
	stOF.lpstrFilter = szFilter;
	stOF.lpstrFile = szFileName;
	stOF.nMaxFile = MAX_PATH;
	stOF.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOF.lpstrDefExt = szDefaultExt;
	if (GetOpenFileName(&stOF))
	{
		if (hFile)
			CloseHandle(hFile);
		hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWinMain, szErrOpenFile, NULL, MB_OK | MB_ICONSTOP);
			return;
		}
		//装入和保存文本
		stES.dwCookie = TRUE;
		stES.pfnCallback = (EDITSTREAMCALLBACK)_ProcStream;//回调函数
		SendMessage(hWinEdit, EM_STREAMIN, SF_TEXT, (LPARAM)&stES);//指示回调函数读入文本
		SendMessage(hWinEdit, EM_SETMODIFY, FALSE, 0);//设置或清除编辑控件的修改标志-未修改
	}
}
//Richedit文本控件输入和输出回调函数
DWORD CALLBACK _ProcStream(DWORD _dwCookie, LPVOID _lpBuffer, DWORD _dwBytes, LPDWORD _lpBytes)
{
	if (_dwCookie)
		ReadFile(hFile, _lpBuffer, _dwBytes, _lpBytes, 0);
	else
		WriteFile(hFile, _lpBuffer, _dwBytes, _lpBytes, 0);
	return 0;
}
void _Init()
{
	CHARFORMAT stCF;//包含有关 Rich Edit 控件中的字符格式的信息
	static TCHAR szClassEdit[] = TEXT("RichEdit20W");	//UNICODE版本
	//static TCHAR szClassEdit[] = TEXT("RichEdit20A");	//ASCII码版本
	static TCHAR szFont[] = TEXT("宋体");

	//注册“查找”对话框消息，初始化“查找”对话框结构
	stFind.hwndOwner = hWinMain;
	idFindMessage = RegisterWindowMessage(FINDMSGSTRING);
	//建立输出文本窗口
	hWinEdit = CreateWindowEx(WS_EX_CLIENTEDGE, szClassEdit, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL,
		0, 0, 0, 0, hWinMain, 0, GetModuleHandle(0), NULL);
	SendMessage(hWinEdit, EM_SETTEXTMODE, TM_PLAINTEXT, 0);//纯文本模式
	RtlZeroMemory(&stCF, sizeof(stCF));
	stCF.cbSize = sizeof(stCF);
	stCF.yHeight = 9 * 20;	//以缇为单位（1/1440英寸或打印机点的1/20）
	stCF.dwMask = (CFM_FACE | CFM_SIZE | CFM_BOLD);//CFE_BOLD值
	lstrcpy(stCF.szFaceName, szFont);//字体名称
	SendMessage(hWinEdit, EM_SETCHARFORMAT, 0, (LPARAM)&stCF);//设置字符格式的信息
	//64K 个字符，在调用 EM _ EXLIMITTEXT 之前，用户可输入的文本量的默认限制为32767个字符
	SendMessage(hWinEdit, EM_EXLIMITTEXT, 0, -1);
}
void _Quit()
{
	if (_CheckModify())
	{
		DestroyWindow(hWinMain);
		PostQuitMessage(0);
	}
	if (hFile)
		CloseHandle(hFile);
}
void _SetStatus()
{
	//根据情况改变菜单状况
	CHARRANGE stRange;//指定 Rich Edit 控件中的字符范围

	SendMessage(hWinEdit, EM_EXGETSEL, 0, (LPARAM)&stRange);
	if (stRange.cpMin == stRange.cpMax)
	{
		EnableMenuItem(hMenu, IDM_COPY, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_CUT, MF_ENABLED);
	}
	else
	{
		EnableMenuItem(hMenu, IDM_COPY, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_CUT, MF_ENABLED);
	}

	if (SendMessage(hWinEdit, EM_CANPASTE, 0, 0))
		EnableMenuItem(hMenu, IDM_PASTE, MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_PASTE, MF_GRAYED);

	if (SendMessage(hWinEdit, EM_CANREDO, 0, 0))
		EnableMenuItem(hMenu, IDM_REDO, MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_REDO, MF_GRAYED);

	if (SendMessage(hWinEdit, EM_CANUNDO, 0, 0))
		EnableMenuItem(hMenu, IDM_UNDO, MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_UNDO, MF_GRAYED);

	if (GetWindowTextLength(hWinEdit))
		EnableMenuItem(hMenu, IDM_SELALL, MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_SELALL, MF_GRAYED);
	//获取编辑控件修改标志的状态
	if (SendMessage(hWinEdit, EM_GETMODIFY, 0, 0))
		EnableMenuItem(hMenu, IDM_SAVE, MF_ENABLED);
	else
		EnableMenuItem(hMenu, IDM_SAVE, MF_GRAYED);

	if (szFindText[0])
	{
		EnableMenuItem(hMenu, IDM_FINDNEXT, MF_ENABLED);
		EnableMenuItem(hMenu, IDM_FINDPREV, MF_ENABLED);
	}
	else
	{
		EnableMenuItem(hMenu, IDM_FINDNEXT, MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FINDPREV, MF_GRAYED);
	}
}
void _FindText()
{
	FINDTEXTEX stFindText;
	static TCHAR szNotFound[] = TEXT("字符串未找到!");

	//设置查找范围--要搜索的字符范围。要在整个控制向前搜索，设置cpMin为0，cpMax就会为-1
	SendMessage(hWinEdit, EM_EXGETSEL, 0, (LPARAM)&stFindText.chrg);

	if (stFind.Flags & FR_DOWN)
		stFindText.chrg.cpMin = stFindText.chrg.cpMax;
	stFindText.chrg.cpMax = -1;//改为向前查找
	//设置查找选项
	stFindText.lpstrText = szFindText;
	stFind.Flags &= (FR_MATCHCASE | FR_DOWN | FR_WHOLEWORD);//匹配大小写，向下查找，全字匹配
	//查找并把光标设置到找到的文本上
	if (SendMessage(hWinEdit, EM_FINDTEXTEX, stFind.Flags, (LPARAM)&stFindText) == -1)
	{
		if (hFindDialog)
		{
			hWinMain = hFindDialog;
		}
		else
			MessageBox(hWinMain, szNotFound, NULL, MB_OK | MB_ICONINFORMATION);
		return;
	}
	SendMessage(hWinEdit, EM_EXSETSEL, 0, (LPARAM)&stFindText.chrgText);//设置选定内容
	SendMessage(hWinEdit, EM_SCROLLCARET, 0, 0);//重新定位滑块位置到找到的文本位置
}
BOOL _CheckModify()
{
	//页面设置对话框
	BOOL dwReturn;
	static TCHAR szModify[] = TEXT("文件已修改，是否保存?");
	int click;

	dwReturn = TRUE;
	if (SendMessage(hWinEdit, EM_GETMODIFY, 0, 0) && hFile)//指示编辑控件的内容是否已被修改，内容已被修改，则返回值为非零值;否则为零
	{
		click = MessageBox(hWinMain, szModify, szCaptionMain, MB_YESNOCANCEL | MB_ICONQUESTION);
		if (click == IDYES)
			_SaveFile();
		else if (click == IDCANCEL)
			dwReturn = FALSE;
	}
	return dwReturn;
}