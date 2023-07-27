#include <windows.h>

// 消息回调函数
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
    // 窗口类名
    static TCHAR szAppName[] = TEXT("helloWin");
    // 窗口句柄
    HWND hwnd;
    // 消息结构变量
    MSG msg;
    // 窗口类结构变量
    WNDCLASS wndclass;

    // 窗口类风格-水平和垂直标识符
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    // 窗口处理回调函数
    wndclass.lpfnWndProc = WndProc;
    // 窗口扩展
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    // 实例句柄
    wndclass.hInstance = hinstance;
    // 窗口最小化图标
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    // 窗口鼠标光标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    // 窗口背景色
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    // 窗口菜单
    wndclass.lpszMenuName = NULL;
    // 窗口类名
    wndclass.lpszClassName = szAppName;
    // 注册一个窗口
    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("this program requires window nt"), szAppName,
                   MB_ICONERROR);
        return 0;
    }
    // 创建一个窗口
    hwnd =
            CreateWindow(szAppName,                 // 类名
                         TEXT("the hello program"), // 窗口标题
                         WS_OVERLAPPEDWINDOW,       // 窗口样式
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL,      // 父窗口句柄
                         NULL,      // 窗口菜单句柄
                         hinstance, // 程序实例句柄
                         NULL);
    // 显示窗口
    ShowWindow(hwnd, showCmd);
    // 更新窗口
    UpdateWindow(hwnd);
    // 从消息队列中获取消息，message字段为WM_QUIT返回0
    while (GetMessage(&msg, NULL, 0, 0)) {
        // 将msg结构返还给Windows，并将虚拟键消息转换为字符消息
        TranslateMessage(&msg);
        // 再将msg结构返还给Windows，分发一个消息给窗口程序
        DispatchMessage(&msg);
    }
    // msg.wParam 来自一条标识退出的消息，返回这个值给系统，从而退出
    return msg.wParam;
}

// WPARAM代表控价id或者虚拟键码
// LPARAM代表客户区的尺寸和鼠标位置
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
    // 外部环境句柄
    HDC hdc;
    // 用于绘制该应用程序拥有的窗口的客户区
    PAINTSTRUCT ps;
    // RECT结构通过其左上角和右下角的坐标定义一个矩形
    RECT rect;

    switch (message) {
        case WM_PAINT:
            // 开始画图 - 获取设备环境
            hdc = BeginPaint(hwnd, &ps);
            // 获取窗口客户区的大小
            GetClientRect(hwnd, &rect);
            // 开始画
            DrawText(hdc, TEXT("Hello, Windows 98!"),
                     -1, // -1表示0结尾字符串
                     &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            // 关闭窗口画
            EndPaint(hwnd, &ps);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
