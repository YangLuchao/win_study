/*--------------------------------------------------------------------------
 POPFILE.C -- Popup Editor File Functions
 ------------------------------------------------------------------------*/
#include <windows.h> 
#include <commdlg.h> 
static OPENFILENAME ofn;

//初始化对话框
void PopFileInitialize(HWND hwnd)
{
    static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0") \
        TEXT("ASCII Files (*.ASC)\0*.asc\0") \
        TEXT("All Files (*.*)\0*.*\0\0");

    ofn.lStructSize = sizeof(OPENFILENAME); //结构体的大小
    ofn.hwndOwner = hwnd; //所属窗口，可以为NULL
    ofn.hInstance = NULL; //对话框模板或模块句柄，ofn.Flags字段相关
    ofn.lpstrFilter = szFilter; //文件筛选字符串
    ofn.lpstrCustomFilter = NULL; //缓冲区，保留用户选择的过滤样式
    ofn.nMaxCustFilter = 0; //以TCHAR为单位的缓冲大小
    ofn.nFilterIndex = 0; //当前选择的过滤器的索引
    ofn.lpstrFile = NULL; // 在打开和关闭函数中设置，全路径文件名缓冲区
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL; // 在打开和关闭函数中设置，接收不含路径的文件名的缓冲区
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = NULL; //在这个字符串中指定初始目录
    ofn.lpstrTitle = NULL; //对话框标题，默认为“打开”或“保存”
    ofn.Flags = 0; //  在打开和关闭函数中设置，，对话框不同行为的标志，只读，多选，覆盖等
    ofn.nFileOffset = 0; //返回文件名字符串中，文件名的起始位置
    ofn.nFileExtension = 0; //扩展名在字符串中的起始位置
    ofn.lpstrDefExt = TEXT("txt"); //指定默认的扩展名
    ofn.lCustData = 0L; //系统传递给由lpfnHook成员标识的hook过程的应用程序定义的数据
    ofn.lpfnHook = NULL; //指向一个钩子程序
    ofn.lpTemplateName = NULL; //是对话框模板资源
}
//创建一个“打开”文件对话框
BOOL PopFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;//全路径文件名
    ofn.lpstrFileTitle = pstrTitleName;//文件名（含扩展名）
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;//隐藏只读复选框，如果文件不存在，则创建文件

    return GetOpenFileName(&ofn);//创建一个“打开”文件对话框，该对话框允许用户指定驱动器，目录以及要打开的文件或文件集的名称。
}
//创建一个“保存”文件对话框
BOOL PopFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.Flags = OFN_OVERWRITEPROMPT;//如果所选文件已存在，则使 “另存为”对话框生成一个消息框。用户必须确认是否覆盖文件。

    return GetSaveFileName(&ofn);//创建一个“保存”对话框，该对话框允许用户指定要保存的文件的驱动器，目录和名称。
}
//读取文件
BOOL PopFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE bySwap;
    DWORD dwBytesRead;
    HANDLE hFile;
    int i, iFileLength, iUniTest;
    PBYTE pBuffer, pText, pConv;
    // 打开文件 
    if (INVALID_HANDLE_VALUE ==
        (hFile = CreateFile(pstrFileName, GENERIC_READ,
            FILE_SHARE_READ,
            NULL, OPEN_EXISTING, 0, NULL)))
        return FALSE;
    // 获取文件大小，以字节为单位，并为读取分配内存。
    iFileLength = GetFileSize(hFile, NULL);
    pBuffer = malloc(iFileLength + 2); //多出两个字节存放两个'\0'
    //读文件到缓冲区，并在文件末尾放\0结束符 
    ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
    CloseHandle(hFile);
    pBuffer[iFileLength] = '\0';
    pBuffer[iFileLength + 1] = '\0';

    //测试文本是否是Unicode编码，前两个字节为0xFEFF或0xFFFE
    //IS_TEXT_UNICODE_SIGNATURE――0xFEFF（小端：高高低低）
    //IS_TEXT_UNICODE_REVERSE_SIGNATURE――0xFFFE（大端）
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
    {
        pText = pBuffer + 2;//跳过前两个字节，指向正文部分
        iFileLength -= 2;
        if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)//如果大端存储，调换字节顺序
        {
            for (i = 0; i < iFileLength / 2; i++)
            {
                bySwap = ((BYTE*)pText)[2 * i];
                ((BYTE*)pText)[2 * i] = ((BYTE*)pText)[2 * i + 1];
                ((BYTE*)pText)[2 * i + 1] = bySwap;
            }
        }
        //为可能的字符串转换分配内存
        pConv = malloc(iFileLength + 2);
        //Edit控件使用Unicode，则显示之前将Unicode文本转化为多字节文本 
#ifndef UNICODE 
        WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
            iFileLength + 2, NULL, NULL); //共iFileLength+2字节，含2个\0
        //Edit控件是非Unicode，则直接拷贝文本
#else 
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif 
    }
    else //非Unicode文件
    {
        pText = pBuffer;
        //ASCII转Unicode，需2倍的空间。额外加两个\0。 
        pConv = malloc(2 * iFileLength + 2);
        // 如果编辑控件是Unicode，则转换ASCII文本。 
#ifdef UNICODE 
        MultiByteToWideChar(CP_ACP, 0, pText, -1, (PTSTR)pConv,
            iFileLength + 1);
        // 如果不是，则直接拷贝
#else 
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif 
    }

    SetWindowText(hwndEdit, (PTSTR)pConv); //将内容写入到编辑框
    free(pBuffer);
    free(pConv);

    return TRUE;
}
//
BOOL PopFileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
    DWORD dwBytesWritten;
    HANDLE hFile;
    int iLength;
    PTSTR pstrBuffer;
    WORD wByteOrderMark = 0xFEFF; //小端模式时，前两个字节被写入FF FE。大端时被写入FE FF

    //打开文件，必要时可创建文件  
    if (INVALID_HANDLE_VALUE ==
        (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0,
            NULL, CREATE_ALWAYS, 0, NULL)))
        return FALSE;
    //获取编辑框中的字符个数，并分配内存 
    iLength = GetWindowTextLength(hwndEdit);
    pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));

    if (!pstrBuffer)
    {
        CloseHandle(hFile);
        return FALSE;
    }
    // 如果编辑框使用Unicode编码，则写入Unicode字节序 
#ifdef UNICODE 
    WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);
#endif 
    //获取编辑框中的文本，并输出到文件
    GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
    WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR),
        &dwBytesWritten, NULL);
    if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten)
    {
        CloseHandle(hFile);
        free(pstrBuffer);
        return FALSE;
    }

    CloseHandle(hFile);
    free(pstrBuffer);

    return TRUE;
}