#include <Windows.h>
#include <tchar.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

// 控件ID
#define IDC_PROGRESS    1
#define IDC_STATUS      2
#define IDC_TIMER       3

TCHAR ClassName[] = _T("CommonControlWinClass"); // 窗口类名
TCHAR AppName[] = _T("Common Control Demo"); // 应用程序名称
TCHAR ProgressClass[] = _T("msctls_progress32"); // 进度条控件类名
TCHAR Message[] = _T("Finished!"); // 完成消息文本

DWORD TimerID;
HINSTANCE g_hInstance;
HWND hwndStatus;
HWND hwndProgress;
DWORD CurrentStep;

// 主窗口过程
LRESULT CALLBACK ProcWinMain(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		// 创建进度条控件
		hwndProgress = CreateWindowEx(0, ProgressClass, NULL, WS_CHILD | WS_VISIBLE,
			100, 200, 300, 20, hWnd, (HMENU)IDC_PROGRESS, g_hInstance, NULL);
		CurrentStep = 1000;
		SendMessage(hwndProgress, PBM_SETRANGE, 0, 65536000); // 设置进度条范围
		SendMessage(hwndProgress, PBM_SETSTEP, 10, 0); // 设置进度条步进
		hwndStatus = CreateStatusWindow(WS_CHILD | WS_VISIBLE, NULL, hWnd, IDC_STATUS); // 创建状态栏
		TimerID = SetTimer(hWnd, IDC_TIMER, 100, NULL); // 设置定时器
		break;
	case WM_DESTROY:
		PostQuitMessage(0); // 退出消息循环
		if (TimerID != 0)
			KillTimer(hWnd, TimerID); // 关闭定时器
		break;
	case WM_TIMER:
		SendMessage(hwndProgress, PBM_STEPIT, 0, 0); // 进度条前进一步
		CurrentStep -= 10;
		if (CurrentStep == 0)
		{
			KillTimer(hWnd, TimerID); // 关闭定时器
			TimerID = 0;
			SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)Message); // 设置状态栏文本
			MessageBox(hWnd, Message, AppName, MB_OK | MB_ICONINFORMATION); // 弹出消息框
			SendMessage(hwndProgress, PBM_SETPOS, 0, 0); // 恢复进度条初始状态
		}
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

// WinMain函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG msg;
	HWND hWnd;
	g_hInstance = hInstance;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ProcWinMain;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE + 1); // 多文档界面 (MDI) 应用程序的背景颜色
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName;
	wc.hIcon = wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE, \
		ClassName, AppName, \
		WS_OVERLAPPEDWINDOW, \
		CW_USEDEFAULT, CW_USEDEFAULT, \
		CW_USEDEFAULT, CW_USEDEFAULT, \
		NULL, \
		NULL, \
		hInstance, \
		NULL\
	);
	ShowWindow(hWnd, SW_SHOWNORMAL); // 显示窗口
	UpdateWindow(hWnd); // 更新窗口
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); // 翻译消息
		DispatchMessage(&msg); // 分发消息
	}
	return msg.wParam;
}
/*
`WNDCLASSEX`是一个结构体，用于定义窗口类的属性。它的各个属性含义如下：

1. `cbSize`：结构体大小，用于指定结构体的大小，必须设置为`sizeof(WNDCLASSEX)`。

2. `style`：窗口类的风格，可以使用`CS_*`风格标志，比如`CS_HREDRAW`和`CS_VREDRAW`分别表示当窗口水平或垂直尺寸改变时重新绘制。

3. `lpfnWndProc`：指向窗口过程的指针，用于处理窗口接收到的消息。

4. `cbClsExtra`：类额外内存，保留字段，通常设置为0。

5. `cbWndExtra`：窗口额外内存，保留字段，通常设置为0。

6. `hInstance`：应用程序实例句柄，表示窗口所属的应用程序实例。

7. `hIcon`：窗口图标的句柄，通常设置为应用程序图标。

8. `hIconSm`：小图标的句柄，通常设置为应用程序图标的缩小版。

9. `hCursor`：窗口光标的句柄，通常设置为窗口所用的光标。

10. `hbrBackground`：窗口背景画刷的句柄，用于填充窗口的背景色。

11. `lpszMenuName`：菜单名称，通常设置为NULL，表示没有菜单。

12. `lpszClassName`：窗口类名，是一个字符串，用于标识窗口类的名称。

以上是`WNDCLASSEX`结构体的所有属性。创建窗口类时，需要填充这些属性，然后通过`RegisterClassEx`函数注册窗口类，使得系统能够创建窗口实例。
*/