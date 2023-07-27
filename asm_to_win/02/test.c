
#include <windows.h>

int WinMain(HINSTANCE hinstance, HINSTANCE preHandInstance, PSTR cmdLine,
            int cmdShow) {

  MessageBox(NULL, "当您看到这个信息的时候，您已经可以编译Win32汇编程序了!",
             "恭喜", 0);
  return 0;
}