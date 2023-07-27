#define WINVER 0x0500
#define _WIN32_WINNT 0x0500   // for Mouse Wheel support

#include <windows.h>
#include "04_SYSETS.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("SysMets");
    HWND hwnd;
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
        MessageBox(NULL, TEXT ("Program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Get System Metrics"),
                         WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
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
这段代码实现了一个具有滚动条和滚动功能的窗口，可以根据窗口的大小和滚动条的位置绘制系统指标的文本。
 它使用了水平和垂直滚动条来控制文本的可见区域，并对鼠标滚轮事件进行了处理，使用户能够通过滚动鼠标滚轮来滚动文本。
 其中涉及到获取文本信息、设置滚动条的范围和页面大小、处理滚动条滚动事件、处理键盘输入和鼠标滚轮事件、绘制文本等操作。
 通过这些功能，可以在窗口中显示大量文本，并在需要时进行滚动查看。

 局部变量的作用如下：

cxChar: 用于存储平均字符宽度。
cxCaps: 用于存储大写字母的宽度。
cyChar: 用于存储字符的高度。
cxClient: 用于存储客户区的宽度。
cyClient: 用于存储客户区的高度。
iMaxWidth: 用于存储三列的最大宽度。
iDeltaPerLine: 用于存储鼠标滚轮每滚动一行时的滚动距离。
iAccumDelta: 用于累积鼠标滚轮滚动的值，以便在达到滚动距离时执行滚动操作。
hdc: 用于存储设备上下文句柄，用于绘制操作。
i, x, y: 临时变量，用于循环迭代和坐标计算。
iVertPos, iHorzPos: 用于存储垂直和水平滚动条的当前位置。
iPaintBeg, iPaintEnd: 用于指定需要绘制的文本行的起始和结束索引。
ps: 用于绘制窗口的绘图结构。
si: 用于设置和获取滚动条的信息。
szBuffer: 临时缓冲区，用于将数字转换为字符串。
tm: 用于存储文本的度量信息，如字符宽度和高度。
ulScrollLines: 用于存储鼠标滚轮每次滚动的行数。
这些变量在 WndProc 函数的不同部分使用，用于存储和处理窗口和滚动条的相关信息，以及进行文本绘制和滚动操作的计算。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
    static int iDeltaPerLine, iAccumDelta;     // for mouse wheel logic
    HDC hdc;
    int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
    PAINTSTRUCT ps;
    SCROLLINFO si;
    TCHAR szBuffer[10];
    TEXTMETRIC tm;
    ULONG ulScrollLines;                  // for mouse wheel logic

    switch (message) {
        case WM_CREATE:
            // 获取设备上下文，用于获取文本信息
            hdc = GetDC(hwnd);

            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
            cyChar = tm.tmHeight + tm.tmExternalLeading;

            ReleaseDC(hwnd, hdc);

            // 保存三列的最大宽度
            iMaxWidth = 40 * cxChar + 22 * cxCaps;

            // Fall through for mouse wheel information

        case WM_SETTINGCHANGE:
            // 获取鼠标滚轮信息
            SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);

            // ulScrollLines 通常为 3 或 0（表示没有滚动）
            // WHEEL_DELTA 等于 120，因此 iDeltaPerLine 将为 40
            if (ulScrollLines)
                iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
            else
                iDeltaPerLine = 0;

            return 0;

        case WM_SIZE:
            // 窗口大小变化事件
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);

            // 设置垂直滚动条的范围和页面大小
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = NUMLINES - 1;
            si.nPage = cyClient / cyChar;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

            // 设置水平滚动条的范围和页面大小
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = 2 + iMaxWidth / cxChar;
            si.nPage = cxClient / cxChar;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            return 0;

        case WM_VSCROLL:
            // 垂直滚动条事件
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            iVertPos = si.nPos; // 保存当前垂直滚动条的位置

            switch (LOWORD(wParam)) {
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

            // 设置滚动条的位置，并获取调整后的位置
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);

            // 如果位置发生变化，滚动窗口并更新显示
            if (si.nPos != iVertPos) {
                ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
                UpdateWindow(hwnd);
            }
            return 0;

        case WM_HSCROLL:
            // 水平滚动条事件
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos; // 保存当前水平滚动条的位置

            switch (LOWORD(wParam)) {
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

            // 设置滚动条的位置，并获取调整后的位置
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            // 如果位置发生变化，水平滚动窗口
            if (si.nPos != iHorzPos) {
                ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0, NULL, NULL);
            }
            return 0;

        case WM_KEYDOWN:
            // 键盘处理键盘按下事件
            switch (wParam) {
                case VK_HOME:
                    // 发送滚动到顶部的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;

                case VK_END:
                    // 发送滚动到底部的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;

                case VK_PRIOR:
                    // 发送向上翻页的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
                    break;

                case VK_NEXT:
                    // 发送向下翻页的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;

                case VK_UP:
                    // 发送向上滚动一行的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;

                case VK_DOWN:
                    // 发送向下滚动一行的消息
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                    break;

                case VK_LEFT:
                    // 发送向左滚动一页的消息
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);
                    break;

                case VK_RIGHT:
                    // 发送向右滚动一页的消息
                    SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);
                    break;
            }
            return 0;
            //鼠标滚轮消息，wParam
            //高序位字指示滑轮旋转的距离，以滑轮增量的倍数或间隔表示，即120。
            //正值表示滚轮向前旋转，远离用户；负值表示滑轮向后旋转，朝向用户。
            //低序位字指示各种虚拟键是否已关闭。
            //lParam
            //低序位字指定指针的×坐标，相对于屏幕的左上角。
            //高序位字指定指针的y坐标（相对于屏幕左上角）。
        case WM_MOUSEWHEEL:
            // 鼠标滚轮事件
            if (iDeltaPerLine == 0)
                break;
            iAccumDelta += (short) HIWORD(wParam); // 120 or -120

            // 当累积滚动的值超过一行的滚动距离时，发送向上滚动一行的消息，并减去滚动距离
            while (iAccumDelta >= iDeltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                iAccumDelta -= iDeltaPerLine;
            }

            // 当累积滚动的值小于一行的负滚动距离时，发送向下滚动一行的消息，并增加滚动距离
            while (iAccumDelta <= -iDeltaPerLine) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
                iAccumDelta += iDeltaPerLine;
            }

            return 0;

        case WM_PAINT:
            // 窗口重绘事件
            hdc = BeginPaint(hwnd, &ps);

            // 获取垂直滚动条的位置
            si.cbSize = sizeof(si);
            si.fMask = SIF_POS;
            GetScrollInfo(hwnd, SB_VERT, &si);
            iVertPos = si.nPos;

            // 获取水平滚动条的位置
            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;

            // 计算绘制范围
            iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
            iPaintEnd = min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);

            for (i = iPaintBeg; i <= iPaintEnd; i++) {
                x = cxChar * (1 - iHorzPos);
                y = cyChar * (i - iVertPos);

                TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
                TextOut(hdc, x + 22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));

                SetTextAlign(hdc, TA_RIGHT | TA_TOP);

                TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
                        wsprintf(szBuffer, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex)));

                SetTextAlign(hdc, TA_LEFT | TA_TOP);
            }

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            // 窗口销毁事件
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}
