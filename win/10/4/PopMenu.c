#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;
TCHAR szAppName[] = TEXT ("PopMenu");

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
    wndclass.hIcon = LoadIcon(NULL, szAppName);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hInst = hInstance;

    hwnd = CreateWindow (szAppName, TEXT("Popup Menu Demonstration"),
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
    /*
- `static HMENU hMenu;`：静态变量，存储菜单句柄。
- `static int idColor[5] = {WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH};`：静态变量，存储背景颜色对应的逻辑笔。
- `static int iSelection = IDM_BKGND_WHITE;`：静态变量，存储当前选中的背景颜色菜单项。
- `POINT point;`：鼠标坐标变量。
     */
    static HMENU hMenu;                        // 菜单句柄
    static int idColor[5] = {WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH, DKGRAY_BRUSH, BLACK_BRUSH};  // 背景颜色对应的逻辑笔
    static int iSelection = IDM_BKGND_WHITE;    // 当前选中的背景颜色菜单项
    POINT point;                                // 鼠标坐标

    switch (message) {
        case WM_CREATE:
            /*
 - `case WM_CREATE:`：处理窗口创建消息。
  - `hMenu = LoadMenu(hInst, szAppName);`：加载菜单。
  - `hMenu = GetSubMenu(hMenu, 0);`：获取第一个子菜单。
  - `return 0;`：返回0表示消息处理完毕。
             */
            hMenu = LoadMenu(hInst, szAppName);      // 加载菜单
            hMenu = GetSubMenu(hMenu, 0);            // 获取第一个子菜单
            return 0;

        case WM_RBUTTONUP:
            /*
- `case WM_RBUTTONUP:`：处理鼠标右键弹起消息。
  - `point.x = LOWORD(lParam);`：获取鼠标点击位置的 x 坐标。
  - `point.y = HIWORD(lParam);`：获取鼠标点击位置的 y 坐标。
  - `ClientToScreen(hwnd, &point);`：将鼠标坐标转换为屏幕坐标。
  - `TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);`：弹出菜单。
  - `return 0;`：返回0表示消息处理完毕。
             */
            point.x = LOWORD(lParam);                 // 获取鼠标点击位置的 x 坐标
            point.y = HIWORD(lParam);                 // 获取鼠标点击位置的 y 坐标
            ClientToScreen(hwnd, &point);              // 将鼠标坐标转换为屏幕坐标

            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL);  // 弹出菜单
            return 0;

        case WM_COMMAND:
            /*
 - `case WM_COMMAND:`：处理命令消息。
  - `switch (LOWORD(wParam))`：根据命令的低位字识别命令ID。
  - `case IDM_FILE_NEW:` 到 `case IDM_EDIT_CLEAR:`：针对文件和编辑命令，发出蜂鸣声并返回0表示处理完毕。
  - `case IDM_BKGND_WHITE:` 到 `case IDM_BKGND_BLACK:`：处理背景颜色菜单项。
    - `CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);`：取消之前选中的背景颜色菜单项的标记。
    - `iSelection = LOWORD(wParam);`：更新当前选中的背景颜色菜单项。
    - `CheckMenuItem(hMenu, iSelection, MF_CHECKED);`：在菜单中标记当前选中的背景颜色菜单项。
    - `SetClassLong(hwnd, -10, (LONG)GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));`：设置窗口类的背景颜色。
    - `InvalidateRect(hwnd, NULL, TRUE);`：使整个窗口无效，触发重绘。
    - `return 0;`：返回0表示消息处理完毕。
  - `case IDM_APP_ABOUT:`：显示关于对话框。
  - `case IDM_APP_EXIT:`：发送关闭窗口消息。
  - `case IDM_APP_HELP:`：显示帮助提示。
  - `break;`：退出当前 switch 语句。
             */
            switch (LOWORD(wParam)) {
                /*
                 以下枚举定义在resource.h文件中，Visual Studio自动生成
IDM_FILE_NEW: 新建文件命令
IDM_FILE_OPEN: 打开文件命令
IDM_FILE_SAVE: 保存文件命令
IDM_FILE_SAVE_AS: 另存为命令
IDM_EDIT_UNDO: 撤销命令
IDM_EDIT_CUT: 剪切命令
IDM_EDIT_COPY: 复制命令
IDM_EDIT_PASTE: 粘贴命令
IDM_EDIT_CLEAR: 清除命令
IDM_BKGND_WHITE: 白色背景菜单项
IDM_BKGND_LTGRAY: 浅灰色背景菜单项
IDM_BKGND_GRAY: 灰色背景菜单项
IDM_BKGND_DKGRAY: 深灰色背景菜单项
IDM_BKGND_BLACK: 黑色背景菜单项
IDM_APP_ABOUT: 关于命令
IDM_APP_EXIT: 退出命令
IDM_APP_HELP: 帮助命令
                 */
                case IDM_FILE_NEW:
                case IDM_FILE_OPEN:
                case IDM_FILE_SAVE:
                case IDM_FILE_SAVE_AS:
                case IDM_EDIT_UNDO:
                case IDM_EDIT_CUT:
                case IDM_EDIT_COPY:
                case IDM_EDIT_PASTE:
                case IDM_EDIT_CLEAR:
                    MessageBeep(0);
                    return 0;

                case IDM_BKGND_WHITE:         // 注意：下面的逻辑假设 IDM_WHITE 到 IDM_BLACK 是连续编号的菜单项
                case IDM_BKGND_LTGRAY:
                case IDM_BKGND_GRAY:
                case IDM_BKGND_DKGRAY:
                case IDM_BKGND_BLACK:

                    CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);    // 取消之前选中的背景颜色菜单项
                    iSelection = LOWORD(wParam);                        // 更新当前选中的背景颜色菜单项
                    CheckMenuItem(hMenu, iSelection, MF_CHECKED);       // 在菜单中标记当前选中的背景颜色菜单项

                    SetClassLong(hwnd, -10,
                                 (LONG) GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE]));  // 设置窗口类的背景颜色

                    InvalidateRect(hwnd, NULL, TRUE);   // 使整个窗口无效，触发重绘
                    return 0;

                case IDM_APP_ABOUT:
                    MessageBox(hwnd, TEXT("Popup Menu Demonstration Program\n")
                                     TEXT("(c) Charles Petzold, 1998"), szAppName, MB_ICONINFORMATION | MB_OK);
                    return 0;

                case IDM_APP_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    return 0;

                case IDM_APP_HELP:
                    MessageBox(hwnd, TEXT("Help not yet implemented!"), szAppName, MB_ICONEXCLAMATION | MB_OK);
                    return 0;
            }
            break;
/*
- `case WM_DESTROY:`：处理窗口销毁消息。
  - `PostQuitMessage(0);`：向消息队列发送退出消息。
  - `return 0;`：返回0表示消息处理完毕。
- `return DefWindowProc(hwnd, message, wParam, lParam);`：调用默认的窗口过程处理其他消息。
 */
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}