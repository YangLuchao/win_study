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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
    HDC hdc;
    int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
    PAINTSTRUCT ps;
    SCROLLINFO si;
    TCHAR szBuffer[10];
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
            hdc = GetDC(hwnd);
            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
            cyChar = tm.tmHeight + tm.tmExternalLeading;

            ReleaseDC(hwnd, hdc);
            // Save the width of the three columns
            iMaxWidth = 40 * cxChar + 22 * cxCaps;
            return 0;
        case WM_SIZE:
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);
            // Set vertical scroll bar range and page size
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = NUMLINES - 1;
            si.nPage = cyClient / cyChar;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            // Set horizontal scroll bar range and page size
            si.cbSize = sizeof(si);
            si.fMask = SIF_RANGE | SIF_PAGE;
            si.nMin = 0;
            si.nMax = 2 + iMaxWidth / cxChar;
            si.nPage = cxClient / cxChar;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            return 0;

        case WM_VSCROLL:
            // Get all the vertical scroll bar information
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);
            // Save the position for comparison later on
            iVertPos = si.nPos;
            switch (LOWORD (wParam)) {
                case SB_TOP:
                    si.nPos = si.nMin;
                    break;

                case SB_BOTTOM:
                    si.nPos = si.nMax;
                    break;

                case SB_LINEUP:
                    si.nPos -= 1;
                    break;

                case SB_LINEDOWN:
                    si.nPos += 1;
                    break;

                case SB_PAGEUP:
                    si.nPos -= si.nPage;
                    break;
                case SB_PAGEDOWN:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBTRACK:
                    si.nPos = si.nTrackPos;
                    break;

                default:
                    break;
            }
            // Set the position and then retrieve it. Due to adjustments
            // by Windows it may not be the same as the value set.
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            GetScrollInfo(hwnd, SB_VERT, &si);
            // If the position has changed, scroll the window and update it
            if (si.nPos != iVertPos) {
                ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos),
                             NULL, NULL);
                UpdateWindow(hwnd);
            }
            return 0;
        case WM_HSCROLL:
            // Get all the vertical scroll bar information
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            // Save the position for comparison later on
            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;
            switch (LOWORD (wParam)) {
                case SB_LINELEFT:
                    si.nPos -= 1;
                    break;

                case SB_LINERIGHT:
                    si.nPos += 1;
                    break;

                case SB_PAGELEFT:
                    si.nPos -= si.nPage;
                    break;

                case SB_PAGERIGHT:
                    si.nPos += si.nPage;
                    break;

                case SB_THUMBPOSITION:
                    si.nPos = si.nTrackPos;
                    break;

                default :
                    break;
            }
            // Set the position and then retrieve it. Due to adjustments
            // by Windows it may not be the same as the value set.
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            GetScrollInfo(hwnd, SB_HORZ, &si);

            // If the position has changed, scroll the window
            if (si.nPos != iHorzPos) {
                ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0,
                             NULL, NULL);
            }
            return 0;
        case WM_PAINT :
            hdc = BeginPaint(hwnd, &ps);
            // Get vertical scroll bar position
            si.cbSize = sizeof(si);
            si.fMask = SIF_POS;
            GetScrollInfo(hwnd, SB_VERT, &si);
            iVertPos = si.nPos;
            // Get horizontal scroll bar position
            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;
            // Find painting limits
            iPaintBeg = max (0, iVertPos + ps.rcPaint.top / cyChar);
            iPaintEnd = min (NUMLINES - 1,
                             iVertPos + ps.rcPaint.bottom / cyChar);

            for (i = iPaintBeg; i <= iPaintEnd; i++) {
                x = cxChar * (1 - iHorzPos);
                y = cyChar * (i - iVertPos);

                TextOut(hdc, x, y,
                        sysmetrics[i].szLabel,
                        lstrlen(sysmetrics[i].szLabel));

                TextOut(hdc, x + 22 * cxCaps, y,
                        sysmetrics[i].szDesc,
                        lstrlen(sysmetrics[i].szDesc));

                SetTextAlign(hdc, TA_RIGHT | TA_TOP);
                TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
                        wsprintf(szBuffer, TEXT ("%5d"),
                                 GetSystemMetrics(sysmetrics[i].iIndex)));
                SetTextAlign(hdc, TA_LEFT | TA_TOP);
            }
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY :
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
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















