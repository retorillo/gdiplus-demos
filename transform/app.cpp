#include "app.h"

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  switch (msg) {
    case WM_CREATE:
      starttime = std::chrono::steady_clock::now();
      return 0;
    case WM_SIZE: {
      if (backbmp)
        DeleteObject(backbmp);
      RECT cr;
      GetClientRect(hwnd, &cr);
      int cw = cr.right - cr.left;
      int ch = cr.bottom - cr.top;
      HDC hdc = GetDC(hwnd);
      backbmp = CreateCompatibleBitmap(hdc, cw, ch);
      ReleaseDC(hwnd, hdc);
      } break;
    case WM_PAINT:{
      RECT cr;
      GetClientRect(hwnd, &cr);
      int cw = cr.right - cr.left;
      int ch = cr.bottom - cr.top;
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      HDC hbdc = CreateCompatibleDC(hdc);
      HGDIOBJ hbbmp = SelectObject(hbdc, reinterpret_cast<HGDIOBJ>(backbmp));
      Gdiplus::Graphics g(hbdc);
      g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(80, 80, 80)), Gdiplus::Rect(cr.left, cr.top, cw, ch));
      double cosval = cos(shaftangle);
      double sinval = sin(shaftangle);
      Gdiplus::Matrix m(cosval, sinval, -sinval, cosval, 0, 0);
      Gdiplus::GraphicsContainer container = g.BeginContainer(
        Gdiplus::Rect(static_cast<double>(cw) / 2, static_cast<double>(ch) / 2, cw, ch),
        Gdiplus::Rect(0, 0, cw, ch),
        Gdiplus::Unit::UnitPixel);
      g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHighQuality);
      g.SetCompositingQuality(Gdiplus::CompositingQuality::CompositingQualityHighQuality);
      g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
      g.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
      g.SetTransform(&m);
      double squarew = static_cast<double>(min(cw, ch)) * 0.25;
      g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0)),
        Gdiplus::RectF(-squarew / 2, -squarew / 2, squarew, squarew));
      g.EndContainer(container);
      BitBlt(hdc, 0, 0, cw, ch, hbdc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &ps);
      SelectObject(hbdc, hbbmp);
      DeleteDC(hbdc);
      } return 0;
    case WM_TIMER: {
      std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - starttime;
      shaftrotation = (elapsed.count() / 60) * SHAFT_RPM;
      shaftangle = (shaftrotation - trunc(shaftrotation)) * M_PI * 2;
      InvalidateRect(hwnd, NULL, TRUE);
      UpdateWindow(hwnd);
      } return 0;
    case WM_DESTROY:
      if (backbmp)
        DeleteObject(backbmp);
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
  cls.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
  cls.hInstance = i;
  cls.lpszClassName = WINDOW_CLASS;

  ATOM atom = RegisterClassEx(&cls);
  if (!atom)
    return -1;

  HWND hwnd = CreateWindowEx(NULL, reinterpret_cast<LPCWSTR>(atom),
    WINDOW_NAME, WS_OVERLAPPEDWINDOW, 0, 0,
    WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, i, NULL);

  ShowWindow(hwnd, s);
  UINT_PTR timerid = SetTimer(hwnd, TIMER_EVENT_ID, TIMER_ELAPSE, NULL);

  MSG m; BOOL r;
  while ((r = GetMessage(&m, NULL, 0, 0)) != 0 && r != -1) {
    TranslateMessage(&m);
    DispatchMessage(&m);
  }
  KillTimer(hwnd, timerid);
  Gdiplus::GdiplusShutdown(gdiplusToken);
  return 0;
}
