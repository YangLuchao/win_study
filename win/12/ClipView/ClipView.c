#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("ClipView"); // 应用程序名称
    HWND hwnd; // 窗口句柄
    MSG msg; // 消息结构体
    WNDCLASS wndclass; // 窗口类结构体

    wndclass.style = CS_HREDRAW | CS_VREDRAW; // 窗口类的样式
    wndclass.lpfnWndProc = WndProc; // 指定窗口过程函数
    wndclass.cbClsExtra = 0; // 类附加内存
    wndclass.cbWndExtra = 0; // 窗口附加内存
    wndclass.hInstance = hInstance; // 实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 加载应用程序图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // 加载箭头光标
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景画刷
    wndclass.lpszMenuName = NULL; // 无菜单
    wndclass.lpszClassName = szAppName; // 类名

    if (!RegisterClass(&wndclass)) { // 注册窗口类
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR); // 显示错误消息
        return 0; // 退出程序
    }

    hwnd = CreateWindow(szAppName, TEXT("Simple Clipboard Viewer (Text Only)"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL); // 创建窗口

    ShowWindow(hwnd, iCmdShow); // 显示窗口
    UpdateWindow(hwnd); // 更新窗口

    while (GetMessage(&msg, NULL, 0, 0)) { // 循环获取消息
        TranslateMessage(&msg); // 翻译消息
        DispatchMessage(&msg); // 分发消息
    }
    return msg.wParam; // 返回退出码
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndNextViewer; // 下一个剪贴板查看器窗口句柄
    HGLOBAL hGlobal; // 全局内存句柄
    HDC hdc; // 设备上下文句柄
    PTSTR pGlobal; // 全局内存指针
    PAINTSTRUCT ps; // 绘图结构体
    RECT rect; // 窗口客户区矩形

    switch (message) {
    case WM_CREATE:
        hwndNextViewer = SetClipboardViewer(hwnd); // 设置为剪贴板查看器
        return 0;

    case WM_CHANGECBCHAIN:
        if ((HWND)wParam == hwndNextViewer)
            hwndNextViewer = (HWND)lParam; // 更新下一个查看器
        else if (hwndNextViewer)
            SendMessage(hwndNextViewer, message, wParam, lParam); // 传递给下一个查看器
        return 0;

    case WM_DRAWCLIPBOARD:
        if (hwndNextViewer)
            SendMessage(hwndNextViewer, message, wParam, lParam); // 传递给下一个查看器

        InvalidateRect(hwnd, NULL, TRUE); // 刷新窗口客户区
        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps); // 开始绘制

        GetClientRect(hwnd, &rect); // 获取客户区矩形

        OpenClipboard(hwnd); // 打开剪贴板

#ifdef UNICODE
        hGlobal = GetClipboardData(CF_UNICODETEXT); // 获取剪贴板中的Unicode文本
#else
        hGlobal = GetClipboardData(CF_TEXT); // 获取剪贴板中的ANSI文本
#endif

        if (hGlobal != NULL) {
            pGlobal = (PTSTR)GlobalLock(hGlobal); // 锁定全局内存
            DrawText(hdc, pGlobal, -1, &rect, DT_EXPANDTABS); // 绘制文本
            GlobalUnlock(hGlobal); // 解锁全局内存
        }

        CloseClipboard(); // 关闭剪贴板
        EndPaint(hwnd, &ps); // 结束绘制
        return 0;

    case WM_DESTROY:
        ChangeClipboardChain(hwnd, hwndNextViewer); // 移除剪贴板查看器
        PostQuitMessage(0); // 发送退出消息
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam); // 默认消息处理
}
