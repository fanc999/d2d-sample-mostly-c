#include "d2d-sample-helper.h"

/* sigh, we need this here, see http://blog.airesoft.co.uk/2014/12/direct2d-scene-of-the-accident/
 * for more details
 */
D2D1_SIZE_F
ID2D1HwndRenderTarget_GetSize (ID2D1HwndRenderTarget *render_target)
{
  D2D1_SIZE_F size = render_target->GetSize ();
  return size;
}