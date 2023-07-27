#define WINVER 0x0500
/**
 typedef struct tagWNDCLASS {
  UINT      style;                  // 窗口类的风格
  WNDPROC   lpfnWndProc;            // 窗口过程函数的指针
  int       cbClsExtra;             // 窗口类额外的内存块大小
  int       cbWndExtra;             // 窗口额外的内存块大小
  HINSTANCE hInstance;              // 窗口实例的句柄
  HICON     hIcon;                  // 窗口类图标的句柄
  HCURSOR   hCursor;                // 窗口类光标的句柄
  HBRUSH    hbrBackground;          // 窗口背景画刷的句柄
  LPCTSTR   lpszMenuName;           // 窗口菜单的名称
  LPCTSTR   lpszClassName;          // 窗口类的名称
} WNDCLASS, *PWNDCLASS;
 */
#include <windows.h>
#include "04_SYSETS.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("SysMets4");
    // 窗口句柄
    HWND hwnd;
    // 消息
    MSG msg;

    // 窗口结构
    WNDCLASS wndclass;

    // 窗口样式
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    // 消息回调
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    // 当前进程句柄
    wndclass.hInstance = hInstance;
    // 图标
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // 鼠标(屏幕指针)
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // 背景颜色
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    // 鼠标
    wndclass.lpszMenuName = NULL;
    // 应用名
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("Program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建窗口
    hwnd = CreateWindow (
    // app名称
            szAppName,
    // 窗口名称
            TEXT("Get System Metrics No. 4"),
    // 窗口样式
            WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
    // 窗口默认的水平和垂直位置
            CW_USEDEFAULT, CW_USEDEFAULT,
    // 窗口默认的宽度和高度
            CW_USEDEFAULT, CW_USEDEFAULT,
    // 父窗口句柄，菜单句柄，进程句斌，父进程句柄
            NULL, NULL, hInstance, NULL);

    // 显示窗口
    ShowWindow(hwnd, iCmdShow);
    // 刷新窗口
    UpdateWindow(hwnd);

    // 获取消息
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // 默认字体宽度，字体个数，默认字体高度，窗口长度， 窗口高度，窗口最大高低
    static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
    // 环境设备句柄
    HDC hdc;
    int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
    // 绘图结构
    PAINTSTRUCT ps;
    // 滚动条结构
    SCROLLINFO si;
    TCHAR szBuffer[10];
    // 字体结构
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
            // 拿到设备句柄
            hdc = GetDC(hwnd);
            // 拿到默认字体信息
            GetTextMetrics(hdc, &tm);
            // 默认字体宽度
            cxChar = tm.tmAveCharWidth;
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
            // 默认字体高度
            cyChar = tm.tmHeight + tm.tmExternalLeading;
            // 释放设备环境句柄
            ReleaseDC(hwnd, hdc);

            // Save the width of the three columns

            iMaxWidth = 40 * cxChar + 22 * cxCaps;
            return 0;

        case WM_SIZE:
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);

            // Set vertical scroll bar range and page size

            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = NUMLINES - 1;
            si.nPage = cyClient / cyChar;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

            // Set horizontal scroll bar range and page size

            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = 2 + iMaxWidth / cxChar;
            si.nPage = cxClient / cxChar;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            return 0;

        case WM_VSCROLL:
            // Get all the vertical scroll bar information

            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            // Save the position for comparison later on

            iVertPos = si.nPos;

            switch (LOWORD (wParam)) {
                case SB_TOP:
                    si.nPos = si.nMin;
                    break;

                case SB_BOTTOM:
                    si.nPos = si.nMax;
                    break;

                case SB_LINEUP:
                    si.nPos -= 1;
                    break;

                case SB_LINEDOWN:
                    si.nPos += 1;
                    break;

                case SB_PAGEUP:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGEDOWN:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBTRACK:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }
            // Set the position and then retrieve it.  Due to adjustments
            //   by Windows it might not be the same as the value set.

            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            // If the position has changed, scroll the window and update it

            if (si.nPos != iVertPos) {
                ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos),
                             NULL, NULL);
                UpdateWindow(hwnd);
            }
            return 0;

        case WM_HSCROLL:
            // Get all the vertical scroll bar information

            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            // Save the position for comparison later on

            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;

            switch (LOWORD (wParam)) {
                case SB_LINELEFT:
                    si.nPos -= 1;
                    break;

                case SB_LINERIGHT:
                    si.nPos += 1;
                    break;

                case SB_PAGELEFT:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGERIGHT:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBPOSITION:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }
            // Set the position and then retrieve it.  Due to adjustments
            //   by Windows it might not be the same as the value set.

            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            // If the position has changed, scroll the window

            if (si.nPos != iHorzPos) {
                ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0,
                             NULL, NULL);
            }
            return 0;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_HOME:
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END:
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
                case VK_PRIOR:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                    break;
                case VK_NEXT:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;
                case VK_UP:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
                case VK_DOWN:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    break;
                case VK_LEFT:
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);
                    break;
                case VK_RIGHT:
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);
                    break;
            }
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            // Get vertical scroll bar position

            si.cbSize = sizeof(si);
            si.fMask = SIF_POS;
            GetScrollInfo(hwnd, SB_VERT, &si);
            iVertPos = si.nPos;

            // Get horizontal scroll bar position

            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;

            // Find painting limits

            iPaintBeg = max (0, iVertPos + ps.rcPaint.top / cyChar);
            iPaintEnd = min (NUMLINES - 1,
                             iVertPos + ps.rcPaint.bottom / cyChar);

            for (i = iPaintBeg; i <= iPaintEnd; i++) {
                x = cxChar * (1 - iHorzPos);
                y = cyChar * (i - iVertPos);

                TextOut(hdc, x, y,
                        sysmetrics[i].szLabel,
                        lstrlen(sysmetrics[i].szLabel));

                TextOut(hdc, x + 22 * cxCaps, y,
                        sysmetrics[i].szDesc,
                        lstrlen(sysmetrics[i].szDesc));

                SetTextAlign(hdc, TA_RIGHT | TA_TOP);

                TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
                        wsprintf(szBuffer, TEXT ("%5d"),
                                 GetSystemMetrics(sysmetrics[i].iIndex)));

                SetTextAlign(hdc, TA_LEFT | TA_TOP);
            }

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
