#define _WIN32_WINNT 0x0601
#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#ifdef __GNUC__
#define WIDL_EXPLICIT_AGGREGATE_RETURNS
#endif

#include <windows.h>
#include <d2d1.h>

#ifdef __cplusplus
extern "C" {
#endif

/* sigh, the C interface for this function is broken... */
#undef ID2D1HwndRenderTarget_GetSize

D2D1_SIZE_F
ID2D1HwndRenderTarget_GetSize (ID2D1HwndRenderTarget *render_target);

#ifdef __cplusplus
}
#endif
