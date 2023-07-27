#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szChildClass[] = TEXT ("Checker3_Child");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Checker3");
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

    // 字段被设置成ChildWndProc,表示子窗口类的窗口过程。
    wndclass.lpfnWndProc = ChildWndProc;
    // cbWndExtra字段被设置成4个字节，或更准确地说是一个长整型变量的大小(sizeof(long))。
    // 这个字段通知Windows在内部结构中给基于这个窗口类的每个窗口预留4个字节的额外空间。
    // 用户可以利用这些空间为每个窗口保存不同的信息。
    wndclass.cbWndExtra = sizeof(long);
    // hIcon字段被设置成NULL,因为类似CHECKER3程序中的子窗口不需要图标。
    wndclass.hIcon = NULL;
    // pszClassName字段被设置成“Checker3_Child”，即子窗口类的名称。
    wndclass.lpszClassName = szChildClass;

    RegisterClass(&wndclass);

    hwnd = CreateWindow (szAppName, TEXT("Checker3 Mouse Hit-Test Demo"),
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
    static HWND hwndChild[DIVISIONS][DIVISIONS];
    int cxBlock, cyBlock, x, y;

    switch (message) {
        //获取主窗口进程句柄hInstance的三种方法：
        //1、hInstance设置为全局变量
        //2、（HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);
        //3、WMCREATE 消息的（CREATESTRUCTA) IParam-&gt;hInstance
        case WM_CREATE:
            // 创建子窗口
            for (x = 0; x < DIVISIONS; x++)
                for (y = 0; y < DIVISIONS; y++)
                    hwndChild[x][y] = CreateWindow(szChildClass, NULL,
                                                   WS_CHILDWINDOW | WS_VISIBLE,//没WS_VISIBLE时，需要自己ShowWind、updateWind
                                                   0, 0, 0, 0,
                                                   hwnd, (HMENU) (y << 8 | x),//菜单句柄子ID作为子窗口的唯一标识
                                                   (HINSTANCE) GetWindowLong(hwnd, 0),//从主窗体获得HINSTANCE
                                                   NULL);
            return 0;

        case WM_SIZE:
            // 计算子窗口大小
            cxBlock = LOWORD(lParam) / DIVISIONS;
            cyBlock = HIWORD(lParam) / DIVISIONS;

            // 调整子窗口位置和大小
            for (x = 0; x < DIVISIONS; x++)
                for (y = 0; y < DIVISIONS; y++)
                    MoveWindow(hwndChild[x][y],
                               x * cxBlock, y * cyBlock,
                               cxBlock, cyBlock, TRUE);
            return 0;

        case WM_LBUTTONDOWN:
            // 鼠标左键按下时的处理
            MessageBeep(0);
            return 0;

        case WM_DESTROY:
            // 窗口销毁时的处理
            PostQuitMessage(0);
            return 0;
    }

    // 默认的窗口过程
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message,
                              WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message) {
        case WM_CREATE:
            // 子窗口创建时的处理
            //wndclass.cbWndExtra=sizeof(long);//给子窗口预留4个字节空间保存信息
            //更改指定窗口的属性
            SetWindowLong(hwnd, 0, 0); // 子窗口额外4个字节存储空间中保存一个
            return 0;

        case WM_LBUTTONDOWN:
            // 鼠标左键按下时的处理
            // 切换子窗口的 on/off 状态
            // 鼠标点击后
            SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0));
            // 强制重绘子窗口
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_PAINT:
            // 绘制子窗口时的处理
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rect);
            // 绘制矩形边框
            Rectangle(hdc, 0, 0, rect.right, rect.bottom);

            if (GetWindowLong(hwnd, 0)) { //检索有关指定窗口的信息，返回值0表示失败
                // 如果子窗口的 on/off 状态为 on，则绘制对角线
                MoveToEx(hdc, 0, 0, NULL);
                LineTo(hdc, rect.right, rect.bottom);
                MoveToEx(hdc, 0, rect.bottom, NULL);
                LineTo(hdc, rect.right, 0);
            }

            EndPaint(hwnd, &ps);
            return 0;
    }

    // 默认的窗口过程
    return DefWindowProc(hwnd, message, wParam, lParam);
}
