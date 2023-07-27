#include <windows.h>

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // 窗口类结构变量
  WNDCLASS wndclass;
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);

  return 0;
}