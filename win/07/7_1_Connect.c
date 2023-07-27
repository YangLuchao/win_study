#include <windows.h>

#define MAXPOINTS 1000

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Connect");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    // 注册窗口类
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

    // 创建窗口
    hwnd = CreateWindow (szAppName, TEXT("Connect-the-Points Mouse Demo"),
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInstance, NULL);

    // 显示窗口
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static POINT pt[MAXPOINTS];
    static int iCount;
    HDC hdc;
    int i, j;
    PAINTSTRUCT ps;

    switch (message) {
        /*按下ALT+A系统消息*/
        case WM_NCHITTEST:
            //直接返回位置信息，阻止系统向所有窗口客户区和非窗口客户区发送鼠标消息
            return (LRESULT) HTNOWHERE;
            //按下ALT+A系统消息
        case WM_SYSKEYDOWN:
            //直接返回，使所有系统键盘消息失效
            return 0;
        case WM_LBUTTONDOWN:// 鼠标左键按下消息
            // 鼠标左键按下，重置点的数量，刷新窗口
            iCount = 0;
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_MOUSEMOVE:// 鼠标移动消息
            // 鼠标移动，记录坐标并绘制点
            if (wParam & MK_LBUTTON && iCount < 1000) {
                pt[iCount].x = LOWORD (lParam);
                pt[iCount++].y = HIWORD (lParam);

                hdc = GetDC(hwnd);
                // 画一个像素点
                SetPixel(hdc, LOWORD (lParam), HIWORD (lParam), 0);
                ReleaseDC(hwnd, hdc);
            }
            return 0;

        case WM_LBUTTONUP:// 鼠标左边提起消息
            // 鼠标左键释放，刷新窗口
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;

        case WM_PAINT:
            // 绘制窗口内容
            hdc = BeginPaint(hwnd, &ps);

            // 设置鼠标转圈圈
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            ShowCursor(TRUE);

            // 连接所有点
            for (i = 0; i < iCount - 1; i++)
                for (j = i + 1; j < iCount; j++) {
                    MoveToEx(hdc, pt[i].x, pt[i].y, NULL);
                    LineTo(hdc, pt[j].x, pt[j].y);
                }

            ShowCursor(FALSE);
            // 设置鼠标正常样子
            SetCursor(LoadCursor(NULL, IDC_ARROW));

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            // 窗口销毁，退出程序
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
HWND CreateWindow(
    LPCTSTR lpClassName,    // 窗口类名或类原子(atom)标识符
    LPCTSTR lpWindowName,   // 窗口标题
    DWORD dwStyle,          // 窗口样式
    int x,                  // 窗口左上角的x坐标
    int y,                  // 窗口左上角的y坐标
    int nWidth,             // 窗口的宽度
    int nHeight,            // 窗口的高度
    HWND hWndParent,        // 父窗口句柄或者是所属窗口句柄
    HMENU hMenu,            // 菜单句柄
    HINSTANCE hInstance,    // 实例句柄
    LPVOID lpParam          // 附加参数
);
 typedef struct tagPAINTSTRUCT {
    HDC  hdc;         // 绘制设备上下文句柄
    BOOL fErase;      // 是否需要擦除背景
    RECT rcPaint;     // 需要更新的矩形区域
    BOOL fRestore;    // 是否从保存的绘制状态中还原
    BOOL fIncUpdate;  // 是否增量更新
    BYTE rgbReserved[32]; // 保留的字节，不使用
} PAINTSTRUCT, *PPAINTSTRUCT;
1:在 WM_PAINT 消息的处理过程中，通过 BeginPaint 函数获取 PAINTSTRUCT 结构。
2:使用 PAINTSTRUCT 结构中的 hdc 句柄来进行绘制操作。
3:在绘制完成后，调用 EndPaint 函数释放资源。
 */