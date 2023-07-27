#include <windows.h>
#include <commdlg.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
    static CHOOSECOLOR cc;                     // CHOOSECOLOR 结构体实例
    static COLORREF crCustColors[16];          // 颜色自定义数组，保存用户自定义的颜色

    cc.lStructSize = sizeof(CHOOSECOLOR);      // 设置 CHOOSECOLOR 结构体的大小
    cc.hwndOwner = NULL;                       // 对话框的父窗口句柄，这里为 NULL
    cc.hInstance = NULL;                       // 当前实例句柄，这里为 NULL
    cc.rgbResult = RGB(0x80, 0x80, 0x80);      // 设置默认颜色为灰色（RGB(128, 128, 128)）
    cc.lpCustColors = crCustColors;            // 设置颜色自定义数组指针
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;       // 选择颜色对话框的标志，包括 RGB 初始化和完全打开
    cc.lCustData = 0;                          // 自定义数据，这里为 0
    cc.lpfnHook = NULL;                        // 钩子函数，这里为 NULL
    cc.lpTemplateName = NULL;                  // 对话框模板的名称，这里为 NULL

    return ChooseColor(&cc);                   // 调用选择颜色对话框，返回用户选择的颜色
}
