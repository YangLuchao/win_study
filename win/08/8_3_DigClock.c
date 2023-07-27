/*-----------------------------------------
   DIGCLOCK.c -- Digital Clock
                 (c) Charles Petzold, 1998
  -----------------------------------------*/

#include <windows.h>

#define ID_TIMER    1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("DigClock");
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

    hwnd = CreateWindow (szAppName, TEXT("Digital Clock"),
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

/*
这段代码实现了一个显示当前时间的窗口程序。窗口的客户区中会以七段数码管的形式显示当前的小时、分钟和秒。以下是代码中各部分的详细说明：

DisplayDigit 函数：根据传入的数字，在指定的设备上下文中绘制对应的七段数码管图形。使用预定义的七段数码管点阵图形和点的坐标数组。

DisplayTwoDigits 函数：在指定的设备上下文中绘制两位数字。根据传入的数字和是否隐藏前导零的标志进行绘制。使用 DisplayDigit 函数绘制每一位数字，并使用 OffsetWindowOrgEx 函数进行坐标偏移。

DisplayColon 函数：在指定的设备上下文中绘制一个冒号（表示小时和分钟之间的分隔符）。使用预定义的坐标数组绘制两个矩形。

DisplayTime 函数：根据当前的系统时间，在指定的设备上下文中绘制当前的时间。根据是否为24小时制和是否隐藏前导零的设置，调用 DisplayTwoDigits 函数分别绘制小时、分钟和秒的部分，并调用 DisplayColon 函数绘制冒号。

在 WndProc 函数中：

WM_CREATE：创建窗口时创建红色画刷，设置定时器以1秒的间隔触发定时器消息。
WM_SETTINGCHANGE：设置更改消息处理部分。获取当前的时间显示格式和是否隐藏前导零的设置，更新变量，并重绘窗口。
WM_SIZE：窗口大小改变消息处理部分。更新窗口客户区的宽度和高度。
WM_PAINT：绘制消息处理部分。获取设备上下文，设置逻辑和视口坐标系，选择画刷和画笔，调用 DisplayTime 函数绘制时间，并进行清理。
WM_DESTROY：窗口销毁消息处理部分。停止定时器，删除画刷对象，发送退出消息，终止消息循环。
这段代码实现了一个简单的数字时钟窗口，根据系统时间在窗口中显示当前的时间。使用七段数码管的图形和预定义的坐标数组来绘制时间的数字部分，并使用矩形表示冒号分隔符。窗口会在定时器触发的消息中更新时间并进行重绘。
 */
void DisplayDigit(HDC hdc, int iNumber) {
    // 表示七段数码管的点阵图形，共10个数字的显示形式
    static BOOL fSevenSegment[10][7] = {
            1, 1, 1, 0, 1, 1, 1,     // 0
            0, 0, 1, 0, 0, 1, 0,     // 1
            1, 0, 1, 1, 1, 0, 1,     // 2
            1, 0, 1, 1, 0, 1, 1,     // 3
            0, 1, 1, 1, 0, 1, 0,     // 4
            1, 1, 0, 1, 0, 1, 1,     // 5
            1, 1, 0, 1, 1, 1, 1,     // 6
            1, 0, 1, 0, 0, 1, 0,     // 7
            1, 1, 1, 1, 1, 1, 1,     // 8
            1, 1, 1, 1, 0, 1, 1      // 9
    };
    // 七段数码管的点坐标，共7段，每段6个点
    static POINT ptSegment[7][6] = {
            7, 6, 11, 2, 31, 2, 35, 6, 31, 10, 11, 10,
            6, 7, 10, 11, 10, 31, 6, 35, 2, 31, 2, 11,
            36, 7, 40, 11, 40, 31, 36, 35, 32, 31, 32, 11,
            7, 36, 11, 32, 31, 32, 35, 36, 31, 40, 11, 40,
            6, 37, 10, 41, 10, 61, 6, 65, 2, 61, 2, 41,
            36, 37, 40, 41, 40, 61, 36, 65, 32, 61, 32, 41,
            7, 66, 11, 62, 31, 62, 35, 66, 31, 70, 11, 70
    };
    int iSeg;

    for (iSeg = 0; iSeg < 7; iSeg++) {
        if (fSevenSegment[iNumber][iSeg]) {
            Polygon(hdc, ptSegment[iSeg], 6);
        }
    }
}

void DisplayTwoDigits(HDC hdc, int iNumber, BOOL fSuppress) {
    if (!fSuppress || (iNumber / 10 != 0)) {
        DisplayDigit(hdc, iNumber / 10);
    }

    OffsetWindowOrgEx(hdc, -42, 0, NULL);
    DisplayDigit(hdc, iNumber % 10);
    OffsetWindowOrgEx(hdc, -42, 0, NULL);
}

void DisplayColon(HDC hdc) {
    POINT ptColon[2][4] = {
            2, 21, 6, 17, 10, 21, 6, 25,
            2, 51, 6, 47, 10, 51, 6, 55
    };

    Polygon(hdc, ptColon[0], 4);
    Polygon(hdc, ptColon[1], 4);

    OffsetWindowOrgEx(hdc, -12, 0, NULL);
}

void DisplayTime(HDC hdc, BOOL f24Hour, BOOL fSuppress) {
    SYSTEMTIME st;

    GetLocalTime(&st); // 获取本地系统时间

    if (f24Hour) {
        DisplayTwoDigits(hdc, st.wHour, fSuppress);
    } else {
        DisplayTwoDigits(hdc, (st.wHour %= 12) ? st.wHour : 12, fSuppress);
    }

    DisplayColon(hdc);
    DisplayTwoDigits(hdc, st.wMinute, FALSE);
    DisplayColon(hdc);
    DisplayTwoDigits(hdc, st.wSecond, FALSE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static BOOL f24Hour, fSuppress;
    static HBRUSH hBrushRed;
    static int cxClient, cyClient;
    HDC hdc;
    PAINTSTRUCT ps;
    TCHAR szBuffer[2];

    switch (message) {
        case WM_CREATE:
            hBrushRed = CreateSolidBrush(RGB (255, 0, 0));
            SetTimer(hwnd, ID_TIMER, 1000, NULL);

            // fall through

        case WM_SETTINGCHANGE:
            // 获取时间显示格式和是否隐藏前导零的设置
            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
            f24Hour = (szBuffer[0] == '1');

            GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
            fSuppress = (szBuffer[0] == '0');

            InvalidateRect(hwnd, NULL, TRUE); // 重绘窗口客户区
            return 0;

        case WM_SIZE:
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            SetMapMode(hdc, MM_ISOTROPIC);
            SetWindowExtEx(hdc, 276, 72, NULL); // 设置逻辑单位
            SetViewportExtEx(hdc, cxClient, cyClient, NULL); // 设置视口单位

            SetWindowOrgEx(hdc, 138, 36, NULL); // 设置逻辑坐标原点
            SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL); // 设置视口坐标原点

            SelectObject(hdc, GetStockObject(NULL_PEN)); // 选择无画笔对象，使边框不可见
            SelectObject(hdc, hBrushRed); // 选择红色画刷

            DisplayTime(hdc, f24Hour, fSuppress); // 绘制时间

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER); // 停止定时器
            DeleteObject(hBrushRed); // 删除画刷对象，释放资源
            PostQuitMessage(0); // 发送退出消息，终止消息循环
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
