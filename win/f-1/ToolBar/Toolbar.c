/*------------------------------------------------------------------------
 073.c 编程达人win32 API每日一练
	 第73个例子：通用控件-工具栏控件
	 TBBUTTON结构
	 NMTOOLBAR结构
	 NMTTDISPINFO结构
	 NMHDR结构结构
	 WM_NOTIFY消息
 (c) www.bcdaren.com 编程达人
-----------------------------------------------------------------------*/
#include <Windows.h>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")
#include "resource.h"
#define ID_TOOLBAR		1	//工具栏控件标识符
#define ID_EDIT			2	//编辑控件标识符
#define NUM_BUTTONS		16

HWND hWinMain;
HWND hWinEdit, hWinToolbar;

//定义工具栏按钮
TBBUTTON tbb[16] = {
	{STD_FILENEW,IDM_NEW,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_FILEOPEN,IDM_OPEN,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_FILESAVE,IDM_SAVE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,-1},
	{STD_PRINTPRE,IDM_PAGESETUP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_PRINT,IDM_PRINT,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,-1},
	{STD_COPY,IDM_COPY,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_CUT,IDM_CUT,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_PASTE,IDM_PASTE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,-1},
	{STD_FIND,IDM_FIND,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{STD_REPLACE,IDM_REPLACE,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,-1},
	{STD_HELP,IDM_HELP,TBSTATE_ENABLED,TBSTYLE_BUTTON,0,0,-1},
	{0,0,TBSTATE_ENABLED,TBSTYLE_SEP,0,0,-1}
};

void _Resize();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)//默认填写
{
	static TCHAR szClassName[] = TEXT("ToolbarExample"); // 类名
	static TCHAR szCaptionMain[] = TEXT("工具栏示例"); // 窗口窗口标题
	MSG msg;      //MSG 结构变量
	WNDCLASSEX wndclass; //WNDCLASSEX结构变量

	wndclass.style = CS_HREDRAW | CS_VREDRAW;//重叠窗口
	wndclass.lpfnWndProc = WndProc;//回调函数指针
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(COLOR_BTNFACE + 1);//对话框窗口背景色
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICO_MAIN));
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);//菜单
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

	hWinMain = CreateWindowEx(WS_EX_CLIENTEDGE,
		szClassName,//窗口类名
		szCaptionMain,//窗口标题
		WS_OVERLAPPEDWINDOW,     //窗口样式
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
		NULL, // 父窗口句柄
		NULL, // 窗口菜单句柄
		hInstance, // 程序实例句柄 
		NULL); // 创建参数

	ShowWindow(hWinMain, nShowCmd); //显示窗口
	UpdateWindow(hWinMain);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	static TCHAR szClass[] = TEXT("EDIT"); // 类名
	static TCHAR szCaption[] = TEXT("命令消息"); // 窗口窗口标题
	static TCHAR szFormat[] = TEXT("收到 WM_COMMAND 消息，命令ID：%d");
	static TCHAR szBuffer[128];
	static NMTOOLBAR NMToolbar;
	LPNMTTDISPINFO lpnmt;	//该结构取代了 TOOLTIPTEXT结构

	LPTOOLTIPTEXT lpToolTipText;
	//TOOLINFO tbToolInfo;	//该TOOLINFO结构包含在工具提示控制工具的信息

	switch (message)
	{
	case WM_CREATE:
		hWinMain = hwnd;
		//创建编辑控件，窗口的边框具有凹陷边缘
		hWinEdit = CreateWindowEx(WS_EX_CLIENTEDGE, szClass, NULL,
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOHSCROLL,
			0, 0, 0, 0, hwnd, (HMENU)ID_EDIT, GetModuleHandle(0), NULL);
		//创建工具栏控件,使用系统预定义位图，注意此函数不支持工具栏的所有功能。请改用CreateWindowEx。
		hWinToolbar = CreateToolbarEx(hWinMain, WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT |
			TBSTYLE_TOOLTIPS | CCS_ADJUSTABLE,
			ID_TOOLBAR, 0, HINST_COMMCTRL,//包含资源模块的句柄
			IDB_STD_SMALL_COLOR,//小型彩色位图
			tbb, NUM_BUTTONS, 0, 0, 0, 0, sizeof(TBBUTTON));
		_Resize();
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDM_EXIT://菜单
			DestroyWindow(hWinMain);
			PostQuitMessage(0);
			break;

		default:
			if (LOWORD(wparam) != ID_EDIT)//非EDIT控件的其他菜单
			{
				wsprintf(szBuffer, szFormat, wparam);
				MessageBox(hwnd, szBuffer, szCaption, MB_OK | MB_ICONINFORMATION);
			}
			break;
		}
		return 0;
	case WM_SIZE:
		_Resize();
		return 0;

		//通过指定TBSTYLE_TOOLTIPS样式和在.rc文件中创建一个STRINGTABLE来向工具栏添加工具提示
	case WM_NOTIFY:
		/*WM_NOTIFY消息的IParam参数指向一个NMHDR
		为什么不像状态栏一样使用NMHDR结构？
		因为不同控件的通知消息都使用 WM_NOTIFY消息，有些通知消息可能需要附带其他数据，
		这时仅使用一个NMHDR结 构来表达是不够的，Windows的处理办法是为需要附带其他数据的WM_NOTIFY消息
		定义不同的数据结构，但这些结构头部都是一个NMHDR结构，NMHDR结构以后才是其他字段，
		这样在得知通知码之前，把lParam参数指针当做一个NMHDR结构来处理总是正确的。
		而且只有先把lParam参数指针当做NMHDR结构处理并从中获取通知码以后，才真正知道lParam指向的究竟是什么结构。
		*/
		/*方法一：使用NMTTDISPINFO结构*/
		//检索显示工具提示窗口所需的信息
		lpnmt = (LPNMTTDISPINFO)lparam;
		if (lpnmt->hdr.code == TTN_NEEDTEXT)//通知码，获取提示信息，与 TTN_GETDISPINFO相同
		{
			lpnmt->lpszText = (LPWSTR)(lpnmt->hdr).idFrom;
			lpnmt->hinst = GetModuleHandle(0);
		}
		//可以在指定按钮的左侧插入一个按钮
		else if (lpnmt->hdr.code == TBN_QUERYINSERT)
		{
			return TRUE;
		}
		//可以从工具栏中删除按钮
		else if (lpnmt->hdr.code == TBN_QUERYDELETE)
		{
			return TRUE;
		}
		//按钮信息已复制到指定的缓冲区
		else if (lpnmt->hdr.code == TBN_GETBUTTONINFO)
		{
			/*使用NMTOOLBAR结构---比较繁琐
			RtlMoveMemory(&(NMToolbar.tbButton),sizeof(NMTOOLBAR)*NUM_BUTTONS+ tbb,sizeof(NUM_BUTTONS));
			LoadString(GetModuleHandle(0), (NMToolbar.tbButton).idCommand,szBuffer,sizeof(szBuffer));
			NMToolbar.pszText = szBuffer;
			NMToolbar.cchText = lstrlen(szBuffer);
			*/
			// 如果系统需要文本，则从资源中进行装载
			LoadString(GetModuleHandle(0),
				lpnmt->hdr.idFrom,    // 字符串ID == 命令ID
				szBuffer,
				sizeof(szBuffer));
			// 将结构指向字符串
			lpnmt->lpszText = szBuffer;

			return TRUE;
		}
		/*
				//方法二：使用TOOLTIPTEXT结构，此结构已不再使用
				lpToolTipText = (LPTOOLTIPTEXT)lparam;
				if (lpToolTipText->hdr.code == TTN_NEEDTEXT)
				{
					// 如果系统需要文本，则从资源中进行装载
					LoadString(GetModuleHandle(0),
						lpToolTipText->hdr.idFrom,    // 字符串ID == 命令ID
						szBuffer,
						sizeof(szBuffer));
					// 将结构指向字符串
					lpToolTipText->lpszText = szBuffer;
				}
		*/
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}
//调整尺寸大小
void _Resize()
{
	RECT rect, rect1;

	SendMessage(hWinToolbar, TB_AUTOSIZE, 0, 0);
	GetClientRect(hWinMain, &rect);
	GetWindowRect(hWinToolbar, &rect1);
	MoveWindow(hWinEdit, 0, rect1.bottom - rect1.top, rect.right, rect.bottom - (rect1.bottom - rect1.top), TRUE);
}
/****************************************************************
	//新结构
	//static NMTTDISPINFO nmt;	//该结构取代了 TOOLTIPTEXT结构
	/*
typedef struct tagNMTTDISPINFOA {
  NMHDR     hdr;		//NMHDR结构包含有关通知的其他信息
  LPSTR     lpszText;	//指向将以工具提示文本显示的以空字符结尾的字符串的指针
  char      szText[80];	//接收工具提示文本的缓冲区
  HINSTANCE hinst;		//处理包含包含用作工具提示文本的字符串资源的实例
  UINT      uFlags;		//指示如何解释所包含的NMHDR结构的idFrom成员的标志
  LPARAM    lParam;		//与工具关联的应用程序定义的数据
} NMTTDISPINFOA, *LPNMTTDISPINFOA;
*********************************************************************/
/*
typedef struct tagNMTOOLBARW {
NMHDR    hdr;		//NMHDR结构包含有关通知的其他信息
int      iItem;	//与通知代码关联的按钮的命令标识符
TBBUTTON tbButton;//BBUTTON结构，其中包含有关与通知代码关联的工具栏按钮的信息
int      cchText;	//按钮文字中的字符数
LPWSTR   pszText;	//包含按钮文本的字符缓冲区的地址
RECT     rcButton;//RECT结构，它定义由按钮所覆盖的区域
} NMTOOLBARW, *LPNMTOOLBARW;
	*/
	/*==================================================================================
	NMHDR结构（richedit.h）包含有关通知消息的信息。
	typedef struct _nmhdr {
	  HWND hwndFrom;//发送消息的控件的窗口句柄
	  UINT idFrom;	//发送消息的控件的标识符
	  UINT code;	//通知代码
	} NMHDR;
	===================================================================================*/
	/*
		//编写 工具提示通知处理程序时，需要使用 TOOLTIPTEXT 结构。 TOOLTIPTEXT 结构的成员包括：
		typedef struct {
			NMHDR     hdr;        // required for all WM_NOTIFY messages
			LPTSTR    lpszText;   // 接收工具文本的字符串的地址
			TCHAR     szText[80]; // 接收工具提示文本的缓存区
			HINSTANCE hinst;      // 包含要用作工具提示文本的字符串的实例的句柄
			UINT      uflags;     // flag indicating how to interpret the
								  // idFrom member of the NMHDR structure
								  // that is included in the structure
		} TOOLTIPTEXT, FAR *LPTOOLTIPTEXT;
		当处理 TTN_NEEDTEXT 通知消息时，请指定要用以下方式之一显示的字符串：
	将文本复制到 szText 成员指定的缓冲区。
	将包含文本的缓冲区的地址复制到 lpszText 成员。
	将字符串资源的标识符复制到 lpszText 成员，并将包含该资源的实例的句柄复制到 hinst 成员。
	==================================================================================
//TBBUTTON结构（commctrl.h）,包含有关工具栏中按钮的信息。
/*
typedef struct _TBBUTTON {
  int       iBitmap;	//按钮在图片资源中的索引，以0开始
  int       idCommand;	//按钮对应命令ID，一般和菜单命令对应
  BYTE      fsState;	//按钮类型
  BYTE      fsStyle;	//按钮样式
#if ...
  BYTE      bReserved[6];//保留
#else
  BYTE      bReserved[2];//保留
#endif
  DWORD_PTR dwData;		//自定义数据
  INT_PTR   iString;	//按钮字符串的从零开始的索引，或者指向包含按钮文本的字符串缓冲区的指针
} TBBUTTON, *PTBBUTTON, *LPTBBUTTON;
*/
