#include <windows.h>
#include <math.h>

#define ID_TIMER    1
#define TWOPI       (2 * 3.14159)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Clock");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = NULL;
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("Program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Analog Clock"),
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
这段代码实现了一个简单的时钟窗口程序，用于在窗口中绘制时钟表盘和时、分、秒针的指示。以下是代码中各部分的详细说明：

SetIsotropic 函数：设置设备上下文的逻辑坐标和视口坐标的映射关系，实现等比例缩放的效果。根据窗口的客户区大小，设置逻辑坐标和视口坐标的窗口范围，并将视口原点设置为窗口客户区的中心点。

RotatePoint 函数：对给定的一组点进行旋转变换。根据给定的角度，通过旋转变换公式计算每个点的新坐标。

DrawClock 函数：在指定的设备上下文中绘制时钟表盘。通过循环绘制一系列圆形，表示时钟的刻度。

DrawHands 函数：在指定的设备上下文中绘制时、分、秒针的指示。根据传入的时间，计算时、分、秒针的旋转角度，并根据是否发生变化决定是否绘制。通过调用 RotatePoint 函数将初始的指针坐标旋转到正确的位置，并使用 Polyline 函数绘制指针的线段。

在 WndProc 函数中：

WM_CREATE：创建窗口时设置定时器，每隔1秒触发一次定时器消息。获取当前的系统时间，并将其赋值给 stPrevious 变量。
WM_SIZE：窗口大小改变消息处理部分。更新窗口客户区的宽度和高度。
WM_TIMER：定时器消息处理部分。获取当前的系统时间，检查时、分是否发生变化，获取设备上下文，设置逻辑和视口坐标系，选择画笔，调用 DrawHands 函数绘制时、分针的指示，释放设备上下文，更新 stPrevious 变量。
WM_PAINT：绘制消息处理部分。获取设备上下文，设置逻辑和视口坐标系，调用 DrawClock 函数绘制时钟表盘，调用 DrawHands 函数绘制时、分针的指示，进行清理。
WM_DESTROY：窗口销毁消息处理部分。停止定时器，发送退出消息，终止消息循环。
这段代码实现了一个简单的时钟窗口程序，根据系统时间在窗口中绘制时钟表盘和指针指示。通过旋转变换和绘图函数来实现指针的位置和角度的更新。定时器用于每秒更新一次时间并进行重绘。
 */

void SetIsotropic(HDC hdc, int cxClient, int cyClient) {
    // 设置逻辑坐标和视口坐标的映射模式为等比例缩放
    SetMapMode(hdc, MM_ISOTROPIC);
    // 设置逻辑坐标的窗口范围为 1000x1000
    SetWindowExtEx(hdc, 1000, 1000, NULL);
    // 设置视口坐标的窗口范围为窗口客户区的一半大小，y轴向下为正方向
    SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, NULL);
    // 设置视口坐标的原点为窗口客户区的中心点
    SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL);
}

void RotatePoint(POINT pt[], int iNum, int iAngle) {
    int i;
    POINT ptTemp;

    for (i = 0; i < iNum; i++) {
        // 使用旋转变换公式计算点的新坐标
        ptTemp.x = (int) (pt[i].x * cos(TWOPI * iAngle / 360) +
                          pt[i].y * sin(TWOPI * iAngle / 360));
        ptTemp.y = (int) (pt[i].y * cos(TWOPI * iAngle / 360) -
                          pt[i].x * sin(TWOPI * iAngle / 360));

        pt[i] = ptTemp;
    }
}

/*
此函数通过循环绘制钟表上的刻度。具体实现细节如下：

使用for循环，每次增加6度，绘制所有刻度。iAngle表示当前刻度的角度。
初始化pt[0]的坐标为(0, 900)，表示刻度的起始点在钟表的12点位置。
调用RotatePoint函数，将刻度的起始点按当前角度旋转，得到实际的位置。
根据当前角度决定刻度的长度，如果角度不是5的倍数，则刻度长度为33，否则为100。
根据刻度起始点和长度计算刻度的结束点的坐标。
选择黑色画刷，使用Ellipse函数绘制一个椭圆，表示钟表上的刻度。
 */
