#include <windows.h>
#include "resource.h"
#include <cstdlib>
#include <ctime>
// Global variables
const LPCWSTR HYDRA_CLASS = L"HydraHeadClass";
int g_headCount = 0;
HFONT g_hFont = NULL;  
HICON g_hIcon = NULL; // Add global icon handle

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateHydraHead(HINSTANCE hInstance);
void InitFontAndIcon(HINSTANCE hInstance); // Combine init for font + icon

// Initialize font and system icon (runs once per window)
void InitFontAndIcon(HINSTANCE hInstance) {
    // Initialize font (existing logic)
    if (g_hFont) {
        DeleteObject(g_hFont);
    }
    g_hFont = (HFONT)GetStockObject(DEVICE_DEFAULT_FONT);

}

// Create new hydra head window (no changes)
HWND CreateHydraHead(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int X_SIZE = 500;
    const int Y_SIZE = 200;

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
        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
    }
    return hWnd;
}

// Window procedure (modified to draw icon)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rect;
            GetClientRect(hWnd, &rect);

            // Draw system icon (new)
            if (g_hIcon) {
                int iconWidth = GetSystemMetrics(SM_CXICON); // Standard icon width
                int iconHeight = GetSystemMetrics(SM_CYICON); // Standard icon height
                int iconX = 20; // Left margin for icon
                int iconY = (rect.bottom - iconHeight) / 2; // Vertically center icon
                DrawIcon(hdc, iconX, iconY, g_hIcon); // Draw icon at (iconX, iconY)
            }

            // Draw text (adjusted to avoid overlapping icon)
            SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
            HFONT hOldFont = (HFONT)SelectObject(hdc, g_hFont);
            
            // Narrow text rect to start after the icon (new: add left padding)
            RECT textRect = rect;
            textRect.left += 40 + GetSystemMetrics(SM_CXICON); // Icon width + extra spacing
            
            DrawTextW(hdc, L"Cut off a head, two more will take its place.", -1, &textRect, 
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(hdc, hOldFont); // Restore old font (fix: added missing font restore)
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_CLOSE: {
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            CreateHydraHead(hInstance);
            CreateHydraHead(hInstance);
            DestroyWindow(hWnd);
            return 0;
        }

        case WM_CREATE: {
            HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
            InitFontAndIcon(hInstance); // Call combined init (new)
            return 0;
        }

        case WM_DESTROY: {
            g_headCount--;
            // Cleanup font and icon (new: add icon destruction)
            if (g_hFont) {
                DeleteObject(g_hFont);
                g_hFont = NULL;
            }
            if (g_hIcon) {
                DestroyIcon(g_hIcon);
                g_hIcon = NULL;
            }
            if (g_headCount == 0) {
                PostQuitMessage(0);
            }
            return 0;
        }

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
	wc.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_ICON)) ; 
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