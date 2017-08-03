#include "app.h"

Gdiplus::Brush* SAMPLE::getBrush(Gdiplus::Rect& rt) {
  if (!sizesensitive && cachedBrush.use_count())
    return cachedBrush.get();
  cachedBrush.reset(createBrush(rt));
  return cachedBrush.get();
}
void SAMPLE::draw(Gdiplus::Graphics& g, Gdiplus::Rect& rt) {
  Gdiplus::SolidBrush textForegroundBrush(TEXT_FOREGROUND_COLOR);
  Gdiplus::SolidBrush textBackgroundBrush(TEXT_BACKGROUND_COLOR);
  Gdiplus::Font defaultFont(L"Consolas",
    SAMPLE_HEIGHT * 0.25f,
    Gdiplus::FontStyle::FontStyleRegular,
    Gdiplus::Unit::UnitPixel);
  Gdiplus::RectF rtf (rt.X, rt.Y, rt.Width, rt.Height);
  Gdiplus::RectF bounds;
  if (transparent)
    g.FillRectangle(&Gdiplus::SolidBrush(HATCH_BACKGROUND_COLOR), rt);
  g.FillRectangle(getBrush(rt), rt);
  g.MeasureString(label, -1, &defaultFont, rtf, &bounds);
  bounds.Offset(rt.Width / 10, (rt.Height - bounds.Height) / 2);
  g.FillRectangle(&textBackgroundBrush, bounds);
  g.DrawString(label, -1, &defaultFont, bounds, 
    Gdiplus::StringFormat::GenericDefault(),
    &textForegroundBrush);
}
void UpdateScrollBar(HWND hwnd) {
  if (!samplesLength)
    while (samples[samplesLength].label != NULL)
      samplesLength++;
  totalPageHeight = SAMPLE_HEIGHT * samplesLength;
  RECT clientRect;
  GetClientRect(hwnd, &clientRect);
  SCROLLINFO si;
  memset(&si, 0, sizeof(SCROLLINFO));
  si.cbSize = sizeof(SCROLLINFO);
  si.fMask = SIF_ALL;
  si.nMin = 0;
  si.nPage = clientRect.bottom - clientRect.top;
  si.nMax = si.nMin + totalPageHeight;// - si.nPage;
  int pos = GetScrollPos(hwnd, SB_VERT);
  SetScrollInfo(hwnd, SB_VERT, &si, FALSE);
  SetScrollPos(hwnd, SB_VERT, max(si.nMin, min(si.nMax, pos)), TRUE);
}
LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
  switch (msg) {
    case WM_CREATE:
      UpdateScrollBar(hwnd);
      return 0;
    case WM_SIZE:
      UpdateScrollBar(hwnd);
      InvalidateRect(hwnd, NULL, FALSE);
      return 0;
    case WM_MOUSEWHEEL: {
      SCROLLINFO si;
      memset(&si, 0, sizeof(SCROLLINFO));
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_ALL;
      GetScrollInfo(hwnd, SB_VERT, &si);
      int cur = GetScrollPos(hwnd, SB_VERT);
      cur -= floor(static_cast<double>(GET_WHEEL_DELTA_WPARAM(w)) / WHEEL_DELTA) * SAMPLE_HEIGHT;
      SetScrollPos(hwnd, SB_VERT, max(si.nMin, min(si.nMax, cur)), TRUE);
      InvalidateRect(hwnd, NULL, FALSE);
      } return 0;
    case WM_VSCROLL: {
      SCROLLINFO si;
      memset(&si, 0, sizeof(SCROLLINFO));
      si.cbSize = sizeof(SCROLLINFO);
      si.fMask = SIF_ALL;
      GetScrollInfo(hwnd, SB_VERT, &si);
      int cur = GetScrollPos(hwnd, SB_VERT);
      RECT clientRect;
      GetClientRect(hwnd, &clientRect);
      switch (LOWORD(w)) {
        case SB_BOTTOM: cur = si.nMax; break;
        case SB_ENDSCROLL: break;
        case SB_LINEDOWN: cur += SAMPLE_HEIGHT; break;
        case SB_LINEUP: cur -= SAMPLE_HEIGHT; break;
        case SB_PAGEDOWN: cur += clientRect.bottom - clientRect.top; break;
        case SB_PAGEUP: cur -= clientRect.bottom - clientRect.top; break;
        case SB_THUMBPOSITION: cur = si.nTrackPos; break; // HIWORD(w); break;
        case SB_THUMBTRACK: cur = si.nTrackPos; break; //  16bit barrier HIWORD(w); break;
        case SB_TOP: cur = si.nMin; break;
      }
      SetScrollPos(hwnd, SB_VERT, max(si.nMin, min(si.nMax, cur)), TRUE);
      InvalidateRect(hwnd, NULL, FALSE);
      } return 0;
    case WM_PAINT: {
      RECT clientRect;
      GetClientRect(hwnd, &clientRect);
      int cw = clientRect.right - clientRect.left;
      int ch = clientRect.bottom - clientRect.top;
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      HDC hbackdc = CreateCompatibleDC(hdc);
      HBITMAP hbackbmp = CreateCompatibleBitmap(hdc, cw, ch);
      HGDIOBJ hbackoldbmp = SelectObject(hbackdc, reinterpret_cast<HGDIOBJ>(hbackbmp));
      Gdiplus::Graphics g(hbackdc);
      int offsety = GetScrollPos(hwnd, SB_VERT);
      int skip = floor(static_cast<double>(offsety) / SAMPLE_HEIGHT);
      offsety %= SAMPLE_HEIGHT;
      Gdiplus::Rect r { clientRect.left, clientRect.top - offsety, cw, SAMPLE_HEIGHT };
      for (int c = skip; c < samplesLength && r.Y <= clientRect.bottom; c++, r.Y += SAMPLE_HEIGHT)
        samples[c].draw(g, r);
      BitBlt(hdc, 0, 0, cw, ch, hbackdc, 0, 0, SRCCOPY);
      EndPaint(hwnd, &ps);
      SelectObject(hbackdc, hbackoldbmp);
      DeleteObject(hbackbmp);
      DeleteDC(hbackdc);
      } return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
  }
  return DefWindowProc(hwnd, msg, w, l);
}
int wWinMain(HINSTANCE i, HINSTANCE p, LPWSTR c, int n) {
  Gdiplus::GdiplusStartupInput gdiplusInput;
  ULONG_PTR gdiplusToken;
  Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, NULL);

  WNDCLASSEX cls;
  memset(&cls, 0, sizeof(WNDCLASSEX));
  cls.cbSize = sizeof(WNDCLASSEX);
  cls.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc);
  cls.hInstance = i;
  cls.lpszClassName = CLASSNAME;
  ATOM atom = RegisterClassEx(&cls);
  if (!atom)
    return 1;

  HWND hwnd = CreateWindowEx(NULL,
    reinterpret_cast<LPCWSTR>(atom),
    WINDOWNAME, WS_OVERLAPPEDWINDOW,
    0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
    NULL, NULL, i, NULL);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG m; BOOL r;
  while ((r = GetMessage(&m, NULL, 0, 0)) != 0 && r != -1) {
    TranslateMessage(&m);
    DispatchMessage(&m);
  }
  return 0;
}
