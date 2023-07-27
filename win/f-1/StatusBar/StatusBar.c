/*------------------------------------------------------------------------
 071.c 编程达人win32 API每日一练
	 第71个例子：通用控件-状态栏控件
	 CreateStatusWindow函数
	 NMHDR结构
	 PAGESETUPDLG结构
	 WM_MENUSELECT消息
	 WM_NOTIFY消息
 (c) www.bcdaren.com 编程达人
-----------------------------------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")	//静态加载导入库
#include "resource.h"
#define  SIZE 256
//#define FINDMSGSTRING TEXT("commdlg_FindReplace")	//commdlg.h头文件已定义
#define ID_STATUSBAR 1
#define ID_EDIT 2

HWND hWinMain;
TCHAR szBuffer[1024];
TCHAR szFileName[MAX_PATH];
TCHAR szCaption[] = TEXT("执行结果");
//FINDREPLACE stFind;//需要定义为全局变量或者static变量才可以在不同消息间共享
HWND hWinStatus;
HWND hWinEdit;
TCHAR* lpsz1;
TCHAR* lpsz2;

void _OpenFile();
void _ChooseColor();
void _ChooseFont();
void _PageSetup();
void _SaveAs();
BOOL CALLBACK ProcDlgMain(HWND, UINT, WPARAM, LPARAM);
void _Resize();//调整尺寸

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)//默认填写
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, ProcDlgMain, 0);
	return 0;
}

BOOL CALLBACK ProcDlgMain(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	HINSTANCE hInstance;
	static FINDREPLACE stFind;//需要定义为全局变量或者static变量才可以在不同消息间共享
	static TCHAR szText[] = TEXT("对话框的简单例子\r\n\r\nC语言版本");
	static TCHAR szFindText[SIZE];
	static TCHAR szReplaceText[SIZE];
	static TCHAR szFindNext[] = TEXT("查找下一个");
	static TCHAR szReplace[] = TEXT("替换");
	static TCHAR szReplaceAll[] = TEXT("全部替换");
	static TCHAR szFormatFind[] = TEXT("您按下了\"%s\"按钮\n查找字符串:%s\n替换字符串：%s");
	static UINT idFindMessage;
	//状态栏
	int dwStatusWidth[] = { 60,140,172,-1 };//宽度坐标值分为4栏，使用了前面3栏
	UINT dwMenuHelp[] = { 0,IDM_MENUHELP,0,0 };//IDM_MENUHELP加上菜单命令ID的数值后不能超过0xFFFF
	static TCHAR szClass[] = TEXT("EDIT");
	static TCHAR sz1[] = TEXT("插入");
	static TCHAR sz2[] = TEXT("改写");
	static TCHAR szFormat0[] = TEXT("%02d:%02d:%02d");
	static TCHAR szFormat1[] = TEXT("字节数:%d");
	SYSTEMTIME st;
	POINT pt;
	RECT rect;

	switch (message)
	{
	case WM_INITDIALOG:
		hInstance = GetModuleHandle(NULL);
		SendMessage(hwnd, WM_SETICON, ICON_BIG,
			(LPARAM)LoadIcon(hInstance, (LPCTSTR)ICO_MAIN));
		/*初始化公用对话框*/
		hWinMain = hwnd;
		stFind.hwndOwner = hwnd;
		stFind.lStructSize = sizeof(stFind);
		stFind.Flags = FR_DOWN;
		stFind.lpstrFindWhat = szFindText;
		stFind.wFindWhatLen = sizeof(szFindText);
		stFind.lpstrReplaceWith = szReplaceText;
		stFind.wReplaceWithLen = sizeof(szReplaceText);
		/****************************************
		查找和替换公用非模态对话框步骤：
		1、获取FINDMSGSTRING注册消息的消息标识符。
		2、显示对话框。
		3、打开对话框时处理FINDMSGSTRING消息。
		*****************************************/
		//初始化应用程序时，调用RegisterWindowMessage函数以获取FINDMSGSTRING注册消息的消息标识符
		idFindMessage = RegisterWindowMessage(FINDMSGSTRING);//注册进程间消息

		//创建状态栏控件并初始化
		//方法1：ID_STATUSBAR为状态栏控件标识符
		//hWinStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP ,NULL,hWinMain,ID_STATUSBAR);
		//方法2：改用CreateWindowEx,L"msctls_statusbar32"为状态栏控件类名
		hWinStatus = CreateWindowEx(0, L"msctls_statusbar32", L"", WS_VISIBLE | WS_CHILD, 0, 0, 0, 0,
			hWinMain, (HMENU)ID_STATUSBAR, (HINSTANCE)GetWindowLong(hWinMain, -6), NULL);

		SendMessage(hWinStatus, SB_SETPARTS, 4, (LPARAM)dwStatusWidth);//设置状态窗口中的零件数以及每个零件右边缘的坐标
		lpsz1 = sz1;
		lpsz2 = sz2;
		SendMessage(hWinStatus, SB_SETTEXT, 2, (LPARAM)lpsz1);//设置状态栏文本
		//创建文本编辑控件
		hWinEdit = CreateWindowEx(WS_EX_CLIENTEDGE, szClass, NULL,
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | ES_AUTOHSCROLL | WS_VSCROLL,
			0, 0, 0, 0, hwnd, (HMENU)ID_EDIT, hInstance, NULL);
		//设置编辑控件的文本限制，默认1为30000个---最大字符数，可以是CHAR或TCHAR字符
		SendMessage(hWinEdit, EM_SETLIMITTEXT, 0, -1);//如果这里不设置，则无法达到最高字符限制30000
		_Resize();
		SetTimer(hwnd, 1, 300, NULL);//设置计时器

		return TRUE;

		/********************************************************************/
		//状态栏控件
	case WM_TIMER:
		GetLocalTime(&st);
		wsprintf(szBuffer, szFormat0, st.wHour, st.wMinute, st.wSecond);
		SendMessage(hWinStatus, SB_SETTEXT, 0, (LPARAM)szBuffer);
		return TRUE;
		//用户选择菜单项时发送到菜单的所有者窗口
	case WM_MENUSELECT://wparam菜单索引，lparam菜单句柄
		hInstance = GetModuleHandle(NULL);
		//在指定的状态窗口中显示有关当前菜单的帮助文本
		MenuHelp(WM_MENUSELECT, wparam, lparam, (HMENU)lparam, hInstance, hWinStatus, dwMenuHelp);
		return TRUE;

	case WM_SIZE:
		_Resize();
		return TRUE;

		//检测用户在第3栏的按鼠标动作并将文字在"插入"和"改写"之间切换
		//通用控件在某些状态发生变化时不通过发送WM_COMMAND而是发送WM_NOTIFY消息和父窗口通讯
	case WM_NOTIFY:
		if (wparam == ID_STATUSBAR)
		{
			/*
			UINT tmp;
			_asm
			{
				mov eax, lparam;
				mov eax,[eax+NMHDR.code]
				mov tmp,eax
			}
			if (tmp == NM_CLICK)
			*/
			//lparam指向NMHDR结构的指针，该结构包含通知代码和其他信息
			if (((LPNMHDR)lparam)->code == NM_CLICK)//通知代码
			{
				GetCursorPos(&pt);
				GetWindowRect(hWinStatus, &rect);
				int left1 = rect.left + 140;
				int left2 = rect.left + 172;
				if ((pt.x > left1) && (pt.x <= left2))//鼠标点击140~172之间
				{
					TCHAR* ptr = lpsz1;
					lpsz1 = lpsz2;
					lpsz2 = ptr;
					/*
					_asm
					{
						mov eax, lpsz1
						xchg eax, lpsz2
						mov lpsz1, eax
					}*/
					SendMessage(hWinStatus, SB_SETTEXT, 2, (LPARAM)lpsz1);
				}
			}
		}
		return TRUE;
		/*********************************************************************/

	case WM_COMMAND:

		switch (LOWORD(wparam))
		{
		case IDM_EXIT:
			EndDialog(hwnd, 0);
			break;
		case IDM_OPEN:
			_OpenFile();
			break;
		case IDM_SAVEAS:
			_SaveAs();
			break;
		case IDM_PAGESETUP:
			_PageSetup();
			break;
		case IDM_FIND:
			stFind.Flags &= (~FR_DIALOGTERM);//设置FR_DIALOGTERM除外的所有标志
			FindText(&stFind);
			break;
		case IDM_REPLACE:
			stFind.Flags &= (~FR_DIALOGTERM);
			ReplaceText(&stFind);
			break;
		case IDM_SELFONT:
			_ChooseFont();
			break;
		case IDM_SELCOLOR:
			_ChooseColor();
			break;
			//编辑控件
		case ID_EDIT:
			if (HIWORD(wparam) == EN_ERRSPACE || HIWORD(wparam) == EN_MAXTEXT)//没有空间了
				MessageBox(hwnd, TEXT("Edit control out of space."),
					NULL, MB_OK | MB_ICONSTOP);
			wsprintf(szBuffer, szFormat1, GetWindowTextLength(hWinEdit));
			SendMessage(hWinStatus, SB_SETTEXT, 1, (LPARAM)szBuffer);
			break;
		}
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return TRUE;

	default://因为switch语句中不可以使用 case idFindMessage：变量，所以改在default中处理
		if (idFindMessage == message)
		{
			if (stFind.Flags & FR_FINDNEXT)
			{
				wsprintf(szBuffer, szFormatFind, szFindNext, szFindText, szReplaceText);
				MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
			}
			else if (stFind.Flags & FR_REPLACE)
			{
				wsprintf(szBuffer, szFormatFind, szReplace, szFindText, szReplaceText);
				MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
			}
			else if (stFind.Flags & FR_REPLACEALL)
			{
				wsprintf(szBuffer, szFormatFind, szReplaceAll, szFindText, szReplaceText);
				MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
			}
			return TRUE;
		}
	}
	return FALSE;
}

