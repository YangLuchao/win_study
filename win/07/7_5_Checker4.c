#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

// 焦点，当前选中的矩形（用子窗口ID来标识）
int idFocus = 0;
TCHAR szChildClass[] = TEXT ("Checker4_Child");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Checker4");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

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

    wndclass.lpfnWndProc = ChildWndProc;
    wndclass.cbWndExtra = sizeof(long);
    wndclass.hIcon = NULL;
    wndclass.lpszClassName = szChildClass;

    RegisterClass(&wndclass);

    hwnd = CreateWindow (szAppName, TEXT("Checker4 Mouse Hit-Test Demo"),
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndChild[DIVISIONS][DIVISIONS];
    int cxBlock, cyBlock, x, y;

    switch (message) {
        case WM_CREATE :
            // 在窗口创建时，创建一组子窗口
            for (x = 0; x < DIVISIONS; x++)
                for (y = 0; y < DIVISIONS; y++)
                    hwndChild[x][y] = CreateWindow (szChildClass, NULL,
                                                    WS_CHILDWINDOW | WS_VISIBLE,
                                                    0, 0, 0, 0,
                                                    hwnd, (HMENU) (y << 8 | x),
                                                    (HINSTANCE) GetWindowLong(hwnd, 0),
                                                    NULL);
            return 0;

        case WM_SIZE :
            // 当窗口大小改变时，调整子窗口的位置和大小
            cxBlock = LOWORD (lParam) / DIVISIONS;
            cyBlock = HIWORD (lParam) / DIVISIONS;

            for (x = 0; x < DIVISIONS; x++)
                for (y = 0; y < DIVISIONS; y++)
                    MoveWindow(hwndChild[x][y],
                               x * cxBlock, y * cyBlock,
                               cxBlock, cyBlock, TRUE);
            return 0;

        case WM_LBUTTONDOWN :
            // 当鼠标左键按下时，发出蜂鸣声
            MessageBeep(0);
            return 0;

        case WM_SETFOCUS:
            // 当焦点被设置时，将焦点设置为子窗口
            SetFocus(GetDlgItem(hwnd, idFocus));
            return 0;

        case WM_KEYDOWN:
            // 当键盘按键被按下时，可能改变焦点窗口
            x = idFocus & 0xFF;
            y = idFocus >> 8;

            switch (wParam) {
                case VK_UP:
                    y--; // 上移
                    break;
                case VK_DOWN:
                    y++; // 下移
                    break;
                case VK_LEFT:
                    x--; // 左移
                    break;
                case VK_RIGHT:
                    x++; // 右移
                    break;
                case VK_HOME:
                    x = y = 0; // 移动到起始位置
                    break;
                case VK_END:
                    x = y = DIVISIONS - 1; // 移动到结束位置
                    break;
                default:
                    return 0;
            }

            x = (x + DIVISIONS) % DIVISIONS; // 确保 x 在有效范围内
            y = (y + DIVISIONS) % DIVISIONS; // 确保 y 在有效范围内

            idFocus = y << 8 | x; // 更新焦点窗口的ID

            SetFocus(GetDlgItem(hwnd, idFocus));
            return 0;

        case WM_DESTROY :
            // 当窗口被销毁时，发送退出消息
            PostQuitMessage(0);
            return 0;
    }

    // 对于其他未处理的消息，使用默认的窗口过程进行处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*
在 WM_CREATE 消息中，通过循环创建一组子窗口，并将它们存储在 hwndChild 数组中。子窗口的数量由 DIVISIONS 定义。
在 WM_SIZE 消息中，根据窗口大小的改变，计算每个子窗口的大小，并通过 MoveWindow 函数调整子窗口的位置和大小。
在 WM_LBUTTONDOWN 消息中，当鼠标左键按下时，使用 MessageBeep 函数发出蜂鸣声。
在 WM_SETFOCUS 消息中，当焦点被设置时，将焦点设置为保存在 idFocus 变量中的子窗口。
在 WM_KEYDOWN 消息中，当键盘按键被按下时，根据按下的键盘码（wParam），可能改变焦点窗口的位置。根据不同的按键，更新 x 和 y 值，然后更新 idFocus 以反映新的焦点窗口。最后，将焦点设置为新的子窗口。
在 WM_DESTROY 消息中，当窗口被销毁时，使用 PostQuitMessage 函数发送退出消息，以终止应用程序。
这段代码实现了一个基本的窗口和子窗口布局，并提供了一些交互功能，如调整子窗口大小、移动焦点窗口等。
 */

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message,
                              WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    switch (message) {
        case WM_CREATE :
            // 在窗口创建时，将初始的开关标志（on/off flag）设置为0
            SetWindowLong(hwnd, 0, 0);
            return 0;

        case WM_KEYDOWN:
            // 大多数按键消息发送给父窗口处理
            // 除了按下 Return 和 Space 键，会继续执行下面的代码以切换方块的状态
            if (wParam != VK_RETURN && wParam != VK_SPACE) {
                SendMessage(GetParent(hwnd), message, wParam, lParam);
                return 0;
            }
            // 对于 Return 和 Space 键，继续执行下面的代码以切换方块的状态

        case WM_LBUTTONDOWN :
            // 当鼠标左键按下时，切换方块的状态（开启/关闭）
            SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0)); // 使用异或操作切换开关标志
            SetFocus(hwnd); // 设置焦点为当前窗口
            InvalidateRect(hwnd, NULL, FALSE); // 无效化当前窗口的矩形区域，以便重绘
            return 0;

        case WM_SETFOCUS:
            // 当窗口获得焦点时，将当前窗口的ID保存到全局变量idFocus中
            idFocus = GetWindowLong(hwnd, GWL_ID);

            // 继续执行

        case WM_KILLFOCUS:
            // 当窗口失去焦点时，无效化窗口的矩形区域以便重绘
            InvalidateRect(hwnd, NULL, TRUE);
            return 0;

        case WM_PAINT :
            // 处理绘制窗口的消息
            hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rect);
            Rectangle(hdc, 0, 0, rect.right, rect.bottom);

            // 绘制 "X" 标记

            if (GetWindowLong(hwnd, 0)) {
                MoveToEx(hdc, 0, 0, NULL);
                LineTo(hdc, rect.right, rect.bottom);
                MoveToEx(hdc, 0, rect.bottom, NULL);
                LineTo(hdc, rect.right, 0);
            }

            // 绘制 "焦点" 矩形

            if (hwnd == GetFocus()) {
                rect.left += rect.right / 10;
                rect.right -= rect.left;
                rect.top += rect.bottom / 10;
                rect.bottom -= rect.top;

                SelectObject(hdc, GetStockObject(NULL_BRUSH));
                SelectObject(hdc, CreatePen(PS_DASH, 0, 0));
                Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
            }

            EndPaint(hwnd, &ps);
            return 0;
    }

    // 对于其他未处理的消息，使用默认的窗口过程进行处理
    return DefWindowProc(hwnd, message, wParam, lParam);
}
/*
注释解释：

在 WM_CREATE 消息中，当窗口创建时，将初始的开关标志（on/off flag）设置为0，并将其保存在窗口的附加数据中。
在 WM_KEYDOWN 消息中，如果按下的按键不是 Return 或 Space 键，则将该消息发送给父窗口进行处理。如果按下的是 Return 或 Space 键，则继续执行下面的代码以切换方块的状态。
在 WM_LBUTTONDOWN 消息中，当鼠标左键按下时，切换方块的状态（开启/关闭）。使用异或操作切换窗口的开关标志，设置焦点为当前窗口，并无效化窗口的矩形区域以便重绘。
在 WM_SETFOCUS 消息中，当窗口获得焦点时，将当前窗口的ID保存到全局变量 idFocus 中。
在 WM_KILLFOCUS 消息中，当窗口失去焦点时，无效化窗口的矩形区域以便重绘。
在 WM_PAINT 消息中，处理绘制窗口的消息。使用 BeginPaint 函数开始绘制，并通过 GetClientRect 函数获取窗口的客户区矩形。然后使用 Rectangle 函数绘制一个填充矩形作为背景。
如果窗口的开关标志为开启状态，使用 MoveToEx 和 LineTo 函数绘制一个交叉的 "X" 标记。
如果窗口是焦点窗口，通过调整矩形的大小，在矩形的四角绘制一个虚线矩形，并将其设置为黑色。
最后，通过 EndPaint 函数结束绘制，并返回0表示消息已处理。
这段代码实现了子窗口的绘制和交互功能。子窗口具有一个开关状态，可以通过鼠标点击或特定按键来切换状态，并在绘制时显示相应的图形效果。同时，它还处理了焦点的变化以及窗口的重绘。
 */