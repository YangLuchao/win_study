#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("StokFont");
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

    hwnd = CreateWindow (szAppName, TEXT("Stock Fonts"),
                         WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // 7中备用习题
    static struct {
        int idStockFont;
        TCHAR *szStockFont;
    }
            stockfont[] = {OEM_FIXED_FONT, TEXT ("OEM_FIXED_FONT"),
                           ANSI_FIXED_FONT, TEXT ("ANSI_FIXED_FONT"),
                           ANSI_VAR_FONT, TEXT ("ANSI_VAR_FONT"),
                           SYSTEM_FONT, TEXT ("SYSTEM_FONT"),
                           DEVICE_DEFAULT_FONT, TEXT ("DEVICE_DEFAULT_FONT"),
                           SYSTEM_FIXED_FONT, TEXT ("SYSTEM_FIXED_FONT"),
                           DEFAULT_GUI_FONT, TEXT ("DEFAULT_GUI_FONT")};

    static int iFont, cFonts = sizeof stockfont / sizeof stockfont[0];
    HDC hdc;
    int i, x, y, cxGrid, cyGrid;
    PAINTSTRUCT ps;
    // 字面名称
    TCHAR szFaceName[LF_FACESIZE], szBuffer[LF_FACESIZE + 64];
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
            // 滚动条范围0~6
            SetScrollRange(hwnd, SB_VERT, 0, cFonts - 1, TRUE);
            return 0;
            // 显示分辨率更改后，CWM_DISPLAYCHANGE消息将发送到所有窗口
        case WM_DISPLAYCHANGE:
            // 改变屏幕分辨率后重绘窗口
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_VSCROLL:// 滑块消息
            switch (LOWORD (wParam)) {
                case SB_TOP:
                    iFont = 0;
                    break;
                case SB_BOTTOM:
                    iFont = cFonts - 1;
                    break;
                case SB_LINEUP:
                case SB_PAGEUP:
                    iFont -= 1;
                    break;
                case SB_LINEDOWN:
                case SB_PAGEDOWN:
                    iFont += 1;
                    break;
                case SB_THUMBPOSITION:
                    iFont = HIWORD (wParam);
                    break;
            }
            iFont = max (0, min(cFonts - 1, iFont));
            SetScrollPos(hwnd, SB_VERT, iFont, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_KEYDOWN:// 按键消息
            switch (wParam) {
                case VK_HOME:
                    SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
                    break;
                case VK_END:
                    SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
                    break;
                case VK_PRIOR: // page up 键
                case VK_LEFT:
                case VK_UP:
                    SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
                    break;
                case VK_NEXT:
                case VK_RIGHT:
                case VK_DOWN:
                    SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
                    break;
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            // 设置字体
            SelectObject(hdc, GetStockObject(stockfont[iFont].idStockFont));
            // 获取字体的字体名称
            GetTextFace(hdc, LF_FACESIZE, szFaceName);
            // 获取字体信息
            GetTextMetrics(hdc, &tm);
            // 2倍字符平均宽度 或 3被最大字符 的较大者
            cxGrid = max (3 * tm.tmAveCharWidth, 2 * tm.tmMaxCharWidth);
            cyGrid = tm.tmHeight + 3;

            TextOut(hdc, 0, 0, szBuffer,
                    wsprintf(szBuffer, TEXT (" %s: Face Name = %s, CharSet = %i"),
                            // 字体
                             stockfont[iFont].szStockFont,
                            // 字体名称和字符集
                             szFaceName, tm.tmCharSet));
            SetTextAlign(hdc, TA_TOP | TA_CENTER);
            // 垂线和水平线---画表格
            for (i = 0; i < 17; i++) {
                MoveToEx(hdc, (i + 2) * cxGrid, 2 * cyGrid, NULL);
                LineTo(hdc, (i + 2) * cxGrid, 19 * cyGrid);
                MoveToEx(hdc, cxGrid, (i + 3) * cyGrid, NULL);
                LineTo(hdc, 18 * cxGrid, (i + 3) * cyGrid);
            }
            //  垂直和水平标题
            for (i = 0; i < 16; i++) {
                TextOut(hdc, (2 * i + 5) * cxGrid / 2, 2 * cyGrid + 2, szBuffer,
                        wsprintf(szBuffer, TEXT ("%X-"), i));

                TextOut(hdc, 3 * cxGrid / 2, (i + 3) * cyGrid + 2, szBuffer,
                        wsprintf(szBuffer, TEXT ("-%X"), i));
            }
            // 输出字符
            for (y = 0; y < 16; y++)
                for (x = 0; x < 16; x++) {
                    TextOut(hdc, (2 * x + 5) * cxGrid / 2,
                            (y + 3) * cyGrid + 2, szBuffer,
                            wsprintf(szBuffer, TEXT ("%c"), 16 * x + y));
                }

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
GetTextFace函数：检索被选择转换为指定的设备上下文的字体的字体名称。
int GetTextFaceA6 (
    HDC hdc,    //设备上下文的句柄
    int c,      //指向的缓冲区的长度。对于ANSI函数，它是一个BYTE计数，对于Unicode函数，它是一个W
    LPSTR lpName//指向接收字体名称的缓冲区的指针。如果此参数为NULL,则该函数返回名称中的字符数，
);

 WM_DISPLAYCHANGE消息：显示分辨率更改后，WM_DISPLAYCHANGE消息将发送到所有窗口。
 窗口通过其WindowProc函数接收此消息。
 */
