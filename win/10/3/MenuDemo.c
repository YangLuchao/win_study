#include <windows.h>
#include "resource.h"

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT ("MenuDemo");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
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
    wndclass.lpszMenuName = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Menu Demonstration"),
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

/**
这段代码是一个窗口过程函数（Window Procedure），用于处理窗口消息。以下是每个部分的含义：

- `WM_COMMAND`：处理菜单和工具栏命令的消息。
- `WM_TIMER`：处理定时器消息，定时触发的操作。
- `WM_DESTROY`：处理窗口销毁消息，进行清理操作并终止消息循环。
- 其他消息：默认情况下调用 `DefWindowProc` 来处理。

该窗口过程函数主要处理以下功能：

- 处理菜单项和工具栏按钮的命令，包括文件操作、编辑操作、背景颜色选择、计时器控制、帮助和关于信息的显示。
- 根据菜单项的选择更新窗口的背景颜色。
- 根据定时器消息发出蜂鸣声和控制计时器的启动和停止。
- 处理窗口销毁消息，进行资源清理操作并退出应用程序。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static int idColor[5] = {WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH};
    // 存储不同背景颜色对应的GDI对象ID，分别为白色、浅灰色、灰色、深灰色和黑色
    static int iSelection = IDM_BKGND_WHITE;  // 当前选中的背景颜色菜单项ID
    HMENU hMenu;
/*
- `IDM_FILE_NEW`：新建文件菜单项的ID。
- `IDM_FILE_OPEN`：打开文件菜单项的ID。
- `IDM_FILE_SAVE`：保存文件菜单项的ID。
- `IDM_FILE_SAVE_AS`：另存为菜单项的ID。
- `IDM_APP_EXIT`：退出程序菜单项的ID。
- `IDM_EDIT_UNDO`：撤销菜单项的ID。
- `IDM_EDIT_CUT`：剪切菜单项的ID。
- `IDM_EDIT_COPY`：复制菜单项的ID。
- `IDM_EDIT_PASTE`：粘贴菜单项的ID。
- `IDM_EDIT_CLEAR`：清除菜单项的ID。
- `IDM_BKGND_WHITE`：白色背景菜单项的ID。
- `IDM_BKGND_LTGRAY`：浅灰色背景菜单项的ID。
- `IDM_BKGND_GRAY`：灰色背景菜单项的ID。
- `IDM_BKGND_DKGRAY`：深灰色背景菜单项的ID。
- `IDM_BKGND_BLACK`：黑色背景菜单项的ID。
- `IDM_TIMER_START`：启动计时器菜单项的ID。
- `IDM_TIMER_STOP`：停止计时器菜单项的ID。
- `IDM_APP_HELP`：帮助菜单项的ID。
- `IDM_APP_ABOUT`：关于菜单项的ID。
 */
    switch (message) {
        case WM_COMMAND:
            hMenu = GetMenu(hwnd);

            switch (LOWORD(wParam)) {
                case IDM_FILE_NEW:
                case IDM_FILE_OPEN:
                case IDM_FILE_SAVE:
                case IDM_FILE_SAVE_AS:
                    // 处理文件操作菜单项的命令
                    MessageBeep(0);  // 发出蜂鸣声
                    return 0;

                case IDM_APP_EXIT:
                    // 处理退出程序菜单项的命令
                    SendMessage(hwnd, WM_CLOSE, 0, 0);  // 发送窗口关闭消息
                    return 0;

                case IDM_EDIT_UNDO:
                case IDM_EDIT_CUT:
                case IDM_EDIT_COPY:
                case IDM_EDIT_PASTE:
                case IDM_EDIT_CLEAR:
                    // 处理编辑操作菜单项的命令
                    MessageBeep(0);  // 发出蜂鸣声
                    return 0;

                case IDM_BKGND_WHITE:         // Note: Logic below
                case IDM_BKGND_LTGRAY:        //   assumes that IDM_WHITE
                case IDM_BKGND_GRAY:          //   through IDM_BLACK are
                case IDM_BKGND_DKGRAY:        //   consecutive numbers in
                case IDM_BKGND_BLACK:         //   the order shown here.
                    // 处理背景颜色菜单项的命令

                    CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);  // 取消之前选中项的勾选
                    iSelection = LOWORD(wParam);  // 更新当前选中的菜单项ID
                    CheckMenuItem(hMenu, iSelection, MF_CHECKED);  // 勾选当前选中项

                    SetClassLong(hwnd, -10, (LONG) GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));
                    // 设置窗口的背景画刷为选中背景颜色对应的GDI对象

                    InvalidateRect(hwnd, NULL, TRUE);  // 使整个客户区无效，触发重绘
                    return 0;

                case IDM_TIMER_START:
                    // 处理启动计时器菜单项的命令
                    if (SetTimer(hwnd, ID_TIMER, 1000, NULL)) {
                        // 设置定时器，每隔1秒触发一次定时器消息
                        EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);  // 禁用“启动计时器”菜单项
                        EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);  // 启用“停止计时器”菜单项
                    }
                    return 0;

                case IDM_TIMER_STOP:
                    // 处理停止计时器菜单项的命令
                    KillTimer(hwnd, ID_TIMER);  // 关闭定时器
                    EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);  // 启用“启动计时器”菜单项
                    EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);  // 禁用“停止计时器”菜单项
                    return 0;

                case IDM_APP_HELP:
                    // 处理帮助菜单项的命令
                    MessageBox(hwnd, TEXT("Help not yet implemented!"), szAppName, MB_ICONEXCLAMATION | MB_OK);
                    // 显示帮助信息框
                    return 0;

                case IDM_APP_ABOUT:
                    // 处理关于菜单项的命令
                    MessageBox(hwnd, TEXT("Menu Demonstration Program\n(c) Charles Petzold, 1998"), szAppName,
                               MB_ICONINFORMATION | MB_OK);
                    // 显示关于对话框
                    return 0;
            }
            break;

        case WM_TIMER:
            // 处理定时器消息
            MessageBeep(0);  // 发出蜂鸣声
            return 0;

        case WM_DESTROY:
            // 处理窗口销毁消息
            PostQuitMessage(0);  // 发送退出消息，使消息循环终止
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}