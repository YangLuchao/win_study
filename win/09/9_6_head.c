/*---------------------------------------------
   HEAD.C -- Displays beginning (head) of file
             (c) Charles Petzold, 1998
  ---------------------------------------------*/

#include <windows.h>

#define ID_LIST     1
#define ID_TEXT     2

#define MAXREAD     8192

/*
在给定的代码中，这两个宏定义了一些标志位的组合，用于表示一组枚举值的含义。

1. `DIRATTR` 宏表示目录属性，其包含以下枚举值的组合：

   - `DDL_READWRITE`: 表示可读写的属性。
   - `DDL_READONLY`: 表示只读的属性。
   - `DDL_HIDDEN`: 表示隐藏的属性。
   - `DDL_SYSTEM`: 表示系统的属性。
   - `DDL_DIRECTORY`: 表示目录的属性。
   - `DDL_ARCHIVE`: 表示存档的属性。
   - `DDL_DRIVES`: 表示驱动器的属性。

   这些标志位用于在操作文件系统中的目录时指定相应的属性。

2. `DTFLAGS` 宏表示绘制文本的标志，其包含以下枚举值的组合：

   - `DT_WORDBREAK`: 在单词边界处换行。
   - `DT_EXPANDTABS`: 扩展制表符为等宽空格。
   - `DT_NOCLIP`: 不剪切文本，超出绘制区域的部分仍然可见。
   - `DT_NOPREFIX`: 不显示快捷键前缀。

   这些标志位用于在绘制文本时指定相应的绘制行为和格式。

这些宏定义的目的是将一组相关的枚举值组合在一起，以便在代码中使用时更加简洁和可读。通过使用这些宏，可以方便地传递一组标志位，以指定特定的行为和选项。
 */
#define DIRATTR     (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
                     DDL_DIRECTORY | DDL_ARCHIVE  | DDL_DRIVES)
#define DTFLAGS     (DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ListProc(HWND, UINT, WPARAM, LPARAM);

WNDPROC OldList;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("head");
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
    wndclass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("head"),
                         WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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
该函数是窗口过程函数，用于处理主窗口的消息。函数中的注释如下：

- `HWND hwnd`: 窗口句柄。
- `UINT message`: 消息标识符，用于指示收到的消息类型。
- `WPARAM wParam`: 附加的消息参数，通常用于传递消息相关的信息。
- `LPARAM lParam`: 附加的消息参数，通常用于传递消息相关的信息。

函数的功能如下：

1. 首先定义一些静态变量，包括文件有效性标志、缓冲区、列表框和静态文本窗口句柄、窗口客户区的矩形、当前文件路径和一些临时缓冲区。
2. 根据收到的消息类型进行相应的处理。
3. 在 `WM_CREATE` 消息中，获取对话框基本单位的字符宽度和高度，并创建列表框和静态文本窗

口。设置列表框的新窗口过程函数为 `ListProc`。
4. 在 `WM_SIZE` 消息中，更新窗口客户区的矩形。
5. 在 `WM_SETFOCUS` 消息中，将焦点设置为列表框。
6. 在 `WM_COMMAND` 消息中，处理列表框项的双击操作。获取当前选中的列表框项的文本，打开相应的文件进行验证。如果文件有效，更新文件路径、显示文件路径文本，否则根据情况更新目录并填充列表框内容。
7. 在 `WM_PAINT` 消息中，根据文件有效性读取文件内容，并在窗口客户区绘制文本。
8. 在 `WM_DESTROY` 消息中，发送退出消息，结束应用程序。

