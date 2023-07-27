#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);

int idFocus;
WNDPROC OldScroll[3];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Colors1");
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
    wndclass.hbrBackground = CreateSolidBrush(0);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Color Scroll"),
                         WS_OVERLAPPEDWINDOW,
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
这个函数实现了一个具有滚动条的颜色选择窗口。
 窗口分为左右两部分，左侧显示一个矩形区域，右侧显示三个滚动条和对应的标签和数值窗口。
 用户可以通过滚动条调节 RGB 颜色分量的值，实时改变矩形区域的颜色。
 窗口处理了滚动条消息、焦点设置、窗口大小调整和销毁等事件，并提供了对应的交互逻辑和界面更新操作。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static COLORREF crPrim[3] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255)};  // RGB颜色数组
    static HBRUSH hBrush[3], hBrushStatic;  // 画刷句柄数组
    static HWND hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;  // 滚动条、标签和数值窗口的句柄数组
    static int color[3], cyChar;  // 颜色值数组和字符高度
    static RECT rcColor;  // 颜色矩形区域
    static TCHAR *szColorLabel[] = {TEXT("Red"), TEXT("Green"), TEXT("Blue")};  // 颜色标签文本数组
    HINSTANCE hInstance;
    int i, cxClient, cyClient;  // 窗口客户区的宽度、高度
    TCHAR szBuffer[10];  // 临时字符串缓冲区

    switch (message) {
        case WM_CREATE:
            hInstance = (HINSTANCE) GetWindowLong(hwnd, -6);  // 获取窗口实例句柄
            hwndRect = CreateWindow(TEXT("static"), NULL, WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                                    0, 0, 0, 0, hwnd, (HMENU) 9, hInstance, NULL);  // 创建用于显示颜色的矩形窗口

            for (i = 0; i < 3; i++) {
                hwndScroll[i] = CreateWindow(TEXT("scrollbar"), NULL,
                                             WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
                                             0, 0, 0, 0, hwnd, (HMENU) i, hInstance, NULL);  // 创建垂直滚动条窗口

                SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, FALSE);  // 设置滚动条范围
                SetScrollPos(hwndScroll[i], SB_CTL, 0, FALSE);  // 设置滚动条位置
                hwndLabel[i] = CreateWindow(TEXT("static"), szColorLabel[i],
                                            WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            0, 0, 0, 0, hwnd, (HMENU) (i + 3), hInstance, NULL);  // 创建颜色标签窗口
                hwndValue[i] = CreateWindow(TEXT("static"), TEXT("0"),
                                            WS_CHILD | WS_VISIBLE | SS_CENTER,
                                            0, 0, 0, 0, hwnd, (HMENU) (i + 6), hInstance, NULL);  // 创建颜色数值窗口

                OldScroll[i] = (WNDPROC) SetWindowLong(hwndScroll[i], -4, (LONG) ScrollProc);  // 设置滚动条窗口过程函数

                hBrush[i] = CreateSolidBrush(crPrim[i]);  // 创建颜色对应的画刷
            }

            hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));  // 创建静态控件的背景画刷

            cyChar = HIWORD(GetDialogBaseUnits());  // 计算字符高度
            return 0;

        case WM_SIZE:
            cxClient = LOWORD(lParam);  // 获取窗口客户区宽度
            cyClient = HIWORD(lParam);  // 获取窗口客户区高度

            SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);  // 设置颜色矩形区域的位置和大小

            MoveWindow(hwndRect, 0, 0, cxClient / 2, cyClient, TRUE);  // 移动颜色矩形窗口

            for (i = 0; i < 3; i++) {
                MoveWindow(hwndScroll[i],
                           (2 * i + 1) * cxClient / 14, 2 * cyChar,
                           cxClient / 14, cyClient - 4 * cyChar, TRUE);  // 移动滚动条窗口

                MoveWindow(hwndLabel[i],
                           (4 * i + 1) * cxClient / 28, cyChar / 2,
                           cxClient / 7, cyChar, TRUE);  // 移动颜色标签窗口

                MoveWindow(hwndValue[i],
                           (4 * i + 1) * cxClient / 28,
                           cyClient - 3 * cyChar / 2,
                           cxClient / 7, cyChar, TRUE);  // 移动颜色数值窗口
            }
            SetFocus(hwnd);  // 设置焦点为主窗口
            return 0;

        case WM_SETFOCUS:
            SetFocus(hwndScroll[idFocus]);  // 设置焦点为当前滚动条窗口
            return 0;

        case WM_VSCROLL:
            i = GetWindowLong((HWND) lParam, GWL_ID);  // 获取滚动条的ID

            /*
- `SB_TOP`: 将滚动条位置设置为最上方。
- `SB_BOTTOM`: 将滚动条位置设置为最下方。
- `SB_LINEUP`: 将滚动条位置向上移动一行。
- `SB_LINEDOWN`: 将滚动条位置向下移动一行。
- `SB_PAGEUP`: 将滚动条位置向上翻一页。
- `SB_PAGEDOWN`: 将滚动条位置向下翻一页。
- `SB_THUMBPOSITION`: 根据滚动条的拇指位置设置滚动条位置。
- `SB_THUMBTRACK`: 根据滚动条的拇指跟踪设置滚动条位置。

这些枚举值对应于滚动条控件的操作和用户交互行为。根据不同的操作，可以通过这些枚举值来调整滚动条的位置和对应的数值。
             例如，`SB_LINEUP`表示向上移动一行，`SB_THUMBPOSITION`表示根据拇指位置设置滚动条位置等。
             通过处理这些枚举值对应的消息，可以实现滚动条的交互功能。
             */
            switch (LOWORD(wParam)) {
                case SB_PAGEDOWN:
                    color[i] += 15;
                    // fall through
                case SB_LINEDOWN:
                    color[i] = min(255, color[i] + 1);  // 增加颜色分量值
                    break;

                case SB_PAGEUP:
                    color[i] -= 15;
                    // fall through
                case SB_LINEUP:
                    color[i] = max(0, color[i] - 1);  // 减少颜色分量值
                    break;

                case SB_TOP:
                    color[i] = 0;  // 设置颜色分量为最小值
                    break;

                case SB_BOTTOM:
                    color[i] = 255;  // 设置颜色分量为最大值
                    break;

                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    color[i] = HIWORD(wParam);  // 设置颜色分量为滚动条拇指位置的值
                    break;

                default:
                    break;
            }
            SetScrollPos(hwndScroll[i], SB_CTL, color[i], TRUE);  // 设置滚动条位置
            wsprintf(szBuffer, TEXT("%i"), color[i]);  // 格式化颜色值为字符串
            SetWindowText(hwndValue[i], szBuffer);  // 设置颜色数值窗口的文本

            DeleteObject((HBRUSH) SetClassLong(hwnd, -10, (LONG) CreateSolidBrush(
                    RGB(color[0], color[1], color[2]))));  // 设置窗口的背景画刷为新的颜色

            InvalidateRect(hwnd, &rcColor, TRUE);  // 刷新颜色矩形区域
            return 0;

        case WM_CTLCOLORSCROLLBAR:
            i = GetWindowLong((HWND) lParam, GWL_ID);  // 获取控件的ID
            return (LRESULT) hBrush[i];  // 返回对应控件的画刷句柄

        case WM_CTLCOLORSTATIC:
            i = GetWindowLong((HWND) lParam, GWL_ID);  // 获取控件的ID

            if (i >= 3 && i <= 8)  // 静态文本控件
            {
                SetTextColor((HDC) wParam, crPrim[i % 3]);  // 设置文本颜色
                SetBkColor((HDC) wParam, GetSysColor(COLOR_BTNHIGHLIGHT));  // 设置背景颜色
                return (LRESULT) hBrushStatic;  // 返回静态控件的背景画刷
            }
            break;

            /*
- `WM_CTLCOLORSCROLLBAR`: 当一个窗口控件的背景颜色需要被绘制时发送的消息。在这个特定的代码中，用于定制滚动条控件的背景颜色。
- `WM_CTLCOLORSTATIC`: 当一个静态文本控件的背景颜色需要被绘制时发送的消息。在这个特定的代码中，用于定制静态文本控件的背景颜色。
- `WM_SYSCOLORCHANGE`: 当系统颜色方案发生改变时发送的消息。在这个特定的代码中，用于处理系统颜色变化的情况。
这些枚举值对应于窗口消息，用于处理窗口控件的外观定制和响应系统颜色变化的情况。
             通过处理这些消息，可以自定义滚动条和静态文本控件的颜色以及适应系统颜色的变化。
             */
        case WM_SYSCOLORCHANGE:
            DeleteObject(hBrushStatic);
            hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));  // 创建静态控件的背景画刷
            return 0;

        case WM_DESTROY:
            DeleteObject((HBRUSH) SetClassLong(hwnd, -10, (LONG) GetStockObject(WHITE_BRUSH)));  // 恢复窗口的默认背景画刷

            for (i = 0; i < 3; i++)
                DeleteObject(hBrush[i]);  // 删除画刷对象

            DeleteObject(hBrushStatic);  // 删除静态控件的背景画刷
            PostQuitMessage(0);  // 发送退出消息
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);  // 默认消息处理
}


