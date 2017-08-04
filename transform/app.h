#define UNICODE
#define _USE_MATH_DEFINES
#include <cmath>
#include <chrono>
#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define WINDOW_NAME L"Graphics::SetTransform example"
#define WINDOW_CLASS L"app"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TIMER_ELAPSE 25
#define TIMER_EVENT_ID 1
#define SHAFT_RPM 80

double shaftangle = 0;
double shaftrotation = 0;
HBITMAP backbmp = NULL;
std::chrono::time_point<std::chrono::steady_clock> starttime;

LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
int wWinMain(HINSTANCE, HINSTANCE, PWSTR, int);