void _OpenFile()
{
	OPENFILENAME stOF;
	static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0") \
		TEXT("All Files (*.*)\0*.*\0\0");
	HANDLE hFile;
	static TCHAR* szBuffer;
	static TCHAR* szBuffer1;
	DWORD dwReaded;
	DWORD fileSize;

	RtlZeroMemory(&stOF, sizeof(stOF));
	stOF.lStructSize = sizeof(stOF);
	stOF.hwndOwner = hWinMain;
	stOF.lpstrFilter = szFilter;
	stOF.lpstrFile = szFileName;
	stOF.nMaxFile = MAX_PATH;
	stOF.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	if (GetOpenFileName(&stOF))
	{
		//MessageBox(hWinMain, szFileName, szCaption, MB_OK);
		hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWinMain, L"文件打开失败！", NULL, MB_OK | MB_ICONSTOP);
			return;
		}
		fileSize = GetFileSize(hFile, NULL);
		szBuffer = malloc(fileSize);
		szBuffer1 = malloc(fileSize * 2);
		ReadFile(hFile, szBuffer, fileSize, &dwReaded, NULL);
		szBuffer[fileSize] = '\0';
		MultiByteToWideChar(CP_ACP, 0, (LPCCH)szBuffer, fileSize, szBuffer1, fileSize * 2);
		SetWindowText(hWinEdit, szBuffer1);
	}

}
void _ChooseColor()
{
	CHOOSECOLOR stCC;
	int dwBackColor = 0;
	int dwCustColors[16] = { 0 };
	static TCHAR szFormatColor[] = TEXT("您选择的颜色值：%08x");

	RtlZeroMemory(&stCC, sizeof(stCC));
	stCC.lStructSize = sizeof(stCC);
	stCC.hwndOwner = hWinMain;
	stCC.rgbResult = dwBackColor;
	stCC.Flags = CC_RGBINIT | CC_FULLOPEN;
	stCC.lpCustColors = dwCustColors;
	if (ChooseColor(&stCC))
	{
		dwBackColor = stCC.rgbResult;
		wsprintf(szBuffer, szFormatColor, dwBackColor);
		MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
	}
}
void _ChooseFont()
{
	CHOOSEFONT stCF;
	LOGFONT stLogFont;
	int dwFontColor = 0;
	static TCHAR szFormatFont[] = TEXT("您的选择：\n字体名称：%s\n")\
		TEXT("字体颜色值：%08x\n字体大小：%d");

	RtlZeroMemory(&stCF, sizeof(stCF));
	stCF.lStructSize = sizeof(stCF);
	stCF.hwndOwner = hWinMain;
	stCF.lpLogFont = &stLogFont;
	stCF.rgbColors = dwFontColor;
	stCF.Flags = CF_INITTOLOGFONTSTRUCT | CF_EFFECTS;
	if (ChooseFont(&stCF))
	{
		dwFontColor = stCF.rgbColors;
		wsprintf(szBuffer, szFormatFont, stLogFont.lfFaceName, dwFontColor, stCF.iPointSize);
		MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
	}
}
//从Windows Vista开始，PageSetupDlg不包含“打印机”按钮。要切换打印机选择，请使用PrintDlg或PrintDlgEx
void _PageSetup()
{
	PAGESETUPDLG stPS;
	static TCHAR szFormatPrt[] = TEXT("您选择的打印机：%s");

	RtlZeroMemory(&stPS, sizeof(stPS));
	stPS.lStructSize = sizeof(stPS);
	stPS.hwndOwner = hWinMain;
	//自动获取当前默认打印机设备
	if (PageSetupDlg(&stPS) && stPS.hDevMode)//"确定"且包含DEVMODE结构的全局内存对象的句柄
	{
		/*汇编写法
		_asm
		{
			mov eax, stPS.hDevMode
			mov eax, [eax]
			mov tmp,eax
		}
		*/
		//C语言写法
		int a = (int)stPS.hDevMode;
		//a = *((CHAR *)a);	//错误
		a = *(PDWORD)a;		//将int a 强制转换为指针，再取其存储的地址
		CHAR* tmp = (CHAR*)a;
		wsprintf(szBuffer, szFormatPrt, tmp);
		MessageBox(hWinMain, szBuffer, szCaption, MB_OK);
	}
}
void _SaveAs()
{
	OPENFILENAME stOF;
	static TCHAR szSaveCaption[] = TEXT("请输入保存的文件名");
	static TCHAR szDefExt[] = TEXT("txt");
	static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0") \
		TEXT("All Files (*.*)\0*.*\0\0");

	RtlZeroMemory(&stOF, sizeof(stOF));
	stOF.lStructSize = sizeof(stOF);
	stOF.hwndOwner = hWinMain;
	stOF.lpstrFilter = szFilter;
	stOF.lpstrFile = szFileName;
	stOF.nMaxFile = MAX_PATH;
	stOF.Flags = OFN_PATHMUSTEXIST;
	stOF.lpstrDefExt = szDefExt;
	stOF.lpstrTitle, szSaveCaption;
	if (GetSaveFileName(&stOF))
		MessageBox(hWinMain, szFileName, szCaption, MB_OK);
}
//调整尺寸
void _Resize()
{
	RECT stRect, stRect1;

	MoveWindow(hWinStatus, 0, 0, 0, 0, TRUE);
	GetWindowRect(hWinStatus, &stRect);
	GetClientRect(hWinMain, &stRect1);
	MoveWindow(hWinEdit, 0, 0, stRect1.right - stRect1.left,
		stRect1.bottom - stRect1.top - stRect.bottom + stRect.top, TRUE);
}
/*
PAGESETUPDLGA 结构：“页面设置”对话框的信息
typedef struct tagPSDA {
  DWORD           lStructSize;	//结构的大小（以字节为单位）
  HWND            hwndOwner;	//拥有对话框的窗口句柄
  HGLOBAL         hDevMode;		//包含DEVMODE结构的全局内存对象的句柄
  HGLOBAL         hDevNames;	//包含DEVNAMES结构的全局内存对象的句柄。此结构包含三个字符串，用于指定驱动程序名称、打印机名称和输出端口名称。
  DWORD           Flags;		//“页面设置”对话框的位标志
  POINT           ptPaperSize;	//用户选择的纸张尺寸
  RECT            rtMinMargin;	//左、上、右和下边距的最小允许宽度。
  RECT            rtMargin;		//左、上、右和下边距的宽度。
  HINSTANCE       hInstance;	//话框模板的应用程序或模块实例的句柄
  LPARAM          lCustData;	//创建对话框时指定的PAGESETUPDLG结构的指针
  LPPAGESETUPHOOK lpfnPageSetupHook;//一个指向PageSetupHook挂钩过程的指针
  LPPAGEPAINTHOOK lpfnPagePaintHook;//一个指向PagePaintHook钩子过程的指针
  LPCSTR          lpPageSetupTemplateName;//对话框模板资源的名称
  HGLOBAL         hPageSetupTemplate;//对话框模板的内存对象的句柄。
} PAGESETUPDLGA, *LPPAGESETUPDLGA;
*/