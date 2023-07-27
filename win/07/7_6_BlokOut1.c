#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("BlokOut1");
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

    hwnd = CreateWindow (szAppName, TEXT("Mouse Button Demo"),
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

void DrawBoxOutline(HWND hwnd, POINT ptBeg, POINT ptEnd) {
    HDC hdc;

    hdc = GetDC(hwnd); // 获取窗口的设备上下文

    SetROP2(hdc, R2_NOT); // 设置绘图模式为反色模式
    SelectObject(hdc, GetStockObject(NULL_BRUSH)); // 选择空画刷
    Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y); // 绘制矩形框

    ReleaseDC(hwnd, hdc); // 释放设备上下文
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static BOOL fBlocking, fValidBox;
    static POINT ptBeg, ptEnd, ptBoxBeg, ptBoxEnd;
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message) {
        case WM_LBUTTONDOWN :
            // 鼠标左键按下事件
            ptBeg.x = ptEnd.x = LOWORD (lParam);
            ptBeg.y = ptEnd.y = HIWORD (lParam);

            DrawBoxOutline(hwnd, ptBeg, ptEnd); // 绘制初始矩形框

            SetCursor(LoadCursor(NULL, IDC_CROSS)); // 设置鼠标光标为十字形

            fBlocking = TRUE; // 设置拦截标志为真
            return 0;

        case WM_MOUSEMOVE :
            // 鼠标移动事件
            if (fBlocking) {
                SetCursor(LoadCursor(NULL, IDC_CROSS)); // 设置鼠标光标为十字形

                DrawBoxOutline(hwnd, ptBeg, ptEnd); // 擦除前一次绘制的矩形框

                ptEnd.x = LOWORD (lParam);
                ptEnd.y = HIWORD (lParam);

                DrawBoxOutline(hwnd, ptBeg, ptEnd); // 绘制新的矩形框
            }
            return 0;

        case WM_LBUTTONUP :
            // 鼠标左键松开事件
            if (fBlocking) {
                DrawBoxOutline(hwnd, ptBeg, ptEnd); // 擦除最后一次绘制的矩形框

                ptBoxBeg = ptBeg;
                ptBoxEnd.x = LOWORD (lParam);
                ptBoxEnd.y = HIWORD (lParam);

                SetCursor(LoadCursor(NULL, IDC_ARROW)); // 设置鼠标光标为箭头形状

                fBlocking = FALSE; // 取消拦截标志
                fValidBox = TRUE; // 设置有效矩形标志为真

                InvalidateRect(hwnd, NULL, TRUE); // 无效化窗口的矩形区域，以便重绘
            }
            return 0;

        case WM_CHAR :
            // 键盘字符输入事件
            if (fBlocking & (wParam == '\x1B')) // 如果正在绘制矩形框且按下的是Escape键
            {
                DrawBoxOutline(hwnd, ptBeg, ptEnd); // 擦除当前绘制的矩形框

                SetCursor(LoadCursor(NULL, IDC_ARROW)); // 设置鼠标光标为箭头形状

                fBlocking = FALSE; // 取消拦截标志
            }
            return 0;

        case WM_PAINT :
            // 绘制窗口事件
            hdc = BeginPaint(hwnd, &ps);

            if (fValidBox) {
                SelectObject(hdc, GetStockObject(BLACK_BRUSH));
                Rectangle(hdc, ptBoxBeg.x, ptBoxBeg.y,
                          ptBoxEnd.x, ptBoxEnd.y);
            }

            if (fBlocking) {
                SetROP2(hdc, R2_NOT);
                SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, ptBeg.x, ptBeg.y, ptEnd.x, ptEnd.y);
            }

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY :
            PostQuitMessage(0);
            return 0;
    }

    // 对于其他未处理的消息，使用默认的窗口过程进行处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
注释解释：

DrawBoxOutline 函数：

DrawBoxOutline 函数用于绘制指定矩形的轮廓线。
通过调用 GetDC 函数获取指定窗口的设备上下文。
使用 SetROP2 函数将绘图模式设置为反色模式。
通过调用 SelectObject 函数选择空画刷。
使用 Rectangle 函数绘制指定矩形的轮廓线。
最后，通过调用 ReleaseDC 函数释放设备上下文。
WndProc 函数：

WndProc 函数是窗口过程，用于处理窗口的消息。
在静态变量中定义了一些用于绘制矩形框的状态和坐标。
在不同的消息处理分支中，根据不同的消息类型执行相应的操作。
在 WM_LBUTTONDOWN 消息中，当鼠标左键按下时，记录鼠标位置并绘制初始矩形框。还设置鼠标光标为十字形，并将拦截标志设置为真。
在 WM_MOUSEMOVE 消息中，当鼠标移动时，如果拦截标志为真，则擦除前一次绘制的矩形框，并绘制新的矩形框。
在 WM_LBUTTONUP 消息中，当鼠标左键松开时，擦除最后一次绘制的矩形框，并记录最终的矩形框的坐标。将鼠标光标设置为箭头形状，取消拦截标志，将有效矩形标志设置为真，并无效化窗口的矩形区域以便重绘。
在 WM_CHAR 消息中，当键盘输入字符时，如果拦截标志为真且按下的是 Escape 键，将在判断条件中进行 Escape 键的检测。如果是，则擦除当前绘制的矩形框，将鼠标光标设置为箭头形状，并取消拦截标志。
在 WM_PAINT 消息中，当窗口需要重绘时，通过调用 BeginPaint 函数开始绘制，并获取设备上下文。如果有效矩形标志为真，则使用黑色画刷绘制最终的矩形框。如果拦截标志为真，则将绘图模式设置为反色模式，并使用空画刷绘制当前正在绘制的矩形框。最后，通过调用 EndPaint 函数结束绘制。
在 WM_DESTROY 消息中，当窗口被销毁时，通过调用 PostQuitMessage 函数发送退出消息来终止应用程序。
这段代码实现了一个可绘制矩形框的窗口，并在鼠标操作和键盘输入时实时更新绘制效果。它通过记录鼠标的位置和状态来跟踪矩形框的绘制过程，并在需要时使用特定的绘图函数来更新和擦除矩形框。
 */