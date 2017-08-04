#define UNICODE
#define _USE_MATH_DEFINES
#include <chrono>
#include <memory>
#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define RGB_FOREGROUND 255, 255, 255
#define RGB_BACKGROUND 20, 20, 20
#define SHAFT_RPM 120
#define TIMER_ID 1
#define TIMER_INTERVAL 10
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_CLASS L"app"
#define WINDOW_TITLE L"InstalledFontCollection::GetFamilies Example"
#define EXAMPLE L"The quick brown fox jumps over lazy dog."

RECT clientRect;
HBITMAP backBitmap = NULL;
int clientWidth;
int clientHeight;
int fontFamilyCount;
std::unique_ptr<Gdiplus::FontFamily[]> fontFamilies;

std::chrono::time_point<std::chrono::steady_clock> startTime
  = std::chrono::steady_clock::now();
double shaftAngle = 0;
double shaftAngleRatio = 0;
double shaftRotation = 0;

LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
int wWinMain(HINSTANCE, HINSTANCE, PWSTR, int);
