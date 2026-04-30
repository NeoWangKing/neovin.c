#ifndef NEOVIN_C_
#define NEOVIN_C_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

typedef struct {
    size_t height;
    size_t width;
    size_t stride;
    uint32_t *data;
} NVC_Canvas;

typedef struct {
    int x;
    int y;
} Vec2D;

void swap_int(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void NVC_Fill(NVC_Canvas pixels, uint32_t color)
{
    for (size_t i = 0; i < pixels.width*pixels.height; ++i) {
        pixels.data[i] = color;
    }
}

void NVC_Fill_Rectangle(NVC_Canvas pixels, Vec2D position, Vec2D size, uint32_t color)
{
    for (int dy = 0; dy < (int)size.y; ++dy) {
        int y = position.y + dy;
        if (0 <= y && y < (int)pixels.height) {
            for (int dx = 0; dx < (int)size.x; ++dx) {
                int x = position.x + dx;
                if (0 <= x && x < (int)pixels.width) {
                    pixels.data[y*pixels.width + x] = color;
                }
            }
        }
    }
}

void NVC_Fill_Circle(NVC_Canvas pixels, Vec2D position, float radius, uint32_t color)
{
    for (int dy = -(int)radius - 1; (float)dy <= radius; ++dy) {
        int y = position.y + dy;
        if (0 <= y && y < (int)pixels.height) {
            for (int dx = -(int)radius - 1; (float)dx < radius; ++dx) {
                int x = position.x + dx;
                if (0 <= x && x < (int)pixels.width) {
                    if ((float)dx*(float)dx + (float)dy*(float)dy <= radius*radius) {
                        pixels.data[y*pixels.width + x] = color;
                    }
                }
            }
        }
    }
}

void NVC_Draw_Line(NVC_Canvas pixels, Vec2D point1, Vec2D point2, uint32_t color)
{
    // (x - x1)(y2 - y1) - (y - y1)(x2 - x1) = 0
    // (y2 - y1)x + (x1 - x2)y + (y1 - y2)x1 + (x2 - x1)y1 = 0
    // (y1 - y2)x + (x2 - x1)y + x1y2 - x2y1 = 0
    //
    // ((y1 - y2)x + (x2 - x1)y + x1y2 - x2y1)/(sqrt((y1 - y2)^2 + (x2 - x1)^2))

    if (point2.x != point1.x) {
        float k = (float)(point2.y - point1.y)/(float)(point2.x - point1.x);

        if (-1.f <= k && k <= 1.f) {
            float c = (float)point1.y - k*(float)point1.x;
            if (point1.x > point2.x) swap_int(&point1.x, &point2.x);
            for (int x = point1.x; x <= point2.x; ++x) {
                if (0 <= x && x < (int)pixels.width) {
                    int y = (int)(k*(float)x + c);
                    if (0 <= y && y < (int)pixels.height) {
                        pixels.data[y*pixels.width + x] = color;
                    }

                }
            }
        } else {
            k = 1 / k;
            float c = (float)point1.x - k*(float)point1.y;
            if (point1.y > point2.y) swap_int(&point1.y, &point2.y);
            for (int y = point1.y; y <= point2.y; ++y) {
                if (0 <= y && y < (int)pixels.height) {
                    int x = (int)(k*(float)y + c);
                    if (0 <= x && x < (int)pixels.width) {
                        pixels.data[y*pixels.width + x] = color;
                    }

                }
            }
        }
    } else {
        int x = point1.x;
        if (0 < x && x < (int)pixels.width) {
            if (point1.y > point2.y) swap_int(&point1.y, &point2.y);
            for (int y = point1.y; y <= point2.y; ++y) {
                if (0 <= y && y < (int)pixels.height) {
                    pixels.data[y*pixels.width + x] = color;
                }
            }
        }
    }
}

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer; } while (0)

Errno NVC_save_to_ppm_file(NVC_Canvas pixels, const char *file_path)
{
    int result = 0;
    FILE *f = NULL;

    {
        f = fopen(file_path, "wb");
        if (f == NULL) return_defer(errno);

        fprintf(f, "P6\n%zu %zu 255\n", pixels.width, pixels.height);
        if (ferror(f)) return_defer(errno);

        for (size_t i = 0; i < pixels.width*pixels.height; ++i) {
            // 0xAABBGGRR
            uint32_t pixel = pixels.data[i];
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