该函数的作用是处理主窗口的各种消息，包括创建窗口、改变窗口大小、设置焦点、处理列表框项的双击操作、绘制窗口内容等。函数根据不同的消息类型执行相应的操作，更新窗口的状态和显示内容。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static BOOL bValidFile;       // 标记是否为有效文件
    static BYTE buffer[MAXREAD];  // 文件读取缓冲区
    static HWND hwndList, hwndText;  // 列表框和文本框句柄
    static RECT rect;             // 窗口客户区矩形
    static TCHAR szFile[MAX_PATH + 1];  // 存储文件路径的字符串缓冲区
    HANDLE hFile;                 // 文件句柄
    HDC hdc;                      // 设备上下文句柄
    int i, cxChar, cyChar;        // 字符宽度和高度
    PAINTSTRUCT ps;               // 绘图结构体
    TCHAR szBuffer[MAX_PATH + 1]; // 通用字符缓冲区

    switch (message) {
        case WM_CREATE :
            // 窗口创建时执行的初始化操作
            cxChar = LOWORD(GetDialogBaseUnits());
            cyChar = HIWORD(GetDialogBaseUnits());
            rect.left = 20 * cxChar;
            rect.top = 3 * cyChar;

            // 创建列表框和文本框
            hwndList = CreateWindow(TEXT("listbox"), NULL,
                                    WS_CHILDWINDOW | WS_VISIBLE | LBS_STANDARD,
                                    cxChar, cyChar * 3,
                                    cxChar * 13 + GetSystemMetrics(SM_CXVSCROLL),
                                    cyChar * 10,
                                    hwnd, (HMENU) ID_LIST,
                                    (HINSTANCE) GetWindowLong(hwnd, -6),
                                    NULL);
            GetCurrentDirectory(MAX_PATH + 1, szBuffer);
            hwndText = CreateWindow(TEXT("static"), szBuffer,
                                    WS_CHILDWINDOW | WS_VISIBLE | SS_LEFT,
                                    cxChar, cyChar, cxChar * MAX_PATH, cyChar,
                                    hwnd, (HMENU) ID_TEXT,
                                    (HINSTANCE) GetWindowLong(hwnd, -6),
                                    NULL);
            OldList = (WNDPROC) SetWindowLong(hwndList, -4,
                                              (LPARAM) ListProc);

            // 向列表框中添加文件列表
            SendMessage(hwndList, LB_DIR, DIRATTR, (LPARAM) TEXT("*.*"));
            return 0;

        case WM_SIZE :
            // 窗口大小改变时更新客户区矩形
            rect.right = LOWORD(lParam);
            rect.bottom = HIWORD(lParam);
            return 0;

        case WM_SETFOCUS :
            // 设置焦点到列表框
            SetFocus(hwndList);
            return 0;

        case WM_COMMAND :
            // 处理菜单和列表框的命令消息
            if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_DBLCLK) {
                // 列表框双击处理
                if (LB_ERR == (i = SendMessage(hwndList, LB_GETCURSEL, 0, 0)))
                    break;

                // 获取选中的文件名
                SendMessage(hwndList, LB_GETTEXT, i, (LPARAM) szBuffer);

                // 检查文件是否有效
                if (INVALID_HANDLE_VALUE != (hFile = CreateFile(szBuffer,
                                                                GENERIC_READ, FILE_SHARE_READ, NULL,
                                                                OPEN_EXISTING, 0, NULL))) {
                    CloseHandle(hFile);
                    bValidFile = TRUE;

                    // 更新当前文件路径并在文本框显示
                    lstrcpy(szFile, szBuffer);
                    GetCurrentDirectory(MAX_PATH + 1, szBuffer);
                    if (szBuffer[lstrlen(szBuffer) - 1] != '\\')
                        lstrcat(szBuffer, TEXT("\\"));
                    SetWindowText(hwndText, lstrcat(szBuffer, szFile));
                } else {
                    // 文件无效时，还原当前路径，并重新列出文件列表
                    bValidFile = FALSE;
                    szBuffer[lstrlen(szBuffer) - 1] = '\0';
                    if (!SetCurrentDirectory(szBuffer + 1)) {
                        szBuffer[3] = ':';
                        szBuffer[4] = '\0';
                        SetCurrentDirectory(szBuffer + 2);
                    }
                    GetCurrentDirectory(MAX_PATH + 1, szBuffer);
                    SetWindowText(hwndText, szBuffer);

                    // 刷新列表框内容
                    SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
                    SendMessage(hwndList, LB_DIR, DIRATTR,
                                (LPARAM) TEXT("*.*"));
                }

                // 使窗口无效，触发重绘
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;

        case WM_PAINT :
            // 绘制窗口内容
            if (!bValidFile)
                break;

            // 打开文件并读取内容
            if (INVALID_HANDLE_VALUE == (hFile = CreateFile(szFile, GENERIC_READ,
                                                            FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL))) {
                bValidFile = FALSE;
                break;
            }
            ReadFile(hFile, buffer, MAXREAD, &i, NULL);
            CloseHandle(hFile);

            // 开始绘制
            hdc = BeginPaint(hwnd, &ps);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));


            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
            SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
            DrawTextA(hdc, buffer, i, &rect, DTFLAGS);

            // 结束绘制
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY :
            // 窗口销毁，向消息队列发送退出消息
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
这个函数是一个自定义的列表框窗口过程处理函数（即回调函数），用于处理列表框窗口的消息。函数中的注释如下：

- `HWND hwnd`: 列表框窗口句柄。
- `UINT message`: 消息标识符，用于指示收到的消息类型。
- `WPARAM wParam`: 附加的消息参数，通常用于传递消息相关的信息。
- `LPARAM lParam`: 附加的消息参数，通常用于传递消息相关的信息。

函数的功能如下：

1. 首先判断收到的消息是否为键盘按下消息（`WM_KEYDOWN`），并且按下的是回车键（`VK_RETURN`）。
2. 如果满足条件，向列表框的父窗口发送一个自定义的命令消息（`WM_COMMAND`），表示列表框项的双击操作。其中，`MAKELONG(1, LBN_DBLCLK)`用于生成命令消息的`wParam`参数，表示命令标识符为1，通知父窗口是列表框的双击操作，`lParam`参数表示列表框窗口句柄。
3. 最后，将消息传递给原始的列表框窗口过程处理函数（`CallWindowProc`），以便保持默认的处理方式，并返回处理结果。

该函数的作用是在列表框接收到回车键按下的消息时，向其父窗口发送一个自定义的命令消息，以通知父窗口发生了列表框项的双击操作，并将消息传递给默认的处理方式进行处理。
 */
LRESULT CALLBACK ListProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_KEYDOWN && wParam == VK_RETURN) {
        // 如果收到键盘按下消息，并且按下的是回车键
        // 向父窗口发送自定义的命令消息，表示双击列表框项
        SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(1, LBN_DBLCLK), (LPARAM) hwnd);
    }
    // 调用原始的列表框窗口过程处理函数，将消息传递给默认的处理方式
    return CallWindowProc(OldList, hwnd, message, wParam, lParam);
}

