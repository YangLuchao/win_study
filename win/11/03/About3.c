#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("About3");
    MSG msg;
    HWND hwnd;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, szAppName);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = EllipPushWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = NULL;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = TEXT ("EllipPush");

    RegisterClass(&wndclass);

    hwnd = CreateWindow (szAppName, TEXT("About Box Demo Program"),
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

// 回调函数 WndProc: 主窗口消息处理函数
// 参数:
// - hwnd: 窗口句柄
// - message: 消息标识
// - wParam, lParam: 消息参数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HINSTANCE hInstance;

    switch (message) {
        case WM_CREATE:
            hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_APP_ABOUT:
                    // 当点击菜单项 "About" 时，打开 About 对话框并等待对话框关闭
                    DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc);
                    return 0;
            }
            break;

        case WM_DESTROY:
            // 当窗口关闭时，发送关闭消息给消息队列，退出消息循环
            PostQuitMessage(0);
            return 0;
    }
    // 对于未处理的消息，调用默认的窗口过程处理函数
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// 回调函数 AboutDlgProc: About 对话框消息处理函数
// 参数:
// - hDlg: 对话框句柄
// - message: 消息标识
// - wParam, lParam: 消息参数
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            // 对话框初始化时，返回 TRUE 表示焦点没有默认设置
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    // 当点击对话框中的 "OK" 按钮时，关闭对话框，返回值 0 表示没有出错
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
            break;
    }
    // 对于未处理的消息，返回 FALSE 表示消息未被处理
    return FALSE;
}

// 回调函数 EllipPushWndProc: 椭圆按钮窗口消息处理函数
// 参数:
// - hwnd: 窗口句柄
// - message: 消息标识
// - wParam, lParam: 消息参数
LRESULT CALLBACK EllipPushWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    TCHAR szText[40];
    HBRUSH hBrush;
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message) {
        case WM_PAINT:
            // 绘制椭圆按钮窗口的内容
            /*
1. `GetClientRect(hwnd, &rect);`: 获取椭圆按钮窗口的客户区矩形区域，存储在 `rect` 变量中。
2. `GetWindowText(hwnd, szText, sizeof(szText));`: 获取椭圆按钮窗口的文本内容，将其存储在 `szText` 字符数组中。
3. `hdc = BeginPaint(hwnd, &ps);`: 开始绘制，获取绘制设备上下文 `hdc`，并存储绘制的相关信息在 `ps` 结构中。
4. `hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));`: 创建一个实心画刷 `hBrush`，颜色与系统窗口背景颜色相同。
5. `hBrush = (HBRUSH) SelectObject(hdc, hBrush);`: 将创建的实心画刷选入设备上下文 `hdc`，以便在绘制时使用。
6. `SetBkColor(hdc, GetSysColor(COLOR_WINDOW));`: 设置绘制设备上下文 `hdc` 的背景颜色为系统窗口背景颜色。
7. `SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));`: 设置绘制设备上下文 `hdc` 的文本颜色为系统窗口文本颜色。
8. `Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);`: 绘制椭圆，根据客户区矩形 `rect` 的坐标绘制椭圆。
9. `DrawText(hdc, szText, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);`: 绘制文本，将按钮窗口的文本内容绘制在指定的客户区矩形 `rect` 中心位置。
10. `DeleteObject(SelectObject(hdc, hBrush));`: 删除并释放之前创建的实心画刷 `hBrush`。
11. `EndPaint(hwnd, &ps);`: 结束绘制，释放绘制设备上下文 `hdc`。
             */
            GetClientRect(hwnd, &rect);
            GetWindowText(hwnd, szText, sizeof(szText));

            hdc = BeginPaint(hwnd, &ps);

            hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
            hBrush = (HBRUSH) SelectObject(hdc, hBrush);
            SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
            SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

            Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
            DrawText(hdc, szText, -1, &rect,
                     DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            DeleteObject(SelectObject(hdc, hBrush));

            EndPaint(hwnd, &ps);
            return 0;

        case WM_KEYUP:
            // 当按键抬起时，如果是空格键，则向父窗口发送消息
            if (wParam != VK_SPACE)
                break;
            // fall through
        case WM_LBUTTONUP:
            // 当鼠标左键抬起时，向父窗口发送消息，以通知按钮点击事件
            SendMessage(GetParent(hwnd), WM_COMMAND,
                        GetWindowLong(hwnd, GWL_ID), (LPARAM) hwnd);
            return 0;
    }
    // 对于未处理的消息，调用默认的窗口过程处理函数
    return DefWindowProc(hwnd, message, wParam, lParam);
}
