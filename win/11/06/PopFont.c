#include <windows.h>
#include <commdlg.h>

static LOGFONT logfont; // 静态变量，用于存储字体信息
static HFONT hFont;     // 静态变量，用于存储字体句柄

// 弹出字体选择对话框，用户选择字体后返回选择结果
BOOL PopFontChooseFont(HWND hwnd) {
    CHOOSEFONT cf;

    cf.lStructSize = sizeof(CHOOSEFONT);             // 结构体大小
    cf.hwndOwner = hwnd;                             // 拥有对话框的窗口句柄
    cf.hDC = NULL;                                   // 设备上下文句柄，设置为NULL以使用屏幕的设备上下文
    cf.lpLogFont = &logfont;                         // 传递给对话框的LOGFONT结构指针
    cf.iPointSize = 0;                               // 字号大小（以1/10磅为单位），设为0表示忽略此选项
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS; // 字体选择对话框选项
    cf.rgbColors = 0;                                // 初始颜色
    cf.lCustData = 0;                                // 用户自定义数据
    cf.lpfnHook = NULL;                              // 钩子函数指针
    cf.lpTemplateName = NULL;                        // 模板名称
    cf.hInstance = NULL;                            // 当前实例句柄
    cf.lpszStyle = NULL;                             // 设定字体的样式字符串
    cf.nFontType = 0;                               // 返回字体类型标志的存储位置，由ChooseFont函数填充
    cf.nSizeMin = 0;                                // 字号的最小值
    cf.nSizeMax = 0;                                // 字号的最大值

    return ChooseFont(&cf);                          // 弹出字体选择对话框，并返回选择结果
}

// 初始化字体设置
void PopFontInitialize(HWND hwndEdit) {
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT),
        (PTSTR)&logfont);                     // 获取系统字体的信息

    hFont = CreateFontIndirect(&logfont);            // 创建字体句柄
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0); // 将字体应用到编辑框
}

// 设置字体
void PopFontSetFont(HWND hwndEdit) {
    HFONT hFontNew;
    RECT rect;

    hFontNew = CreateFontIndirect(&logfont);         // 创建新字体句柄
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontNew, 0); // 将新字体应用到编辑框
    DeleteObject(hFont);                            // 删除旧字体句柄
    hFont = hFontNew;                               // 更新字体句柄
    GetClientRect(hwndEdit, &rect);                 // 获取编辑框客户区矩形
    InvalidateRect(hwndEdit, &rect, TRUE);          // 使客户区无效，触发重绘
}

// 清理字体设置
void PopFontDeinitialize(void) {
    DeleteObject(hFont);                           // 删除字体句柄
}
