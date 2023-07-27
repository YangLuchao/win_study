#include <windows.h>

#define NUMLINES ((int) (sizeof devcaps / sizeof devcaps [0]))

// 结构体数组，存储设备能力的相关信息
struct {
    int iIndex;        // 设备能力的索引
    TCHAR *szLabel;    // 设备能力的标签
    TCHAR *szDesc;     // 设备能力的描述
}
        devcaps[] =
        {
                HORZSIZE, TEXT ("HORZSIZE"), TEXT ("Width in millimeters:"),
                VERTSIZE, TEXT ("VERTSIZE"), TEXT ("Height in millimeters:"),
                HORZRES, TEXT ("HORZRES"), TEXT ("Width in pixels:"),
                VERTRES, TEXT ("VERTRES"), TEXT ("Height in raster lines:"),
                BITSPIXEL, TEXT ("BITSPIXEL"), TEXT ("Color bits per pixel:"),
                PLANES, TEXT ("PLANES"), TEXT ("Number of color planes:"),
                NUMBRUSHES, TEXT ("NUMBRUSHES"), TEXT ("Number of device brushes:"),
                NUMPENS, TEXT ("NUMPENS"), TEXT ("Number of device pens:"),
                NUMMARKERS, TEXT ("NUMMARKERS"), TEXT ("Number of device markers:"),
                NUMFONTS, TEXT ("NUMFONTS"), TEXT ("Number of device fonts:"),
                NUMCOLORS, TEXT ("NUMCOLORS"), TEXT ("Number of device colors:"),
                PDEVICESIZE, TEXT ("PDEVICESIZE"), TEXT ("Size of device structure:"),
                ASPECTX, TEXT ("ASPECTX"), TEXT ("Relative width of pixel:"),
                ASPECTY, TEXT ("ASPECTY"), TEXT ("Relative height of pixel:"),
                ASPECTXY, TEXT ("ASPECTXY"), TEXT ("Relative diagonal of pixel:"),
                LOGPIXELSX, TEXT ("LOGPIXELSX"), TEXT ("Horizontal dots per inch:"),
                LOGPIXELSY, TEXT ("LOGPIXELSY"), TEXT ("Vertical dots per inch:"),
                SIZEPALETTE, TEXT ("SIZEPALETTE"), TEXT ("Number of palette entries:"),
                NUMRESERVED, TEXT ("NUMRESERVED"), TEXT ("Reserved palette entries:"),
                COLORRES, TEXT ("COLORRES"), TEXT ("Actual color resolution:")
        };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("DevCaps1");
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
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建窗口
    hwnd = CreateWindow (szAppName, TEXT("Device Capabilities"),
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         NULL, NULL, hInstance, NULL);

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
    // cxCaps表示标签中大写字母的宽度（以像素为单位）
    static int cxChar, cxCaps, cyChar;
    TCHAR szBuffer[10];
    HDC hdc;
    int i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message) {
        case WM_CREATE:
            // 获取设备上下文句柄，计算字符和标签的宽度
            hdc = GetDC(hwnd);

            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
            cyChar = tm.tmHeight + tm.tmExternalLeading;

            ReleaseDC(hwnd, hdc);
            return 0;

        case WM_PAINT:
            // 绘制窗口
            hdc = BeginPaint(hwnd, &ps);

            for (i = 0; i < NUMLINES; i++) {
                // 绘制标签和描述
                TextOut(hdc, 0, cyChar * i,
                        devcaps[i].szLabel,
                        lstrlen(devcaps[i].szLabel));

                TextOut(hdc, 14 * cxCaps, cyChar * i,
                        devcaps[i].szDesc,
                        lstrlen(devcaps[i].szDesc));

                SetTextAlign(hdc, TA_RIGHT | TA_TOP);

                // 获取设备能力的值并绘制
                TextOut(hdc, 14 * cxCaps + 35 * cxChar, cyChar * i, szBuffer,
                        wsprintf(szBuffer, TEXT ("%5d"),
                                 GetDeviceCaps(hdc, devcaps[i].iIndex)));

                SetTextAlign(hdc, TA_LEFT | TA_TOP);
            }

            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
 typedef struct tagTEXTMETRIC {
  LONG  tmHeight;            // 字体的字符高度
  LONG  tmAscent;            // 字体的基线以上的高度
  LONG  tmDescent;           // 字体的基线以下的高度
  LONG  tmInternalLeading;   // 字符之间的垂直间距，不包括外部间距
  LONG  tmExternalLeading;   // 字符之间的外部垂直间距
  LONG  tmAveCharWidth;      // 字符的平均宽度
  LONG  tmMaxCharWidth;      // 字符的最大宽度
  LONG  tmWeight;            // 字体的粗细程度
  LONG  tmOverhang;          // 字符的悬挂部分（正数表示悬挂，负数表示削减）
  LONG  tmDigitizedAspectX;  // 字符的水平方向分辨率
  LONG  tmDigitizedAspectY;  // 字符的垂直方向分辨率
  TCHAR tmFirstChar;         // 字符集中的第一个字符
  TCHAR tmLastChar;          // 字符集中的最后一个字符
  TCHAR tmDefaultChar;       // 用作替代字符的默认字符
  TCHAR tmBreakChar;         // 字符断开换行的字符
  BYTE  tmItalic;            // 字体是否为斜体（非零表示斜体）
  BYTE  tmUnderlined;        // 字体是否有下划线（非零表示有下划线）
  BYTE  tmStruckOut;         // 字体是否有删除线（非零表示有删除线）
  BYTE  tmPitchAndFamily;    // 字符集的字体类型和族类
  BYTE  tmCharSet;           // 字符集
} TEXTMETRIC, *PTEXTMETRIC, *NPTEXTMETRIC, *LPTEXTMETRIC;

 */