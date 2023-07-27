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
    static int cxClient;// 新客户端宽度
    static int iMaxWidth;//最大高度
    static int iVscrollPos;//滚动条滑块的位置，默认值0~100之间
    HDC hdc;            // 设备环境句柄
    // iVertPos:垂直滚动条位置
    // iVertPos:水平滚动条位置
    // iPaintBeg:开始绘制位置
    // iPaintEnd：结束绘制位置
    int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
    SCROLLINFO si;
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

            //保存三列文本的宽度，水平方向一页足以显示全部文本，因此会隐藏了水平滚动条
            iMaxWidth = 40 * cxChar + 22 * cxCaps;
            return 0;

        case WM_SIZE:
            cxClient = LOWORD(lparam);
            cyClient = HIWORD(lparam);
            // 初始化滚动条参数结构变量
            // si.nPos和si.nTrackPos两个参数没有初始化
            // 因为操作滚动条时才会知道滚动条和滑块的确切位置信息
            si.cbSize = sizeof(si); // SCROLLINFO 结构的字节数
            si.fMask = SIF_RANGE | SIF_PAGE; //指定滚动条的范围和页面大小
            si.nMin = 0;
            // 滚动范围的最小值
            si.nMax = NUMLINES - 1;//滚动范围的最大值
            si.nPage = cyClient / cyChar;//页面大小
            // 设置垂直滚动条范围和页面大小,true是重绘
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            // 再次初始化滚动条参数结构变量
            si.cbSize = sizeof(si);
            // SIF_DISABLENOSCROLL:滚动条不需要时，禁用而不是删除，则可以显式滚动条
            si.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
            //si.fMask=SIF_RANGE|SIF_PAGE;
            si.nMin = 0;
            si.nMax = 2 + iMaxWidth / cxChar;
            //客户区宽度/平均字符宽度=列数，即水平滚动条一页的大小
            si.nPage = cxClient / cxChar;
            // 设置水平滚动条范围和页面大小
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            return 0;
            // 当窗口的标准垂直滚动条中有滚动的事件发生时，操作系统捕捉到后发送 WM_VSCROLL 到消息队列
            // 然后GetMessage消息循环从消息队列中取出消息，并DispatchMessage分发给操作系统调用WndProc
            // 此实例暂时未处理键盘按键操作滚动条的消息
        case WM_VSCROLL:
            //获取所有垂直滚动条信息之前，先设置下面两个参数
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);
            //保存位置以便稍后进行比较
            iVertPos = si.nPos; //指定当前的滚动框位置
            switch (LOWORD(wparam))//wParam低16位是滚动条值
            {
                case SB_TOP://给滚动条增加键盘接口（第6章）
                    si.nPos = si.nMin;
                    break;
                case SB_BOTTOM://给滚动条增加键盘接口（第6章）
                    si.nPos = si.nMax;
                    break;
                case SB_LINEUP:
                    //向上滚动一行
                    si.nPos -= 1;
                    break;
                case SB_LINEDOWN://向下滚动一行
                    si.nPos += 1;
                    break;
                case SB_PAGEUP://向上滚动一页
                    si.nPos -= si.nPage;//客户区高/行距=一页的行数
                    break;
                case SB_PAGEDOWN://向下滚动页
                    si.nPos += si.nPage;
                    break;
                case SB_THUMBPOSITION://拖动滚动条
                    si.nPos = HIWORD(wparam);//wParam高16位则指定滚动框的当前位置
                    break;
                case SB_THUMBTRACK://拖动滚动条到指定位置
                    si.nPos = si.nTrackPos;//当前追踪位置
                    break;
                default:
                    break;
            }
            //设置位置，然后检索它。由于窗口的调整，它可能与设置的值不一样。
            si.fMask = SIF_POS;
            //设置滑块位置（可能会超出范围）
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            //返回正确的位置，实时跟踪
            GetScrollInfo(hwnd, SB_VERT, &si);
            //如果与当前滚动条滑块的位置不相等，重新设置滑块位置并重绘
            if (iVscrollPos != si.nPos) {
                // 设置滚动条位置
                SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
                UpdateWindow(hwnd);//立即更新
            }
            return 0;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            //获取垂直滚动条位置
            si.cbSize = sizeof(si);
            si.fMask = SIF_POS;//指定滚动条的位置
            //返回垂直滑块正确位置
            GetScrollInfo(hwnd, SB_VERT, &si);
            iVertPos = si.nPos;//垂直滚动的位置
            //获取水平滚动条的位置
            GetScrollInfo(hwnd, SB_HORZ, &si);
            iHorzPos = si.nPos;//水平滚动的位置
            //计算绘制窗口客户区的范围iVertPos+行数，例如0~49,1~50
            iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);//当前客户区顶部显示
            iPaintEnd = min(NUMLINES - 1,
                            iVertPos + ps.rcPaint.bottom / cyChar);////当前客户区底部显示的行数+iVe
            for (i = 0; i < NUMLINES; ++i) {
                //y坐标，为负时输出到客户区外
                x = cxChar * (1 - iHorzPos);
                y = cyChar * (i - iVertPos);
                //用当前选择的字体、背景颜色和正文颜色将一个字符串写到指定位置
                //输出系统信息
                TextOut(hdc, x, y,
                        sysmetrics[i].szLabel,
                        lstrlen(sysmetrics[i].szLabel));
                //输出描述信息
                TextOut(hdc, x + 22 * cxCaps, y,
                        sysmetrics[i].szDesc,
                        lstrlen(sysmetrics[i].szDesc));
                //设置文本对齐标志，向右和顶部对齐
                SetTextAlign(hdc, TA_RIGHT + TA_TOP);
                //获取系统信息参数值
                TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
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

SCROLLINFO结构：包含滚动条参数。由SetScrollInfo函数（或SBM_SETSCROLLINFO消息）设置，
或由GetScrollInfo函数（或SBM GETSCROLLINFO消息）检索。
typedef struct tagSCROLLINFO {
    UINT cbsize;    // 指定此结构的大小（以字节为单位）。调用者必须将此设置为sizeof(SCROLLINFO
    UINT fMask;     // 指定要设置或检索的滚动条参数。
    int nMin;       //指定最小滚动位置。
    int nMax;       //指定最大滚动位置
    UINT nPage;     //指定页面大小，以设备为单位。滚动条使用此值来确定比例滚动框的适当大小。
    int nPos;       //指定滚动框的位置。
    int nTrackPos;  //指定用户正在拖动的滚动框的立即位置。
                    //应用程序可以在处理SB_THUMBTRACK请求代码时检索此值。
                    //应用程序无法设置即时滚动位置；SetScrollInfo函数忽略此成员。
} SCROLLINFO, *LPSCROLLINFO;
 */















