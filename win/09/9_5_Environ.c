#include <windows.h>

#define ID_LIST     1
#define ID_TEXT     2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT ("Environ");
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
    wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT ("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow (szAppName, TEXT("Environment List Box"),
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
这个函数的作用是填充一个列表框（ListBox）控件，其中包含当前进程的环境变量名称。

函数的实现逻辑如下：

1. 使用 `GetEnvironmentStrings` 函数获取指向当前进程环境块（Environment Block）的指针 `pVarBlock`。

2. 进入循环，遍历环境块中的每个环境变量。

3. 检查当前环境变量是否以 '=' 开头，如果是则跳过，因为这些变量是特殊变量，不需要显示在列表框中。

4. 如果当前环境变量不是以 '=' 开头，则将 `pVarBeg` 指向变量名的起始位置，并向后扫描直到找到等号 '=' 的位置，将 `pVarEnd` 指向等号的位置。

5. 计算变量名的长度，即 `iLength`，以便后续分配内存。

6. 使用 `calloc` 函数为变量名分配内存，并将变量名拷贝到新分配的内存空间中。

7. 将变量名添加到列表框中，使用 `SendMessage` 函数发送 `LB_ADDSTRING` 消息给列表框控件。

8. 释放为变量名分配的内存，使用 `free` 函数。

9. 在环境块中继续向后扫描，直到遇到终止符 '\0'。

10. 使用 `FreeEnvironmentStrings` 函数释放环境块的内存。

这个函数的目的是获取当前进程的环境变量，并将环境变量名称显示在列表框中，供用户查看。
 */
void FillListBox(HWND hwndList) {
    int iLength;
    TCHAR *pVarBlock, *pVarBeg, *pVarEnd, *pVarName;

    pVarBlock = GetEnvironmentStrings();  // Get pointer to environment block

    while (*pVarBlock) {
        if (*pVarBlock != '=')   // Skip variable names beginning with '='
        {
            pVarBeg = pVarBlock;              // Beginning of variable name
            while (*pVarBlock++ != '=');      // Scan until '='
            pVarEnd = pVarBlock - 1;          // Points to '=' sign
            iLength = pVarEnd - pVarBeg;      // Length of variable name

            // Allocate memory for the variable name and terminating
            // zero. Copy the variable name and append a zero.

            pVarName = calloc(iLength + 1, sizeof(TCHAR));
            CopyMemory (pVarName, pVarBeg, iLength * sizeof(TCHAR));
            pVarName[iLength] = '\0';

            // Put the variable name in the list box and free memory.

            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) pVarName);
            free(pVarName);
        }
        while (*pVarBlock++ != '\0');     // Scan until terminating zero
    }
    FreeEnvironmentStrings(pVarBlock);
}

