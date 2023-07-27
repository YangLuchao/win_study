#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Checker1");
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

    hwnd = CreateWindow (szAppName, TEXT("Checker1 Mouse Hit-Test Demo"),
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
    // 记录方块的状态，用于切换填充和非填充
    static BOOL fState[DIVISIONS][DIVISIONS];
    static int cxBlock, cyBlock; // 方块的宽度和高度
    HDC hdc;
    int x, y;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message) {
        case WM_SIZE:
            // 在窗口大小改变时更新方块的宽度和高度
            cxBlock = LOWORD(lParam) / DIVISIONS;
            cyBlock = HIWORD(lParam) / DIVISIONS;
            return 0;

        case WM_LBUTTONDOWN:// 鼠标左键按下消息
            // 处理鼠标左键按下事件
            x = LOWORD(lParam) / cxBlock;
            y = HIWORD(lParam) / cyBlock;

            if (x < DIVISIONS && y < DIVISIONS) {
                fState[x][y] ^= 1; // 切换方块的填充状态（和1异或，求反）

                // 根据方块位置计算需要重绘的矩形区域
                rect.left = x * cxBlock;
                rect.top = y * cyBlock;
                rect.right = (x + 1) * cxBlock;
                rect.bottom = (y + 1) * cyBlock;
                // 标记矩形区域为无效，触发重绘
                InvalidateRect(hwnd, &rect, FALSE);
            } else
                MessageBeep(0); // 播放错误提示音
            return 0;

        case WM_PAINT:
            // 处理窗口重绘事件
            hdc = BeginPaint(hwnd, &ps);

            for (x = 0; x < DIVISIONS; x++)
                for (y = 0; y < DIVISIONS; y++) {
                    Rectangle(hdc, x * cxBlock, y * cyBlock,
                              (x + 1) * cxBlock, (y + 1) * cyBlock); // 绘制方块

                    if (fState[x][y]) {
                        // 如果方块是填充状态，则绘制斜线
                        MoveToEx(hdc, x * cxBlock, y * cyBlock, NULL);
                        LineTo(hdc, (x + 1) * cxBlock, (y + 1) * cyBlock);
                        MoveToEx(hdc, x * cxBlock, (y + 1) * cyBlock, NULL);
                        LineTo(hdc, (x + 1) * cxBlock, y * cyBlock);
                    }
                }
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            // 处理窗口销毁事件
            PostQuitMessage(0); // 发送退出消息，退出消息循环
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
BOOL Rectangle(
  HDC hdc,   // 绘图设备的句柄（Device Context）
  int left,  // 矩形的左上角 x 坐标
  int top,   // 矩形的左上角 y 坐标
  int right, // 矩形的右下角 x 坐标
  int bottom // 矩形的右下角 y 坐标
);
BOOL MoveToEx(
  HDC hdc,     // 绘图设备的句柄（Device Context）
  int x,       // 起点的 x 坐标
  int y,       // 起点的 y 坐标
  LPPOINT lppt // 用于接收前一个位置的指针
);
BOOL LineTo(
  HDC hdc, // 绘图设备的句柄（Device Context）
  int x,   // 终点的 x 坐标
  int y    // 终点的 y 坐标
);
这两个函数配合使用，可以在设备上下文中绘制连续的线段。
 MoveToEx 用于设置起点，LineTo 用于绘制直线到指定的终点。
 每次调用 LineTo 时，起点都会自动更新为上一次调用 LineTo 的终点。

 */