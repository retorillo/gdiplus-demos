#include "app.h"

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  switch (msg) {
    case WM_CREATE: {
      Gdiplus::InstalledFontCollection f;
      fontFamilyCount = f.GetFamilyCount();
      fontFamilies.reset(new Gdiplus::FontFamily[fontFamilyCount]);
      f.GetFamilies(fontFamilyCount,
        reinterpret_cast<Gdiplus::FontFamily*>(fontFamilies.get()),
        &fontFamilyCount);
      } return 0;
    case WM_SIZE: {
      GetClientRect(hwnd, &clientRect);
      clientWidth = clientRect.right - clientRect.left;
      clientHeight = clientRect.bottom - clientRect.top;
      HDC hdc = GetDC(hwnd);
      if (backBitmap)
        DeleteObject(backBitmap);
      backBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
      ReleaseDC(hwnd, hdc);
      InvalidateRect(hwnd, NULL, FALSE);
      UpdateWindow(hwnd);
      } break;
    case WM_TIMER: {
      std::chrono::duration<double> elapsed
        = std::chrono::steady_clock::now() - startTime;
      shaftRotation = elapsed.count() / 60 * SHAFT_RPM;
      shaftAngleRatio = shaftRotation - trunc(shaftRotation);
      shaftAngle = shaftAngleRatio * M_PI * 2;
      InvalidateRect(hwnd, NULL, FALSE);
      UpdateWindow(hwnd);
      } break;
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      HDC hbdc = CreateCompatibleDC(hdc);
      HGDIOBJ hobmp = SelectObject(hbdc, backBitmap);
      Gdiplus::Graphics g(hbdc);
      g.Clear(Gdiplus::Color(RGB_BACKGROUND));
      WCHAR familyName[LF_FACESIZE];
      int fontIndex = static_cast<int>(floor(shaftRotation)) % fontFamilyCount;
      Gdiplus::FontFamily& fontFamily = fontFamilies.get()[fontIndex];
      fontFamily.GetFamilyName(familyName, LANG_NEUTRAL);
      double opacity = sin(shaftAngle / 2);
      double fontLargeHeight = 0.1 * min(clientWidth, clientHeight);
      double fontSmallHeight = 0.25 * min(clientWidth, clientHeight);
      Gdiplus::Font largeFont(&fontFamily, fontLargeHeight, 0, Gdiplus::Unit::UnitPixel);
      Gdiplus::Font smallFont(&fontFamily, fontSmallHeight, 0, Gdiplus::Unit::UnitPixel);
      Gdiplus::RectF crect(0, 0, clientWidth, clientHeight);
      Gdiplus::RectF textBounds;
      g.MeasureString(familyName, -1, &largeFont, crect, &textBounds);
      double entranceOffset = (clientWidth * 0.1);
      textBounds.X = entranceOffset - (1.0 - (cos(shaftAngle / 4) + 1) / 2) * entranceOffset;
      textBounds.Y += (crect.Height - textBounds.Height) / 2;
      g.DrawString(EXAMPLE, -1, &smallFont, crect,
        Gdiplus::StringFormat::GenericDefault(), 
        &Gdiplus::SolidBrush(Gdiplus::Color(round(opacity * 255 * 0.1),
          RGB_FOREGROUND)));
      g.DrawString(familyName, -1, &largeFont, textBounds,
        Gdiplus::StringFormat::GenericDefault(), 
        &Gdiplus::SolidBrush(Gdiplus::Color(round(opacity * 255),
          RGB_FOREGROUND)));
      BitBlt(hdc, 0, 0, clientWidth, clientHeight, hbdc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &ps);
      SelectObject(hbdc, hobmp);
      DeleteDC(hbdc);
      } break;
    case WM_DESTROY:
      if (backBitmap)
        DeleteObject(backBitmap);
      PostQuitMessage(0);
      break;
  }
  return DefWindowProc(hwnd, msg, w, l);
}

int wWinMain(HINSTANCE i, HINSTANCE p, PWSTR c, int s) {
  Gdiplus::GdiplusStartupInput gdiplusInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, NULL);
  WNDCLASSEX cls;
  memset(&cls, 0, sizeof(WNDCLASSEX));
  cls.cbSize = sizeof(WNDCLASSEX);
  cls.lpfnWndProc = WndProc;
  cls.hInstance = i;
  cls.lpszClassName = WINDOW_CLASS;
  ATOM atom = RegisterClassEx(&cls);
  if (!atom)
    return -1;
  HWND hwnd = CreateWindowEx(NULL, reinterpret_cast<LPWSTR>(atom),
    WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
    0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    NULL, NULL, i, NULL);
  ShowWindow(hwnd, s);
  ULONG_PTR timerId = SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, NULL);
  MSG m; int r;
  while ((r = GetMessage(&m, NULL, 0, 0)) != 0 && r != -1) {
    TranslateMessage(&m);
    DispatchMessage(&m);
  }
  KillTimer(hwnd, timerId);
  Gdiplus::GdiplusShutdown(gdiplusToken);
  return 0;
}
