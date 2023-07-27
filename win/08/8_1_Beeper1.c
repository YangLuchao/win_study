/*-----------------------------------------
   BEEPER1.C  -- Timer Demo Program No. 1
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Beeper1");
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

    hwnd = CreateWindow (szAppName, TEXT("Beeper1 Timer Demo"),
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
    static BOOL fFlipFlop = FALSE; // 用于切换颜色的标志
    HBRUSH hBrush;                 // 画刷句柄
    HDC hdc;                       // 设备上下文句柄
    PAINTSTRUCT ps;                // 绘图结构
    RECT rc;                       // 窗口客户区矩形

    switch (message) {
        case WM_CREATE:
            // 在窗口创建时设置定时器，每隔1秒触发一次
            SetTimer(hwnd, ID_TIMER, 1000, NULL);
            return 0;

        case WM_TIMER:
            // 定时器消息处理
            MessageBeep(-1);        // 播放蜂鸣声
            fFlipFlop = !fFlipFlop; // 切换颜色标志
            InvalidateRect(hwnd, NULL, FALSE); // 重绘窗口客户区
            return 0;

        case WM_PAINT:
            // 窗口重绘消息处理
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rc); // 获取窗口客户区矩形
            hBrush = CreateSolidBrush(fFlipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255)); // 创建画刷，根据颜色标志选择红色或蓝色
            FillRect(hdc, &rc, hBrush); // 使用画刷填充窗口客户区矩形

            EndPaint(hwnd, &ps);
            DeleteObject(hBrush); // 删除画刷对象，释放资源
            return 0;

        case WM_DESTROY:
            // 窗口销毁消息处理
            KillTimer(hwnd, ID_TIMER); // 停止定时器
            PostQuitMessage(0); // 发送退出消息，终止消息循环
            return 0;
    }

    // 默认消息处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
这段代码实现了一个窗口程序，其中包含一个定时器和窗口重绘功能。窗口的客户区背景颜色会每隔1秒钟交替变化。以下是代码中各部分的详细说明：

fFlipFlop: 一个布尔值，用于切换颜色的标志。初始值为FALSE。
hBrush: 画刷句柄，用于绘制背景色。
hdc: 设备上下文句柄，用于绘制操作。
ps: PAINTSTRUCT结构，用于窗口绘制。
rc: RECT结构，表示窗口的客户区矩形。
在不同的消息处理部分：

WM_CREATE：在窗口创建时设置定时器，以1秒的间隔触发定时器消息。
WM_TIMER：定时器消息处理部分。调用MessageBeep(-1)播放蜂鸣声，并切换fFlipFlop标志的值。然后调用InvalidateRect函数，将整个客户区标记为无效，触发窗口重绘。
WM_PAINT：窗口重绘消息处理部分。获取窗口的客户区矩形，根据fFlipFlop标志创建红色或蓝色的画刷，使用画刷填充整个客户区矩形。最后释放画刷资源。
WM_DESTROY：窗口销毁消息处理部分。停止定时器，发送退出消息，终止消息循环。
这段代码实现了在窗口中定时切换背景颜色的功能，使用了定时器和窗口重绘的相关操作。
 */