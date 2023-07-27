#include <windows.h> // 包含Windows API头文件

// 函数原型声明窗口过程
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Windows程序入口点  
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    // 应用程序名称
    static TCHAR szAppName[] = TEXT("HexCalc");

    // 主窗口句柄 
    HWND         hwnd;

    // 消息结构体
    MSG          msg;

    // 窗口类结构体
    WNDCLASS     wndclass;

    // 设置窗口类属性
    wndclass.style = CS_HREDRAW | CS_VREDRAW; // 宽度/高度改变时重绘      
    wndclass.lpfnWndProc = WndProc; // 设置WndProc为窗口过程
    wndclass.cbClsExtra = 0; // 无额外类内存
    wndclass.cbWndExtra = DLGWINDOWEXTRA; // 对话框额外内存
    wndclass.hInstance = hInstance; // 应用程序实例句柄
    wndclass.hIcon = LoadIcon(hInstance, szAppName); // 加载图标
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); // 标准箭头光标
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // 按钮面颜色
    wndclass.lpszMenuName = NULL; // 无菜单
    wndclass.lpszClassName = szAppName; // 类名

    // 注册窗口类
    if (!RegisterClass(&wndclass))
    {
        // 注册失败显示错误
        MessageBox(NULL, TEXT("This program requires Windows NT!"),
            szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建主对话窗口
    hwnd = CreateDialog(hInstance, szAppName, 0, NULL);

    // 显示窗口
    ShowWindow(hwnd, iCmdShow);

    // 消息循环
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg); // 翻译键盘消息
        DispatchMessage(&msg); // 发送到WndProc
    }

    // 返回退出码
    return msg.wParam;
}

// 在窗口中显示数字
void ShowNumber(HWND hwnd, UINT iNumber)
{
    TCHAR szBuffer[20]; // 数字字符串缓冲区

    wsprintf(szBuffer, TEXT("%X"), iNumber); // 转换为十六进制字符串
    SetDlgItemText(hwnd, VK_ESCAPE, szBuffer); // 设置窗口文本
}

// 对两个数执行运算
DWORD CalcIt(UINT iFirstNum, int iOperation, UINT iNum)
{
    switch (iOperation)
    {
    case '=': return iNum; // 等于,直接返回第二个数
    case '+': return iFirstNum + iNum; // 加法
    case '-': return iFirstNum - iNum; // 减法
    case '*': return iFirstNum * iNum; // 乘法
    case '&': return iFirstNum & iNum; // 逻辑与
    case '|': return iFirstNum | iNum;  // 逻辑或
    case '^': return iFirstNum ^ iNum; // 逻辑异或
    case '<': return iFirstNum << iNum; // 左移
    case '>': return iFirstNum >> iNum; // 右移
    case '/': return iNum ? iFirstNum / iNum : MAXDWORD; //除法,检查除0错误
    case '%': return iNum ? iFirstNum % iNum : MAXDWORD; //取模,检查除0错误
    default: return 0; // 未知操作
    }
}

// 主窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static BOOL bNewNumber = TRUE; // 新数输入标志
    static int iOperation = '='; // 当前操作
    static UINT iNumber, iFirstNum; // 数值

    HWND hButton; // 按钮句柄

    switch (message)
    {
    case WM_KEYDOWN: // 按键按下

        // 左箭头映射到退格键
        if (wParam != VK_LEFT)
            break;

        wParam = VK_BACK;

        // 跳转到退格键处理代码

    case WM_CHAR: // 字符输入

        // 转换为大写
        wParam = (WPARAM)CharUpper((TCHAR*)wParam);

        // Enter键映射到等号操作符
        if (wParam == VK_RETURN)
            wParam = '=';

        // 如果键对应按钮,模拟按钮点击
        if (hButton = GetDlgItem(hwnd, wParam))
        {
            SendMessage(hButton, BM_SETSTATE, 1, 0); // 设置按钮状态
            Sleep(100); // 暂停
            SendMessage(hButton, BM_SETSTATE, 0, 0); // 清除按钮状态 
        }
        else
        {
            MessageBeep(0); // 非法键,响铃
            break;
        }

        // 跳转到操作符处理代码

    case WM_COMMAND: // 命令(按钮点击)

        SetFocus(hwnd); // 设置焦点到窗口

        // 处理退格键
        if (LOWORD(wParam) == VK_BACK)
            ShowNumber(hwnd, iNumber /= 16);

        // 处理Escape键
        else if (LOWORD(wParam) == VK_ESCAPE)
            ShowNumber(hwnd, iNumber = 0);

        // 十六进制数字输入
        else if (isxdigit(LOWORD(wParam)))
        {
            if (bNewNumber) // 开始新数值
            {
                iFirstNum = iNumber;
                iNumber = 0;
            }

            bNewNumber = FALSE; // 当前数值

            // 检查数值溢出
            if (iNumber <= MAXDWORD >> 4)
                ShowNumber(hwnd, iNumber = 16 * iNumber + wParam -
                    (isdigit(wParam) ? '0' : 'A' - 10));
            else
                MessageBeep(0); // 溢出,响铃
        }

        // 其它按键是操作符
        else
        {
            if (!bNewNumber) // 存在当前数值
                ShowNumber(hwnd, iNumber = CalcIt(iFirstNum, iOperation, iNumber));

            bNewNumber = TRUE; // 准备新数值  
            iOperation = LOWORD(wParam); // 设置当前操作
        }

        return 0;

    case WM_DESTROY: // 窗口销毁
        PostQuitMessage(0); // 发出退出消息
        return 0;
    }

    // 其他消息调用默认窗口过程
    return DefWindowProc(hwnd, message, wParam, lParam);
}