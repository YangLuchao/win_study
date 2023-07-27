#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int iCurrentColor = IDC_BLACK,
        iCurrentFigure = IDC_RECT;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("About2");
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
/*
当前的代码是一个简单的Windows图形界面程序。主要包含以下几个函数：

1. `PaintWindow(HWND hwnd, int iColor, int iFigure)`：该函数用于在指定的窗口中绘制一个矩形或椭圆，并使用指定的颜色填充。
 参数`iColor`表示颜色选择，参数`iFigure`表示图形选择（矩形或椭圆）。
 在该函数中，通过`crColor`数组存储了8种颜色的RGB值，根据用户选择的颜色和图形进行绘制。

2. `PaintTheBlock(HWND hCtrl, int iColor, int iFigure)`：该函数用于更新指定控件（即块），使其显示当前选择的颜色和图形。
 它会先调用`InvalidateRect`和`UpdateWindow`函数刷新控件，然后调用`PaintWindow`函数绘制相应的图形。

3. `WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)`：该函数是窗口过程函数，用于处理窗口消息。
 在`WM_CREATE`消息中，保存应用程序实例句柄。在`WM_COMMAND`消息中，响应菜单命令，当选择“About”菜单时，显示关于对话框。
 在`WM_PAINT`消息中，绘制窗口的背景，并调用`PaintWindow`函数绘制当前选择的图形。

4. `AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)`：该函数是关于对话框的过程函数，用于处理对话框消息。
 在`WM_INITDIALOG`消息中，初始化对话框控件的状态。在`WM_COMMAND`消息中，响应对话框按钮和颜色/图形选择的变化，并更新显示。
 在`WM_PAINT`消息中，调用`PaintTheBlock`函数更新显示当前选择的图形和颜色。

整体代码结构清晰，是一个基本的Windows图形界面程序。
 */

// 函数 PaintWindow: 绘制指定颜色和图形的图案在窗口中
// 参数:
// - hwnd: 窗口句柄
// - iColor: 颜色选项 (IDC_BLACK, IDC_RED, IDC_GREEN, IDC_YELLOW, IDC_BLUE, IDC_MAGENTA, IDC_CYAN, IDC_WHITE)
// - iFigure: 图形选项 (IDC_RECT 表示矩形, IDC_ELLIPSE 表示椭圆)
void PaintWindow(HWND hwnd, int iColor, int iFigure) {
    static COLORREF crColor[8] = {RGB(0, 0, 0), RGB(0, 0, 255),
                                  RGB(0, 255, 0), RGB(0, 255, 255),
                                  RGB(255, 0, 0), RGB(255, 0, 255),
                                  RGB(255, 255, 0), RGB(255, 255, 255)};

    HBRUSH hBrush;
    HDC hdc;
    RECT rect;

    hdc = GetDC(hwnd); // 获取设备上下文
    GetClientRect(hwnd, &rect); // 获取窗口客户区域大小
    hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]); // 创建指定颜色的画刷
    hBrush = (HBRUSH) SelectObject(hdc, hBrush); // 选择画刷为当前画刷

    if (iFigure == IDC_RECT)
        Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom); // 绘制矩形
    else
        Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom); // 绘制椭圆

    DeleteObject(SelectObject(hdc, hBrush)); // 删除画刷并还原原画刷
    ReleaseDC(hwnd, hdc); // 释放设备上下文
}

// 函数 PaintTheBlock: 重绘指定控件，并调用 PaintWindow 进行绘制
// 参数:
// - hCtrl: 控件句柄
// - iColor: 颜色选项 (IDC_BLACK, IDC_RED, IDC_GREEN, IDC_YELLOW, IDC_BLUE, IDC_MAGENTA, IDC_CYAN, IDC_WHITE)
// - iFigure: 图形选项 (IDC_RECT 表示矩形, IDC_ELLIPSE 表示椭圆)
void PaintTheBlock(HWND hCtrl, int iColor, int iFigure) {
    InvalidateRect(hCtrl, NULL, TRUE); // 标记控件区域无效，触发 WM_PAINT 消息
    UpdateWindow(hCtrl); // 更新控件区域
    PaintWindow(hCtrl, iColor, iFigure); // 调用 PaintWindow 绘制图案
}

// 回调函数 WndProc: 主窗口消息处理函数
// 参数:
// - hwnd: 窗口句柄
// - message: 消息标识
// - wParam, lParam: 消息参数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HINSTANCE hInstance;
    PAINTSTRUCT ps;

    switch (message) {
        case WM_CREATE:
            hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_APP_ABOUT:
                    // 打开 About 对话框，并根据返回结果决定是否重绘主窗口
                    if (DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc))
                        InvalidateRect(hwnd, NULL, TRUE);
                    return 0;
            }
            break;

        case WM_PAINT:
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);

            PaintWindow(hwnd, iCurrentColor, iCurrentFigure); // 绘制主窗口图案
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// 回调函数 AboutDlgProc: About 对话框消息处理函数
// 参数:
// - hDlg: 对话框句柄
// - message: 消息标识
// - wParam, lParam: 消息参数
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hCtrlBlock;
    static int iColor, iFigure;

    switch (message) {
        case WM_INITDIALOG:
            // 初始化对话框，设置当前颜色和图形选项，并将焦点设在当前颜色按钮上
            iColor = iCurrentColor;
            iFigure = iCurrentFigure;

            CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
            CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, iFigure);

            hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

            SetFocus(GetDlgItem(hDlg, iColor));
            return FALSE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    // 确定按钮，保存当前选项并结束对话框
                    iCurrentColor = iColor;
                    iCurrentFigure = iFigure;
                    EndDialog(hDlg, TRUE);
                    return TRUE;

                case IDCANCEL:
                    // 取消按钮，不保存选项并结束对话框
                    EndDialog(hDlg, FALSE);
                    return TRUE;

                case IDC_BLACK:
                case IDC_RED:
                case IDC_GREEN:
                case IDC_YELLOW:
                case IDC_BLUE:
                case IDC_MAGENTA:
                case IDC_CYAN:
                case IDC_WHITE:
                    // 根据颜色选项重绘对话框中的图案
                    iColor = LOWORD(wParam);
                    CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
                    PaintTheBlock(hCtrlBlock, iColor, iFigure);
                    return TRUE;

                case IDC_RECT:
                case IDC_ELLIPSE:
                    // 根据图形选项重绘对话框中的图案
                    iFigure = LOWORD(wParam);
                    CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
                    PaintTheBlock(hCtrlBlock, iColor, iFigure);
                    return TRUE;
            }
            break;

        case WM_PAINT:
            PaintTheBlock(hCtrlBlock, iColor, iFigure); // 绘制对话框图案
            break;
    }
    return FALSE;
}
