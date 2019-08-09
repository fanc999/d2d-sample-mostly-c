#include "d2d-sample-helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

/* later! */
/*#include <dwrite.h>*/
#include <wincodec.h>

typedef struct d2dobj
{
  HMODULE hmodule;
  HWND hwnd;
  ID2D1Factory *d2d_factory;
  ID2D1HwndRenderTarget *hwnd_render_target;
  ID2D1SolidColorBrush *gray_brush;
  ID2D1SolidColorBrush *blue_brush;
  BOOL hwnd_render_target_created;
  BOOL gray_brush_created;
  BOOL blue_brush_created;
} d2dobj;

HRESULT DemoApp_d2d_init (d2dobj *obj)
{
  HRESULT hr;
  D2D1_FACTORY_OPTIONS d2d_opt = {D2D1_DEBUG_LEVEL_NONE};

  hr = D2D1CreateFactory (D2D1_FACTORY_TYPE_MULTI_THREADED,
                          &IID_ID2D1Factory,
                          &d2d_opt,
                          &obj->d2d_factory);

  return hr;
}

static D2D1_COLOR_F
get_d2d_color_f_from_color (UINT32 in_color)
{
  const UINT32 sc_redShift   = 16;
  const UINT32 sc_greenShift = 8;
  const UINT32 sc_blueShift  = 0; 
  const UINT32 sc_redMask = 0xff << sc_redShift;
  const UINT32 sc_greenMask = 0xff << sc_greenShift;
  const UINT32 sc_blueMask = 0xff << sc_blueShift;

  D2D1_COLOR_F color;

  color.r = (float)((in_color & sc_redMask) >> sc_redShift) / 255.f;
  color.g = (float)((in_color & sc_greenMask) >> sc_greenShift) / 255.f;
  color.b = (float)((in_color & sc_blueMask) >> sc_blueShift) / 255.f;
  color.a = 1.0;

  return color;
}

HRESULT
DemoApp_CreateDeviceResources (d2dobj *obj)
{
  HRESULT hr = S_OK;
  RECT rect;
  D2D1_SIZE_U size;
  D2D1_RENDER_TARGET_PROPERTIES target_props;
  D2D1_HWND_RENDER_TARGET_PROPERTIES hwnd_target_props;
  const D2D1_PIXEL_FORMAT format = {DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_UNKNOWN};

  if (!obj->hwnd_render_target_created)
    {
      GetClientRect (obj->hwnd, &rect);

      size.width = rect.right - rect.left;
      size.height = rect.bottom - rect.top;

      target_props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
      target_props.pixelFormat = format;
      target_props.dpiX = 0.0;
      target_props.dpiY = 0.0;
      target_props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
      target_props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

      hwnd_target_props.hwnd = obj->hwnd;
      hwnd_target_props.pixelSize = size;
      hwnd_target_props.presentOptions = D2D1_PRESENT_OPTIONS_NONE;

      hr = ID2D1Factory_CreateHwndRenderTarget (obj->d2d_factory,
                                                &target_props,
                                                &hwnd_target_props,
                                                &obj->hwnd_render_target);
    }

  if (!obj->hwnd_render_target_created && SUCCEEDED (hr))
    obj->hwnd_render_target_created = TRUE;

  if (obj->hwnd_render_target_created && !obj->gray_brush_created)
    {
      /* Create gray brush: LightSlateGray = 0x778899 */
      D2D1_COLOR_F color = get_d2d_color_f_from_color (0x778899);

      hr = ID2D1HwndRenderTarget_CreateSolidColorBrush (obj->hwnd_render_target,
                                                        &color,
                                                        NULL, /* no brush properties */
                                                        &obj->gray_brush);
    }

  if (!obj->gray_brush_created && SUCCEEDED (hr))
    obj->gray_brush_created = TRUE;

  if (obj->hwnd_render_target_created && !obj->blue_brush_created)
    {
      /* Create blue brush: CornflowerBlue = 0x6495ED */
      D2D1_COLOR_F color = get_d2d_color_f_from_color (0x6495ED);

      hr = ID2D1HwndRenderTarget_CreateSolidColorBrush (obj->hwnd_render_target,
                                                        &color,
                                                        NULL, /* no brush properties */
                                                        &obj->blue_brush);
    }

  if (!obj->blue_brush_created && SUCCEEDED (hr))
    obj->blue_brush_created = TRUE;

  return hr;
}

