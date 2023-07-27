#include <windows.h> //包含Windows API头文件

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); //声明主窗口过程

BOOL CALLBACK ColorScrDlg(HWND, UINT, WPARAM, LPARAM); //声明模式化色彩滚动条对话框过程

HWND hDlgModeless; //模式化对话框句柄

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, //应用程序入口
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Colors2"); //应用程序名称
    HWND hwnd; //主窗口句柄
    MSG msg; //消息
    WNDCLASS wndclass; //窗口类

    //设置窗口类属性
    wndclass.style = CS_HREDRAW | CS_VREDRAW; //重绘
    wndclass.lpfnWndProc = WndProc; //主窗口过程
    wndclass.cbClsExtra = 0; //无额外类内存
    wndclass.cbWndExtra = 0; //无额外窗口内存
    wndclass.hInstance = hInstance; //实例句柄
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //默认图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); //默认箭头光标
    wndclass.hbrBackground = CreateSolidBrush(0L); //黑色背景刷子
    wndclass.lpszMenuName = NULL;  //无菜单
    wndclass.lpszClassName = szAppName; //类名

    //注册窗口类
    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR); //显示错误
        return 0;
    }

    //创建主窗口
    hwnd = CreateWindow(szAppName, TEXT("Color Scroll"),
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, //窗口样式
                        CW_USEDEFAULT, CW_USEDEFAULT, //位置
                        CW_USEDEFAULT, CW_USEDEFAULT, //大小
                        NULL, NULL, hInstance, NULL);

    //显示并更新主窗口
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    //创建模式化色彩滚动条对话框
    hDlgModeless = CreateDialog(hInstance, TEXT("ColorScrDlg"),
                                hwnd, ColorScrDlg);

    //消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        //检查对话框消息
        if (hDlgModeless == 0 || !IsDialogMessage(hDlgModeless, &msg)) {
            TranslateMessage(&msg); //翻译按键
            DispatchMessage(&msg); //分发消息
        }
    }
    return msg.wParam; //返回退出码
}

//主窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_DESTROY: //窗口销毁
            //设置白色背景刷子
            DeleteObject((HGDIOBJ) SetClassLong(hwnd, -10,
                                                (LONG) GetStockObject(WHITE_BRUSH)));
            PostQuitMessage(0); //发出退出消息
            return 0;
    }
    //其他消息使用默认处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}

//模式化色彩滚动条对话框过程
BOOL CALLBACK ColorScrDlg(HWND hDlg, UINT message,
                          WPARAM wParam, LPARAM lParam) {
    static int iColor[3]; //RGB颜色值
    HWND hwndParent, hCtrl; //父窗口、控件句柄
    int iCtrlID, iIndex; //控件ID、颜色索引

    switch (message) {
        case WM_INITDIALOG: //初始化对话框
            //初始化滚动条
            for (iCtrlID = 10; iCtrlID < 13; iCtrlID++) {
                hCtrl = GetDlgItem(hDlg, iCtrlID); //获取控件句柄
                SetScrollRange(hCtrl, SB_CTL, 0, 255, FALSE); //设置滚动范围
                SetScrollPos(hCtrl, SB_CTL, 0, FALSE); //设置滚动位置
            }
            return TRUE;

        case WM_VSCROLL: //处理滚动消息
            hCtrl = (HWND) lParam; //滚动条控件句柄
            iCtrlID = GetWindowLong(hCtrl, GWL_ID); //获取控件ID
            iIndex = iCtrlID - 10; //计算颜色索引
            hwndParent = GetParent(hDlg); //父窗口句柄

            //根据滚动消息更新颜色值
            switch (LOWORD(wParam)) {
                case SB_PAGEDOWN:
                    iColor[iIndex] += 15;
                case SB_LINEDOWN:
                    iColor[iIndex] = min(255, iColor[iIndex] + 1);
                    break;
                case SB_PAGEUP:
                    iColor[iIndex] -= 15;
                case SB_LINEUP:
                    iColor[iIndex] = max(0, iColor[iIndex] - 1);
                    break;
                case SB_TOP:
                    iColor[iIndex] = 0;
                    break;
                case SB_BOTTOM:
                    iColor[iIndex] = 255;
                    break;
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    iColor[iIndex] = HIWORD(wParam);
                    break;
                default:
                    return FALSE;
            }

            SetScrollPos(hCtrl, SB_CTL, iColor[iIndex], TRUE); //设置滚动位置
            SetDlgItemInt(hDlg, iCtrlID + 3, iColor[iIndex], FALSE); //设置数值显示

            //设置父窗口背景刷子
            DeleteObject((HGDIOBJ) SetClassLong(hwndParent, -10,
                                                (LONG) CreateSolidBrush(RGB(iColor[0], iColor[1], iColor[2]))));

            InvalidateRect(hwndParent, NULL, TRUE); //重绘父窗口
            return TRUE;
    }

    return FALSE; //其他消息使用默认处理
}