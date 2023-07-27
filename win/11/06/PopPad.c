#include <windows.h>
#include <commdlg.h>
#include "resource.h"

#define EDITID   1
#define UNTITLED TEXT("(无标题)")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

// POPFILE.C 中的函数声明
void PopFileInitialize(HWND);
BOOL PopFileOpenDlg(HWND, PTSTR, PTSTR);
BOOL PopFileSaveDlg(HWND, PTSTR, PTSTR);
BOOL PopFileRead(HWND, PTSTR);
BOOL PopFileWrite(HWND, PTSTR);

// POPFIND.C 中的函数声明
HWND PopFindFindDlg(HWND);
HWND PopFindReplaceDlg(HWND);
BOOL PopFindFindText(HWND, int*, LPFINDREPLACE);
BOOL PopFindReplaceText(HWND, int*, LPFINDREPLACE);
BOOL PopFindNextText(HWND, int*);
BOOL PopFindValidFind(void);

// POPFONT.C 中的函数声明
void PopFontInitialize(HWND);
BOOL PopFontChooseFont(HWND);
void PopFontSetFont(HWND);
void PopFontDeinitialize(void);

// POPPRINT.C 中的函数声明
BOOL PopPrntPrintFile(HINSTANCE, HWND, HWND, PTSTR);

static HWND hDlgModeless;
static TCHAR szAppName[] = TEXT("PopPad");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow) {
    MSG msg;
    HWND hwnd;
    HACCEL hAccel;
    WNDCLASS wndclass;

    // 注册窗口类
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(hInstance, szAppName);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = szAppName;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass)) {
        MessageBox(NULL, TEXT("该程序需要在Windows NT下运行!"),
            szAppName, MB_ICONERROR);
        return 0;
    }

    // 创建主窗口
    hwnd = CreateWindow(szAppName, NULL,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, szCmdLine);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    hAccel = LoadAccelerators(hInstance, szAppName);

    // 消息循环
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (hDlgModeless == NULL || !IsDialogMessage(hDlgModeless, &msg)) {
            if (!TranslateAccelerator(hwnd, hAccel, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return msg.wParam;
}

void DoCaption(HWND hwnd, TCHAR* szTitleName) {
    TCHAR szCaption[64 + MAX_PATH];

    wsprintf(szCaption, TEXT("%s - %s"), szAppName,
        szTitleName[0] ? szTitleName : UNTITLED);

    SetWindowText(hwnd, szCaption);
}

void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName) {
    TCHAR szBuffer[64 + MAX_PATH];

    wsprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED);

    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}

