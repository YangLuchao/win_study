/*------------------------------------------------------------------------
 068.c 编程达人win32 API每日一练
     第68个例子：公用对话框-文本编辑器
     POPPAD.C
     POPFILE.C //文件菜单处理
     POPFIND.C //SEARCH菜单处理
     POPFONT.C //FONT菜单处理
     POPPRINTO.C  //打印处理
 (c) www.bcdaren.com 编程达人
-----------------------------------------------------------------------*/
#include <windows.h> 
#include <commdlg.h> //公用对话框库
#include "resource.h" 
#define EDITID 1    //顶级菜单ID：FILE 0 ,EDIT 1,SEARCH 2,FORMAT 3,HELP 4
#define UNTITLED TEXT ("(untitled)") 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

// Functions in POPFILE.C 
void PopFileInitialize(HWND); //初始化File对话框
BOOL PopFileOpenDlg(HWND, PTSTR, PTSTR); //创建“打开”文件对话框
BOOL PopFileSaveDlg(HWND, PTSTR, PTSTR); //创建“保存”文件对话框
BOOL PopFileRead(HWND, PTSTR); //读文件
BOOL PopFileWrite(HWND, PTSTR); //写文件
// Functions in POPFIND.C 
HWND PopFindFindDlg(HWND);       //初始化Find对话框
HWND PopFindReplaceDlg(HWND);    //初始化FindReplace对话框
BOOL PopFindFindText(HWND, int*, LPFINDREPLACE); //查找字符串
BOOL PopFindReplaceText(HWND, int*, LPFINDREPLACE); //替换字符串
BOOL PopFindNextText(HWND, int*); //查找下一处
BOOL PopFindValidFind(void); //无效查找
// Functions in POPFONT.C 
void PopFontInitialize(HWND); //初始化对话框字体
BOOL PopFontChooseFont(HWND); //选择字体
void PopFontSetFont(HWND);       //设置字体
void PopFontDeinitialize(void);  //删除字体
// Functions in POPPRNT.C 
BOOL PopPrntPrintFile(HINSTANCE, HWND, HWND, PTSTR); //打印文件
// Global variables 
static HWND hDlgModeless; //通用对话框句柄
static TCHAR szAppName[] = TEXT("PopPad");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
    MSG msg;
    HWND hwnd;
    HACCEL hAccel;
    WNDCLASS wndclass;

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

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"),
            szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName, NULL,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, szCmdLine);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);
    hAccel = LoadAccelerators(hInstance, szAppName);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (hDlgModeless == NULL || !IsDialogMessage(hDlgModeless, &msg))//对话框为空或不是对话框消息时
        {
            if (!TranslateAccelerator(hwnd, hAccel, &msg))//如果加速键消息未被翻译过TranslateAccelerator函数返回值为0
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return msg.wParam;
}
//设置标题
void DoCaption(HWND hwnd, TCHAR* szTitleName)
{
    TCHAR szCaption[64 + MAX_PATH];
    wsprintf(szCaption, TEXT("%s - %s"), szAppName,
        szTitleName[0] ? szTitleName : UNTITLED);
    SetWindowText(hwnd, szCaption);
}
//向用户显示消息框提示信息
void OkMessage(HWND hwnd, TCHAR* szMessage, TCHAR* szTitleName)
{
    TCHAR szBuffer[64 + MAX_PATH];
    wsprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName : UNTITLED);
    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
}
// 是否保存文件
short AskAboutSave(HWND hwnd, TCHAR* szTitleName)
{
    TCHAR szBuffer[64 + MAX_PATH];
    int iReturn;

    wsprintf(szBuffer, TEXT("Save current changes in %s?"),
        szTitleName[0] ? szTitleName : UNTITLED);

    iReturn = MessageBox(hwnd, szBuffer, szAppName,
        MB_YESNOCANCEL | MB_ICONQUESTION);
    if (iReturn == IDYES)
        if (!SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0))//IDM_FILE_SAVE返回0为失败，1成功
            iReturn = IDCANCEL;//YES但保存失败

    return iReturn;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
    lParam)
{
    static BOOL bNeedSave = FALSE;
    static HINSTANCE hInst;
    static HWND hwndEdit;
    static int iOffset;
    static TCHAR szFileName[MAX_PATH], szTitleName[MAX_PATH]; //完全路径文件名缓冲区和文件名缓冲区
    static UINT messageFindReplace;
    int iSelBeg, iSelEnd, iEnable;
    LPFINDREPLACE pfr;

    switch (message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam)->hInstance;
        // 创建编辑控件子窗口
        hwndEdit = CreateWindow(TEXT("edit"), NULL,
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
            WS_BORDER | ES_LEFT | ES_MULTILINE |
            ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL, //ES_NOHIDESEL反转所选文本
            0, 0, 0, 0,
            hwnd, (HMENU)EDITID, hInst, NULL);
        //设置文本编辑控件的最大字符数，0L为long类型常量0
        SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
        // 初始化公用对话框 
        PopFileInitialize(hwnd);
        PopFontInitialize(hwndEdit);  //创建字体
        //定义一个新的窗口消息，仅当多个应用程序必须处理同一条消息时才使用RegisterWindowMessage
         //“查找或替换”对话框会将 FINDMSGSTRING 注册的消息发送到其所有者窗口的窗口过程。
        messageFindReplace = RegisterWindowMessage(FINDMSGSTRING); //
        DoCaption(hwnd, szTitleName); //窗口标题栏显示文件名
        return 0;
    case WM_SETFOCUS:
        SetFocus(hwndEdit);//设置焦点
        return 0;

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;
        //当下拉菜单或子菜单即将变为活动状态时发送
    case WM_INITMENUPOPUP:
        switch (lParam)
        {
        case 1: // Edit 菜单 

            // Enable Undo if edit control can do it 启用“Undo”

            EnableMenuItem((HMENU)wParam, IDM_EDIT_UNDO, //启用，禁用或显示指定的菜单项
                SendMessage(hwndEdit, EM_CANUNDO, 0, 0L) ?
                MF_ENABLED : MF_GRAYED);

            // 如果文本在剪贴板中，则启用粘贴

            EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE,
                IsClipboardFormatAvailable(CF_TEXT) ?
                MF_ENABLED : MF_GRAYED);

            //  if text is selected 启用剪切、复制和删除选中的文本

            SendMessage(hwndEdit, EM_GETSEL, (WPARAM)&iSelBeg, //获取选定文本的位置信息
                (LPARAM)&iSelEnd);

            iEnable = iSelBeg != iSelEnd ? MF_ENABLED : MF_GRAYED; //如果开始位置等于结束位置，禁用菜单

            EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, iEnable);
            break;

        case 2: // Search menu 

            //如果非模态对话框==NULL时，激活菜单
            iEnable = hDlgModeless == NULL ? MF_ENABLED : MF_GRAYED;
            EnableMenuItem((HMENU)wParam, IDM_SEARCH_FIND, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_SEARCH_NEXT, iEnable);
            EnableMenuItem((HMENU)wParam, IDM_SEARCH_REPLACE, iEnable);
            break;
        }
        return 0;

    case WM_COMMAND:
        // 处理"EDIT"控件消息

        if (lParam && LOWORD(wParam) == EDITID)
        {
            switch (HIWORD(wParam)) //控件通知码
            {
            case EN_UPDATE: //当编辑控件即将重绘时发送。在控件格式化文本之后但在显示文本之前发送此通知代码。
                bNeedSave = TRUE;
                return 0;
            case EN_ERRSPACE: //空间不足
            case EN_MAXTEXT:
                MessageBox(hwnd, TEXT("Edit control out of space."),
                    szAppName, MB_OK | MB_ICONSTOP);
                return 0;
            }
            break;
        }

        switch (LOWORD(wParam)) //加速键ID或菜单ID，这里两个ID相等
        {
            //处理"FILE"菜单消息
        case IDM_FILE_NEW:
            //保存旧文件，如果保存时失败，则什么都不做，直接返回。
            if (bNeedSave && IDCANCEL == AskAboutSave
            (hwnd, szTitleName))
                return 0;
            //清除编辑框内容
            SetWindowText(hwndEdit, TEXT("\0"));//设置标题栏缓冲区为空
            szFileName[0] = '\0';//设置文件名缓冲区为空
            szTitleName[0] = '\0';//设置标题缓冲区为空
            DoCaption(hwnd, szTitleName);//显示标题栏
            bNeedSave = FALSE;
            return 0;

        case IDM_FILE_OPEN:
            //保存旧文件，如果保存时失败，则什么都不做，直接返回。
            if (bNeedSave && IDCANCEL == AskAboutSave(hwnd, szTitleName))
                return 0;
            if (PopFileOpenDlg(hwnd, szFileName, szTitleName))
            {
                if (!PopFileRead(hwndEdit, szFileName))
                {
                    OkMessage(hwnd, TEXT("Could not read file %s!"),
                        szTitleName);
                    szFileName[0] = '\0';
                    szTitleName[0] = '\0';
                }
            }
            DoCaption(hwnd, szTitleName);
            bNeedSave = FALSE;
            return 0;

        case IDM_FILE_SAVE:
            if (szFileName[0])
            {
                if (PopFileWrite(hwndEdit, szFileName))
                {
                    bNeedSave = FALSE;
                    return 1;
                }
                else
                {
                    OkMessage(hwnd, TEXT("Could not write file %s"),
                        szTitleName);
                    return 0;
                }
            }
            //fall through 
        case IDM_FILE_SAVE_AS:
            if (PopFileSaveDlg(hwnd, szFileName, szTitleName))
            {
                DoCaption(hwnd, szTitleName);

                if (PopFileWrite(hwndEdit, szFileName))
                {
                    bNeedSave = FALSE;
                    return 1;
                }
                else
                {
                    OkMessage(hwnd, TEXT("Could not write file %s"),
                        szTitleName);
                    return 0;
                }
            }
            return 0;
        case IDM_FILE_PRINT:
            if (!PopPrntPrintFile(hInst, hwnd, hwndEdit,
                szTitleName))
                OkMessage(hwnd, TEXT("Could not print file %s"),
                    szTitleName);
            return 0;

        case IDM_APP_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;

            // Messages from Edit menu 

        case IDM_EDIT_UNDO:
            SendMessage(hwndEdit, WM_UNDO, 0, 0);
            return 0;

        case IDM_EDIT_CUT:
            SendMessage(hwndEdit, WM_CUT, 0, 0);
            return 0;

        case IDM_EDIT_COPY:
            SendMessage(hwndEdit, WM_COPY, 0, 0);
            return 0;

        case IDM_EDIT_PASTE:
            SendMessage(hwndEdit, WM_PASTE, 0, 0);
            return 0;

        case IDM_EDIT_CLEAR:
            SendMessage(hwndEdit, WM_CLEAR, 0, 0);
            return 0;

        case IDM_EDIT_SELECT_ALL:
            SendMessage(hwndEdit, EM_SETSEL, 0, -1);
            return 0;

            // Messages from Search menu 
        case IDM_SEARCH_FIND:
            //查找将从iOffset位置开始（首获取选中文本后面的位置iOffset)
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
            hDlgModeless = PopFindFindDlg(hwnd);
            return 0;

        case IDM_SEARCH_NEXT:
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);

            if (PopFindValidFind())
                PopFindNextText(hwndEdit, &iOffset);
            else
                hDlgModeless = PopFindFindDlg(hwnd);

            return 0;

        case IDM_SEARCH_REPLACE:
            SendMessage(hwndEdit, EM_GETSEL, 0, (LPARAM)&iOffset);
            hDlgModeless = PopFindReplaceDlg(hwnd);
            return 0;

        case IDM_FORMAT_FONT:
            if (PopFontChooseFont(hwnd))
                PopFontSetFont(hwndEdit);

            return 0;

            // Messages from Help menu 

        case IDM_HELP:
            OkMessage(hwnd, TEXT("Help not yet implemented!"),
                TEXT("\0"));
            return 0;

        case IDM_APP_ABOUT:
            DialogBox(hInst, TEXT("AboutBox"), hwnd, AboutDlgProc);
            return 0;
        }
        break;
    case WM_CLOSE:
        if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
            DestroyWindow(hwnd);

        return 0;
        //结束会话或者系统关机发送的消息
    case WM_QUERYENDSESSION:
        if (!bNeedSave || IDCANCEL != AskAboutSave(hwnd, szTitleName))
            return 1;//结束

        return 0;//不结束

    case WM_DESTROY:
        PopFontDeinitialize();
        PostQuitMessage(0);
        return 0;

    default:
        //处理“查找”、“替换”发送的特殊消息 
        if (message == messageFindReplace)
        {
            pfr = (LPFINDREPLACE)lParam;
            //用户点击了“取消”按钮
            if (pfr->Flags & FR_DIALOGTERM)
                hDlgModeless = NULL;
            //用户点击了“查找下一个”按钮
            if (pfr->Flags & FR_FINDNEXT)
                if (!PopFindFindText(hwndEdit, &iOffset, pfr))
                    OkMessage(hwnd, TEXT("Text not found!"), TEXT("\0"));
            //用户点击了“替换”或“替换全部”
            if (pfr->Flags & FR_REPLACE || pfr->Flags & FR_REPLACEALL)
                if (!PopFindReplaceText(hwndEdit, &iOffset, pfr))
                    OkMessage(hwnd, TEXT("Text not found!"), TEXT("\0"));

            if (pfr->Flags & FR_REPLACEALL)
                while (PopFindReplaceText(hwndEdit, &iOffset, pfr));

            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}