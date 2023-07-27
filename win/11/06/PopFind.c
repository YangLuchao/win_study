#include <windows.h>
#include <commdlg.h>
#include <tchar.h>            

#define MAX_STRING_LEN   256

static TCHAR szFindText[MAX_STRING_LEN]; // 存储查找文本的静态缓冲区
static TCHAR szReplText[MAX_STRING_LEN]; // 存储替换文本的静态缓冲区

// 弹出查找对话框
HWND PopFindFindDlg(HWND hwnd) {
    static FINDREPLACE fr;

    fr.lStructSize = sizeof(FINDREPLACE);
    fr.hwndOwner = hwnd;
    fr.hInstance = NULL;
    fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD; // 隐藏“向上”、“向下”按钮和“匹配大小写”、“全字匹配”复选框
    fr.lpstrFindWhat = szFindText; // 指向查找文本缓冲区
    fr.lpstrReplaceWith = NULL;
    fr.wFindWhatLen = MAX_STRING_LEN; // 查找文本缓冲区长度
    fr.wReplaceWithLen = 0;
    fr.lCustData = 0;
    fr.lpfnHook = NULL;
    fr.lpTemplateName = NULL;

    return FindText(&fr); // 弹出查找对话框，并返回对话框句柄
}

// 弹出替换对话框
HWND PopFindReplaceDlg(HWND hwnd) {
    static FINDREPLACE fr;       // 必须是静态的以在模式对话框上工作

    fr.lStructSize = sizeof(FINDREPLACE);
    fr.hwndOwner = hwnd;
    fr.hInstance = NULL;
    fr.Flags = FR_HIDEUPDOWN | FR_HIDEMATCHCASE | FR_HIDEWHOLEWORD; // 隐藏“向上”、“向下”按钮和“匹配大小写”、“全字匹配”复选框
    fr.lpstrFindWhat = szFindText; // 指向查找文本缓冲区
    fr.lpstrReplaceWith = szReplText; // 指向替换文本缓冲区
    fr.wFindWhatLen = MAX_STRING_LEN; // 查找文本缓冲区长度
    fr.wReplaceWithLen = MAX_STRING_LEN; // 替换文本缓冲区长度
    fr.lCustData = 0;
    fr.lpfnHook = NULL;
    fr.lpTemplateName = NULL;

    return ReplaceText(&fr); // 弹出替换对话框，并返回对话框句柄
}

// 在编辑框中查找文本
BOOL PopFindFindText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr) {
    int iLength, iPos;
    PTSTR pstrDoc, pstrPos;

    iLength = GetWindowTextLength(hwndEdit);

    if (NULL == (pstrDoc = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR))))
        return FALSE;

    GetWindowText(hwndEdit, pstrDoc, iLength + 1);

    pstrPos = _tcsstr(pstrDoc + *piSearchOffset, pfr->lpstrFindWhat); // 查找文本在编辑框文本中的位置
    free(pstrDoc);

    if (pstrPos == NULL)
        return FALSE;

    iPos = pstrPos - pstrDoc; // 计算查找文本在编辑框中的偏移量
    *piSearchOffset = iPos + lstrlen(pfr->lpstrFindWhat); // 更新查找偏移量

    SendMessage(hwndEdit, EM_SETSEL, iPos, *piSearchOffset); // 在编辑框中选中查找到的文本
    SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0); // 将编辑框滚动到光标位置

    return TRUE;
}

// 在编辑框中查找下一个文本
BOOL PopFindNextText(HWND hwndEdit, int* piSearchOffset) {
    FINDREPLACE fr;

    fr.lpstrFindWhat = szFindText;

    return PopFindFindText(hwndEdit, piSearchOffset, &fr); // 调用查找函数进行查找
}

// 在编辑框中替换文本
BOOL PopFindReplaceText(HWND hwndEdit, int* piSearchOffset, LPFINDREPLACE pfr) {
    // 查找文本

    if (!PopFindFindText(hwndEdit, piSearchOffset, pfr))
        return FALSE;

    SendMessage(hwndEdit, EM_REPLACESEL, 0, (LPARAM)pfr->lpstrReplaceWith); // 替换找到的文本

    return TRUE;
}

// 检查是否有有效的查找文本
BOOL PopFindValidFind(void) {
    return *szFindText != '\0'; // 检查查找文本是否为空
}