/*
该函数根据消息类型进行不同的操作：

- `WM_KEYDOWN`：当键盘按键被按下时触发。在这个函数中，如果按下的键是 Tab 键（`wParam == VK_TAB`），
 则根据 Shift 键的状态选择下一个获得焦点的控件，并使用 `SetFocus` 函数将焦点设置在该控件上。
 根据滚动条控件的 ID，使用 `GetDlgItem` 获取父窗口中下一个控件的句柄，并将焦点设置在该控件上。
- `WM_SETFOCUS`：当控件获得焦点时触发。在这个函数中，将 `idFocus` 设置为当前控件的 ID，表示当前焦点所在的控件。

最后，函数通过调用 `CallWindowProc` 将消息传递给原始的窗口过程函数来处理。返回值为原始窗口过程函数的返回值，以完成消息的处理。
 */
LRESULT CALLBACK ScrollProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int id = GetWindowLong(hwnd, GWL_ID); // 获取滚动条控件的ID

    switch (message) {
        case WM_KEYDOWN :
            if (wParam == VK_TAB)
                SetFocus(GetDlgItem(GetParent(hwnd),
                                    (id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % 3));
            break;

        case WM_SETFOCUS :
            idFocus = id; // 设置焦点所在的控件ID
            break;
    }

    return CallWindowProc(OldScroll[id], hwnd, message, wParam, lParam); // 调用原始的窗口过程函数处理消息
}

