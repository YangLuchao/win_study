#include "04_SYSETS.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("System1");

    HWND hwnd;
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
    ShowWindow(hwnd, iCmdShow);
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    static int cxChar;  // 字符平均宽度
    static int cxCaps;  // 字符间距
    static int cyChar;  // 字符行距
    static int cyClient;// 新客户端高度
    static int iVscrollPos;//滚动条滑块的位置，默认值0~100之间
    HDC hdc;            // 设备环境句柄
    int i, y;
    PAINTSTRUCT ps;     // 绘图结构
    TCHAR szBuffer[10];
    TEXTMETRIC tm; // 物理字体基本信息

    switch (message) {
        case WM_CREATE:
            hdc = GetDC(hwnd); // 获取环境设备句柄
            GetTextMetrics(hdc, &tm);// 从当前设备中获取程序当前的默认字体信息
            cxChar = tm.tmAveCharWidth; //
            //tm.tmPitchAndFamily字体间距，tmPitchAndFamily字段的低位决定字体是否为等宽字体.1代表等宽的字体，0代表变宽的字体
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2; // 字体间距
            cyChar = tm.tmHeight + tm.tmExternalLeading; //字符行距=字符高+字符顶部空间的数目
            ReleaseDC(hwnd, hdc); // 释放设备句柄

            //设置所指定滚动条范围的最小值和最大值。
            //SB_VERT垂直滚动调，FALSE不立即重绘滚动条，标准滚动条的默认范围是0到100
            SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
            //设置所指定滚动条中的滚动按钮的位置。
            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);//TRUE重绘
            return 0;

        case WM_SIZE:
            cyClient = HIWORD(lparam);//客户区的新高度
            return 0;
            // 当窗口的标准垂直滚动条中有滚动的事件发生时，操作系统捕捉到后发送 WM_VSCROLL 到消息队列
            // 然后GetMessage消息循环从消息队列中取出消息，并DispatchMessage分发给操作系统调用WndProc
            // 此实例暂时未处理键盘按键操作滚动条的消息
        case WM_VSCROLL:
            switch (LOWORD(wparam))//wParam低16位是滚动条值
            {
                case SB_LINEUP:
                    //向上滚动一行
                    iVscrollPos -= 1;
                    break;
                case SB_LINEDOWN://向下滚动一行
                    iVscrollPos += 1;
                    break;
                case SB_PAGEUP://向上滚动一页
                    iVscrollPos -= cyClient / cyChar;//客户区高/行距=一页的行数
                    break;
                case SB_PAGEDOWN://向下滚动页
                    iVscrollPos += cyClient / cyChar;
                    break;
                case SB_THUMBPOSITION://拖动滚动条
                    iVscrollPos = HIWORD(wparam);//wParam高16位则指定滚动框的当前位置
                    break;
                default:
                    break;
            }
            //更新滑块位置
            iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));
            //如果与当前滚动条滑块的位置不相等，重新设置滑块位置并重绘
            if (iVscrollPos != GetScrollPos(hwnd, SB_VERT)) {
                // 设置滚动条位置
                SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
                // 并擦除背景
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);//立即更新
            }
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            for (i = 0; i < NUMLINES; ++i) {
                //y坐标，为负时输出到客户区外
                y = cyChar * (i - iVscrollPos);
                //用当前选择的字体、背景颜色和正文颜色将一个字符串写到指定位置
                //输出系统信息
                TextOut(hdc, 0, y,
                        sysmetrics[i].szLabel,
                        lstrlen(sysmetrics[i].szLabel));
                //输出描述信息
                TextOut(hdc, 22 * cxCaps, y,
                        sysmetrics[i].szDesc,
                        lstrlen(sysmetrics[i].szDesc));
                //设置文本对齐标志，向右和顶部对齐
                SetTextAlign(hdc, TA_RIGHT + TA_TOP);
                //获取系统信息参数值
                TextOut(hdc, 22 * cxCaps + 40 * cxChar, y, szBuffer,
                        wsprintf(szBuffer, TEXT("%5d"),
                                 GetSystemMetrics(sysmetrics[i].iIndex)));//根据索引检索其对应的系
                //恢复默认对齐方式---向左和顶部对齐
                SetTextAlign(hdc, TA_LEFT | TA_TOP);
            }
            EndPaint(hwnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, message, wparam, lparam);
}
/*
 *
PAINTSTRUCT结构：包含应用程序的信息。此信息可用于绘制该应用程序拥有的窗口的客户区。
typedef struct tagPAINTSTRUCT {
    HDC hdc;        //用于绘画的显示器DC的句柄。
    BOOL fErase;    //指示是否必须擦除背景。
    RECT rcPaint;   //一个RECT结构，以相对于客户区域左上角的设备单位指定请求绘制的矩形的左上角和
    B00L fRestore;  //保留；由系统内部使用。
    B00L fIncUpdate;///保留；由系统内部使用。
    BYTE rgbReserved[32];//保留；由系统内部使用。
} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

TEXTMETRIC结构：包含物理字体的基本信息。所有大小均以逻辑单位指定；也就是说，它们取决于显示上下文的当
typedef struct tagTEXTMETRIC { // tm
    LONG tmHeight;      //字符高度
    LONG tmAscent;      //字符上部高度（基线以上）
    LONG tmDescent;     //字符下部高度（基线以下）
    LONG tmInternalLeading, //由tmHeight定义的字符高度的顶部空间数目
    LONG tmExternalLeading, //夹在两行之间的空间数目
    LONG tmAveCharWidth,    //平均字符宽度
    LONG tmMaxCharWidth,    //最宽字符的宽度
    LONG tmWeight;          //字体的粗细轻重程度
    LONG tmoverhang,        //加入某些拼接字体上的附加高度
    LONG tmDigitizedAspectX,//字体设计所针对的设备水平方向
    LONG tmDigitizedAspectY,//字体设计所针对的设备垂直方向
    BCHAR tmFirstChar;      //为字体定义的第一个字符
    BCHAR tmLastChar;       //为字体定义的最后一个字符
    BCHAR tmDefaultChar;    //字体中所没有字符的替代字符
    BCHAR tmBreakChar;      //用于拆字的字符
    BYTE tmItalic,          //字体为斜体时非零
    BYTE tmUnderlined,      //字体为下划线时非零
    BYTE tmStruck0ut,       //字体被删去时非零
    BYTE tmPitchAndFamily,  //字体间距（低4位）和族（高4位）
    BYTE tmCharSet;         //字体的字符集
} TEXTMETRICA, *PTEXTMETRICA, *NPTEXTMETRICA,  *LPTEXTMETRICA;
 */















