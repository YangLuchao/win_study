/*----------------------------------------
   BTNLOOK.C -- Button Look Program
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#include <windows.h>

struct {
    int iStyle;
    TCHAR *szText;
}
        button[] =
        {
                BS_PUSHBUTTON, TEXT ("PUSHBUTTON"),
                BS_DEFPUSHBUTTON, TEXT ("DEFPUSHBUTTON"),
                BS_CHECKBOX, TEXT ("CHECKBOX"),
                BS_AUTOCHECKBOX, TEXT ("AUTOCHECKBOX"),
                BS_RADIOBUTTON, TEXT ("RADIOBUTTON"),
                BS_3STATE, TEXT ("3STATE"),
                BS_AUTO3STATE, TEXT ("AUTO3STATE"),
                BS_GROUPBOX, TEXT ("GROUPBOX"),
                BS_AUTORADIOBUTTON, TEXT ("AUTORADIO"),
                BS_OWNERDRAW, TEXT ("OWNERDRAW")
        };

#define NUM (sizeof button / sizeof button[0])

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("BtnLook");
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
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Button Look"),
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndButton[NUM];
    static RECT rect;
    static TCHAR szTop[] = TEXT ("message            wParam       lParam"),
            szUnd[] = TEXT ("_______            ______       ______"),
            szFormat[] = TEXT ("%-16s%04X-%04X    %04X-%04X"),
            szBuffer[50];
    static int cxChar, cyChar;
    HDC hdc;
    PAINTSTRUCT ps;
    int i;

    switch (message) {
        case WM_CREATE :
            cxChar = LOWORD (GetDialogBaseUnits());
            cyChar = HIWORD (GetDialogBaseUnits());

            /*
             下面是对 CreateWindow 函数的参数的解释：

第一个参数是按钮的窗口类名，这里使用了 "button"。
第二个参数是按钮的文本，根据 button[i].szText 获取相应的文本。
第三个参数是按钮的样式，这里使用了 WS_CHILD、WS_VISIBLE 和 button[i].iStyle 的组合，这些值存储在 button 结构数组中。
第四个和第五个参数是按钮的位置，cxChar 和 cyChar * (1 + 2 * i) 分别表示按钮的 x 坐标和 y 坐标。这样可以使按钮在窗口中垂直排列。
第六个和第七个参数是按钮的宽度和高度，这里使用了 20 * cxChar 和 7 * cyChar / 4 来确定按钮的尺寸。
第八个参数是按钮的父窗口句柄，即 hwnd。
第九个参数是按钮的菜单标识符（HMENU），这里使用 i 作为标识符。
第十个参数是按钮所属的应用程序实例句柄，这里通过 (LPCREATESTRUCT) lParam)->hInstance 获取。
通过循环创建多个按钮，并为每个按钮设置相应的样式、文本、位置和尺寸，可以在窗口中显示多个可交互的按钮。
             */
            for (i = 0; i < NUM; i++)
                hwndButton[i] = CreateWindow (TEXT("button"),
                                              button[i].szText,
                                              WS_CHILD | WS_VISIBLE | button[i].iStyle,
                                              cxChar, cyChar * (1 + 2 * i),
                                              20 * cxChar, 7 * cyChar / 4,
                                              hwnd, (HMENU) i,
                                              ((LPCREATESTRUCT) lParam)->hInstance, NULL);
            return 0;

        case WM_SIZE :
            rect.left = 24 * cxChar;
            rect.top = 2 * cyChar;
            rect.right = LOWORD (lParam);
            rect.bottom = HIWORD (lParam);
            return 0;

        case WM_PAINT :
            InvalidateRect(hwnd, &rect, TRUE);

            hdc = BeginPaint(hwnd, &ps);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            SetBkMode(hdc, TRANSPARENT);

            TextOut(hdc, 24 * cxChar, cyChar, szTop, lstrlen(szTop));
            TextOut(hdc, 24 * cxChar, cyChar, szUnd, lstrlen(szUnd));

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DRAWITEM :
        case WM_COMMAND :
            // 窗口滑动
            ScrollWindow(hwnd, 0, -cyChar, &rect, &rect);

            hdc = GetDC(hwnd);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

            TextOut(hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
                    szBuffer,
                    wsprintf(szBuffer, szFormat,
                             message == WM_DRAWITEM ? TEXT ("WM_DRAWITEM") :
                             TEXT ("WM_COMMAND"),
                             HIWORD (wParam), LOWORD (wParam),
                             HIWORD (lParam), LOWORD (lParam)));

            ReleaseDC(hwnd, hdc);
            ValidateRect(hwnd, &rect);
            break;

        case WM_DESTROY :
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
