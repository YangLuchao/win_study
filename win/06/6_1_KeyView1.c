#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("KeyView1");
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

    hwnd = CreateWindow (szAppName, TEXT("Keyboard Message Viewer #1"),
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
    // cxClientMax, 窗口最大宽
    // cyClientMax, 窗口最大高
    // cxClient, 窗口的宽
    // cyClient, 窗口的高
    // cxChar, 字符的宽
    // cyChar, 字符的高
    static int cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar;
    // 当前窗口最大行数
    static int cLinesMax, cLines;
    // 消息指针
    static PMSG pmsg;
    // 窗口滚动区域
    static RECT rectScroll;
    // 表头
    static TCHAR szTop[] = TEXT("Message        Key       Char     Repeat Scan Ext ALT Prev Tran");
    static TCHAR szUnd[] = TEXT("_______        ___       ____     ______ ____ ___ ___ ____ ____");

    static TCHAR *szFormat[2] = {
            TEXT("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s"),
            TEXT("%-13s            0x%04X%1s%c %6u %4d %3s %3s %4s %4s")
    };

    static TCHAR *szYes = TEXT("Yes");
    static TCHAR *szNo = TEXT("No");
    static TCHAR *szDown = TEXT("Down");
    static TCHAR *szUp = TEXT("Up");

    // 消息类型
    static TCHAR *szMessage[] = {
            TEXT("WM_KEYDOWN"), TEXT("WM_KEYUP"),
            TEXT("WM_CHAR"), TEXT("WM_DEADCHAR"),
            TEXT("WM_SYSKEYDOWN"), TEXT("WM_SYSKEYUP"),
            TEXT("WM_SYSCHAR"), TEXT("WM_SYSDEADCHAR")
    };
    // 设备环境句柄
    HDC hdc;
    int i, iType;
    // 绘图结构
    PAINTSTRUCT ps;
    TCHAR szBuffer[128], szKeyName[32];
    // 字体结构体
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
        case WM_DISPLAYCHANGE: //显示器分辨率改变时，此消息仅发送到顶级窗口
            // 获得最大的客户区
            cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
            cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

            // 获取等宽字体大小
            hdc = GetDC(hwnd);

            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cyChar = tm.tmHeight;

            ReleaseDC(hwnd, hdc);

            // 为消息数组分配内存
            if (pmsg)
                free(pmsg);
            // 计算窗口最大行数，窗口最大高度 / 字体高度
            cLinesMax = cyClientMax / cyChar;
            // 分配内存
            pmsg = malloc(cLinesMax * sizeof(MSG));
            cLines = 0;
            // fall through
        case WM_SIZE:
            // size消息中，lparam中，低位是窗口宽度，高位是窗口高度
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            // 计算滚动窗口范围
            rectScroll.left = 0;
            rectScroll.right = cxClient;
            rectScroll.top = cyChar; // 从第二行开始，因为第一行为表头
            rectScroll.bottom = cyChar * (cyClient / cyChar);
            // 重绘，该行不可删除，可能是WM_INPUTLANGCHANGE或WM_DISPLAYCHANGE引起
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_DEADCHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
        case WM_SYSDEADCHAR:
            // 重新安排数组消息
            for (i = cLinesMax - 1; i > 0; i--) {
                pmsg[i] = pmsg[i - 1];
            }
            // 把消息存入消息数组的首元素
            pmsg[0].hwnd = hwnd;
            pmsg[0].message = message;
            pmsg[0].wParam = wParam;
            pmsg[0].lParam = lParam;
            // 每按一个按键，增加一行
            cLines = min(cLines + 1, cLinesMax);
            // 滚屏
            ScrollWindow(hwnd, 0, -cyChar, &rectScroll, &rectScroll);
            //不直接return,因为系统击键消息还要调用DefWindowProc处理
            break;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            // 背景模式 -- 透明模式
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 0, 0, szTop, lstrlen(szTop));
            TextOut(hdc, 0, 0, szUnd, lstrlen(szUnd));
            // 显示消息数组内容
            for (i = 0; i < min(cLines, cyClient / cyChar - 1); i++) {
                iType = pmsg[i].message == WM_CHAR ||
                        pmsg[i].message == WM_SYSCHAR ||
                        pmsg[i].message == WM_DEADCHAR ||
                        pmsg[i].message == WM_SYSDEADCHAR;
                // 获取按键的名称
                GetKeyNameText(pmsg[i].lParam, szKeyName,
                               sizeof(szKeyName) / sizeof(TCHAR));

                TextOut(hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer,
                        wsprintf(szBuffer, szFormat[iType],
                                // 第1个参数，消息的名称
                                 szMessage[pmsg[i].message - WM_KEYFIRST],
                                // 第2个参数，击键时虚拟键代码，字符消息时显示字符的十六进制
                                 pmsg[i].wParam,
                                // 第3个参数根据消息类型显示按键名称
                                 (PTSTR)(iType ? TEXT(" ") : szKeyName),
                                // 第4个参数：字符消息时，显示字符本身
                                 (TCHAR)(iType ? pmsg[i].wParam : ' '),
                                // 第5个参数，重复次数
                                 LOWORD(pmsg[i].lParam),
                                // 第6个参数，扫描码在第16-23位，共8位。
                                 HIWORD(pmsg[i].lParam) & 0xFF,
                                // 第7个参数：扩展标记，在第24位
                                 0x01000000 & pmsg[i].lParam ? szYes : szNo,
                                // 第8个参数：ALT标记，在第29位
                                 0x20000000 & pmsg[i].lParam ? szYes : szNo,
                                // 第9个参数：先前状态，在第30位
                                 0x40000000 & pmsg[i].lParam ? szDown : szUp,
                                // 第10个参数：转换状态，在第31位
                                 0x80000000 & pmsg[i].lParam ? szUp : szDown));
            }
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
