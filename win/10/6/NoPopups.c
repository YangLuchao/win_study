#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("NoPopUps");
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

    hwnd = CreateWindow (szAppName,
                         TEXT("No-Popup Nested Menu Demonstration"),
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
- `WM_CREATE`：窗口创建消息，当窗口被创建时触发。在这里，加载菜单资源文件并保存菜单句柄。
- `hMenuMain`：主菜单的句柄。
- `hMenuFile`：File菜单的句柄。
- `hMenuEdit`：Edit菜单的句柄。

在函数的逻辑中，根据收到的消息类型进行处理：

- 当收到 `WM_CREATE` 消息时，加载菜单资源并将主菜单设置为窗口的菜单。
- 当收到 `WM_COMMAND` 消息时，根据 `LOWORD(wParam)` 的值判断具体的命令。在这里，处理了主菜单、File菜单和Edit菜单的选择命令。通过调用 `SetMenu` 函数将相应的菜单设置为窗口的菜单。
- 当收到 `WM_DESTROY` 消息时，窗口即将被销毁，设置主菜单为窗口的菜单，然后销毁File菜单和Edit菜单，并发送退出消息以终止应用程序。

对于其他未处理的消息，调用 `DefWindowProc` 函数进行默认的窗口过程处理。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HMENU hMenuMain, hMenuEdit, hMenuFile; // 静态变量，用于保存菜单句柄
    HINSTANCE hInstance;

    switch (message) {
        case WM_CREATE:
            // 窗口创建消息
            hInstance = (HINSTANCE) GetWindowLong(hwnd, -6);

            // 加载菜单资源
            hMenuMain = LoadMenu(hInstance, TEXT("MenuMain"));
            hMenuFile = LoadMenu(hInstance, TEXT("MenuFile"));
            hMenuEdit = LoadMenu(hInstance, TEXT("MenuEdit"));

            // 将主菜单设置为窗口菜单
            SetMenu(hwnd, hMenuMain);
            return 0;

        case WM_COMMAND:
            // 命令消息
            switch (LOWORD(wParam)) {
                case IDM_MAIN:
                    // 主菜单项被选择
                    SetMenu(hwnd, hMenuMain); // 设置主菜单
                    return 0;

                case IDM_FILE:
                    // "File"菜单项被选择
                    SetMenu(hwnd, hMenuFile); // 设置File菜单
                    return 0;

                case IDM_EDIT:
                    // "Edit"菜单项被选择
                    SetMenu(hwnd, hMenuEdit); // 设置Edit菜单
                    return 0;

                case IDM_FILE_NEW:
                case IDM_FILE_OPEN:
                case IDM_FILE_SAVE:
                case IDM_FILE_SAVE_AS:
                case IDM_EDIT_UNDO:
                case IDM_EDIT_CUT:
                case IDM_EDIT_COPY:
                case IDM_EDIT_PASTE:
                case IDM_EDIT_CLEAR:
                    // 处理File和Edit菜单的子菜单项命令
                    MessageBeep(0);
                    return 0;
            }
            break;

        case WM_DESTROY:
            // 窗口销毁消息
            SetMenu(hwnd, hMenuMain); // 将主菜单设置为窗口菜单
            DestroyMenu(hMenuFile); // 销毁File菜单
            DestroyMenu(hMenuEdit); // 销毁Edit菜单

            PostQuitMessage(0); // 发送退出消息
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}