/*--------------------------------------------------------------
   HelloMsg.c -- Displays "Hello, Windows 98!" in a message box
                 (c) Charles Petzold, 1998
  --------------------------------------------------------------*/

#include <Windows.h>

/*
HINSTANCE hinstance
    WinMain函数：Windows程序入口函数：该函数的功能是被系统调用，作为一个32位应用程序的入口点。
    WinMain函数应初始化应用程序，显示主窗口，进入一个消息接收一发送循环，这个循环是应用程序执行的其余部分的顶级控制结构。
    函数返回值类型：int
    WINAPI : 函数调用约定，_stdcall
    hinstance :
应用程序（进程）句柄，与指针、引用差不多，但又不同。（句柄无类型，其值会因操作系统中内存页面转换而不同，由操作系统维护
    hPrevinstance : 通过此参数可查看是否另一个实例进程正在运行。为NULL,
兼容windows16的遗留，现在一般不用。 szCmdLine : 命令行参数 iCmdShow :
窗体显示方式一正常、最大化、最小化、全屏等
*/
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR szCmdLine,
                   int iCmdShow) {
    MessageBox(NULL, TEXT("Hello, Windows 98!"), TEXT("HelloMsg"), 0);
    return 0;
}
