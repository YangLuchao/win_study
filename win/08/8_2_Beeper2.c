/*----------------------------------------
   BEEPER2.C -- Timer Demo Program No. 2
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

VOID CALLBACK TimerProc(HWND, UINT, UINT, DWORD);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Beeper2");
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

    hwnd = CreateWindow (szAppName, TEXT("Beeper2 Timer Demo"),
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
    switch (message) {
        case WM_CREATE:
            // 在窗口创建时设置定时器，每隔1秒触发一次，调用TimerProc函数
            SetTimer(hwnd, ID_TIMER, 1000, TimerProc);
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

// 定时器回调函数
VOID CALLBACK TimerProc(HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime) {
    static BOOL fFlipFlop = FALSE; // 用于切换颜色的标志
    HBRUSH hBrush;                 // 画刷句柄
    HDC hdc;                       // 设备上下文句柄
    RECT rc;                       // 窗口客户区矩形

    MessageBeep(-1);        // 播放蜂鸣声
    fFlipFlop = !fFlipFlop; // 切换颜色标志

    GetClientRect(hwnd, &rc); // 获取窗口客户区矩形

    hdc = GetDC(hwnd);
    hBrush = CreateSolidBrush(fFlipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255)); // 创建画刷，根据颜色标志选择红色或蓝色

    FillRect(hdc, &rc, hBrush); // 使用画刷填充窗口客户区矩形

    ReleaseDC(hwnd, hdc);
    DeleteObject(hBrush); // 删除画刷对象，释放资源
}

/*
这段代码实现了一个窗口程序，其中包含一个定时器和窗口重绘功能。窗口的客户区背景颜色会每隔1秒钟交替变化。以下是代码中各部分的详细说明：

在 WndProc 函数中：

WM_CREATE：在窗口创建时设置定时器，以1秒的间隔触发定时器消息，并调用 TimerProc 函数处理定时器消息。
WM_DESTROY：窗口销毁消息处理部分。停止定时器，发送退出消息，终止消息循环。
在 TimerProc 函数中：

fFlipFlop：一个布尔值，用于切换颜色的标志。初始值为 FALSE。

hBrush：画刷句柄，用于绘制背景色。

hdc：设备上下文句柄，用于绘制操作。

rc：窗口的客户区矩形。

MessageBeep(-1)：播放蜂鸣声。

fFlipFlop = !fFlipFlop：切换颜色标志的值。

GetClientRect(hwnd, &rc)：获取窗口的客户区矩形。

hdc = GetDC(hwnd)：获取窗口的设备上下文句柄。

hBrush = CreateSolidBrush(fFlipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255))：根据颜色标志创建红色或蓝色的画刷。

FillRect(hdc, &rc, hBrush)：使用画刷填充整个客户区矩形。

ReleaseDC(hwnd, hdc)：释放设备上下文句柄。

DeleteObject(hBrush)：删除画刷对象，释放资源。

这段代码实现了在窗口中定时切换背景颜色的功能，使用了定时器和窗口重绘的相关操作。
 */