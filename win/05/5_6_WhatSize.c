/*
027.C--A daily practice
    第27个例子：GDI映射模式
    SetMapMode函数
    SetWindowExtEx函数
    SetViewportExtEx函数
    MM_ANISOTROPIC映射模式
    MM_ISOTROPIC映射模式
    MM_HIENGLISH
    MM_HIMETRIC
    MM_LOENGLISH
    MM_LOMETRIC
    MM_TEXT
    MM TWIPS
 */
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("WhatSize");
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

    hwnd = CreateWindow (szAppName, TEXT("What Size is the Window?"),
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

// 显示窗口客户区的尺寸
void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode,
          TCHAR *szMapMode) {
    TCHAR szBuffer[60];
    RECT rect;
    // 保存当前设备的环境句柄
    SaveDC(hdc);
    // 指定行的映射模式
    SetMapMode(hdc, iMapMode);
    // 获取窗口客户区的大小，使用当前hdc中映射模式的度量单位
    GetClientRect(hwnd, &rect);
    // 设备坐标转化为逻辑坐标
    DPtoLP(hdc, (PPOINT) &rect, 2);
    // 恢复原来的设备环境，-1 恢复最近保存的状态
    RestoreDC(hdc, -1);

    TextOut(hdc, xText, yText, szBuffer,
            wsprintf(szBuffer, TEXT ("%-20s %7d %7d %7d %7d"), szMapMode,
                     rect.left, rect.right, rect.top, rect.bottom));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static TCHAR szHeading[] =
            TEXT ("Mapping Mode            Left   Right     Top  Bottom");
    static TCHAR szUndLine[] =
            TEXT ("------------            ----   -----     ---  ------");
    static int cxChar, cyChar;
    HDC hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
            hdc = GetDC(hwnd);
            // 选择字体
            // 等宽字体
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            // 获取字体参数
            GetTextMetrics(hdc, &tm);
            // 字符平均宽度
            cxChar = tm.tmAveCharWidth;
            // 总高度
            cyChar = tm.tmHeight + tm.tmExternalLeading;

            ReleaseDC(hwnd, hdc);
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            // 选入字体
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            // 指定新的映射模式，逻辑单位被映射到具有任意缩放轴的任意单位
            SetMapMode(hdc, MM_ANISOTROPIC);//拉伸图像以满足需要
            // 指定新的映射模式，逻辑单位被映射到具有同等比例轴的任意单位
            // SetMapMode(hdc,MM_ISOTROPIC);
            // SetWindowExtEx和SetViewportExtEx决定了窗口和视口之间的缩放因子
            // 只有MM_ANISOTROPIC和MM_ISOTROPIC两种映射模式需要设置窗口和视口设备范围
            // 设置窗口设备上下文的水平和垂直范围，该窗口是指页面空间的逻辑坐标系
            SetWindowExtEx(hdc, 1, 1, NULL);//x轴=1个像素，y轴=1个像素
            // 设置视口设备上下文的水平和垂直范围
            SetViewportExtEx(hdc, cxChar, cyChar, NULL);//x轴=1个字符宽，y轴=1个字符
            //该映射模式显示范围不受限制，可以任意拉伸以满足需求
            TextOut(hdc, 1, 1, szHeading, lstrlen(szHeading));
            TextOut(hdc, 1, 2, szUndLine, lstrlen(szUndLine));
            // 在距离左边1个字符、距离顶部3个字符处的客户区显示文本
            // 每个逻辑单元映射到一个设备像素
            Show(hwnd, hdc, 1, 3, MM_TEXT, TEXT ("TEXT (pixels)"));
            // 每个逻辑单位映射到0.1毫米
            Show(hwnd, hdc, 1, 4, MM_LOMETRIC, TEXT ("LOMETRIC (.1 mm)"));
            // 每个逻单位立映谢到0.01毫米
            Show(hwnd, hdc, 1, 5, MM_HIMETRIC, TEXT ("HIMETRIC (.01 mm)"));
            // 每个逻辑单元都映射到0.1英寸
            Show(hwnd, hdc, 1, 6, MM_LOENGLISH, TEXT ("LOENGLISH (.01 in)"));
            // 每个逻辑单元都映射到0.01英寸
            Show(hwnd, hdc, 1, 7, MM_HIENGLISH, TEXT ("HIENGLISH (.001 in)"));
            // 逻辑堤，每个逻辑单位都映射到打印机点的二十分之一
            Show(hwnd, hdc, 1, 8, MM_TWIPS, TEXT ("TWIPS (1/1440 in)"));

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
