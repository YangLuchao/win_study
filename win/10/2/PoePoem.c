/*-------------------------------------------
   POEPOEM.C -- Demonstrates Custom Resource
                (c) Charles Petzold, 1998
  -------------------------------------------*/

#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    TCHAR szAppName[16], szCaption[64], szErrMsg[64];
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    LoadString(hInstance, IDS_APPNAME, szAppName,
               sizeof(szAppName) / sizeof(TCHAR));

    LoadString(hInstance, IDS_CAPTION, szCaption,
               sizeof(szCaption) / sizeof(TCHAR));

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, szAppName);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        LoadStringA(hInstance, IDS_APPNAME, (char *) szAppName,
                    sizeof(szAppName));

        LoadStringA(hInstance, IDS_ERRMSG, (char *) szErrMsg,
                    sizeof(szErrMsg));

        MessageBoxA(NULL, (char *) szErrMsg,
                    (char *) szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, szCaption,
                         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
该函数是窗口过程函数，用于处理主窗口的消息。函数中的注释如下：

- `HWND hwnd`: 窗口句柄。
- `UINT message`: 消息标识符，用于指示收到的消息类型。
- `WPARAM wParam`: 附加的消息参数，通常用于传递消息相关的信息。
- `LPARAM lParam`: 附加的消息参数，通常用于传递消息相关的信息。

函数的功能如下：

1. 首先定义一些静态变量，包括文本指针、资源句柄、滚动条句柄以及其他一些变量。
2. 根据收到的消息类型进行相应的处理。
3. 在 `WM_CREATE` 消息中，获取设备上下文句柄并获取文本度量信息，计算出字符宽度和字符高度。创建垂直滚动条，加载资源并锁定资源中的文本，计算文本行数，设置滚动条的范围和初始位置。
4. 在 `WM_SIZE` 消息中，调整滚动条的位置和尺寸，并设置焦点。
5. 在 `WM_SETFOCUS` 消息中，将焦点设置到滚动条上。
6. 在 `WM_VSCROLL` 消息中，根据滚动条的操作更新滚动条的位置，确保位置在范围内，并根据位置更新滚动条的显示，并重新绘制窗口。
7. 在 `WM_PAINT` 消息中，开始绘制，锁定资源中的文本，获取客户区矩形，并根据滚动条的位置和字符高度计算文本的起始绘制位置，使用 `DrawTextA` 函数绘制文本。
8. 在 `WM_DESTROY` 消息中，释放资源并发送退出消息，结束应用程序。

该函数的作用是处理主窗口的各种消息，包括创建窗口、改变窗口大小、绘制窗口内容、滚动条操作等。函数根据不同的消息类型执行相应的操作，更新窗口的状态、显示内容和滚动条位置。在 `WM_PAINT` 消息中，根据滚动条的位置和字符高度计算文本的起始绘制位置，并绘制文本内容。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static char *pText;           // 文本指针
    static HGLOBAL hResource;     // 资源句柄
    static HWND hScroll;          // 滚动条句柄
    /*
这些局部变量的含义如下：

- `iPosition`: 当前滚动条的位置，表示文本显示的起始行数。
- `cxChar`: 字符的平均宽度。
- `cyChar`: 字符的高度，包括字符本身和外部行距。
- `cyClient`: 客户区的高度。
- `iNumLines`: 文本的总行数。
- `xScroll`: 垂直滚动条的宽度。

这些变量在函数中用于记录和控制文本的显示和滚动，以及窗口的尺寸调整和绘制。
     通过这些变量的值，可以确定滚动条的位置、文本的显示范围和起始行数，并在绘制文本时进行相应的计算和调整。
     */
    static int iPosition, cxChar, cyChar, cyClient, iNumLines, xScroll;   // 其他变量
    HDC hdc;                      // 设备上下文句柄
    PAINTSTRUCT ps;               // 绘图结构体
    RECT rect;                    // 矩形区域
    TEXTMETRIC tm;                // 文本信息结构体

    switch (message) {
        case WM_CREATE:
            hdc = GetDC(hwnd);
            GetTextMetrics(hdc, &tm);            // 获取文本的度量信息
            cxChar = tm.tmAveCharWidth;          // 平均字符宽度
            cyChar = tm.tmHeight + tm.tmExternalLeading;    // 字符高度加外部行距
            ReleaseDC(hwnd, hdc);

            xScroll = GetSystemMetrics(SM_CXVSCROLL);    // 获取垂直滚动条的宽度

            // 创建垂直滚动条
            hScroll = CreateWindow(TEXT("scrollbar"), NULL,
                                   WS_CHILD | WS_VISIBLE | SBS_VERT,
                                   0, 0, 0, 0,
                                   hwnd, (HMENU) 1, hInst, NULL);

            // 加载资源并锁定资源中的文本
            hResource = LoadResource(hInst,
                                     FindResource(hInst, TEXT("AnnabelLee"), TEXT("TEXT")));
            pText = (char *) LockResource(hResource);

            // 计算文本行数
            iNumLines = 0;
            while (*pText != '\\' && *pText != '\0') {
                if (*pText == '\n')
                    iNumLines++;
                pText = AnsiNext(pText);
            }
            *pText = '\0';   // 添加字符串结束符

            // 设置滚动条的范围和初始位置
            SetScrollRange(hScroll, SB_CTL, 0, iNumLines, FALSE);
            SetScrollPos(hScroll, SB_CTL, 0, FALSE);
            return 0;

        case WM_SIZE:
            MoveWindow(hScroll, LOWORD(lParam) - xScroll, 0,
                       xScroll, cyClient = HIWORD(lParam), TRUE);
            SetFocus(hwnd);
            return 0;

        case WM_SETFOCUS:
            SetFocus(hScroll);
            return 0;

            /*
在这段代码中，`WM_VSCROLL` 消息处理滚动条的操作。根据滚动条的用户操作，采取不同的操作来更新滚动条的位置和文本的显示。

- `SB_TOP`：将滚动条置于顶部位置，即将 `iPosition` 设置为0，显示文本的第一行。
- `SB_BOTTOM`：将滚动条置于底部位置，即将 `iPosition` 设置为 `iNumLines`，显示文本的最后一行。
- `SB_LINEUP`：向上滚动一行，将 `iPosition` 减去1，即向上滚动一行文本。
- `SB_LINEDOWN`：向下滚动一行，将 `iPosition` 加上1，即向下滚动一行文本。
- `SB_PAGEUP`：向上滚动一页，将 `iPosition` 减去 `cyClient / cyChar`，即向上滚动一页的文本。
- `SB_PAGEDOWN`：向下滚动一页，将 `iPosition` 加上 `cyClient / cyChar`，即向下滚动一页的文本。
- `SB_THUMBPOSITION`：根据滚动条的位置设置 `iPosition`，通过 `HIWORD(wParam)` 获取滚动条的位置值。

在每个 `case` 中，根据操作更新了 `iPosition` 的值，并确保其在合理的范围内（0 到 `iNumLines` 之间）。
             如果 `iPosition` 的值与当前滚动条的位置不同，就更新滚动条的位置，并通过 `InvalidateRect` 函数使窗口无效，
             从而触发窗口的重绘操作以显示新的文本内容。

最后，返回值为 0，表示已经处理了 `WM_VSCROLL` 消息。
             */
        case WM_VSCROLL:
            switch (LOWORD(wParam)) {
                case SB_TOP:
                    iPosition = 0;
                    break;
                case SB_BOTTOM:
                    iPosition = iNumLines;
                    break;
                case SB_LINEUP:
                    iPosition -= 1;
                    break;
                case SB_LINEDOWN:
                    iPosition += 1;
                    break;
                case SB_PAGEUP:
                    iPosition -= cyClient / cyChar;
                    break;
                case SB_PAGEDOWN:
                    iPosition += cyClient / cyChar;
                    break;
                case SB_THUMBPOSITION:
                    iPosition = HIWORD(wParam);
                    break;
            }
            iPosition = max(0, min(iPosition, iNumLines));

            if (iPosition != GetScrollPos(hScroll, SB_CTL)) {
                SetScrollPos(hScroll, SB_CTL, iPosition, TRUE);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            pText = (char *) LockResource(hResource);

            GetClientRect(hwnd, &rect);
            rect.left += cxChar;
            rect.top += cyChar * (1 - iPosition);
            DrawTextA(hdc, pText, -1, &rect, DT_EXTERNALLEADING);

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            FreeResource(hResource);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

