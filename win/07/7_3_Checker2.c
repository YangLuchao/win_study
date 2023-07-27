#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Checker2");
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
        MessageBox(NULL, TEXT ("Program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Checker2 Mouse Hit-Test Demo"),
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
    static BOOL fState[DIVISIONS][DIVISIONS];  // 保存方块状态的二维数组
    static int cxBlock, cyBlock;  // 方块的宽度和高度
    HDC hdc;  // 设备上下文句柄
    int x, y;  // 方块的坐标
    PAINTSTRUCT ps;  // 绘图结构
    POINT point;  // 鼠标位置
    RECT rect;  // 方块矩形

    switch (message) {
        case WM_SIZE:
            // 根据窗口大小计算方块的宽度和高度
            cxBlock = LOWORD(lParam) / DIVISIONS;
            cyBlock = HIWORD(lParam) / DIVISIONS;
            return 0;

        case WM_SETFOCUS: // 获得焦点消息
            // 当窗口获得焦点时显示光标
            ShowCursor(TRUE);
            return 0;

        case WM_KILLFOCUS:// 失去焦点消息
            // 当窗口失去焦点时隐藏光标
            ShowCursor(FALSE);
            return 0;

        case WM_KEYDOWN:
            // 键盘按下事件处理
            GetCursorPos(&point);  // 获取鼠标位置
            ScreenToClient(hwnd, &point);  // 将鼠标位置转换为窗口内坐标

            x = max(0, min(DIVISIONS - 1, point.x / cxBlock));  // 计算方块的 x 坐标
            y = max(0, min(DIVISIONS - 1, point.y / cyBlock));  // 计算方块的 y 坐标

            switch (wParam) {
                case VK_UP:
                    y--;  // 上移
                    break;

                case VK_DOWN:
                    y++;  // 下移
                    break;

                case VK_LEFT:
                    x--;  // 左移
                    break;

                case VK_RIGHT:
                    x++;  // 右移
                    break;

                case VK_HOME:
                    x = y = 0;  // 移动到左上角
                    break;

                case VK_END:
                    x = y = DIVISIONS - 1;  // 移动到右下角
                    break;

                case VK_RETURN:
                case VK_SPACE:
                    // 模拟鼠标左键按下事件
                    SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x * cxBlock, y * cyBlock));
                    break;
            }

            // 处理方块坐标的循环
            x = (x + DIVISIONS) % DIVISIONS;
            y = (y + DIVISIONS) % DIVISIONS;

            // 根据方块坐标计算鼠标位置
            point.x = x * cxBlock + cxBlock / 2;
            point.y = y * cyBlock + cyBlock / 2;

            ClientToScreen(hwnd, &point);  // 将鼠标位置转换为屏幕坐标
            SetCursorPos(point.x, point.y);  // 设置鼠标位置将屏幕坐标转换为窗口坐标
            return 0;

        case WM_LBUTTONDOWN:
            // 鼠标左键按下事件处理
            x = LOWORD(lParam) / cxBlock;  // 计算方块的 x 坐标
            y = HIWORD(lParam) / cyBlock;  // 计算方块的 y 坐标

            if (x < DIVISIONS && y < DIVISIONS) {
                fState[x][y] ^= 1;  // 切换方块的状态

                // 更新方块所在的矩形区域
                rect.left = x * cxBlock;
                rect.top = y * cyBlock;
                rect.right = (x + 1) * cxBlock;
                rect.bottom = (y + 1) * cyBlock;

                InvalidateRect(hwnd, &rect, FALSE);  // 使方块区域无效，触发重绘
            } else {
                MessageBeep(0);  // 播放错误提示音
            }
            return 0;

        case WM_PAINT:
            // 窗口重绘事件处理
            hdc = BeginPaint(hwnd, &ps);  // 开始绘图，获取绘图设备上下文和绘图结构

            for (x = 0; x < DIVISIONS; x++) {
                for (y = 0; y < DIVISIONS; y++) {
                    // 绘制方块的矩形
                    Rectangle(hdc, x * cxBlock, y * cyBlock,
                              (x + 1) * cxBlock, (y + 1) * cyBlock);

                    if (fState[x][y]) {
                        // 绘制方块内的交叉线
                        MoveToEx(hdc, x * cxBlock, y * cyBlock, NULL);
                        LineTo(hdc, (x + 1) * cxBlock, (y + 1) * cyBlock);
                        MoveToEx(hdc, x * cxBlock, (y + 1) * cyBlock, NULL);
                        LineTo(hdc, (x + 1) * cxBlock, y * cyBlock);
                    }
                }
            }

            EndPaint(hwnd, &ps);  // 结束绘图，释放绘图设备上下文
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);  // 发送退出消息，终止消息循环
            return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);  // 默认消息处理
}

