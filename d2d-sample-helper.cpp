#include "d2d-sample-helper.h"

void
ID2D1HwndRenderTarget_CppGetSize (ID2D1HwndRenderTarget *render_target,
                                  D2D1_SIZE_F           *size)
{
  *size = render_target->GetSize ();
}