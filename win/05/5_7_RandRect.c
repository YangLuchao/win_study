#include <windows.h>
#include <stdlib.h>           // for the rand function

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawRectangle(HWND);

int cxClient, cyClient;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("RandRect");
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

    hwnd = CreateWindow (szAppName, TEXT("Random Rectangles"),
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (TRUE) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            // 消息队列不为空
            // 如果是WM_QUIT消息，退出循环
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else // 消息队列为空
            DrawRectangle(hwnd); // 绘制随机矩形
    }
    return msg.wParam;
}

/*
/*
PeekMessage函数：调度传入的已发送消息，检查线程消息队列中是否有已发布消息，并检索消息（如果存在）。
BOOL PeekMessageA(
    LPMSG LpMsg,    //指向接收消息信息的MSG结构的指针
    HWND hWnd,      //要获取其消息的窗口的句柄。该窗口必须属于当前线程
    UINT wMsgFilterMin, //在要检查的消息范围内的第一条消息的值
    UINT WMsgFilterMax, //在要检查的消息范围内的最后一条消息的值
    UINT WRemoveMsg     //指定如何处理消息。此参数可以是以下一个或多个值
                        //PM_REMOVE允许一个程序检查程序队列中的下一个消息，而不是真实地获得并
);
SetRect函数：设置指定矩形的坐标。这等效于将left,top,right和bottom参数分配给RECT结构的适当成员。
 *
 */

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
        case WM_SIZE:
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

// 绘制随机矩形
void DrawRectangle(HWND hwnd) {
    HBRUSH hBrush;
    HDC hdc;
    RECT rect;

    if (cxClient == 0 || cyClient == 0)
        return;

    SetRect(&rect, rand() % cxClient, rand() % cyClient,
            rand() % cxClient, rand() % cyClient);
    // 生成画刷，取随机值
    hBrush = CreateSolidBrush(
            RGB (rand() % 256, rand() % 256, rand() % 256));

    hdc = GetDC(hwnd);
    // 填充矩形
    FillRect(hdc, &rect, hBrush);
    ReleaseDC(hwnd, hdc);
    // 删除画刷
    DeleteObject(hBrush);
}
