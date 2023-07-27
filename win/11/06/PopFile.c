#include <windows.h>
#include <commdlg.h>

static OPENFILENAME ofn; // 定义文件对话框数据结构

// 初始化文件对话框数据结构
void PopFileInitialize(HWND hwnd) {
    static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0")  \
        TEXT("ASCII Files (*.ASC)\0*.asc\0") \
        TEXT("All Files (*.*)\0*.*\0\0");

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = NULL;
    ofn.lpstrFilter = szFilter; // 设置文件类型过滤器
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = NULL; // 设置文件名缓冲区
    ofn.nMaxFile = MAX_PATH; // 文件名缓冲区的最大长度
    ofn.lpstrFileTitle = NULL; // 设置文件标题缓冲区
    ofn.nMaxFileTitle = MAX_PATH; // 文件标题缓冲区的最大长度
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0; // 设置文件对话框标志
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = TEXT("txt"); // 设置默认文件扩展名
    ofn.lCustData = 0L;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
}

// 弹出文件打开对话框
BOOL PopFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName) {
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName; // 设置文件名缓冲区
    ofn.lpstrFileTitle = pstrTitleName; // 设置文件标题缓冲区
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT; // 设置文件对话框标志，隐藏只读和创建提示

    return GetOpenFileName(&ofn); // 打开文件对话框，返回是否成功选择文件
}

// 弹出文件保存对话框
BOOL PopFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName) {
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName; // 设置文件名缓冲区
    ofn.lpstrFileTitle = pstrTitleName; // 设置文件标题缓冲区
    ofn.Flags = OFN_OVERWRITEPROMPT; // 设置文件对话框标志，显示覆盖提示

    return GetSaveFileName(&ofn); // 打开文件对话框，返回是否成功选择文件
}

// 从文件中读取文本到编辑框
BOOL PopFileRead(HWND hwndEdit, PTSTR pstrFileName) {
    BYTE bySwap;
    DWORD dwBytesRead;
    HANDLE hFile;
    int i, iFileLength, iUniTest;
    PBYTE pBuffer, pText, pConv;

    if (INVALID_HANDLE_VALUE ==
        (hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, 0, NULL)))
        return FALSE;

    iFileLength = GetFileSize(hFile, NULL);
    pBuffer = malloc(iFileLength + 2);

    ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
    CloseHandle(hFile);
    pBuffer[iFileLength] = '\0';
    pBuffer[iFileLength + 1] = '\0';

    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;

    if (IsTextUnicode(pBuffer, iFileLength, &iUniTest)) {
        pText = pBuffer + 2;
        iFileLength -= 2;

        if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE) {
            for (i = 0; i < iFileLength / 2; i++) {
                bySwap = ((BYTE*)pText)[2 * i];
                ((BYTE*)pText)[2 * i] = ((BYTE*)pText)[2 * i + 1];
                ((BYTE*)pText)[2 * i + 1] = bySwap;
            }
        }

        pConv = malloc(iFileLength + 2);

#ifndef UNICODE
        // 如果编辑框不是Unicode，进行Unicode到多字节转换
        WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
            iFileLength + 2, NULL, NULL);
#else
        // 如果编辑框是Unicode，直接复制文本
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif

    }
    else {
        pText = pBuffer;

        pConv = malloc(2 * iFileLength + 2);

#ifdef UNICODE
        // 如果编辑框是Unicode，进行多字节到Unicode转换
        MultiByteToWideChar(CP_ACP, 0, pText, -1, (PTSTR)pConv,
            iFileLength + 1);
#else
        // 如果编辑框不是Unicode，直接复制文本
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
    }

    SetWindowText(hwndEdit, (PTSTR)pConv);
    free(pBuffer);
    free(pConv);

    return TRUE;
}

// 将编辑框中的文本写入文件
BOOL PopFileWrite(HWND hwndEdit, PTSTR pstrFileName) {
    DWORD dwBytesWritten;
    HANDLE hFile;
    int iLength;
    PTSTR pstrBuffer;
    WORD wByteOrderMark = 0xFEFF;

    if (INVALID_HANDLE_VALUE ==
        (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0,
            NULL, CREATE_ALWAYS, 0, NULL)))
        return FALSE;

    iLength = GetWindowTextLength(hwndEdit);
    pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));

    if (!pstrBuffer) {
        CloseHandle(hFile);
        return FALSE;
    }

#ifdef UNICODE
    // 如果编辑框是Unicode，写入字节顺序标记
    WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);
#endif

    GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
    WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR),
        &dwBytesWritten, NULL);

    if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten) {
        CloseHandle(hFile);
        free(pstrBuffer);
        return FALSE;
    }

    CloseHandle(hFile);
    free(pstrBuffer);

    return TRUE;
}
