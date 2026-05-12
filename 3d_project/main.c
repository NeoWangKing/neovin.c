#include "../tools/platform.c"
#include <stdint.h>

uint32_t pixels[WIDTH*HEIGHT];
#define BACKGROUND_COLOR 0xFF181818

uint32_t *render(float dt)
{
    NVC_Canvas oc = NVC_CANVAS(pixels, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Background(oc, BACKGROUND_COLOR);
    return pixels;
}