void DemoApp_DiscardDeviceResources (d2dobj *obj)
{
  if (obj->blue_brush_created)
    {
      ID2D1SolidColorBrush_Release (obj->blue_brush);
      obj->blue_brush_created = FALSE;
    }

  if (obj->gray_brush_created)
    {
      ID2D1SolidColorBrush_Release (obj->gray_brush);
      obj->blue_brush_created = FALSE;
    }

  if (obj->hwnd_render_target_created)
    {
      ID2D1HwndRenderTarget_Release (obj->hwnd_render_target);
      obj->hwnd_render_target_created = FALSE;
    }
}

HRESULT DemoApp_OnRender (d2dobj *obj)
{
  HRESULT hr = S_OK;

  hr = DemoApp_CreateDeviceResources (obj);

  if (SUCCEEDED (hr))
    {
      /* White = 0xFFFFFF */
      D2D_COLOR_F white = get_d2d_color_f_from_color (0xFFFFFF);
      D2D_MATRIX_3X2_F identity;
      D2D1_SIZE_F rtSize;
      D2D1_RECT_F rect1, rect2;
      int width, height, x, y;

      identity._11 = 1.f;
      identity._12 = 0.f;
      identity._21 = 0.f;
      identity._22 = 1.f;
      identity._31 = 0.f;
      identity._32 = 0.f;

      ID2D1HwndRenderTarget_BeginDraw (obj->hwnd_render_target);
      ID2D1HwndRenderTarget_SetTransform (obj->hwnd_render_target, &identity);
      ID2D1HwndRenderTarget_Clear (obj->hwnd_render_target, &white);
      ID2D1HwndRenderTarget_CppGetSize (obj->hwnd_render_target, &rtSize); /* We need C++ to help us */

      /* Draw a grid background. */
      width = (int)rtSize.width;
      height = (int)rtSize.height;

      for (x = 0; x < width; x += 10)
        {
          D2D1_POINT_2F p1 = {(float)x, 0.0f};
          D2D1_POINT_2F p2 = {(float)x, rtSize.height};

          ID2D1HwndRenderTarget_DrawLine (obj->hwnd_render_target,
                                          p1,
                                          p2,
                                          obj->gray_brush,
                                          0.5f,
                                          NULL);
        }
      
      for (y = 0; y < height; y += 10)
        {
          D2D1_POINT_2F p1 = {0.0f, (float)y};
          D2D1_POINT_2F p2 = {rtSize.width, (float)y};

          ID2D1HwndRenderTarget_DrawLine (obj->hwnd_render_target,
                                          p1,
                                          p2,
                                          obj->blue_brush,
                                          0.5f,
                                          NULL);
        }

      rect1.left = rtSize.width/2 - 50.0f;
      rect1.top = rtSize.height/2 - 50.0f;
      rect1.right = rtSize.width/2 + 50.0f;
      rect1.bottom = rtSize.height/2 + 50.0f;

      rect2.left = rtSize.width/2 - 100.0f;
      rect2.top = rtSize.height/2 - 100.0f;
      rect2.right = rtSize.width/2 + 100.0f;
      rect2.bottom = rtSize.height/2 + 100.0f;

      ID2D1HwndRenderTarget_FillRectangle (obj->hwnd_render_target,
                                           &rect1,
                                           obj->gray_brush);
      ID2D1HwndRenderTarget_DrawRectangle (obj->hwnd_render_target,
                                           &rect2,
                                           obj->blue_brush,
                                           1.0f,
                                           NULL);
      hr = ID2D1HwndRenderTarget_EndDraw (obj->hwnd_render_target, NULL, NULL);
    }

  if (hr == D2DERR_RECREATE_TARGET)
    {
      hr = S_OK;
      DemoApp_DiscardDeviceResources (obj);
    }

  return hr;
}

void DemoApp_OnResize (d2dobj *obj, UINT width, UINT height)
{
  if (obj->hwnd_render_target_created)
    {
      D2D_SIZE_U size = {width, height};

      /* Note: This method can fail, but it's okay to ignore the
       * error here, because the error will be returned again
       * the next time EndDraw is called.
       */
      ID2D1HwndRenderTarget_Resize (obj->hwnd_render_target, &size);
    }
}

