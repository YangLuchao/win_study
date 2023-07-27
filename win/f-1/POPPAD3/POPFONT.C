/*----------------------------------------------------
 POPFONT.C -- Popup Editor Font Functions
------------------------------------------------------*/
#include <windows.h> 
#include <commdlg.h> 
static LOGFONT logfont; //字体的属性结构变量
static HFONT hFont;

//选择字体
BOOL PopFontChooseFont(HWND hwnd)
{
    CHOOSEFONT cf; //包含ChooseFont函数用来初始化“字体”对话框的信息。用户关闭对话框后，系统将在此结构中返回有关用户选择的信息。
    cf.lStructSize = sizeof(CHOOSEFONT); //结构的长度，以字节为单位。
    cf.hwndOwner = hwnd; //拥有对话框的窗口的句柄。
    cf.hDC = NULL;       //忽略
    cf.lpLogFont = &logfont; //指向LOGFONT结构的指针 。
    cf.iPointSize = 0; //选择的字体的大小，单位是1/10磅
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | //用logfont初始化对话框，屏幕字体、效果复选框
        CF_EFFECTS;
    cf.rgbColors = 0; //文本颜色
    cf.lCustData = 0;
    cf.lpfnHook = NULL;
    cf.lpTemplateName = NULL;
    cf.hInstance = NULL;
    cf.lpszStyle = NULL; //字体样式
    cf.nFontType = 0; // 所选的字体
    cf.nSizeMin = 0; //用户可以选择的最小点数
    cf.nSizeMax = 0; //用户可以选择的最大点数

    return ChooseFont(&cf);
}
//初始化对话框字体
void PopFontInitialize(HWND hwndEdit)
{
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), //获取系统字体
        (PTSTR)&logfont);
    hFont = CreateFontIndirect(&logfont);   //创建字体
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0);//初始化对话框为系统字体
}
//设置字体
void PopFontSetFont(HWND hwndEdit)
{
    HFONT hFontNew;
    RECT rect;

    hFontNew = CreateFontIndirect(&logfont);
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontNew, 0);//改变对话框字体
    DeleteObject(hFont);
    hFont = hFontNew;
    GetClientRect(hwndEdit, &rect);
    InvalidateRect(hwndEdit, &rect, TRUE);
}
//删除字体
void PopFontDeinitialize(void)
{
    DeleteObject(hFont);
}