#include <windows.h>

#define ID_SMALLER      1
#define ID_LARGER       2

#define BTN_WIDTH        (8 * cxChar)
#define BTN_HEIGHT       (4 * cyChar)

/*
上面的代码实现了一个具有自绘制按钮的窗口程序。窗口程序中包含两个按钮，一个用于缩小窗口，一个用于放大窗口。以下是代码中的重点部分和难点的说明：

BTN_WIDTH 和 BTN_HEIGHT 定义了按钮的宽度和高度，以字符大小为单位。
Triangle 函数用于绘制三角形，接受一个 HDC 和三个点的数组作为参数，使用黑色刷子绘制三角形。
在 WM_CREATE 消息中，使用 CreateWindow 函数创建两个自绘制按钮，并存储其句柄。
在 WM_SIZE 消息中，根据窗口的新尺寸移动按钮的位置，使其位于窗口的中心。
在 WM_COMMAND 消息中，根据按钮的标识符改变窗口的大小。点击 "Smaller" 按钮会使窗口减小，点击 "Larger" 按钮会使窗口增大。
在 WM_DRAWITEM 消息中，处理自绘制按钮的绘制。首先填充按钮区域为白色背景，然后使用黑色框线框起来。
 根据按钮的标识符绘制相应的内部和外部三角形。如果按钮被选中，则反转按钮的区域。如果按钮具有焦点，则绘制焦点矩形。
在 WM_DESTROY 消息中，调用 PostQuitMessage 函数退出程序。
通过这段代码，实现了一个带有自绘制按钮的窗口程序，可以通过点击按钮来改变窗口的大小。
 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("OwnDraw");
    MSG msg;
    HWND hwnd;
    WNDCLASS wndclass;

    hInst = hInstance;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Owner-Draw Button Demo"),
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
Triangle 函数用于绘制三角形图形。它接受一个设备上下文句柄 HDC 和一个包含三个点的数组 POINT pt[] 作为参数。
 该函数会在给定的设备上下文中绘制一个黑色的填充三角形，使用 SelectObject 函数选择黑色刷子，然后使用 Polygon 函数绘制三角形。

Triangle 函数的目的是为了在自绘制按钮的绘制过程中绘制三角形。在这个特定的窗口程序中，根据按钮的标识符不同，
 Triangle 函数会在按钮的区域内绘制不同的三角形。在 WM_DRAWITEM 消息中，
 根据按钮标识符的不同调用 Triangle 函数来绘制相应的内部和外部三角形，从而实现自定义按钮的外观效果。
 */