/*
这个函数是一个窗口过程函数（WndProc），用于处理窗口消息并实现相应的功能。

函数的作用如下：

1. 在窗口创建消息（WM_CREATE）中：
   - 根据窗口的基本单元（Dialog Base Units）计算每个字符的宽度（`cxChar`）和高度（`cyChar`）。
   - 创建一个列表框（Listbox）控件和一个静态文本框（Static Text）控件，并设置它们的样式、位置和大小。
   - 调用 `FillListBox` 函数填充列表框控件。

2. 在窗口设置焦点消息（WM_SETFOCUS）中：
   - 将焦点设置到列表框控件上。

3. 在窗口命令消息（WM_COMMAND）中：
   - 检查触发消息的控件是否为列表框控件，并且消息是选择改变（LBN_SELCHANGE）的消息。
   - 获取当前列表框的选择项索引（iIndex）。
   - 根据索引获取选中项的文本长度（iLength）并为变量名（pVarName）分配内存。
   - 通过发送消息（SendMessage）获取选中项的文本，并将文本复制到变量名中。
   - 使用 GetEnvironmentVariable 函数获取与变量名对应的环境变量的值，并为变量值（pVarValue）分配内存。
   - 将环境变量的值设置为静态文本框的文本内容。
   - 释放为变量名和变量值分配的内存。

4. 在窗口销毁消息（WM_DESTROY）中：
   - 发送退出消息（PostQuitMessage）以终止消息循环。

该函数的主要功能是创建一个包含列表框和静态文本框的窗口，并允许用户选择列表框中的环境变量，然后将所选环境变量的值显示在静态文本框中。
 */
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hwndList, hwndText;
    int iIndex, iLength, cxChar, cyChar;
    TCHAR *pVarName, *pVarValue;

    switch (message) {
        case WM_CREATE :
            cxChar = LOWORD (GetDialogBaseUnits());
            cyChar = HIWORD (GetDialogBaseUnits());

            // Create listbox and static text windows.

            hwndList = CreateWindow (TEXT("listbox"), NULL,
            /*
- `WS_CHILD`: 指定窗口作为父窗口的子窗口。
- `WS_VISIBLE`: 指定窗口在创建时可见。
- `LBS_STANDARD`: 指定列表框具有标准样式，包括可滚动、支持多选和扩展选项等。

具体含义如下：

- `WS_CHILD`: 使用该样式创建的窗口将成为父窗口的子窗口。子窗口在父窗口的客户区内进行布局，并随父窗口的移动和调整大小而相应地调整位置和大小。
- `WS_VISIBLE`: 使用该样式创建的窗口将在创建时可见。如果不指定该样式，则窗口将在创建后通过显示窗口消息（如 `ShowWindow`）显式设置为可见。
- `LBS_STANDARD`: 使用该样式创建的列表框具有标准样式和行为。它是一个可滚动的列表框，可以通过鼠标选择单个或多个项，并提供了一些扩展选项（如自动水平和垂直滚动）。

这些样式通过按位 OR 运算符（`|`）组合在一起，以指定窗口的外观和行为。在给定的示例中，使用这些样式创建的列表框是一个可见的子窗口，具有标准的列表框外观和行为。
             */
                                     WS_CHILD | WS_VISIBLE | LBS_STANDARD,
                                     cxChar, cyChar * 3,
            /*
`GetSystemMetrics` 函数用于检索与系统相关的度量信息。它允许您获取有关系统设置和指定参数的信息。通过调用 `GetSystemMetrics` 函数并传递相应的参数，您可以获取诸如窗口边框宽度、屏幕尺寸、滚动条尺寸、字体尺寸等与系统相关的度量信息。

该函数的常见用法包括：

- 获取窗口边框的宽度和高度，以便进行窗口布局和调整。
- 获取屏幕的尺寸和分辨率，以便适应不同的显示设备。
- 获取滚动条的尺寸，以便在绘制和布局中进行适当的调整。
- 获取系统字体的尺寸和行间距，以便进行文本布局和渲染。

通过调用 `GetSystemMetrics` 函数并传递相应的参数，您可以获取所需的系统度量信息，并根据需要在应用程序中进行相应的调整和适配。
             */
                                     cxChar * 16 + GetSystemMetrics(SM_CXVSCROLL),
                                     cyChar * 5,
                                     hwnd, (HMENU) ID_LIST,
                                     (HINSTANCE) GetWindowLong(hwnd, -6),
                                     NULL);

            hwndText = CreateWindow (TEXT("static"), NULL,
                                     WS_CHILD | WS_VISIBLE | SS_LEFT,
                                     cxChar, cyChar,
                                     GetSystemMetrics(SM_CXSCREEN), cyChar,
                                     hwnd, (HMENU) ID_TEXT,
                                     (HINSTANCE) GetWindowLong(hwnd, -6),
                                     NULL);

            FillListBox(hwndList);
            return 0;

        case WM_SETFOCUS :
            SetFocus(hwndList);
            return 0;

            /*
上述代码片段处理了`WM_COMMAND`消息，当用户选择列表框中的项发生改变时执行相应操作。具体注释如下：

- 在条件判断中，`LOWORD(wParam) == ID_LIST`表示消息源是列表框，`HIWORD(wParam) == LBN_SELCHANGE`表示选中项发生了改变。
- `iIndex`变量存储当前选中项的索引。
- `iLength`变量存储选中项文本的长度加上终止符的空间。
- `pVarName`变量是用于存储选中项文本的指针，通过`calloc`函数动态分配内存空间。
- 通过`SendMessage`函数的`LB_GETTEXT`消息，将选中项的文本存储到`pVarName`指向的内存空间。
- 通过`GetEnvironmentVariable`函数，获取选中项所对应的环境变量的值所需的缓冲区长度。
- `pVarValue`变量是用于存储环境变量值的指针，通过`calloc`函数动态分配内存空间。
- 通过`GetEnvironmentVariable`函数，将选中环境变量的值存储到`pVarValue`指向的内存空间。
- 通过`SetWindowText`函数，将环境变量的值显示在文本窗口中。
- 最后，释放动态分配的内存空间。
             */
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE) {
                // 当选择列表框中的项发生改变时执行以下操作

                // 获取当前选中项的索引
                iIndex = SendMessage(hwndList, LB_GETCURSEL, 0, 0);

                // 获取选中项文本的长度并加上终止符的空间
                iLength = SendMessage(hwndList, LB_GETTEXTLEN, iIndex, 0) + 1;

                // 分配内存用于存储选中项的文本
                pVarName = calloc(iLength, sizeof(TCHAR));

                // 获取选中项的文本
                SendMessage(hwndList, LB_GETTEXT, iIndex, (LPARAM) pVarName);

                // 获取环境变量的值

                // 获取环境变量值所需的缓冲区长度
                iLength = GetEnvironmentVariable(pVarName, NULL, 0);

                // 分配内存用于存储环境变量的值
                pVarValue = calloc(iLength, sizeof(TCHAR));

                // 获取环境变量的值
                GetEnvironmentVariable(pVarName, pVarValue, iLength);

                // 在文本窗口中显示环境变量的值
                SetWindowText(hwndText, pVarValue);

                // 释放内存
                free(pVarName);
                free(pVarValue);
            }
            return 0;


        case WM_DESTROY :
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
