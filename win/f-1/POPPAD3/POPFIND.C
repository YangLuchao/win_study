/*--------------------------------------------------------------------------
 POPFIND.C -- Popup Editor Search and Replace Functions
 ------------------------------------------------------------------------*/
#include <windows.h> 
#include <commdlg.h> 
#include <tchar.h> // for _tcsstr (strstr for Unicode & non-Unicode) 
#define MAX_STRING_LEN 256 
static TCHAR szFindText[MAX_STRING_LEN];
static TCHAR szReplText[MAX_STRING_LEN];

//初始化Find对话框
HWND PopFindFindDlg(HWND hwnd)
{
    static FINDREPLACE fr; // 非模态对话框必须是静态的!!

    fr.lStructSize = sizeof(FINDREPLACE); //结构的长度（以字节为单位）。
    fr.hwndOwner = hwnd;   //拥有对话框的窗口的句柄。
    fr.hInstance = NULL;
    fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | //隐藏搜索方向单选按钮，隐藏“区分大小写”复选框，
        FR_HIDEWHOLEWORD; //隐藏“仅匹配整个单词”复选框
    fr.lpstrFindWhat = szFindText; //键入的搜索字符串缓冲区的长度
    fr.lpstrReplaceWith = NULL; //键入的替换字符串缓冲区的长度，
    //如果FINDMSGSTRING消息指定FR_REPLACE或FR_REPLACEALL标志，
    //则lpstrReplaceWith包含替换字符串，该FINDTEXT函数忽略这个成员。
    fr.wFindWhatLen = MAX_STRING_LEN; //lpstrFindWhat成员指向的缓冲区的长度（以字节为单位）。
    fr.wReplaceWithLen = 0; //lpstrReplaceWith成员指向的缓冲区的长度（以字节为单位）。
    fr.lCustData = 0; //Hook过程的应用程序定义的数据
    fr.lpfnHook = NULL; //指向FRHookProc Hook过程的指针，该过程可以处理用于对话框的消息。
    fr.lpTemplateName = NULL; //hInstance成员标识的模块中对话框模板资源的名称。

    return FindText(&fr); //创建查找对话框并返回其句柄
}
//初始化FindReplace对话框
HWND PopFindReplaceDlg(HWND hwnd)
{
    static FINDREPLACE fr; // 非模态对话框必须是静态的!!

    fr.lStructSize = sizeof(FINDREPLACE);
    fr.hwndOwner = hwnd;
    fr.hInstance = NULL;
    fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE |
        FR_HIDEWHOLEWORD;
    fr.lpstrFindWhat = szFindText;
    fr.lpstrReplaceWith = szReplText;
    fr.wFindWhatLen = MAX_STRING_LEN;
    fr.wReplaceWithLen = MAX_STRING_LEN;
    fr.lCustData = 0;
    fr.lpfnHook = NULL;
    fr.lpTemplateName = NULL;

    return ReplaceText(&fr);
}
//查找字符串
BOOL PopFindFindText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr)
{
    int iLength, iPos;
    PTSTR pstrDoc, pstrPos; //文件指针和当前位置指针

    //读取编辑框内容

    iLength = GetWindowTextLength(hwndEdit);

    if (NULL == (pstrDoc = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR))))
        return FALSE;

    GetWindowText(hwndEdit, pstrDoc, iLength + 1);

    // 在文档中搜索查找字符串 
    pstrPos = _tcsstr(pstrDoc + *piSearchOffset, pfr->lpstrFindWhat);
    free(pstrDoc);

    //如果找不到，返回FALSE  
    if (pstrPos == NULL)
        return FALSE;

    //找到字符串，将偏移设置到新的位置  
    iPos = pstrPos - pstrDoc;
    *piSearchOffset = iPos + lstrlen(pfr->lpstrFindWhat); //偏移设到找到的字符串的后面。

    //选中找到的文本 
    SendMessage(hwndEdit, EM_SETSEL, iPos, *piSearchOffset);
    SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0); //可视窗口滚动到插入符号的位置，以便可见找到的文本。

    return TRUE;
}
//查找下一处
BOOL PopFindNextText(HWND hwndEdit, int* piSearchOffset)
{
    FINDREPLACE fr;
    fr.lpstrFindWhat = szFindText;
    return PopFindFindText(hwndEdit, piSearchOffset, &fr);
}
//替换文本字符串
BOOL PopFindReplaceText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr)
{
    //查找文本
    if (!PopFindFindText(hwndEdit, piSearchOffset, pfr))
        return FALSE;

    // 替换
    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)pfr->
        lpstrReplaceWith);
    return TRUE;
}
//无效查找
BOOL PopFindValidFind(void)
{
    return *szFindText != '\0'; //没输入查找文本时，是无效的查找。
}