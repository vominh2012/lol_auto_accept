#include "Windows.h"
#include <cstdio>

typedef unsigned char u8;
typedef unsigned int u32;

struct Vec2 {
    u32 x;
    u32 y;
};

struct RGB {
    u8 r;
    u8 g;
    u8 b;
};


bool is_match(HDC dc, Vec2 position, RGB color)
{
    COLORREF color_ref = GetPixel(dc, position.x, position.y);
    RGB rgb = {GetRValue(color_ref), GetGValue(color_ref), GetBValue(color_ref)};
    
    printf("Loc (%d, %d) Color(%d, %d, %d)\n", position.x, position.y, rgb.r, rgb.g, rgb.b);
    return memcmp(&rgb, &color, sizeof(color)) == 0;
}

void send_key(WORD vk_key)
{
    INPUT inputs[2] = { 0 };
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vk_key;
    inputs[0].ki.dwFlags = 0;
    
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk_key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}


void send_mouse_click(HWND hwnd, Vec2 position)
{
#if 1
    RECT lol_rect;
    GetWindowRect(hwnd, &lol_rect);
    MapWindowPoints(GetDesktopWindow(), GetParent(hwnd), (LPPOINT)&lol_rect, 2);
    
    POINT cursor_pos = {0};
    GetCursorPos(&cursor_pos);
    
    SetCursorPos(lol_rect.left + position.x, lol_rect.top + position.y);
    
    INPUT inputs[2] = { 0 };
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    
    Sleep(50);
    SetCursorPos(cursor_pos.x, cursor_pos.y);
#endif
#if 0
    PostMessage(hwnd,WM_LBUTTONDOWN,0,MAKELPARAM(position.x,position.y));
    Sleep(100);
    PostMessage(hwnd,WM_LBUTTONUP,0,MAKELPARAM(position.x,position.y));
#endif
}

bool match_click(HWND hwnd, Vec2 position, RGB color)
{
    HDC dc = GetDC(hwnd);
    bool match = is_match(dc, position, color);
    if (match)
    {
        printf("detect at (%d, %d) color(%d, %d, %d)\n", position.x, position.y, color.r, color.g, color.b);
        send_mouse_click(hwnd, position);
    }
    
    ReleaseDC(hwnd, dc);
    
    return match;
}


bool is_key_down(int key)
{
    return (GetKeyState(key) & 0x8000);
}

volatile bool running = true;
volatile bool pause = false;

DWORD WINAPI worker_proc(LPVOID param_ref)
{
    param_ref;
    
    while (running)
    {
        if (is_key_down(VK_CONTROL) && is_key_down(VK_SHIFT))
        {
            if (is_key_down(VK_ESCAPE))
            {
                running = false;
                printf("Exit\n");
                break;
            }
            if (is_key_down('R'))
            {
                printf("Resume\n");
                pause = false;
            }
            
            if (is_key_down('S')) 
            {
                printf("Pause\n");
                pause = true;
            }
        }
        
        Sleep(100);
    }
    
    return 0;
}

int main()
{
    HANDLE thread_handle = CreateThread(0,0, worker_proc, 0, 0,0);
    
    HWND hwnd = 0;
    
    
    while (running)
    {
        if (pause) 
        {
            Sleep(1000);
            continue;
        }
        
        if (hwnd == 0 || !IsWindow(hwnd)) {
            hwnd = FindWindowA(NULL, "League of Legends");
            if (hwnd)
            {
                printf("League found!\n");
            } 
        }
        
        Sleep(1000);
        
        if(hwnd && GetForegroundWindow() == hwnd)
        {
            // match button
            if (match_click(hwnd, { 430, 540 }, { 31, 76, 101 }))
            {
                continue;
            }
            
            // accept button
            if (match_click(hwnd, { 540, 440 }, { 167, 199, 199 }))
            {
                continue;
            }
            
            // continue button
            if (match_click(hwnd, { 430, 540 }, { 76, 129, 147 }))
            {
                continue;
            }
            
            // play again button
            if (match_click(hwnd, { 430, 540 }, { 210, 250, 237 }))
            {
                continue;
            }
            
        }
    }
    
    CloseHandle(thread_handle);
    
    return 0;
}