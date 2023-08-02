#include <windows.h>
#include <tchar.h>
#include "resource.h"

// 全局变量声明
HWND hWinMain;          // 主窗口句柄
WNDPROC lpOldProcEdit;  // 编辑框的旧窗口过程函数指针

// 函数声明
void _SuperClass();     // 创建并注册新的窗口类
LRESULT CALLBACK ProcEdit(HWND, UINT, WPARAM, LPARAM);  // 编辑框的新窗口过程函数
BOOL CALLBACK ProcDlgMain(HWND, UINT, WPARAM, LPARAM);  // 主对话框的窗口过程函数

// WinMain 函数，程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nShowCmd)
{
	// 创建并注册新的窗口类
	_SuperClass();
	// 显示主对话框
	DialogBoxParam(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, ProcDlgMain, 0);
	return 0;
}

// 创建并注册新的窗口类
void _SuperClass()
{
	WNDCLASSEX stWC;
	const TCHAR szEditClass[] = TEXT("Edit");
	const TCHAR szClass[] = TEXT("HexEdit");

	stWC.cbSize = sizeof(stWC);
	// 获取 Edit 类的信息
	GetClassInfoEx(NULL, szEditClass, &stWC);
	lpOldProcEdit = stWC.lpfnWndProc;
	// 将编辑框的窗口过程替换为 ProcEdit
	stWC.lpfnWndProc = ProcEdit;
	stWC.hInstance = GetModuleHandle(0);
	stWC.lpszClassName = szClass;
	// 注册新的窗口类
	RegisterClassEx(&stWC);
}

// 主对话框的窗口过程函数
BOOL CALLBACK ProcDlgMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE hInstance;

	switch (message)
	{
	case WM_INITDIALOG:
		hInstance = GetModuleHandle(NULL);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInstance, (LPCTSTR)ICO_MAIN));
		return TRUE;

	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}

// 编辑框的新窗口过程函数
LRESULT CALLBACK ProcEdit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 允许的十六进制字符（包括退格键）
	static CHAR szAllowedChar[] = "0123456789ABCDEFabcdef\b";

	if (message == WM_CHAR)
	{
		// 检查输入的字符是否为允许的十六进制字符
		PBYTE p = szAllowedChar;
		int len = sizeof(szAllowedChar);
		char ch = (char)LOWORD(wParam);
		BOOL flag = FALSE;
		while (len--)
		{
			if (p[len] == ch)
			{
				flag = TRUE;
				break;
			}
		}

		if (flag)
		{
			// 若是允许的十六进制字符，则转换为大写字母并调用旧的窗口过程函数
			if (ch > '9')
				ch = toupper(ch);
			return CallWindowProc((WNDPROC)lpOldProcEdit, hwnd, message, (WPARAM)ch, lParam);
		}
		else
			return 0; // 非十六进制字符，不作处理
	}

	// 调用旧的窗口过程函数
	return CallWindowProc((WNDPROC)lpOldProcEdit, hwnd, message, wParam, lParam);
}