/* The windows procedure */
static LRESULT CALLBACK
DemoApp_WndProc (HWND hWnd,
                 UINT message,
                 WPARAM wParam,
                 LPARAM lParam)
{
  LRESULT result = 0;

  if (message == WM_CREATE)
    {
      LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
      d2dobj *obj = (d2dobj *)pcs->lpCreateParams;

      SetWindowLongPtrW (hWnd,
                         GWLP_USERDATA,
                         (LONG_PTR)obj);

      result = 1;
    }
  else
    {
      d2dobj *obj = (d2dobj *) GetWindowLongPtrW (hWnd, GWLP_USERDATA);
      BOOL wasHandled = FALSE;
      
      if (obj)
        {
          switch (message)
            {
              case WM_SIZE:
                /* LOWORD (lParam) = width, HIWORD (lParam) = height */
                DemoApp_OnResize (obj, LOWORD (lParam), HIWORD (lParam));

                result = 0;
                wasHandled = TRUE;
                break;

              case WM_DISPLAYCHANGE:
                InvalidateRect (hWnd, NULL, FALSE);

                result = 0;
                wasHandled = TRUE;
                break;

              case WM_PAINT:
                DemoApp_OnRender (obj);
                ValidateRect(hWnd, NULL);

                result = 0;
                wasHandled = TRUE;
                break;

              case WM_DESTROY:
                PostQuitMessage(0);

                result = 1;
                wasHandled = TRUE;
                break;
          }
        }

      if (!wasHandled)
        result = DefWindowProc (hWnd, message, wParam, lParam);
    }

  return result;
}

HRESULT DemoApp_init (d2dobj *obj)
{
  HRESULT hr;

  hr = DemoApp_d2d_init (obj);
  obj->hmodule = NULL;
  obj->hwnd = NULL;

  if (SUCCEEDED (hr))
    {
      FLOAT dpi_x, dpi_y;
      WNDCLASSEXW wcex;
      ATOM atom;

      GetModuleHandleExW (GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, 
                          NULL,
                          &obj->hmodule);

      memset (&wcex, 0, sizeof (WNDCLASSEXW));
      wcex.cbSize        = sizeof (wcex);
      wcex.style         = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc   = DemoApp_WndProc;
      wcex.cbClsExtra    = 0;
      wcex.cbWndExtra    = sizeof(LONG_PTR);
      wcex.hInstance     = obj->hmodule;
      wcex.hbrBackground = NULL;
      wcex.lpszMenuName  = NULL;
      wcex.hCursor       = LoadCursor(NULL, IDI_APPLICATION);
      wcex.lpszClassName = L"D2DDemoApp";

      atom = RegisterClassExW (&wcex);

      ID2D1Factory_GetDesktopDpi (obj->d2d_factory, &dpi_x, &dpi_y);

      obj->hwnd = CreateWindowExW (WS_EX_OVERLAPPEDWINDOW,
                                   L"D2DDemoApp",
                                   L"Direct2D Demo App",
                                   WS_OVERLAPPED,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   (UINT)(ceil (640.f * dpi_x / 96.f)),
                                   (UINT)(ceil (480.f * dpi_y / 96.f)),
                                   NULL,
                                   NULL,
                                   obj->hmodule,
                                   obj);

       hr = obj->hwnd ? S_OK : E_FAIL;
       if (SUCCEEDED (hr))
         {
           ShowWindow(obj->hwnd, SW_SHOWNORMAL);
           UpdateWindow(obj->hwnd);
         }
    }

  return hr;
}

void DemoApp_RunMessageLoop ()
{
  MSG msg;
 
  while (GetMessage (&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
}

int WINAPI wWinMain (HINSTANCE hinstance,
                     HINSTANCE pinstance,
                     LPWSTR    argv,
                     int       argc)
{
  d2dobj obj;
  obj.hwnd_render_target_created = FALSE;
  obj.gray_brush_created = FALSE;
  obj.blue_brush_created = FALSE;

  HeapSetInformation (NULL, HeapEnableTerminationOnCorruption, NULL, 0);

  if (SUCCEEDED (CoInitialize (NULL)))
    {
      if (SUCCEEDED (DemoApp_init (&obj)))
        DemoApp_RunMessageLoop ();
    }

  DestroyWindow (obj.hwnd);

  if (obj.hmodule != NULL)
    FreeLibrary (obj.hmodule);

  ID2D1Factory_Release (obj.d2d_factory);
  CoUninitialize ();

  return 0;
}