/*------------------------------------------
   WHATCLR.C -- Displays Color Under Cursor
                (c) Charles Petzold, 1998
  ------------------------------------------*/

#include <windows.h>

#define ID_TIMER    1

void FindWindowSize(int *, int *);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("WhatClr");
    HWND hwnd;
    int cxWindow, cyWindow;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    FindWindowSize(&cxWindow, &cyWindow);

    hwnd = CreateWindow (szAppName, TEXT("What Color"),
                         WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         cxWindow, cyWindow,
                         NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

/*
FindWindowSize函数用于计算窗口的大小，以适应所需的文本显示。
创建一个用于屏幕显示的设备上下文 hdcScreen。
获取设备上下文的字体信息 tm。
删除设备上下文，释放资源。
根据字体信息和系统指标计算窗口的宽度和高度。
WndProc函数是窗口过程的主体，处理窗口消息和事件。
在 WM_CREATE 消息中创建用于屏幕显示的设备上下文 hdcScreen，设置定时器。
在 WM_DISPLAYCHANGE 消息中删除当前设备上下文并创建新的设备上下文，以响应显示更改。
在 WM_TIMER 消息中获取鼠标光标位置和像素颜色值，如果颜色值发生变化，触发窗口重绘。
在 WM_PAINT 消息中开始绘制过程，获取客户区域的矩形大小，格式化颜色值为字符串，绘制文本于矩形中央。
在 WM_DESTROY 消息中删除设备上下文，停止定时器，并发送退出消息。
默认情况下，使用 DefWindowProc 处理其他未处理的消息。
以上注释提供了对代码的更详细解释，帮助读者理解其功能和实现方式。

以下是这几个局部变量的作用：

COLORREF cr, crLast：这些变量用于存储颜色值。cr 用于存储当前鼠标所在位置的像素颜色值，crLast 用于存储上一次记录的颜色值。这些变量用于比较当前颜色值与上一次颜色值是否不同，以确定是否需要进行窗口重绘。

HDC hdcScreen：这是一个设备上下文句柄，用于创建与屏幕显示相关的设备上下文。它在窗口创建时被创建，并在 WM_DISPLAYCHANGE 消息中根据显示更改进行更新。

HDC hdc：这是一个设备上下文句柄，用于在绘图操作中进行绘制。它在 WM_PAINT 消息中通过 BeginPaint 函数获取。

PAINTSTRUCT ps：这是一个结构体，用于在绘图操作中存储绘图信息，如绘图区域的矩形和设备上下文句柄。它在 WM_PAINT 消息中通过 BeginPaint 函数获取。

POINT pt：这是一个结构体，用于存储鼠标光标的位置。它在 WM_TIMER 消息中通过 GetCursorPos 函数获取。

RECT rc：这是一个矩形结构，用于存储窗口或客户区的坐标。它在 WM_PAINT 消息中通过 GetClientRect 函数获取。

TCHAR szBuffer[16]：这是一个用于存储文本的字符数组。在 WM_PAINT 消息中，用于格式化颜色值为字符串，并在窗口中心绘制文本。

这些局部变量在函数的不同部分使用，用于存储和处理绘图、颜色和位置等信息，以实现窗口的显示和交互功能。

 */
void FindWindowSize(int *pcxWindow, int *pcyWindow) {
    HDC hdcScreen;
    TEXTMETRIC tm;

    hdcScreen = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL); // 创建用于屏幕显示的设备上下文
    GetTextMetrics(hdcScreen, &tm); // 获取设备上下文的字体信息
    DeleteDC(hdcScreen); // 删除设备上下文，释放资源

    *pcxWindow = 2 * GetSystemMetrics(SM_CXBORDER) + // 计算窗口的宽度
                 12 * tm.tmAveCharWidth;

    *pcyWindow = 2 * GetSystemMetrics(SM_CYBORDER) + // 计算窗口的高度
                 GetSystemMetrics(SM_CYCAPTION) +
                 2 * tm.tmHeight;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static COLORREF cr, crLast;
    static HDC hdcScreen;
    HDC hdc;
    PAINTSTRUCT ps;
    POINT pt;
    RECT rc;
    TCHAR szBuffer[16];

    switch (message) {
        case WM_CREATE:
            hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL); // 创建用于屏幕显示的设备上下文
            SetTimer(hwnd, ID_TIMER, 100, NULL); // 设置定时器
            return 0;

        case WM_DISPLAYCHANGE:
            DeleteDC(hdcScreen); // 删除设备上下文，释放资源
            hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL); // 创建新的设备上下文
            return 0;

        case WM_TIMER:
            GetCursorPos(&pt); // 获取鼠标光标位置
            cr = GetPixel(hdcScreen, pt.x, pt.y); // 获取光标所在位置的像素颜色值

            if (cr != crLast) { // 检查当前颜色值是否与上一次不同
                crLast = cr;
                InvalidateRect(hwnd, NULL, FALSE); // 重绘窗口
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps); // 开始绘制

            GetClientRect(hwnd, &rc); // 获取客户区域的矩形大小

            wsprintf(szBuffer, TEXT("  %02X %02X %02X  "),
                     GetRValue(cr), GetGValue(cr), GetBValue(cr)); // 将颜色值格式化为字符串

            DrawText(hdc, szBuffer, -1, &rc,
                     DT_SINGLELINE | DT_CENTER | DT_VCENTER); // 在矩形中央绘制文本

            EndPaint(hwnd, &ps); // 结束绘制
            return 0;

        case WM_DESTROY:
            DeleteDC(hdcScreen); // 删除设备上下文，释放资源
            KillTimer(hwnd, ID_TIMER); // 停止定时器
            PostQuitMessage(0); // 发送退出消息
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam); // 默认消息处理
}
