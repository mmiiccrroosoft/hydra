#include <windows.h>
#include "resource.h"
#include <cstdlib>
#include <ctime>
#include <commctrl.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='x86'\"")
#pragma comment(lib, "comctl32.lib")
// Global variables
const LPCWSTR HYDRA_CLASS = L"HydraHeadClass";
int g_headCount = 0;
HFONT g_hFont = NULL;  
HICON g_hIcon = NULL; // Hydra icon handle
const LPCWSTR OK_BUTTON_ID = L"OKButton"; // Button control ID

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateHydraHead(HINSTANCE hInstance);
void InitFontAndIcon(HINSTANCE hInstance);
void CreateOKButton(HWND hParentWnd); // New: Create OK button

// Initialize font and hydra icon
void InitFontAndIcon(HINSTANCE hInstance) {
    // Initialize font
    if (g_hFont) {
        DeleteObject(g_hFont);
    }
    g_hFont = (HFONT)GetStockObject(DEVICE_DEFAULT_FONT);

    // Load hydra icon from resources (only if not already loaded)
    if (!g_hIcon) {
        g_hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)); // Use your icon's resource ID
        // Fallback to system icon if hydra icon not found
        if (!g_hIcon) {
            g_hIcon = LoadIcon(NULL, IDI_APPLICATION);
        }
    }
}

// New: Create OK button (centered at bottom of window)
void CreateOKButton(HWND hParentWnd) {
    RECT clientRect;
    GetClientRect(hParentWnd, &clientRect);
    const int BUTTON_WIDTH = 80;
    const int BUTTON_HEIGHT = 30;

    // Calculate centered position (horizontal center, bottom margin 20px)
    int btnX = (clientRect.right - clientRect.left - BUTTON_WIDTH) / 2;
    int btnY = clientRect.bottom - BUTTON_HEIGHT - 20;

    // Create button control
    CreateWindowEx(
        0,
        L"BUTTON", // Predefined button class
        L"OK",     // Button text
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        btnX, btnY,
        BUTTON_WIDTH, BUTTON_HEIGHT,
        hParentWnd,
        (HMENU)1001, // Unique button ID (for message handling)
        (HINSTANCE)GetWindowLongPtr(hParentWnd, GWLP_HINSTANCE),
        NULL
    );
}
// Create new hydra head window
HWND CreateHydraHead(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int X_SIZE = 430;
    const int Y_SIZE = 150;

    int x = rand() % (screenWidth - X_SIZE);
    int y = rand() % (screenHeight - Y_SIZE);

    HWND hWnd = CreateWindowEx(
        0,
        HYDRA_CLASS,
        L"HYDRA",
        WS_BORDER | WS_CAPTION | WS_SYSMENU,
        x, y,
        X_SIZE, Y_SIZE,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWnd) {
        g_headCount++;
        CreateOKButton(hWnd); // Add OK button when window is created
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
    }
    return hWnd;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    RECT rect;
    GetClientRect(hWnd, &rect);

    // Draw hydra icon
    if (g_hIcon) {
        int iconWidth = GetSystemMetrics(SM_CXICON);
        int iconHeight = GetSystemMetrics(SM_CYICON);
        int iconX = 20; 
        // Move icon up - reduced top margin from 30 to 20
        int iconY = 20; 
        DrawIcon(hdc, iconX, iconY, g_hIcon);
    }

    // Draw text (centered horizontally, moved up)
    SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
    HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
    
    RECT textRect = rect;
    textRect.left += 20 + GetSystemMetrics(SM_CXICON);
    // Restrict text area to upper portion (above button area)
    textRect.bottom = rect.bottom - 50; // Increased from original to move up more
    
    // Ensure text is horizontally centered in its area and vertically aligned with icon
    DrawTextW(hdc, L"Cut off a head, two more will take its place.", -1, &textRect, 
             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    EndPaint(hWnd, &ps);
    return 0;
}

        // New: Handle OK button click (close window, same as WM_CLOSE behavior)
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1001) { // Match button ID
                HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
                CreateHydraHead(hInstance);
                CreateHydraHead(hInstance);
                DestroyWindow(hWnd);
            }
            break;
        }

        case WM_CLOSE: {
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            CreateHydraHead(hInstance);
            CreateHydraHead(hInstance);
            DestroyWindow(hWnd);
            return 0;
        }
					   case WM_ACTIVATE: {
            if (WA_ACTIVE == wParam || WA_CLICKACTIVE == wParam) {
                HWND hBtn = GetDlgItem(hWnd, 1001); 
                if (hBtn) SetFocus(hBtn); 
            }
            break;
        }

        case WM_CREATE: {
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            InitFontAndIcon(hInstance);
            return 0;
        }

        case WM_DESTROY: {
            g_headCount--;
            // Cleanup resources (only when last window closes)
            if (g_headCount == 0) {
                if (g_hFont) {
                    DeleteObject(g_hFont);
                    g_hFont = NULL;
                }
                if (g_hIcon) {
                    DestroyIcon(g_hIcon);
                    g_hIcon = NULL;
                }
                PostQuitMessage(0);
            }
            return 0;
        }

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // --------------- Added for Windows XP Compatibility ---------------
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES; // Initialize standard button controls
    InitCommonControlsEx(&icex);
    // ------------------------------------------------------------------

    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)); // Use hydra icon for window
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = HYDRA_CLASS;

    if (!RegisterClassEx(&wc)) {
        MessageBoxW(NULL, L"Window registration failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    srand((unsigned int)time(NULL));

    if (!CreateHydraHead(hInstance)) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}