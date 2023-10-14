#include "efifb.h"
#include "fb.h"
#include "libk.h"

fb_info_t __efifb_info;

fb_info_t *efifb_get_info(void) {
    return &__efifb_info;
}

void efifb_setpixel(int x, int y, uint32_t pixel) {
    g_handoff.fb_buffer[y * g_handoff.fb_pixelsPerScanLine + x] = pixel;
}

fb_driver_t efifb_driver;

void efifb_module_init() {
    efifb_driver.get_info = &efifb_get_info;
    efifb_driver.setpixel = &efifb_setpixel;

    __efifb_info.width = g_handoff.fb_width;
    __efifb_info.height = g_handoff.fb_height;
    for (int idx = 0; idx < g_handoff.fb_pixelsPerScanLine * g_handoff.fb_width; idx++)
        g_handoff.fb_buffer[idx] = 0;
    fb_register_driver(&efifb_driver);
}