void DrawClock(HDC hdc) {
    int iAngle;
    POINT pt[3];

    for (iAngle = 0; iAngle < 360; iAngle += 6) {
        pt[0].x = 0;
        pt[0].y = 900;

        RotatePoint(pt, 1, iAngle);

        pt[2].x = pt[2].y = iAngle % 5 ? 33 : 100;

        pt[0].x -= pt[2].x / 2;
        pt[0].y -= pt[2].y / 2;

        pt[1].x = pt[0].x + pt[2].x;
        pt[1].y = pt[0].y + pt[2].y;

        SelectObject(hdc, GetStockObject(BLACK_BRUSH));

        Ellipse(hdc, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
    }
}

/*
声明并初始化一个二维数组pt，其中包含了时针、分针和秒针的起始点和终点坐标。
定义数组iAngle，分别存储时针、分针和秒针的角度。
将pt数组的内容复制到临时数组ptTemp中，以便根据角度进行旋转时不影响原始数据。
根据系统时间计算时针、分针和秒针的角度。
时针角度的计算：每小时30度，再加上分钟数除以2的角度修正。
分针角度的计算：每分钟6度。
秒针角度的计算：每秒钟6度。
使用memcpy函数将pt数组的内容复制到ptTemp数组中，以便根据角度旋转指针坐标。
对于需要更新的指针（fChange为TRUE）或全部指针（fChange为FALSE），循环绘制指针：
调用RotatePoint函数，将指针坐标按指定角度旋转。
使用Polyline函数绘制指针，连接旋转后的坐标点，形成指针形状。
 */
void DrawHands(HDC hdc, SYSTEMTIME *pst, BOOL fChange) {
    static POINT pt[3][5] = {
            {0, -150, 100, 0, 0, 600, -100, 0, 0, -150},   // 时针
            {0, -200, 50,  0, 0, 800, -50,  0, 0, -200},   // 分针
            {0, 0,    0,   0, 0, 0,   0,    0, 0, 800}     // 秒针
    };
    int i, iAngle[3];
    POINT ptTemp[3][5];

    iAngle[0] = (pst->wHour * 30) % 360 + pst->wMinute / 2; // 计算时针角度
    iAngle[1] = pst->wMinute * 6;                         // 计算分针角度
    iAngle[2] = pst->wSecond * 6;                         // 计算秒针角度

    memcpy(ptTemp, pt, sizeof(pt));

    for (i = fChange ? 0 : 2; i < 3; i++) {
        RotatePoint(ptTemp[i], 5, iAngle[i]); // 根据角度旋转指针坐标

        Polyline(hdc, ptTemp[i], 5); // 绘制指针
    }
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static int cxClient, cyClient;
    static SYSTEMTIME stPrevious;
    BOOL fChange;
    HDC hdc;
    PAINTSTRUCT ps;
    SYSTEMTIME st;

    switch (message) {
        case WM_CREATE:
            SetTimer(hwnd, ID_TIMER, 1000, NULL);
            GetLocalTime(&st);
            stPrevious = st;
            return 0;

        case WM_SIZE:
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            return 0;

        case WM_TIMER:
            GetLocalTime(&st);

            fChange = st.wHour != stPrevious.wHour ||
                      st.wMinute != stPrevious.wMinute;

            hdc = GetDC(hwnd);

            SetIsotropic(hdc, cxClient, cyClient);

            SelectObject(hdc, GetStockObject(WHITE_PEN));
            DrawHands(hdc, &stPrevious, fChange);

            SelectObject(hdc, GetStockObject(BLACK_PEN));
            DrawHands(hdc, &st, TRUE);

            ReleaseDC(hwnd, hdc);

            stPrevious = st;
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            SetIsotropic(hdc, cxClient, cyClient);
            DrawClock(hdc);
            DrawHands(hdc, &stPrevious, TRUE);

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
