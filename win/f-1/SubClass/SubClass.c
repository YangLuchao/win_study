#include <windows.h>
#include <ctype.h>
#include <tchar.h>
#include <string.h>
#include "resource.h"

// 声明全局变量
HWND hWinMain;          // 主窗口句柄
LONG lpOldProcEdit;     // 编辑框的旧窗口过程函数指针
BOOL dwOption = FALSE;  // 用于避免重入的标志

// 函数声明
void _DecToHex();       // 将十进制字符串转换为十六进制
void _HexToDec();       // 将十六进制字符串转换为十进制
LRESULT CALLBACK ProcEdit(HWND, UINT, WPARAM, LPARAM);  // 编辑框的新窗口过程函数
BOOL CALLBACK ProcDlgMain(HWND, UINT, WPARAM, LPARAM);  // 主对话框的窗口过程函数

// 显示错误消息的函数
void ShowErrMsg()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    // 使用 FormatMessage 函数获取错误消息
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // 弹出消息框显示错误消息
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, "系统错误", MB_OK | MB_ICONSTOP);

    LocalFree(lpMsgBuf);
}

// WinMain 函数，程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nShowCmd)
{
    // 显示主对话框
    DialogBoxParam(hInstance, MAKEINTRESOURCE(DLG_MAIN), NULL, ProcDlgMain, 0);
    return 0;
}

// 主对话框的窗口过程函数
BOOL CALLBACK ProcDlgMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInstance;

    switch (message)
    {
    case WM_INITDIALOG:
        hInstance = GetModuleHandle(NULL);
        // 设置主对话框图标
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInstance, (LPCTSTR)ICO_MAIN));
        // 初始化对话框
        hWinMain = hWnd;
        // 限制编辑框文本长度，单位为 TCHAR
        SendDlgItemMessage(hWnd, IDC_HEX, EM_LIMITTEXT, 8, 0);
        SendDlgItemMessage(hWnd, IDC_DEC, EM_LIMITTEXT, 10, 0);
        // 替换编辑框的旧窗口过程函数
        lpOldProcEdit = SetWindowLong(GetDlgItem(hWnd, IDC_HEX), -4, (LONG)ProcEdit);
        return TRUE;

        /*********************************************************************/
    case WM_COMMAND:
        // 用 dwOption 避免重入，以防止递归调用
        if (!dwOption)
        {
            dwOption = TRUE;
            switch (LOWORD(wParam))
            {
            case IDC_HEX:
                _HexToDec();
                break;
            case IDC_DEC:
                _DecToHex();
                break;
            }
            dwOption = FALSE;
        }
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

// 将十六进制字符串转换为十进制
void _HexToDec()
{
    CHAR szBuffer[512];
    const CHAR szFmtHexToDec[] = "%u";
    int result = 0;

    // 获取编辑框中的文本内容
    GetDlgItemTextA(hWinMain, IDC_HEX, szBuffer, sizeof(szBuffer));

    CHAR* p = szBuffer;
    while (TRUE)
    {
        // 遍历十六进制字符串，将其转换为十进制数值
        if (*p == 0)
            break;
        if (*p > '9')
            *p -= ('A' - 10);
        else
            *p -= '0';
        result *= 16;
        result += *p;
        p++;
    }
    // 将结果以十进制格式显示在编辑框中
    wsprintfA(szBuffer, szFmtHexToDec, result);
    SetDlgItemTextA(hWinMain, IDC_DEC, szBuffer);
}

// 将十进制字符串转换为十六进制
void _DecToHex()
{
    TCHAR szBuffer[512];
    const TCHAR szFmtDecToHex[] = TEXT("%08X");

    // 获取编辑框中的十进制数值
    wsprintf(szBuffer, szFmtDecToHex, GetDlgItemInt(hWinMain, IDC_DEC, NULL, FALSE));

    // 将结果以十六进制格式显示在编辑框中
    SetDlgItemText(hWinMain, IDC_HEX, szBuffer);
}
