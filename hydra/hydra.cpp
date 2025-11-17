#include <windows.h>
#include <cstdlib>
#include <ctime>

// Global variables
const LPCWSTR HYDRA_CLASS = L"HydraHeadClass";
int g_headCount = 0;

// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateHydraHead(HINSTANCE hInstance);

// Create new hydra head window
HWND CreateHydraHead(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int X_SIZE =500;
	const int Y_SIZE = 200;

    // Random position (stay on screen)
    int x = rand() % (screenWidth - X_SIZE);
    int y = rand() % (screenHeight - Y_SIZE);

    // Create window
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

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rect;
            GetClientRect(hWnd, &rect);
            DrawTextW(hdc, L"Cut off a head, two more will take its place.", -1, &rect, 
                     DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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

        case WM_DESTROY: {
            g_headCount--;
            if (g_headCount == 0) {
                PostQuitMessage(0);
            }
            return 0;
        }

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

// Correct WinMain signature for VS2008 + Windows XP (Unicode)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window class
    WNDCLASSEX wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = HYDRA_CLASS;

    if (!RegisterClassEx(&wc)) {
        MessageBoxW(NULL, L"Window registration failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    srand((unsigned int)time(NULL));

    // Create initial head
    if (!CreateHydraHead(hInstance)) {
        MessageBoxW(NULL, L"Window creation failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}