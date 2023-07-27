#include <windows.h>
#include "resource.h"

#define ID_EDIT     1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT ("PopPad2");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    HACCEL hAccel;
    HWND hwnd;
    MSG msg;
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

    hwnd = CreateWindow (szAppName, szAppName,
                         WS_OVERLAPPEDWINDOW,
                         GetSystemMetrics(SM_CXSCREEN) / 4,
                         GetSystemMetrics(SM_CYSCREEN) / 4,
                         GetSystemMetrics(SM_CXSCREEN) / 2,
                         GetSystemMetrics(SM_CYSCREEN) / 2,
                         NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    hAccel = LoadAccelerators(hInstance, szAppName);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return msg.wParam;
}


/*
这个函数名为 `AskConfirmation`，它接受一个 `HWND` 类型的参数 `hwnd`，代表一个窗口句柄。函数的作用是弹出一个消息框，询问用户是否真的要关闭 PopPad2 程序。

在消息框中，显示了一个文本消息："Really want to close PopPad2?"，窗口标题为 `szAppName`。消息框提供了两个按钮选项："Yes" 和 "No"。用户可以选择 "Yes" 表示确认关闭程序，或选择 "No" 表示取消关闭操作。

函数返回值是一个整型值，它代表用户的选择。如果用户选择 "Yes"，返回值为 `IDYES`，如果选择 "No"，返回值为 `IDNO`。调用该函数后，程序可以根据用户的选择来执行相应的操作，比如关闭窗口或取消关闭操作。
 */
int AskConfirmation(HWND hwnd) {
    return MessageBox(hwnd, TEXT ("Really want to close PopPad2?"),
                      szAppName, MB_YESNO | MB_ICONQUESTION);
}


/*
这个函数是 PopPad2 程序的主窗口过程函数。它用于处理窗口的各种消息，包括创建窗口、响应用户的操作、处理菜单命令、关闭窗口等。

- `WM_CREATE`：在窗口创建时，创建一个编辑框控件，并进行相关的初始化。
- `WM_SETFOCUS`：设置焦点到编辑框控件。
- `WM_SIZE`：在窗口大小变化时，调整编辑框控件的大小以适应新的窗口尺寸。
- `WM_INITMENUPOPUP`：在菜单弹出时，根据编辑框的状态，设置菜单项的可用性。
- `WM_COMMAND

`：处理菜单和编辑框的命令消息。
- `WM_CLOSE`：在关闭窗口之前，询问用户是否确认关闭，调用 `AskConfirmation` 函数。
- `WM_QUERYENDSESSION`：在关闭会话之前，询问用户是否确认关闭，调用 `AskConfirmation` 函数。
- `WM_DESTROY`：销毁窗口，并向消息队列发送退出消息，结束程序。

函数中的其他细节包括对菜单项的响应、对编辑框的操作，例如撤销、剪切、复制、粘贴、清除文本等。

请注意，函数中使用了一些未定义的标识符，例如 `AskConfirmation` 和 `szAppName`。这些标识符的定义应该在代码的其他部分或头文件中提供。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;   // 编辑框句柄
    int iSelect, iEnable;    // 临时变量

    switch (message) {
        case WM_CREATE:
            // 创建编辑框窗口
            hwndEdit = CreateWindow(TEXT("edit"), NULL,
                                    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                    WS_BORDER | ES_LEFT | ES_MULTILINE |
                                    ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                    0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                                    ((LPCREATESTRUCT) lParam)->hInstance, NULL);
            return 0;

        case WM_SETFOCUS:
            // 设置焦点到编辑框
            SetFocus(hwndEdit);
            return 0;

        case WM_SIZE:
            // 调整编辑框大小以适应窗口
            MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            return 0;

        case WM_INITMENUPOPUP:
            if (lParam == 1) {
                // 初始化编辑菜单弹出窗口
                EnableMenuItem((HMENU) wParam, IDM_EDIT_UNDO,
                               SendMessage(hwndEdit, EM_CANUNDO, 0, 0) ?
                               MF_ENABLED : MF_GRAYED);

                EnableMenuItem((HMENU) wParam, IDM_EDIT_PASTE,
                               IsClipboardFormatAvailable(CF_TEXT) ?
                               MF_ENABLED : MF_GRAYED);

                iSelect = SendMessage(hwndEdit, EM_GETSEL, 0, 0);

                if (HIWORD(iSelect) == LOWORD(iSelect))
                    iEnable = MF_GRAYED;
                else
                    iEnable = MF_ENABLED;

                EnableMenuItem((HMENU) wParam, IDM_EDIT_CUT, iEnable);
                EnableMenuItem((HMENU) wParam, IDM_EDIT_COPY, iEnable);
                EnableMenuItem((HMENU) wParam, IDM_EDIT_CLEAR, iEnable);
                return 0;
            }
            break;

        case WM_COMMAND:
            if (lParam) {
                // 编辑框命令处理
                if (LOWORD(lParam) == ID_EDIT &&
                    (HIWORD(wParam) == EN_ERRSPACE ||
                     HIWORD(wParam) == EN_MAXTEXT))
                    MessageBox(hwnd, TEXT("Edit control out of space."),
                               szAppName, MB_OK | MB_ICONSTOP);
                return 0;
            } else {
                // 菜单命令处理
                switch (LOWORD(wParam)) {
                    case IDM_FILE_NEW:
                    case IDM_FILE_OPEN:
                    case IDM_FILE_SAVE:
                    case IDM_FILE_SAVE_AS:
                    case IDM_FILE_PRINT:
                        MessageBeep(0);
                        return 0;

                    case IDM_APP_EXIT:
                        // 关闭窗口
                        SendMessage(hwnd, WM_CLOSE, 0, 0);
                        return 0;

                    case IDM_EDIT_UNDO:
                        // 撤销操作
                        SendMessage(hwndEdit, WM_UNDO, 0, 0);
                        return 0;

                    case IDM_EDIT_CUT:
                        // 剪切选中文本
                        SendMessage(hwndEdit, WM_CUT, 0, 0);
                        return 0;

                    case IDM_EDIT_COPY:
                        // 复制选中文本
                        SendMessage(hwndEdit, WM_COPY, 0, 0);
                        return 0;

                    case IDM_EDIT_PASTE:
                        // 粘贴剪贴板中的文本
                        SendMessage(hwndEdit, WM_PASTE, 0, 0);
                        return 0;

                    case IDM_EDIT_CLEAR:
                        // 清除选中文本
                        SendMessage(hwndEdit, WM_CLEAR, 0, 0);
                        return 0;

                    case IDM_EDIT_SELECT_ALL:
                        // 选中全部文本
                        SendMessage(hwndEdit, EM_SETSEL, 0, -1);
                        return 0;

                    case IDM_HELP_HELP:
                        MessageBox(hwnd, TEXT("Help not yet implemented!"),
                                   szAppName, MB_OK | MB_ICONEXCLAMATION);
                        return 0;

                    case IDM_APP_ABOUT:
                        MessageBox(hwnd, TEXT("POPPAD2 (c) Charles Petzold, 1998"),
                                   szAppName, MB_OK | MB_ICONINFORMATION);
                        return 0;
                }
            }
            break;

        case WM_CLOSE:
            // 关闭窗口前询问用户确认
            if (IDYES == AskConfirmation(hwnd))
                DestroyWindow(hwnd);
            return 0;

        case WM_QUERYENDSESSION:
            // 关闭会话前询问用户确认
            if (IDYES == AskConfirmation(hwnd))
                return 1;
            else
                return 0;

        case WM_DESTROY:
            // 销毁窗口并退出消息循环
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
