#include <windows.h>
#include "10_1_RESOURCE.H"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    TCHAR szAppName[] = TEXT ("IconDemo");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE (IDI_ICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Icon Demo"),
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
该函数是窗口过程函数，用于处理主窗口的消息。函数中的注释如下：

- `HWND hwnd`: 窗口句柄。
- `UINT message`: 消息标识符，用于指示收到的消息类型。
- `WPARAM wParam`: 附加的消息参数，通常用于传递消息相关的信息。
- `LPARAM lParam`: 附加的消息参数，通常用于传递消息相关的信息。

函数的功能如下：

1. 首先定义一些静态变量，包括窗口图标句柄、图标的宽度和高度以及窗口客户区的宽度和高度。
2. 根据收到的消息类型进行相应的处理。
3. 在 `WM_CREATE` 消息中，获取实例句柄并加载图标资源，获取图标的宽度和高度。
4. 在 `WM_SIZE` 消息中，更新窗口客户区的宽度和高度。
5. 在 `WM_PAINT` 消息中，开始绘制，循环绘制图标，并按行列排列在窗口客户区。使用 `DrawIcon` 函数绘制图标。
6. 在 `WM_DESTROY` 消息中，发送退出消息，结束应用程序。

该函数的作用是处理主窗口的各种消息，包括创建窗口、改变窗口大小、绘制窗口内容等。函数根据不同的消息类型执行相应的操作，更新窗口的状态和显示内容。在 `WM_PAINT` 消息中，通过循环绘制图标，将图标按行列排列在窗口客户区上。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HICON hIcon;               // 窗口图标句柄
    static int cxIcon, cyIcon;        // 图标的宽度和高度
    static int cxClient, cyClient;    // 窗口客户区的宽度和高度
    HDC hdc;                          // 设备上下文句柄
    HINSTANCE hInstance;              // 实例句柄
    PAINTSTRUCT ps;                    // 绘图结构体
    int x, y;                         // 循环变量

    switch (message) {
        case WM_CREATE :
            hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
            hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));    // 加载图标资源
            cxIcon = GetSystemMetrics(SM_CXICON);                       // 获取图标的宽度
            cyIcon = GetSystemMetrics(SM_CYICON);                       // 获取图标的高度
            return 0;

        case WM_SIZE :
            cxClient = LOWORD(lParam);    // 窗口客户区的宽度
            cyClient = HIWORD(lParam);    // 窗口客户区的高度
            return 0;

        case WM_PAINT :
            hdc = BeginPaint(hwnd, &ps);    // 开始绘制

            // 循环绘制图标，按行列排列
            for (y = 0; y < cyClient; y += cyIcon) {
                for (x = 0; x < cxClient; x += cxIcon) {
                    DrawIcon(hdc, x, y, hIcon);    // 绘制图标
                }
            }

            EndPaint(hwnd, &ps);    // 结束绘制
            return 0;

        case WM_DESTROY :
            PostQuitMessage(0);    // 发送退出消息，结束应用程序
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
