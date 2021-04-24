// NFS_NotePad.cpp : Определяет точку входа для приложения.
//
#pragma warning (disable: 6387)
#pragma comment (lib, "winmm.lib")
#define _CRT_SECURE_NO_WARNINGS
#define ID_MUSIC 2021

#include "framework.h"
#include "NFS_NotePad.h"


#include <time.h>
#include "CommCtrl.h"
#include <windows.h>
//#include "resource.h"
//#include "Resource.h"
#include <commdlg.h>
#include <stdio.h>
//#include <windowsx.h>
#include "playsoundapi.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// 0
HWND hWnd, hWndEdit, hWndStatus, hWndToolBar, hWndDlg;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

INT_PTR CALLBACK    Exit(HWND, UINT, WPARAM, LPARAM);
BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName);
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName);
DWORD WINAPI thread2(LPVOID t);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NFSNOTEPAD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NFSNOTEPAD));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(hWnd, hAccelTable, &msg) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDR_MENU_MY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect, rect2, rect3;
    LONG width, height;
    int userReply;

    static HBITMAP Image;
    BITMAP  bm;

    OPENFILENAME ofn;
    static char szFileName[MAX_PATH] = "";
    static int chars_count;
    static char fg_file = 0, fg_music = 0;

    switch (message)
    {
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        POINT ptMin = { 400, 200 };
        pInfo->ptMinTrackSize = ptMin;
        return 0;
    }

    case WM_CREATE:
    {
        //ShellExecute(hWnd, "open", "MySound.mp3", NULL, NULL, SW_SHOW);
        //hInst = ((LPCREATESTRUCT)lParam)->hInstance; // дескриптор приложения
      //  Image = LoadBitmap(hInst, MAKEINTRESOURCE(137));
        Image = (HBITMAP)LoadImage(NULL, "bitmap3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
       /* LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NFSNOTEPAD))*/
      //  GetObject(Image, sizeof(BITMAP), (LPSTR)&bm);
        HMODULE handle = GetModuleHandle(NULL);
        HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDB_BITMAP3), MAKEINTRESOURCE(2));
        HGLOBAL rcData = LoadResource(handle, rc);
        DWORD size = SizeofResource(handle, rc);
        if (Image == NULL)
        {
            MessageBox(NULL, "разраб ДОЛБОЁБ.", "Error", MB_OK | MB_ICONERROR);
            SendMessage(hWnd, WM_DESTROY, 0, 0);
        }

        hWndStatus = CreateWindow(STATUSCLASSNAME, "",
            WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | CCS_BOTTOM,
            0, 0, 0, 0, hWnd, (HMENU)999, hInst, NULL);
        int statwidths[] = { 300, 700,  -1 };

        SendMessage(hWndStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);

        SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)"Какой - то там язык");
        SendMessage(hWndStatus, SB_SETTEXT, 1, (LPARAM)"Здесь может быть ваша реклама!");
        SendMessage(hWndStatus, SB_SETTEXT, 2, (LPARAM)"Hi there :)");
        //GetClientRect(hWnd, &rect);
        //width = rect.right - rect.left;
        //height = rect.bottom - rect.top;
        hWndEdit = CreateWindow("EDIT", "Hello world!", WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL |
            ES_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)888, hInst, (LPVOID)0);

        TBADDBITMAP tbab;
        TBBUTTON tbb[8];

        hWndToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR)NULL, WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)1000,
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

        if (!hWndToolBar)
        {
            MessageBox(NULL, "Tool Bar Failed.", "Error", MB_OK | MB_ICONERROR);
            return 0;
        }

        SendMessage(hWndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
        SendMessage(hWndToolBar, TB_SETBITMAPSIZE, (WPARAM)0, (LPARAM)MAKELONG(20, 20));

        tbab.hInst = NULL;
        tbab.nID = 137;
        SendMessage(hWndToolBar, TB_ADDBITMAP, 1, (LPARAM)&tbab);

        ZeroMemory(tbb, sizeof(tbb));
        tbb[0].iBitmap = STD_FILEOPEN;
        tbb[0].idCommand = ID_OPEN;
        tbb[0].fsState = TBSTATE_ENABLED;
        tbb[0].fsStyle = TBSTYLE_BUTTON;

        tbb[1].iBitmap = STD_FILESAVE;
        tbb[1].idCommand = ID_SAVE;
        tbb[1].fsState = TBSTATE_ENABLED;
        tbb[1].fsStyle = TBSTYLE_BUTTON;

        tbb[2].iBitmap = 137;
        tbb[2].idCommand = ID_SAVE_AS;
        tbb[2].fsState = TBSTATE_ENABLED;
        tbb[2].fsStyle = TBSTYLE_BUTTON;

        tbb[3].iBitmap = STD_UNDO;
        tbb[3].idCommand = ID_CANCEL;
        tbb[3].fsState = TBSTATE_ENABLED;
        tbb[3].fsStyle = TBSTYLE_BUTTON;

        tbb[4].iBitmap = STD_CUT;
        tbb[4].idCommand = ID_CUT;
        tbb[4].fsState = TBSTATE_ENABLED;
        tbb[4].fsStyle = TBSTYLE_BUTTON;

        tbb[5].iBitmap = STD_COPY;
        tbb[5].idCommand = ID_COPY;
        tbb[5].fsState = TBSTATE_ENABLED;
        tbb[5].fsStyle = TBSTYLE_BUTTON;

        tbb[6].iBitmap = STD_PASTE;
        tbb[6].idCommand = ID_PASTE;
        tbb[6].fsState = TBSTATE_ENABLED;
        tbb[6].fsStyle = TBSTYLE_BUTTON;

        tbb[7].iBitmap = 135;
        tbb[7].idCommand = ID_MUSIC;
        tbb[7].fsState = TBSTATE_ENABLED;
        tbb[7].fsStyle = TBSTYLE_BUTTON;

        SendMessage(hWndToolBar, TB_ADDBUTTONS, 8, (LPARAM)&tbb);
        ShowWindow(hWndToolBar, SW_SHOW);
    }
        break;

    case WM_SIZE:
        SendMessage(hWndStatus, WM_SIZE, 0, 0);
        GetClientRect(hWnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
        GetClientRect(hWndStatus, &rect2);
        GetWindowRect(hWndToolBar, &rect3);
        MoveWindow(hWndEdit, 0, rect3.bottom - rect3.top, width, height - (rect2.bottom - rect2.top) - (rect3.bottom - rect3.top), FALSE);
        SendMessage(hWndToolBar, TB_AUTOSIZE, 0, 0);
        //SendMessage(hWndEdit, BN_KILLFOCUS, 0, 0);
        break;

    case WM_INPUTLANGCHANGEREQUEST:
        switch (LOWORD(GetKeyboardLayout(::GetCurrentThreadId())))
        {
        case 0x409:
            SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)"Английский");
            break;
        case 0x419:
            SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)"Русский");
            break;
        default:
            break;
        }
        break;

    case WM_COMMAND:
        {

            // Разобрать выбор в меню:
            switch (LOWORD(wParam))
            {

            case ID_OPEN:

                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                ofn.lpstrDefExt = "txt";

                if (GetOpenFileName(&ofn)) LoadTextFileToEdit(hWndEdit, ofn.lpstrFile);

                if (szFileName[0] != NULL) fg_file = 1;
                SendMessage(hWndStatus, SB_SETTEXT, 2, (LPARAM)szFileName);

                break;

            case ID_SAVE:
            {
                if (fg_file == 0)
                {
                    SendMessage(hWnd, WM_COMMAND, ID_SAVE_AS, 0);
                    return 0;
                }
                FILE* file;
                if ((file = fopen(szFileName, "w")) == NULL)
                {
                    MessageBox(NULL, "Не удалось открыть файл.", "Error", MB_OK | MB_ICONERROR);
                    return 0;
                }
                chars_count = GetWindowTextLengthA(hWndEdit);
                char* chars;
                chars = (char*)malloc(chars_count + 1);
                if (chars == (char*) 1)
                {
                    if ((file = fopen(szFileName, "w")) == NULL)
                    {
                        MessageBox(NULL, "Не удалось открыть файл.", "Error", MB_OK | MB_ICONERROR);
                        return 0;
                    }
                }
                else
                {
                    GetWindowText(hWndEdit, chars, chars_count + 1);
                    fprintf(file, "%s", chars);
                }

                free (chars);
                fclose(file);
            }
                break;

            case ID_SAVE_AS:
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                ofn.lpstrDefExt = "txt";

                if (GetSaveFileName(&ofn)) SaveTextFileFromEdit(hWndEdit, ofn.lpstrFile);

                if (szFileName[0] != NULL) fg_file = 1;
                SendMessage(hWndStatus, SB_SETTEXT, 2, (LPARAM)szFileName);

                break;

            case ID_CANCEL:
                SendMessage(hWndEdit, EM_UNDO, 0, 0);
                break;

            case ID_CUT:
                SendMessage(hWndEdit, WM_CUT, 0, 0);
                break;

            case ID_COPY:
                SendMessage(hWndEdit, WM_COPY, 0, 0);
                break;

            case ID_PASTE:
                SendMessage(hWndEdit, WM_PASTE, 0, 0);
                break;

            case ID_MUSIC:
            {
                HANDLE thread = CreateThread(NULL, 0, thread2, NULL, 0, NULL);
            }
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            case IDM_EXIT:
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_CLOSE:
        if(SendMessage(hWndEdit, EM_GETMODIFY, 0, 0) != 0)
        { 
           userReply = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Exit);
           if (userReply == 1) DestroyWindow(hWnd);
           if (userReply == 2)
           {
               SendMessage(hWnd, WM_COMMAND, ID_SAVE, 0);
               return 0;
           }
           if (userReply == 3) return 0;
        }
            DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        DeleteObject(Image);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        RECT r, r1;
        GetClientRect(hWnd, &r);
        GetClientRect(hDlg, &r1);
        SetWindowPos(hDlg, NULL, r.right / 2. - r1.right / 2., r.bottom / 2. - r1.bottom / 2., NULL, NULL, SWP_NOSIZE | SWP_SHOWWINDOW); // смещаем 2е  окно
        return (INT_PTR)TRUE;
    }
        

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Exit(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        RECT r, r1;
        GetClientRect(hWnd, &r);
        GetClientRect(hDlg, &r1);
        SetWindowPos(hDlg, NULL, r.right / 2. - r1.right / 2., r.bottom / 2. - r1.bottom / 2., NULL, NULL, SWP_NOSIZE | SWP_SHOWWINDOW); // смещаем 2е  окно
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            EndDialog(hDlg, 1);
            return (INT_PTR)TRUE;
            break;
        case IDCANCEL:
            EndDialog(hDlg, 3);
            return (INT_PTR)FALSE;
        break;
        case ID_SAVE:
            EndDialog(hDlg, 2);
            return (INT_PTR)TRUE;
        break;
        }
    }
    return (INT_PTR)FALSE;
}

//My function
BOOL LoadTextFileToEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize;

        dwFileSize = GetFileSize(hFile, NULL);
        if (dwFileSize != 0xFFFFFFFF)
        {
            LPSTR pszFileText;

            pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
            if (pszFileText != NULL)
            {
                DWORD dwRead;

                if (ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
                {
                    pszFileText[dwFileSize] = 0; // Add null terminator
                    if (SetWindowText(hEdit, pszFileText))
                        bSuccess = TRUE; // It worked!
                }
                GlobalFree(pszFileText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLength;

        dwTextLength = GetWindowTextLength(hEdit);
        // No need to bother if there's no text.
        if (dwTextLength > 0)
        {
            LPSTR pszText;
            DWORD dwBufferSize = dwTextLength + 1;

            pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
            if (pszText != NULL)
            {
                if (GetWindowText(hEdit, pszText, dwBufferSize))
                {
                    DWORD dwWritten;

                    if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

DWORD WINAPI thread2(LPVOID t)
{
    PlaySound((LPCSTR)"I_Am_Rock.wav", NULL, SND_FILENAME);
    return 0;
}