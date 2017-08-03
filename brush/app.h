#define UNICODE
#include <windows.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <functional>
#include <memory>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define CLASSNAME L"app"
#define WINDOWNAME L"Gdiplus::Brush Example"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define SAMPLE_HEIGHT 50

void UpdateScrollBar(HWND hwnd);
LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
int wWinMain(HINSTANCE i, HINSTANCE p, LPWSTR c, int n);

int totalPageHeight = 0;
int samplesLength = 0;

struct SAMPLE {
  const WCHAR* label;
  std::function<Gdiplus::Brush*(Gdiplus::Rect&)> createBrush;
  bool transparent;
  bool sizesensitive;
  std::shared_ptr<Gdiplus::Brush> cachedBrush;
  Gdiplus::Brush* getBrush(Gdiplus::Rect&);
  void draw(Gdiplus::Graphics&, Gdiplus::Rect&);
};

#include "samples.hpp"
