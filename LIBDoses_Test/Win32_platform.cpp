#ifndef UNICODE
#define UNICODE
#endif 

// Lib Includes

#include <windows.h>
#include <wingdi.h>
#include <thread>
#include <string>
#include "UIDoses.cpp"

using namespace std;

// Debug Variables

bool didClick;
int Clicks;

// Globals

bool running = true; //for main loop

HWND hwnd;

struct RenderInfo // IMPORTANT VARIABLES
{
    struct buffer
    {
        int width = 1000, height = 800;
        void* memory;
        int size = width * height * sizeof(unsigned int);
        BITMAPINFO bitmap_info;
    }buffer;
    HDC hdc;
    RECT window;
    bool render = false;

    int screenWidth()
    {
        GetClientRect(hwnd, &window);
        return window.right - window.left;
    }

    int screenHeight()
    {
        GetClientRect(hwnd, &window);
        return window.bottom - window.top;
    }

    int screenOriginX()
    {
        GetClientRect(hwnd, &window);
        return window.left;
    }

    int screenOriginY()
    {
        GetClientRect(hwnd, &window);
        return window.right;
    }
}RInfo;

struct MessageInfo
{
    struct Mouse
    {
        bool l = false;
        bool r = false;
        bool m = false;
        int cx = 0;
        int cy = 0;
    }Mouse;
}MInfo;

void Program();
void Reallocate();

// For windows message handling
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    switch (uMsg)
    {
    case WM_CLOSE:
    case WM_DESTROY: {
        running = false;
        //PostQuitMessage(0)
    } break;
    case WM_LBUTTONDOWN: {
        MInfo.Mouse.l = true;
        //didClick = true;
        Clicks++;
    }break;
    case WM_LBUTTONUP: {
        MInfo.Mouse.l = false;
    }break;
    /*
    case WM_PAINT:
    {        
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    */
    default:
    {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return result;
    }
}

// Entry Point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    //resolution settings
    int originx = (GetSystemMetrics(SM_CXSCREEN) - RInfo.buffer.width) / 2;
    int originy = (GetSystemMetrics(SM_CYSCREEN) - RInfo.buffer.height) / 2;

    RInfo.buffer.memory = VirtualAlloc(0, RInfo.buffer.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    hwnd = CreateWindowExW(0, CLASS_NAME, L"EverythingSuite 2021.a.0", WS_OVERLAPPEDWINDOW, originx, originy, RInfo.buffer.width, RInfo.buffer.height, NULL, NULL, hInstance, NULL);

    RInfo.hdc = GetDC(hwnd);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    thread ProgramLoop(Program); //Kicks off main program on a seperate thread (allows windows messages to be handled by main thread)

    while (running) // message loop
    {
        MSG msg = { };
        while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }        
    }

    ProgramLoop.join();

    return 0;
}

void Program()// main program---------------------------------------------------------
{
    POINT mouse_cords;
    long mx = 0;
    long my = 0;

    WINDOWPLACEMENT wp;

    //(required)
    UID_Input.mouseL = &MInfo.Mouse.l;
    UID_Input.mouseR = &MInfo.Mouse.r;
    UID_Input.mouseM = &MInfo.Mouse.m;
    UID_Input.mouseX = &MInfo.Mouse.cx;
    UID_Input.mouseY = &MInfo.Mouse.cy;

    
    UID_Rect Rect0(0, 200, 300, 200, 200, UID.Add_Palette({ 0xffffff, 0xff0000 }), nullptr);
    UID_Rect Rect1(0, 100, 100, 100, 80, UID.Add_Palette({ 0x00ff80 }), nullptr);
    //UID_Rect Ian1(0, 400, 300, 300, 50, UID.Add_Palette({ UID_PURPLE, UID_CYAN }), nullptr);
    //UID_Rect Ian2(0, 400, 50, 100, 500, UID.Add_Palette({ UID_ORANGE, UID_YELLOW }), nullptr);
    
    //UID_Curve curv0(0, 400, 400, 403, 500, UID.Add_Palette({0xffffff}), nullptr);
    //curv0.ptr->HoldOn.push_back(followMouse);

    
    Rect0.addHoldOn(followMouse);
    Rect0.addHoverOn(incPal);
    Rect0.addHoverOff(decPal);
    /*
    Rect1.addHoldOn(followMouse);

    Ian1.addHoldOn(followMouse);
    Ian1.addHoverOn(incPal);
    Ian1.addHoverOff(decPal);

    Ian2.addHoldOn(followMouse);
    Ian2.addHoverOn(incPal);
    Ian2.addHoverOff(decPal);
    */

    //float theta = 0;
    //curv0.ptr->line_weight = 1;
    while (running)
    {
        //write logic here

        //curv0.setPoint(1, 100 * cos(theta toRad) + 400, 100 * sin(theta toRad) + 400);
        //curv0.ptr->line_weight += .05;
        //theta+=.1;

        //curv0.movePoint(1, 1, 0);

        //get mouse input (required)
        if (GetCursorPos(&mouse_cords) && GetWindowPlacement(hwnd, &wp))
        {
            MInfo.Mouse.cx = mouse_cords.x - wp.rcNormalPosition.left - 8;
            MInfo.Mouse.cy = mouse_cords.y - wp.rcNormalPosition.top - 32;
        }

        //render
        if (RInfo.screenHeight() != RInfo.buffer.height || RInfo.screenWidth() != RInfo.buffer.width) { Reallocate(); }
        UID.Render(RInfo.buffer.memory, RInfo.buffer.height, RInfo.buffer.width);
        StretchDIBits(RInfo.hdc, 0, RInfo.buffer.height, RInfo.buffer.width, -RInfo.buffer.height - 1, 0, 0, RInfo.buffer.width, RInfo.buffer.height, RInfo.buffer.memory, &RInfo.buffer.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    }
}

void Reallocate()
{
    RInfo.buffer.size = RInfo.screenWidth() * RInfo.screenHeight() * sizeof(unsigned int);
    RInfo.buffer.width = RInfo.screenWidth();
    RInfo.buffer.height = RInfo.screenHeight();

    if (RInfo.buffer.memory) VirtualFree(RInfo.buffer.memory, 0, MEM_RELEASE);
    RInfo.buffer.memory = VirtualAlloc(0, RInfo.buffer.size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    RInfo.buffer.bitmap_info.bmiHeader.biSize = sizeof(RInfo.buffer.bitmap_info.bmiHeader);
    RInfo.buffer.bitmap_info.bmiHeader.biWidth = RInfo.buffer.width;
    RInfo.buffer.bitmap_info.bmiHeader.biHeight = RInfo.buffer.height;
    RInfo.buffer.bitmap_info.bmiHeader.biPlanes = 1;
    RInfo.buffer.bitmap_info.bmiHeader.biBitCount = 32;
    RInfo.buffer.bitmap_info.bmiHeader.biCompression = BI_RGB;
}