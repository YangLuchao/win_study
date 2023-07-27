#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Bezier");
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

    hwnd = CreateWindow (szAppName, TEXT("Bezier Splines"),
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

void DrawBezier(HDC hdc, POINT apt[]) {
    // 画贝塞尔曲线
    MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
    POINT szApt[] = {apt[1], apt[2], apt[3]};
    PolyBezierTo(hdc, szApt, 3);

    // 画两条控制线
    MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
    LineTo(hdc, apt[1].x, apt[1].y);

    MoveToEx(hdc, apt[2].x, apt[2].y, NULL);
    LineTo(hdc, apt[3].x, apt[3].y);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static POINT apt[4];
    HDC hdc;
    int cxClient, cyClient;
    PAINTSTRUCT ps;

    switch (message) {
        case WM_SIZE:// 调整大小的请求
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);

            // 填充坐标数组
            apt[0].x = cxClient / 4;
            apt[0].y = cyClient / 2;

            apt[1].x = cxClient / 2;
            apt[1].y = cyClient / 4;

            apt[2].x = cxClient / 2;
            apt[2].y = 3 * cyClient / 4;

            apt[3].x = 3 * cxClient / 4;
            apt[3].y = cyClient / 2;

            return 0;

        case WM_LBUTTONDOWN:// 点击鼠标左键
        case WM_RBUTTONDOWN:// 点击鼠标右键
        case WM_MOUSEMOVE:// 拖动鼠标
            if (wParam & MK_LBUTTON || wParam & MK_RBUTTON) {
                hdc = GetDC(hwnd);
                // 用白色画刷重绘一遍，即擦除旧的曲线
                SelectObject(hdc, GetStockObject(WHITE_PEN));
                DrawBezier(hdc, apt);

                if (wParam & MK_LBUTTON) {
                    apt[1].x = LOWORD (lParam);
                    apt[1].y = HIWORD (lParam);
                }

                if (wParam & MK_RBUTTON) {
                    apt[2].x = LOWORD (lParam);
                    apt[2].y = HIWORD (lParam);
                }

                // 用黑色的画笔绘制曲线
                SelectObject(hdc, GetStockObject(BLACK_PEN));
                DrawBezier(hdc, apt);
                ReleaseDC(hwnd, hdc);
            }
            return 0;

            // 其他消息中的绘图api都由wm_paint消息来处理
        case WM_PAINT:
            // 使整个窗口客户区无效，并重绘背景，如果窗口客户区显示其他内容，先清除
            InvalidateRect(hwnd, NULL, TRUE);

            hdc = BeginPaint(hwnd, &ps);

            DrawBezier(hdc, apt);// 绘制贝塞尔曲线

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
