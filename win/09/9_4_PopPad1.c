#include <windows.h>

#define ID_EDIT     1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szAppName[] = TEXT ("PopPad1");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
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
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, szAppName,
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
hwnd: 窗口句柄，表示当前消息所属的窗口
message: 消息类型，表示当前收到的消息
wParam: 附加的消息参数，根据不同的消息类型可能有不同的含义
lParam: 附加的消息参数，根据不同的消息类型可能有不同的含义
hwndEdit: 静态变量，存储编辑框的句柄，用于后续对编辑框的操作
WM_CREATE: 窗口创建消息，用于创建子窗口和初始化窗口数据
WM_SETFOCUS: 焦点设置消息，用于将焦点设置到指定的窗口上
WM_SIZE: 窗口大小改变消息，用于调整子窗口的大小以适应窗口的大小
WM_COMMAND: 命令消息，用于处理来自子窗口的命令消息
WM_DESTROY: 窗口销毁消息，用于销毁窗口并发送退出消息给消息队列
CreateWindow: 创建一个子窗口，此处创建一个编辑框作为子窗口
SetFocus: 设置焦点到指定的窗口
MoveWindow: 调整窗口的位置和大小
MessageBox: 弹出消息框显示提示信息
PostQuitMessage: 向消息队列发送退出消息，使程序退出
DefWindowProc: 默认的窗口消息处理过程，用于处理未处理的消息类型
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;  // 句柄变量，用于存储编辑框的句柄

    switch (message) {
        case WM_CREATE :
            // 创建一个编辑框作为子窗口
            hwndEdit = CreateWindow(TEXT("edit"), NULL,
            /*
             WS_CHILD: 将编辑框作为子窗口创建，它将被嵌入到父窗口中。
            WS_VISIBLE: 创建后立即显示编辑框，使其可见。
            WS_HSCROLL: 启用水平滚动条，允许用户水平滚动编辑框的内容。
            WS_VSCROLL: 启用垂直滚动条，允许用户垂直滚动编辑框的内容。
            WS_BORDER: 给编辑框添加一个边框。
            ES_LEFT: 文本在编辑框中左对齐。
            ES_MULTILINE: 允许多行文本输入，可以换行。
            ES_AUTOHSCROLL: 自动水平滚动，当文本超过编辑框的宽度时，自动显示水平滚动条。
            ES_AUTOVSCROLL: 自动垂直滚动，当文本超过编辑框的高度时，自动显示垂直滚动条。
             */
                                    WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                                    WS_BORDER | ES_LEFT | ES_MULTILINE |
                                    ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                                    0, 0, 0, 0, hwnd, (HMENU) ID_EDIT,
                                    ((LPCREATESTRUCT) lParam)->hInstance, NULL);
            return 0;

        case WM_SETFOCUS :
            // 将焦点设置到编辑框上
            SetFocus(hwndEdit);
            return 0;

        case WM_SIZE :
            // 调整编辑框的大小以适应窗口的大小
            MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            return 0;

        case WM_COMMAND :
            /*
- `EN_MAXTEXT`: 定义了编辑框控件发送的通知消息中的一个标识符。
             当用户试图在编辑框中输入的字符数超过其限制时，编辑框会发送 `EN_MAXTEXT` 消息给其父窗口。
             这个消息通知父窗口编辑框已达到最大文本长度。

- `EN_ERRSPACE`: 定义了编辑框控件发送的通知消息中的一个标识符。
             当编辑框无法分配足够的内存来存储用户输入的文本时，编辑框会发送 `EN_ERRSPACE` 消息给其父窗口。
             这个消息通知父窗口编辑框内存不足。

- `ID_EDIT`: 定义了编辑框控件的标识符。它用于在窗口创建时指定编辑框的标识符，以便后续识别和操作该编辑框。

这些枚举值用于识别和处理编辑框控件发送的通知消息，并提供与编辑框相关的信息，例如文本长度超过限制或内存不足的情况。
             开发者可以根据这些枚举值来采取适当的操作或显示相应的消息框来处理这些情况。
             */
            // 处理来自编辑框的命令消息
            if (LOWORD(wParam) == ID_EDIT) {
                // 检查命令源是否为编辑框
                if (HIWORD(wParam) == EN_ERRSPACE ||
                    HIWORD(wParam) == EN_MAXTEXT) {
                    // 处理编辑框空间不足的情况
                    MessageBox(hwnd, TEXT("Edit control out of space."),
                               szAppName, MB_OK | MB_ICONSTOP);
                }
            }
            return 0;

        case WM_DESTROY :
            // 销毁窗口，并发送退出消息给消息队列
            PostQuitMessage(0);
            return 0;
    }

    // 默认消息处理过程
    return DefWindowProc(hwnd, message, wParam, lParam);
}