short AskAboutSave(HWND hwnd, TCHAR* szTitleName) {
    TCHAR szBuffer[64 + MAX_PATH];
    int iReturn;

    wsprintf(szBuffer, TEXT("是否保存当前更改到%s？"), szTitleName[0] ? szTitleName : UNTITLED);

    iReturn = MessageBox(hwnd, szBuffer, szAppName,
        MB_YESNOCANCEL | MB_ICONQUESTION);

    if (iReturn == IDYES) {
        if (!SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))
            iReturn = IDCANCEL;
    }

    return iReturn;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static BOOL bNeedSave = FALSE;
    static HINSTANCE hInst;
    static HWND hwndEdit;
    static int iOffset;
    static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH];
    static UINT messageFindReplace;
    int iSelBeg, iSelEnd, iEnable;
    LPFINDREPLACE pfr;

    switch (message) {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam)->hInstance;

        hwndEdit = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            WS_BORDER | ES_LEFT | ES_MULTILINE |
            ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0, 0, 0, 0,
            hwnd, (HMENU)EDITID, hInst, NULL);

        SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);

        PopFileInitialize(hwnd);
        PopFontInitialize(hwndEdit);

        messageFindReplace = RegisterWindowMessage(FINDMSGSTRING);

        DoCaption(hwnd, szTitleName);
        return 0;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        return 0;

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_INITMENUPOPUP:
        switch (lParam) {
        case 1:
            // 编辑菜单
            EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO,
                SendMessage(hwndEdit, EM_CANUNDO, 0, 0L) ?
                MF_ENABLED : MF_GRAYED);

            EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE,
                IsClipboardFormatAvailable(CF_TEXT) ?
                MF_ENABLED : MF_GRAYED);

            SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&iSelBeg,
                (LPARAM)&iSelEnd);

            iEnable = iSelBeg != iSelEnd ? MF_ENABLED : MF_GRAYED;

            EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, iEnable);
            break;

        case 2:
            // 查找菜单
            iEnable = hDlgModeless == NULL ?
                MF_ENABLED : MF_GRAYED;

            EnableMenuItem((HMENU)wParam, IDM_SEARCH_FIND, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_SEARCH_NEXT, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_SEARCH_REPLACE, iEnable);
            break;
        }
        return 0;

    case WM_COMMAND:

        if (lParam && LOWORD(wParam) == EDITID) {
            // 编辑控件的消息
            switch (HIWORD(wParam)) {
            case EN_UPDATE:
                // 文本已更改
                bNeedSave = TRUE;
                return 0;

            case EN_ERRSPACE:
            case EN_MAXTEXT:
                // 编辑控件空间不足
                MessageBox(hwnd, TEXT("编辑控件空间不足。"),
                    szAppName, MB_OK | MB_ICONSTOP);
                return 0;
            }
            break;
        }

        switch (LOWORD(wParam)) {
        case IDM_FILE_NEW:
            // 新建文件
            if (bNeedSave && IDCANCEL == AskAboutSave(hwnd, szTitleName))
                return 0;

            SetWindowText(hwndEdit, TEXT(""));
            szFileName[0] = '\0';
            szTitleName[0] = '\0';
            DoCaption(hwnd, szTitleName);
            bNeedSave = FALSE;
            return 0;

        case IDM_FILE_OPEN:
            // 打开文件
            if (bNeedSave && IDCANCEL == AskAboutSave(hwnd, szTitleName))
                return 0;

            if (PopFileOpenDlg(hwnd, szFileName, szTitleName)) {
                if (!PopFileRead(hwndEdit, szFileName)) {
                    OkMessage(hwnd, TEXT("无法读取文件 %s！"),
                        szTitleName);
                    szFileName[0] = '\0';
                    szTitleName[0] = '\0';
                }
            }

            DoCaption(hwnd, szTitleName);
            bNeedSave = FALSE;
            return 0;

        case IDM_FILE_SAVE:
            // 保存文件
            if (szFileName[0]) {
                if (PopFileWrite(hwndEdit, szFileName)) {
                    bNeedSave = FALSE;
                    return 1;
                }
                else {
                    OkMessage(hwnd, TEXT("无法写入文件 %s"),
                        szTitleName);
                    return 0;
                }
            }
            // 若未指定文件名则另存为
            // fall through
        case IDM_FILE_SAVE_AS:
            // 另存为
            if (PopFileSaveDlg(hwnd, szFileName, szTitleName)) {
                DoCaption(hwnd, szTitleName);

                if (PopFileWrite(hwndEdit, szFileName)) {
                    bNeedSave = FALSE;
                    return 1;
                }
                else {
                    OkMessage(hwnd, TEXT("无法写入文件 %s"),
                        szTitleName);
                    return 0;
                }
            }
            return 0;

        case IDM_FILE_PRINT:
            // 打印文件
            if (!PopPrntPrintFile(hInst, hwnd, hwndEdit, szTitleName))
                OkMessage(hwnd, TEXT("无法打印文件 %s"),
                    szTitleName);
            return 0;

        case IDM_APP_EXIT:
            // 退出应用
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;

        case IDM_EDIT_UNDO:
            // 撤销
            SendMessage(hwndEdit, WM_UNDO, 0, 0);
            return 0;

        case IDM_EDIT_CUT:
            // 剪切
            SendMessage(hwndEdit, WM_CUT, 0, 0);
            return 0;

        case IDM_EDIT_COPY:
            // 复制
            SendMessage(hwndEdit, WM_COPY, 0, 0);
            return 0;

        case IDM_EDIT_PASTE:
            // 粘贴
            SendMessage(hwndEdit, WM_PASTE, 0, 0);
            return 0;

        case IDM_EDIT_CLEAR:
            // 删除
            SendMessage(hwndEdit, WM_CLEAR, 0, 0);
            return 0;

        case IDM_EDIT_SELECT_ALL:
            // 全选
            SendMessage(hwndEdit, EM_SETSEL, 0, -1);
            return 0;

        case IDM_SEARCH_FIND:
            // 查找
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
            hDlgModeless = PopFindFindDlg(hwnd);
            return 0;

        case IDM_SEARCH_NEXT:
            // 查找下一个
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);

            if (PopFindValidFind())
                PopFindNextText(hwndEdit, &iOffset);
            else
                hDlgModeless = PopFindFindDlg(hwnd);

            return 0;

        case IDM_SEARCH_REPLACE:
            // 替换
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
            hDlgModeless = PopFindReplaceDlg(hwnd);
            return 0;

        case IDM_FORMAT_FONT:
            // 字体格式
            if (PopFontChooseFont(hwnd))
                PopFontSetFont(hwndEdit);

            return 0;

        case IDM_HELP:
            // 帮助
            OkMessage(hwnd, TEXT("帮助尚未实现！"),
                TEXT(""));
            return 0;

        case IDM_APP_ABOUT:
            // 关于
            DialogBox(hInst, TEXT("AboutBox"), hwnd, AboutDlgProc);
            return 0;
        }
        break;

    case WM_CLOSE:
        // 窗口关闭
        if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
            DestroyWindow(hwnd);

        return 0;

    case WM_QUERYENDSESSION:
        // 询问是否结束会话
        if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
            return 1;

        return 0;

    case WM_DESTROY:
        // 窗口销毁
        PopFontDeinitialize();
        PostQuitMessage(0);
        return 0;

    default:
        // 其他消息处理
        if (message == messageFindReplace) {
            pfr = (LPFINDREPLACE)lParam;

            if (pfr->Flags & FR_DIALOGTERM)
                hDlgModeless = NULL;

            if (pfr->Flags & FR_FINDNEXT) {
                if (!PopFindFindText(hwndEdit, &iOffset, pfr))
                    OkMessage(hwnd, TEXT("未找到文本！"),
                        TEXT(""));
            }

            if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL) {
                if (!PopFindReplaceText(hwndEdit, &iOffset, pfr))
                    OkMessage(hwnd, TEXT("未找到文本！"),
                        TEXT(""));
            }

            if (pfr->Flags & FR_REPLACEALL) {
                while (PopFindReplaceText(hwndEdit, &iOffset, pfr));
            }

            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}
