#include <windows.h>
#include <math.h>

#define NUM    1000
#define TWOPI  (2 * 3.14159)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("SineWave");
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

    hwnd = CreateWindow (szAppName, TEXT("Sine Wave Using Polyline"),
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
    static int cxClient, cyClient;
    HDC hdc;
    int i;
    PAINTSTRUCT ps;
    // 坐标点数组
    POINT apt[NUM];

    switch (message) {
        case WM_SIZE:
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            // 画X轴，带ex,扩展函数
            MoveToEx(hdc, 0, cyClient / 2, NULL);
            // 画直线
            LineTo(hdc, cxClient, cyClient / 2);
            // 计算并填充正弦波曲线坐标数组
            for (i = 0; i < NUM; i++) {
                apt[i].x = i * cxClient / NUM;
                apt[i].y = (int) (cyClient / 2 * (1 - sin(TWOPI * i / NUM)));
            }
            // 绘制正弦曲线
            Polyline(hdc, apt, NUM);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
MoveToEx函数功能更新当前位置到指定点和可选返回原来的位置。
B00L MoveToEx(
    HDC hdc,//设备上下文句柄
    int x,//指定新位置的X坐标（以逻辑单位为单位）
    int y,//指定新位置的Y坐标（以逻辑单位为单位）
    LPPOINT lppt/指向接收先前或当前位置的POINT结构的指针。如果此参数是NULL指针，则不返回前一个
);
LineTo函数绘制从当前位置到一条线，但不包括当前指定点
BOOL LineTo(
    HDC hdc,//设备上下文句柄
    int x,//指定新位置的X坐标（以逻辑单位为单位）
    int y//指定新位置的Y坐标（以逻辑单位为单位）
);
 */