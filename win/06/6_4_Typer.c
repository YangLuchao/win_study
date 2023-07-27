/*

第34个例子：文本编辑器---插入符号
    GetFoculs函数     获取焦点
    CreateCaret函数   创建光标
    SetCaretPos函数   设置光标位子
    ShowCaret函数     显示光标
    HideCaret函数     隐藏光标
    DestroyCaret函数  销毁光标
    GetCaretPos函数   获取光标位置
    WMY_SETFOCUS消息，810123
    WM_KILLFOCUS消息
注：TYPER程序 不允许使用 双字节宽度的字符
无搜索，替换，保存文件，拼写检查，滚动，帮助等扩展功能。
 */
#include <windows.h>

#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Typer");
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

    hwnd = CreateWindow (szAppName, TEXT("Typing Program"),
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
    static DWORD dwCharSet = DEFAULT_CHARSET;
    static int cxChar, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, xCaret, yCaret;
    static TCHAR *pBuffer = NULL;
    HDC hdc;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message) {
        case WM_INPUTLANGCHANGE:
            // 当输入语言更改时，更新字符集
            dwCharSet = wParam;
            // 继续执行下去

        case WM_CREATE:
            // 窗口创建时的处理
            hdc = GetDC(hwnd);

            // 创建逻辑字体，以便计算字符宽度和高度
            SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth; // 字符平均宽度
            cyChar = tm.tmHeight; // 字符高度

            // 删除逻辑字体并释放设备上下文
            DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
            ReleaseDC(hwnd, hdc);
            // 继续执行下去

        case WM_SIZE:
            // 窗口大小改变时的处理
            // 获取窗口大小，以像素为单位
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            // 计算窗口大小(字符)
            cxBuffer = max(1, cxClient / cxChar); // 窗口字符宽度
            cyBuffer = max(1, cyClient / cyChar); // 窗口字符高度

            // 为缓冲区分配内存并清除它
            if (pBuffer != NULL)
                free(pBuffer);
            // 创建保存窗口所有字符的缓冲区
            pBuffer = (TCHAR *) malloc(cxBuffer * cyBuffer * sizeof(TCHAR));
            // 初始化 pBuffer 为一个空字符 ('')
            for (y = 0; y < cyBuffer; y++)
                for (x = 0; x < cxBuffer; x++)
                    BUFFER(x, y) = ' ';
            // 插入符号定位在左上角
            xCaret = 0;
            yCaret = 0;
            // 如果窗口有焦点，将插入符号位置设置为当前窗口位置
            if (hwnd == GetFocus())
                SetCaretPos(xCaret * cxChar, yCaret * cyChar);
            // 重新绘制窗口
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_SETFOCUS:
            // 窗口获得焦点时的处理
            // 创建插入符号，指定大小 (cxChar, cyChar)，NULL 表示实心
            CreateCaret(hwnd, NULL, cxChar, cyChar);
            // 设置插入符号位置
            SetCaretPos(xCaret * cxChar, yCaret * cyChar);
            // 显示插入符号
            ShowCaret(hwnd);
            return 0;

        case WM_KILLFOCUS:
            // 窗口失去焦点时的处理
            // 隐藏插入符号
            HideCaret(hwnd);
            // 销毁插入符号
            DestroyCaret();
            return 0;

        case WM_KEYDOWN:
            // 键盘按下时的处理，用于移动光标
            switch (wParam) {
                case VK_HOME:
                    xCaret = 0;
                    break;

                case VK_END:
                    xCaret = cxBuffer - 1;
                    break;

                case VK_PRIOR:
                    yCaret = 0;
                    break;

                case VK_NEXT:
                    yCaret = cyBuffer - 1;
                    break;

                case VK_LEFT:
                    xCaret = max(xCaret - 1, 0);
                    break;

                case VK_RIGHT:
                    xCaret = min(xCaret + 1, cxBuffer - 1);
                    break;

                case VK_UP:
                    yCaret = max(yCaret - 1, 0);
                    break;

                case VK_DOWN:
                    yCaret = min(yCaret + 1, cyBuffer - 1);
                    break;

                case VK_DELETE:
                    // 处理删除键 (VK_DELETE)
                    // 将当前位置后面的字符依次向前移动一格
                    for (x = xCaret; x < cxBuffer - 1; x++)
                        BUFFER(x, yCaret) = BUFFER(x + 1, yCaret);

                    // 最后一个字符设为空字符 (' ')
                    BUFFER(cxBuffer - 1, yCaret) = ' ';
                    // 隐藏插入符号后开始绘制
                    HideCaret(hwnd);
                    hdc = GetDC(hwnd);

                    SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

                    // 在位置 (xCaret, yCaret) 处绘制字符
                    TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(xCaret, yCaret), cxBuffer - xCaret);

                    DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
                    ReleaseDC(hwnd, hdc);
                    ShowCaret(hwnd);
                    break;
            }
            // 设置插入符号位置
            SetCaretPos(xCaret * cxChar, yCaret * cyChar);
            return 0;

        case WM_CHAR:
            // 字符输入时的处理
            for (i = 0; i < (int) LOWORD(lParam); i++) {
                // 处理转义字符
                switch (wParam) {
                    case '\b':
                        // 处理退格键 ('\b')，删除前一个字符
                        if (xCaret > 0) {
                            xCaret--;
                            SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);
                        }
                        break;

                    case '\t':
                        // 处理制表符 ('\t')，相当于插入8个空格字符
                        do {
                            SendMessage(hwnd, WM_CHAR, ' ', 1);
                        } while (xCaret % 8 != 0);
                        break;

                    case '\n':
                        // 处理换行符 ('\n')，将 yCaret 增加 1，x 不变
                        if (++yCaret == cyBuffer)
                            yCaret = 0;
                        break;

                    case '\r':
                        // 处理回车符 ('\r')，将 xCaret 设为 0，yCaret 增加 1
                        xCaret = 0;
                        if (++yCaret == cyBuffer)
                            yCaret = 0;
                        break;

                    case '\x1B':
                        // 处理 ESC 键 ('\x1B')，清空屏幕，即将缓冲区的字符全部设为空格字符 (' ')
                        for (y = 0; y < cyBuffer; y++)
                            for (x = 0; x < cxBuffer; x++)
                                BUFFER(x, y) = ' ';

                        // 将插入符号位置设为左上角
                        xCaret = 0;
                        yCaret = 0;

                        // 强制重绘窗口
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;

                    default:
                        // 处理普通字符
                        // 将当前位置的字符设为输入的字符
                        BUFFER(xCaret, yCaret) = (TCHAR) wParam;

                        // 隐藏插入符号后开始绘制
                        HideCaret(hwnd);
                        hdc = GetDC(hwnd);

                        SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

                        // 在位置 (xCaret, yCaret) 处绘制字符
                        TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(xCaret, yCaret), 1);

                        DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
                        ReleaseDC(hwnd, hdc);
                        ShowCaret(hwnd);

                        // 将插入符号向右移动一格
                        if (++xCaret == cxBuffer) {
                            xCaret = 0;

                            // 如果插入符号已到达最后一行，则将其移到第一行
                            if (++yCaret == cyBuffer)
                                yCaret = 0;
                        }
                        break;
                }
            }

            // 设置插入符号位置
            SetCaretPos(xCaret * cxChar, yCaret * cyChar);
            return 0;

        case WM_PAINT:
            // 绘制窗口时的处理
            hdc = BeginPaint(hwnd, &ps);

            SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, NULL));

            // 在窗口上绘制缓冲区中的字符
            for (y = 0; y < cyBuffer; y++)
                TextOut(hdc, 0, y * cyChar, &BUFFER(0, y), cxBuffer);

            DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            // 窗口销毁时的处理
            PostQuitMessage(0);
            return 0;
    }

    // 默认的窗口过程
    return DefWindowProc(hwnd, message, wParam, lParam);
}

