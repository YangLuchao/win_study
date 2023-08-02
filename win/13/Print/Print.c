#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL PrintMyPage(HWND);

extern HINSTANCE hInst;
extern TCHAR szAppName[];
extern TCHAR szCaption[];

// 主函数入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
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
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hInst = hInstance;

    // 创建窗口并显示
    hwnd = CreateWindow(szAppName, szCaption, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // 消息循环
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// 打印函数，打印页面内容
void PageGDICalls (HDC hdcPrn, int cxPage, int cyPage)
{
     static TCHAR szTextStr[] = TEXT ("Hello, Printer!") ;
     // 使用GDI函数在设备上绘制页面内容
     Rectangle (hdcPrn, 0, 0, cxPage, cyPage) ;
     
     MoveToEx (hdcPrn, 0, 0, NULL) ;
     LineTo   (hdcPrn, cxPage, cyPage) ;
     MoveToEx (hdcPrn, cxPage, 0, NULL) ;
     LineTo   (hdcPrn, 0, cyPage) ;
     
     SaveDC (hdcPrn) ;
     
     SetMapMode       (hdcPrn, MM_ISOTROPIC) ;
     SetWindowExtEx   (hdcPrn, 1000, 1000, NULL) ;
     SetViewportExtEx (hdcPrn, cxPage / 2, -cyPage / 2, NULL) ;
     SetViewportOrgEx (hdcPrn, cxPage / 2,  cyPage / 2, NULL) ;
     
     Ellipse (hdcPrn, -500, 500, 500, -500) ;
     
     SetTextAlign (hdcPrn, TA_BASELINE | TA_CENTER) ;
     TextOut (hdcPrn, 0, 0, szTextStr, lstrlen (szTextStr)) ;

     RestoreDC (hdcPrn, -1) ;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient, cyClient;
    HDC hdc;
    HMENU hMenu;
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_CREATE:
        // 在系统菜单中添加“Print”选项
        hMenu = GetSystemMenu(hwnd, FALSE);
        AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenu(hMenu, 0, 1, TEXT("&Print"));
        return 0;

    case WM_SIZE:
        // 获取客户区大小
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        return 0;

    case WM_SYSCOMMAND:
        // 响应系统菜单的打印选项
        if (wParam == 1)
        {
            if (!PrintMyPage(hwnd))
                MessageBox(hwnd, TEXT("Could not print page!"), szAppName, MB_OK | MB_ICONEXCLAMATION);
            return 0;
        }
        break;

    case WM_PAINT:
        // 绘制窗口客户区内容
        hdc = BeginPaint(hwnd, &ps);
        PageGDICalls(hdc, cxClient, cyClient);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        // 销毁窗口
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}