#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // 声明主窗口过程函数

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);  // 声明对话框过程函数

// WinMain函数，是Windows程序的入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("About1");  // 程序名称
    MSG msg;  // 消息结构体
    HWND hwnd;  // 主窗口句柄
    WNDCLASS wndclass;  // 窗口类结构体

    // 注册窗口类
    wndclass.style = CS_HREDRAW | CS_VREDRAW;  // 窗口样式
    wndclass.lpfnWndProc = WndProc;  // 主窗口过程函数
    wndclass.cbClsExtra = 0;  // 类附加内存大小
    wndclass.cbWndExtra = 0;  // 窗口附加内存大小
    wndclass.hInstance = hInstance;  // 应用程序实例句柄
    wndclass.hIcon = LoadIcon(hInstance, szAppName);  // 应用程序图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);  // 鼠标光标
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);  // 背景画刷
    wndclass.lpszMenuName = szAppName;  // 菜单资源名
    wndclass.lpszClassName = szAppName;  // 窗口类名

    // 注册窗口类，如果失败则显示错误信息
    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建主窗口
    hwnd = CreateWindow(szAppName, TEXT("About Box Demo Program"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    // 显示并更新主窗口
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 消息循环，等待并处理消息
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

// 主窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HINSTANCE hInstance;  // 应用程序实例句柄

    switch (message) {
        case WM_CREATE :
            // 在窗口创建时保存应用程序实例句柄
            hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
            return 0;

        case WM_COMMAND :
            switch (LOWORD(wParam)) {
                case IDM_APP_ABOUT :
                    // 当菜单项"About"被点击时弹出关于对话框
                    DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc);
                    break;
            }
            return 0;

        case WM_DESTROY :
            // 窗口销毁时向消息队列发送退出消息
            PostQuitMessage(0);
            return 0;
    }

    // 默认情况下交由系统处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// 关于对话框过程函数
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG :
            // 对话框初始化时返回TRUE
            return TRUE;

        case WM_COMMAND :
            switch (LOWORD(wParam)) {
                case IDOK :
                case IDCANCEL :
                    // 点击"OK"或"Cancel"按钮时关闭对话框
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
            break;
    }

    // 默认情况下交由系统处理
    return FALSE;
}