void Triangle(HDC hdc, POINT pt[]) {
    SelectObject(hdc, GetStockObject(BLACK_BRUSH));
    Polygon(hdc, pt, 3);
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndSmaller, hwndLarger;
    static int cxClient, cyClient, cxChar, cyChar;
    int cx, cy;
    LPDRAWITEMSTRUCT pdis;
    POINT pt[3];
    RECT rc;

    switch (message) {
        case WM_CREATE :
            cxChar = LOWORD (GetDialogBaseUnits());
            cyChar = HIWORD (GetDialogBaseUnits());

            // Create the owner-draw pushbuttons

            hwndSmaller = CreateWindow (TEXT("button"), TEXT(""),
                                        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                        0, 0, BTN_WIDTH, BTN_HEIGHT,
                                        hwnd, (HMENU) ID_SMALLER, hInst, NULL);

            hwndLarger = CreateWindow (TEXT("button"), TEXT(""),
                                       WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                                       0, 0, BTN_WIDTH, BTN_HEIGHT,
                                       hwnd, (HMENU) ID_LARGER, hInst, NULL);
            return 0;

        case WM_SIZE :
            cxClient = LOWORD (lParam);
            cyClient = HIWORD (lParam);

            // Move the buttons to the new center

            MoveWindow(hwndSmaller, cxClient / 2 - 3 * BTN_WIDTH / 2,
                       cyClient / 2 - BTN_HEIGHT / 2,
                       BTN_WIDTH, BTN_HEIGHT, TRUE);

            MoveWindow(hwndLarger, cxClient / 2 + BTN_WIDTH / 2,
                       cyClient / 2 - BTN_HEIGHT / 2,
                       BTN_WIDTH, BTN_HEIGHT, TRUE);
            return 0;

        case WM_COMMAND :
            GetWindowRect(hwnd, &rc);

            // Make the window 10% smaller or larger

            switch (wParam) {
                case ID_SMALLER :
                    rc.left += cxClient / 20;
                    rc.right -= cxClient / 20;
                    rc.top += cyClient / 20;
                    rc.bottom -= cyClient / 20;
                    break;

                case ID_LARGER :
                    rc.left -= cxClient / 20;
                    rc.right += cxClient / 20;
                    rc.top -= cyClient / 20;
                    rc.bottom += cyClient / 20;
                    break;
            }

            MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left,
                       rc.bottom - rc.top, TRUE);
            return 0;

        case WM_DRAWITEM :
            pdis = (LPDRAWITEMSTRUCT) lParam;

            // Fill area with white and frame it black

            FillRect(pdis->hDC, &pdis->rcItem,
                     (HBRUSH) GetStockObject(WHITE_BRUSH));

            FrameRect(pdis->hDC, &pdis->rcItem,
                      (HBRUSH) GetStockObject(BLACK_BRUSH));

            // Draw inward and outward black triangles

            cx = pdis->rcItem.right - pdis->rcItem.left;
            cy = pdis->rcItem.bottom - pdis->rcItem.top;

            switch (pdis->CtlID) {
                case ID_SMALLER :
                    pt[0].x = 3 * cx / 8;
                    pt[0].y = 1 * cy / 8;
                    pt[1].x = 5 * cx / 8;
                    pt[1].y = 1 * cy / 8;
                    pt[2].x = 4 * cx / 8;
                    pt[2].y = 3 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 7 * cx / 8;
                    pt[0].y = 3 * cy / 8;
                    pt[1].x = 7 * cx / 8;
                    pt[1].y = 5 * cy / 8;
                    pt[2].x = 5 * cx / 8;
                    pt[2].y = 4 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 5 * cx / 8;
                    pt[0].y = 7 * cy / 8;
                    pt[1].x = 3 * cx / 8;
                    pt[1].y = 7 * cy / 8;
                    pt[2].x = 4 * cx / 8;
                    pt[2].y = 5 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 1 * cx / 8;
                    pt[0].y = 5 * cy / 8;
                    pt[1].x = 1 * cx / 8;
                    pt[1].y = 3 * cy / 8;
                    pt[2].x = 3 * cx / 8;
                    pt[2].y = 4 * cy / 8;

                    Triangle(pdis->hDC, pt);
                    break;

                case ID_LARGER :
                    pt[0].x = 5 * cx / 8;
                    pt[0].y = 3 * cy / 8;
                    pt[1].x = 3 * cx / 8;
                    pt[1].y = 3 * cy / 8;
                    pt[2].x = 4 * cx / 8;
                    pt[2].y = 1 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 5 * cx / 8;
                    pt[0].y = 5 * cy / 8;
                    pt[1].x = 5 * cx / 8;
                    pt[1].y = 3 * cy / 8;
                    pt[2].x = 7 * cx / 8;
                    pt[2].y = 4 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 3 * cx / 8;
                    pt[0].y = 5 * cy / 8;
                    pt[1].x = 5 * cx / 8;
                    pt[1].y = 5 * cy / 8;
                    pt[2].x = 4 * cx / 8;
                    pt[2].y = 7 * cy / 8;

                    Triangle(pdis->hDC, pt);

                    pt[0].x = 3 * cx / 8;
                    pt[0].y = 3 * cy / 8;
                    pt[1].x = 3 * cx / 8;
                    pt[1].y = 5 * cy / 8;
                    pt[2].x = 1 * cx / 8;
                    pt[2].y = 4 * cy / 8;

                    Triangle(pdis->hDC, pt);
                    break;
            }

            // Invert the rectangle if the button is selected

            if (pdis->itemState & ODS_SELECTED)
                InvertRect(pdis->hDC, &pdis->rcItem);

            // Draw a focus rectangle if the button has the focus

            if (pdis->itemState & ODS_FOCUS) {
                pdis->rcItem.left += cx / 16;
                pdis->rcItem.top += cy / 16;
                pdis->rcItem.right -= cx / 16;
                pdis->rcItem.bottom -= cy / 16;

                DrawFocusRect(pdis->hDC, &pdis->rcItem);
            }
            return 0;

        case WM_DESTROY :
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}