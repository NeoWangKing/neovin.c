#ifndef NEOVIN_C_
#define NEOVIN_C_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

typedef struct {
    size_t rows;
    size_t cols;
    size_t stride;
    uint32_t *es;
} NVC_Canvas;

void swap_int(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void NVC_Fill(uint32_t *pixels, size_t width, size_t height, uint32_t color)
{
    for (size_t i = 0; i < width*height; ++i) {
        pixels[i] = color;
    }
}

void NVC_Fill_Rectangle(uint32_t *pixels, size_t pixels_width, size_t pixels_height, int x0, int y0, size_t w, size_t h, uint32_t color)
{
    for (int dy = 0; dy < (int)h; ++dy) {
        int y = y0 + dy;
        if (0 <= y && y < (int)pixels_height) {
            for (int dx = 0; dx < (int)w; ++dx) {
                int x = x0 + dx;
                if (0 <= x && x < (int)pixels_width) {
                    pixels[y*pixels_width + x] = color;
                }
            }
        }
    }
}

void NVC_Fill_Circle(uint32_t *pixels, size_t pixels_width, size_t pixels_height, int cx, int cy, float radius, uint32_t color)
{
    for (int dy = -(int)radius - 1; (float)dy <= radius; ++dy) {
        int y = cy + dy;
        if (0 <= y && y < (int)pixels_height) {
            for (int dx = -(int)radius - 1; (float)dx < radius; ++dx) {
                int x = cx + dx;
                if (0 <= x && x < (int)pixels_width) {
                    if ((float)dx*(float)dx + (float)dy*(float)dy <= radius*radius) {
                        pixels[y*pixels_width + x] = color;
                    }
                }
            }
        }
    }
}

void NVC_Draw_Line(uint32_t *pixels, size_t pixels_width, size_t pixels_height, int x1, int y1, int x2, int y2, uint32_t color)
{
    // (x - x1)(y2 - y1) - (y - y1)(x2 - x1) = 0
    // (y2 - y1)x + (x1 - x2)y + (y1 - y2)x1 + (x2 - x1)y1 = 0
    // (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0
    //
    // ((y1 - y2)x + (x2 - x1)y + x1y2 - x2y1)/(sqrt((y1 - y2)^2 + (x2 - x1)^2))

    if (x2 != x1) {
        float k = (float)(y2 - y1)/(float)(x2 - x1);

        if (-1.f <= k && k <= 1.f) {
            float c = (float)y1 - k*(float)x1;
            if (x1 > x2) swap_int(&x1, &x2);
            for (int x = x1; x <= x2; ++x) {
                if (0 <= x && x < (int)pixels_width) {
                    int y = (int)(k*(float)x + c);
                    if (0 <= y && y < (int)pixels_height) {
                        pixels[y*pixels_width + x] = color;
                    }

                }
            }
        } else {
            k = 1 / k;
            float c = (float)x1 - k*(float)y1;
            if (y1 > y2) swap_int(&y1, &y2);
            for (int y = y1; y <= y2; ++y) {
                if (0 <= y && y < (int)pixels_height) {
                    int x = (int)(k*(float)y + c);
                    if (0 <= x && x < (int)pixels_width) {
                        pixels[y*pixels_width + x] = color;
                    }

                }
            }
        }
    } else {
        int x = x1;
        if (0 < x && x < (int)pixels_width) {
            if (y1 > y2) swap_int(&y1, &y2);
            for (int y = y1; y <= y2; ++y) {
                if (0 <= y && y < (int)pixels_height) {
                    pixels[y*pixels_width + x] = color;
                }
            }
        }
    }
}

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer; } while (0)

Errno NVC_save_to_ppm_file(uint32_t *pixels, size_t width, size_t height, const char *file_path)
{
    int result = 0;
    FILE *f = NULL;

    {
        f = fopen(file_path, "wb");
        if (f == NULL) return_defer(errno);

        fprintf(f, "P6\n%zu %zu 255\n", width, height);
        if (ferror(f)) return_defer(errno);

        for (size_t i = 0; i < width*height; ++i) {
            // 0xAABBGGRR
            uint32_t pixel = pixels[i];
            uint8_t bytes[3] = {
                (pixel>>(8*0))&0xFF,
                (pixel>>(8*1))&0xFF,
                (pixel>>(8*2))&0xFF,
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            if (ferror(f)) return_defer(errno);
        }
    }

defer:
    if (f) fclose(f);
    return result;
}

#endif // NEOVIN_C_
