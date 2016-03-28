// calculator.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "calculator.h"
#include "strsafe.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWndDisplayResult;                         // handle to the display result

wstring resultDisplay = L"";                    // text of the display result
int operand;                                    // the number to be used in the equation
int operation = 0;                              // operation selected by the user
int runningTotal = 0;                           // the running total is used as the left most equation value
bool fSetOperand = false;                       // state flag. used to indicate if user input is setting the running total or operand
bool fResetOperand = false;                     // state flag. indicate that the display number should be re-entered

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                PerformOperation(int);
void                NumericalInput(wstring);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CALCULATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CALCULATOR));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CALCULATOR));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CALCULATOR);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, 0, 230, 360, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_CREATE   - initialize all the buttons
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {

        // alexval_todo remove magic numbers here and make things realitive

        // ROW 1 - numerical display
        hWndDisplayResult = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"0", WS_CHILD | WS_VISIBLE | ES_READONLY,
            10, 10, 190, 20, hWnd, (HMENU)IDC_RESULT, GetModuleHandle(NULL), NULL);
        // SendMessage(hEdit,WM_SETTEXT,NULL,(LPARAM)"Insert text here..."); // update display text

        // ROW 2 - operation buttons
        CreateWindowEx(NULL, L"BUTTON", L"+", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 50, 40, 40, hWnd, (HMENU)IDC_ADD, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"-", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            60, 50, 40, 40, hWnd, (HMENU)IDC_SUB, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"*", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            110, 50, 40, 40, hWnd, (HMENU)IDC_MULT, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"/", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 50, 40, 40, hWnd, (HMENU)IDC_DIV, GetModuleHandle(NULL), NULL);

        // ROW 3 - numbers 7, 8 & 9
        CreateWindowEx(NULL, L"BUTTON", L"7", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 100, 40, 40, hWnd, (HMENU)IDC_7, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"8", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            60, 100, 40, 40, hWnd, (HMENU)IDC_8, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"9", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            110, 100, 40, 40, hWnd, (HMENU)IDC_9, GetModuleHandle(NULL), NULL);

        // ROW 4 - numbers 4, 5 & 6
        CreateWindowEx(NULL, L"BUTTON", L"4", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 150, 40, 40, hWnd, (HMENU)IDC_4, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"5", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            60, 150, 40, 40, hWnd, (HMENU)IDC_5, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"6", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            110, 150, 40, 40, hWnd, (HMENU)IDC_6, GetModuleHandle(NULL), NULL);

        // ROW 5 - numbers 1, 2 & 3
        CreateWindowEx(NULL, L"BUTTON", L"1", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 200, 40, 40, hWnd, (HMENU)IDC_1, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"2", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            60, 200, 40, 40, hWnd, (HMENU)IDC_2, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"3", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            110, 200, 40, 40, hWnd, (HMENU)IDC_3, GetModuleHandle(NULL), NULL);

        // ROW 6 - clear, number 0, and +/-
        CreateWindowEx(NULL, L"BUTTON", L"clear", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 250, 40, 40, hWnd, (HMENU)IDC_CLEAR, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"0", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            60, 250, 40, 40, hWnd, (HMENU)IDC_0, GetModuleHandle(NULL), NULL);
        CreateWindowEx(NULL, L"BUTTON", L"+/-", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            110, 250, 40, 40, hWnd, (HMENU)IDC_PLUS_MINUS, GetModuleHandle(NULL), NULL);

        CreateWindowEx(NULL, L"BUTTON", L"=", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            160, 100, 40, 140, hWnd, (HMENU)IDC_EQ, GetModuleHandle(NULL), NULL);
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_ADD:
        case IDC_SUB:
        case IDC_MULT:
        case IDC_DIV:
        case IDC_EQ:
        case IDC_PLUS_MINUS:
            PerformOperation(wmId);
            break;
        case IDC_0:
            NumericalInput(STR_0);
            break;
        case IDC_1:
            NumericalInput(STR_1);
            break;
        case IDC_2:
            NumericalInput(STR_2);
            break;
        case IDC_3:
            NumericalInput(STR_3);
            break;
        case IDC_4:
            NumericalInput(STR_4);
            break;
        case IDC_5:
            NumericalInput(STR_5);
            break;
        case IDC_6:
            NumericalInput(STR_6);
            break;
        case IDC_7:
            NumericalInput(STR_7);
            break;
        case IDC_8:
            NumericalInput(STR_8);
            break;
        case IDC_9:
            NumericalInput(STR_9);
            break;
        case IDC_CLEAR:
            fResetOperand = false;
            fSetOperand = false;
            operation = 0;
            runningTotal = 0;
            resultDisplay = L"";
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        // user pressed an operation button. Meaning we display the old value until new input is recieved for operand 2
        if (fResetOperand) {
            resultDisplay = to_wstring(runningTotal);
        }
        // user input is setting the value of the operand
        else if (fSetOperand) {
            operand = _wtoi(resultDisplay.c_str());
            resultDisplay = to_wstring(operand);
        }
        else {
            runningTotal = _wtoi(resultDisplay.c_str());
            resultDisplay = to_wstring(runningTotal);
        }

        // update the display text in all cases
        SendMessage(hWndDisplayResult, WM_SETTEXT, NULL, (LPARAM)resultDisplay.c_str());
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

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

void NumericalInput(wstring num) {
    if (fResetOperand) {
        resultDisplay = L"";
        fResetOperand = false;
    }
    resultDisplay.append(num);
}

void PerformOperation(int op) {
    // the plus minus operator is a special case.
    if (op == IDC_PLUS_MINUS) {
        runningTotal *= -1;
        return;
    }

    int displayNumber = _wtoi(resultDisplay.c_str()); // integer representation of what is in the edit box

    // no operation is set previously
    // set the operation to perform and save contents of the edit box as the running total AKA operand 1
    if (operation == 0) {
        operation = op;
        runningTotal = displayNumber;   // store the display number into the first operand
        fSetOperand = true;             // enter the state where we set the second operand
    }
    else {
        switch (operation) {
        case IDC_ADD:
            runningTotal += displayNumber;
            break;
        case IDC_SUB:
            runningTotal -= displayNumber;
            break;
        case IDC_MULT:
            runningTotal *= displayNumber;
            break;
        case IDC_DIV:
            runningTotal /= displayNumber;
            break;
        case IDC_EQ:
            operation = 0;
            break;
        }
        operation = op; // set operation for the next time around
    }

    fResetOperand = true;
}
