#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#ifdef UNICODE
#define CF_TCHAR CF_UNICODETEXT
TCHAR szDefaultText[] = TEXT("Default Text - Unicode Version"); // 默认文本 - Unicode版本
TCHAR szCaption[] = TEXT("Clipboard Text Transfers - Unicode Version"); // 标题栏文本 - Unicode版本
#else
#define CF_TCHAR CF_TEXT
TCHAR szDefaultText[] = TEXT("Default Text - ANSI Version"); // 默认文本 - ANSI版本
TCHAR szCaption[] = TEXT("Clipboard Text Transfers - ANSI Version"); // 标题栏文本 - ANSI版本
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("ClipText"); // 应用程序名称
    HACCEL hAccel; // 快捷键句柄
    HWND hwnd; // 窗口句柄
    MSG msg; // 消息结构体
    WNDCLASS wndclass; // 窗口类结构体

    wndclass.style = CS_HREDRAW | CS_VREDRAW; // 窗口类的样式
    wndclass.lpfnWndProc = WndProc; // 指定窗口过程函数
    wndclass.cbClsExtra = 0; // 类附加内存
    wndclass.cbWndExtra = 0; // 窗口附加内存
    wndclass.hInstance = hInstance; // 实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 加载应用程序图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // 加载箭头光标
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景画刷
    wndclass.lpszMenuName = szAppName; // 菜单名
    wndclass.lpszClassName = szAppName; // 类名

    if (!RegisterClass(&wndclass)) { // 注册窗口类
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR); // 显示错误消息
        return 0; // 退出程序
    }

    hwnd = CreateWindow(szAppName, szCaption, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL); // 创建窗口

    ShowWindow(hwnd, iCmdShow); // 显示窗口
    UpdateWindow(hwnd); // 更新窗口

    hAccel = LoadAccelerators(hInstance, szAppName); // 加载快捷键

    while (GetMessage(&msg, NULL, 0, 0)) { // 循环获取消息
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) { // 处理加速键消息
            TranslateMessage(&msg); // 翻译消息
            DispatchMessage(&msg); // 分发消息
        }
    }
    return msg.wParam; // 返回退出码
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static PTSTR pText; // 文本指针
    BOOL bEnable; // 按钮使能标志
    HGLOBAL hGlobal; // 全局内存句柄
    HDC hdc; // 设备上下文句柄
    PTSTR pGlobal; // 全局内存指针
    PAINTSTRUCT ps; // 绘图结构体
    RECT rect; // 窗口客户区矩形

    switch (message) {
    case WM_CREATE:
        SendMessage(hwnd, WM_COMMAND, IDM_EDIT_RESET, 0); // 发送重置命令
        return 0;

    case WM_INITMENUPOPUP:
        EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, // 使能或禁用粘贴菜单项
            IsClipboardFormatAvailable(CF_TCHAR) ? MF_ENABLED : MF_GRAYED);

        bEnable = pText ? MF_ENABLED : MF_GRAYED; // 判断剪贴板中是否有文本

        EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, bEnable); // 使能或禁用剪切菜单项
        EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, bEnable); // 使能或禁用复制菜单项
        EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, bEnable); // 使能或禁用清空菜单项
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_EDIT_PASTE: // 粘贴菜单项命令
            OpenClipboard(hwnd); // 打开剪贴板

            if (hGlobal = GetClipboardData(CF_TCHAR)) { // 获取剪贴板中的数据
                pGlobal = GlobalLock(hGlobal); // 锁定全局内存

                if (pText) { // 如果已有文本，释放内存
                    free(pText);
                    pText = NULL;
                }
                pText = malloc(GlobalSize(hGlobal)); // 分配内存保存剪贴板中的数据
                lstrcpy(pText, pGlobal); // 复制数据到 pText
                InvalidateRect(hwnd, NULL, TRUE); // 刷新客户区
            }
            CloseClipboard(); // 关闭剪贴板
            return 0;

        case IDM_EDIT_CUT: // 剪切菜单项命令
        case IDM_EDIT_COPY: // 复制菜单项命令
            if (!pText) // 如果没有文本，返回
                return 0;

            hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (lstrlen(pText) + 1) * sizeof(TCHAR)); // 分配全局内存
            pGlobal = GlobalLock(hGlobal); // 锁定全局内存
            lstrcpy(pGlobal, pText); // 复制数据到全局内存
            GlobalUnlock(hGlobal); // 解锁全局内存

            OpenClipboard(hwnd); // 打开剪贴板
            EmptyClipboard(); // 清空剪贴板
            SetClipboardData(CF_TCHAR, hGlobal); // 设置剪贴板数据
            CloseClipboard(); // 关闭剪贴板

            if (LOWORD(wParam) == IDM_EDIT_COPY) // 如果是复制命令，直接返回
                return 0;
        case IDM_EDIT_CLEAR: // 清空菜单项命令
            if (pText) { // 如果有文本，释放内存
                free(pText);
                pText = NULL;
            }
            InvalidateRect(hwnd, NULL, TRUE); // 刷新客户区
            return 0;

        case IDM_EDIT_RESET: // 重置菜单项命令
            if (pText) { // 如果有文本，释放内存
                free(pText);
                pText = NULL;
            }

            pText = malloc((lstrlen(szDefaultText) + 1) * sizeof(TCHAR)); // 分配内存并复制默认文本
            lstrcpy(pText, szDefaultText); // 复制默认文本到 pText
            InvalidateRect(hwnd, NULL, TRUE); // 刷新客户区
            return 0;
        }
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps); // 开始绘制

        GetClientRect(hwnd, &rect); // 获取客户区矩形

        if (pText != NULL)
            DrawText(hdc, pText, -1, &rect, DT_EXPANDTABS | DT_WORDBREAK); // 绘制文本

        EndPaint(hwnd, &ps); // 结束绘制
        return 0;

    case WM_DESTROY:
        if (pText)
            free(pText); // 释放内存

        PostQuitMessage(0); // 发送退出消息
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam); // 默认消息处理
}
