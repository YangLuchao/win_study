//
// Created by ylc on 2023/6/16.
//
// 不过，无论何时在程式中使用其他表头档案时，都应在所有其他表头档案之前包含
// WINDOWS.H
#include <Windows.h>
// TCHAR.H 为需要字串参数的标准执行时期程式库函式提供了一系列
// 的替代名称（例如，_tprintf 和_tcslen）。 有时这些名称也称为「通用」函式
// 名称，因为它们既可以指向函式的 Unicode 版也可以指向非 Unicode 版。
#include <stdio.h>
#include <tchar.h>

int CDECL MessageBoxPrintf(TCHAR *szCaption, TCHAR *szFormat, ...) {
    TCHAR szBuffer[1024];
    va_list pArgList;

    //---------------- 做文本格式化的模板，就这样写就行
            va_start(pArgList, szFormat);
    _vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);
            va_end(pArgList);
    //---------------- 做文本格式化的模板，就这样写就行

    // 第二个参数是标题
    // 第三个参数是内容
    // 第四个参数是弹窗类型
    return MessageBox(NULL, szBuffer, szCaption, 0);
}

/*
HINSTANCE hinstance
    WinMain函数：Windows程序入口函数：该函数的功能是被系统调用，作为一个32位应用程序的入口点。
    WinMain函数应初始化应用程序，显示主窗口，进入一个消息接收一发送循环，这个循环是应用程序执行的其余部分的顶级控制结构。
    函数返回值类型：int
    WINAPI : 函数调用约定，_stdcall
    hlnstance :
应用程序（进程）句柄，与指针、引用差不多，但又不同。（句柄无类型，其值会因操作系统中内存页面转换而不同，由操作系统维护
    hPrevlnstance : 通过此参数可查看是否另一个实例进程正在运行。为NULL,
兼容windows16的遗留，现在一般不用。 lpCmdLine : 命令行参数 nCmdShow :
窗体显示方式一正常、最大化、最小化、全屏等
*/
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPreInstance, PSTR szCmdLine,
                   int iCmdShow) {
    int cxScreen, cyScreen;
    // GetSystemMetric获取系统的信息
    // SM_CXSCREEN： 主显示器的屏幕宽度（以像素为单位）
    cxScreen = GetSystemMetrics(SM_CXSCREEN);
    // SM_CYBORDER：窗口边框的高度（以像素为单位）
    cyScreen = GetSystemMetrics(SM_CYBORDER);

    MessageBoxPrintf(TEXT("ScrnSize"),
                     TEXT("The screen is %i pixels wide by %i pixels high."),
                     cxScreen, cyScreen);
    return 0;
}
