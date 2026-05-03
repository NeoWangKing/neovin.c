#ifndef NEOVIN_C_
#define NEOVIN_C_

#include <stdint.h>

#define NVC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define NVC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define NVC_ABS(T, x) (NVC_SIGN(T, x)*(x))

typedef struct {
    int height;
    int width;
    int stride;
    uint32_t *data;
} NVC_Canvas;

// Remember to do:
//
// NVC_Canvas canvas = {
//     .width = 800,
//     .height = 600,
//     .stride = 800,
//     .data = (uint32_t*)malloc(sizeof(uint32_t) * 800 * 600)
// };

typedef struct {
    float x;
    float y;
} Vec2D;

float NVC_Vec2D_Length(Vec2D vec);
float NVC_Vec2D_Angle(Vec2D vec);
Vec2D NVC_Vec2D_Add(Vec2D vec1, Vec2D vec2);
Vec2D NVC_Vec2D_Subtract(Vec2D vec1, Vec2D vec2);
float NVC_Vec2D_Dot(Vec2D vec1, Vec2D vec2);
float NVC_Vec2D_Cross(Vec2D vec1, Vec2D vec2);

typedef enum {
    COMP_RED = 0,
    COMP_GREEN,
    COMP_BLUE,
    COMP_ALPHA,
    COUNT_COMP
} COLOR;

void Unpack_RGBA32(uint32_t color, uint8_t comp[COUNT_COMP]);
uint32_t Pack_RGBA32(uint8_t comp[COUNT_COMP]);
uint32_t NVC_Mix_Color_Alpha(uint32_t color_b, uint32_t color_t);
uint32_t NVC_Blend_Color_Normal(uint32_t color_b, uint32_t color_t);
uint32_t NVC_Blend_Color_Darken(uint32_t color_b, uint32_t color_t);
uint32_t NVC_Blend_Color_Multiply(uint32_t color_b, uint32_t color_t);

void NVC_Draw_Pixel(NVC_Canvas pixels, Vec2D location, uint32_t color);
void NVC_Fill_Background(NVC_Canvas pixels, uint32_t color);
void NVC_Fill_Rectangle(NVC_Canvas pixels, Vec2D position, Vec2D size, uint32_t color);
void NVC_Fill_Circle(NVC_Canvas pixels, Vec2D position, float radius, uint32_t color);
void NVC_Draw_Line(NVC_Canvas pixels, Vec2D p1, Vec2D p2, uint32_t color);
void NVC_Fill_Triangle(NVC_Canvas pixels, Vec2D p1, Vec2D p2, Vec2D p3, uint32_t color);

#endif // NEOVIN_C_

#ifdef NEOVIN_C_IMPLEMENTATION

#include <math.h>

float NVC_Vec2D_Length(Vec2D vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

float NVC_Vec2D_Angle(Vec2D vec)
{
    return atan2f(vec.y, vec.x);
}

Vec2D NVC_Vec2D_Add(Vec2D vec1, Vec2D vec2)
{
    return (Vec2D) { .x = vec1.x + vec2.x, .y = vec1.y + vec2.y };
}

Vec2D NVC_Vec2D_Subtract(Vec2D vec1, Vec2D vec2)
{
    return (Vec2D) { .x = vec1.x - vec2.x, .y = vec1.y - vec2.y };
}

float NVC_Vec2D_Dot(Vec2D vec1, Vec2D vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y);
}

float NVC_Vec2D_Cross(Vec2D vec1, Vec2D vec2)
{
    return (vec1.x*vec2.y - vec1.y*vec2.x);
}

// =============================================================================

void Unpack_RGBA32(uint32_t color, uint8_t comp[COUNT_COMP])
{
    for (int i = 0; i < COUNT_COMP; ++i) {
        comp[i] = color&0xFF;
        color >>= 8;
    }
}

uint32_t Pack_RGBA32(uint8_t comp[COUNT_COMP])
{
    uint32_t result = 0;
    for (int i = 0; i < COUNT_COMP; ++i) {
        result |= comp[i]<<(8*i);
    }
    return result;
}

uint32_t NVC_Mix_Color_Alpha(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = comp_t[COMP_ALPHA] + comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA])/255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        uint32_t sum = (uint32_t)comp_t[i]*comp_t[COMP_ALPHA]*255
                     + (uint32_t)comp_b[i]*comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA]);
        comp_f[i] = (uint8_t)(sum/ (255*comp_f[COMP_ALPHA]));
    }

    return Pack_RGBA32(comp_f);
}

uint32_t NVC_Blend_Color_Normal(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i];
    }

    return Pack_RGBA32(comp_f);
}

uint32_t NVC_Blend_Color_Darken(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i];
        if (comp_b[i] < comp_t[i]) comp_f[i] = comp_b[i];
    }

    return Pack_RGBA32(comp_f);
}

uint32_t NVC_Blend_Color_Multiply(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i] * comp_b[i] / 255;
    }

    return Pack_RGBA32(comp_f);
}

// =============================================================================

void NVC_Draw_Pixel(NVC_Canvas pixels, Vec2D location, uint32_t color)
{
    int x = (int)location.x;
    int y = (int)location.y;
    pixels.data[y*pixels.width + x] = NVC_Mix_Color_Alpha(pixels.data[y*pixels.width + x], color);
}

void NVC_Fill_Background(NVC_Canvas pixels, uint32_t color)
{
    for (int i = 0; i < pixels.width*pixels.height; ++i) {
        // pixels.data[i] = NVC_Mix_Color(pixels.data[i], color);
        pixels.data[i] = color;
    }
}
void NVC_Fill_Rectangle(NVC_Canvas pixels, Vec2D position, Vec2D size, uint32_t color)
{
    int w = (int)size.x;
    int h = (int)size.y;
    int sx = NVC_SIGN(int, w);
    int sy = NVC_SIGN(int, h);
    int dx = 0;
    int dy = 0;

    while (1) {
        int y = position.y + dy;
        int x = position.x + dx;
        if (0 <= x && x < pixels.width && 0 <= y && y < pixels.height) {
            NVC_Draw_Pixel(pixels, (Vec2D) { x, y }, color);
        }
        dx += sx;
        if (dx == w) { dx = 0; dy += sy; }
        if (dy == h) break;
    }
}

void NVC_Fill_Circle(NVC_Canvas pixels, Vec2D position, float radius, uint32_t color)
{
    for (int dy = -(int)radius - 1; (float)dy <= radius; ++dy) {
        int y = position.y + dy;
        if (0 <= y && y < pixels.height) {
            for (int dx = -(int)radius - 1; (float)dx < radius; ++dx) {
                int x = position.x + dx;
                if (0 <= x && x < pixels.width) {
                    if ((float)dx*(float)dx + (float)dy*(float)dy <= radius*radius) {
                        NVC_Draw_Pixel(pixels, (Vec2D) { x, y }, color);
                    }
                }
            }
        }
    }
}

void NVC_Draw_Line(NVC_Canvas pixels, Vec2D p1, Vec2D p2, uint32_t color)
{
    int x1 = (int)p1.x;
    int y1 = (int)p1.y;
    int x2 = (int)p2.x;
    int y2 = (int)p2.y;

    int dx = NVC_ABS(int, x2 - x1);
    int dy = -NVC_ABS(int, y2 - y1);
    int sx = NVC_SIGN(int, x2 - x1);
    int sy = NVC_SIGN(int, y2 - y1);
    int err = dx + dy;

    while (1) {
        // 只在画布范围内画点
        if (0 <= x1 && x1 < pixels.width && 0 <= y1 && y1 < pixels.height)
            NVC_Draw_Pixel(pixels, (Vec2D) { x1, y1 }, color);

        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void NVC_Fill_Triangle(NVC_Canvas pixels, Vec2D p1, Vec2D p2, Vec2D p3, uint32_t color)
{
    int x_max = (int)p1.x;
    int y_max = (int)p1.y;
    if (p2.x > x_max) x_max = p2.x;
    if (p2.y > y_max) y_max = p2.y;
    if (p3.x > x_max) x_max = p3.x;
    if (p3.y > y_max) y_max = p3.y;
    int x_min = (int)p1.x;
    int y_min = (int)p1.y;
    if (p2.x < x_min) x_min = p2.x;
    if (p2.y < y_min) y_min = p2.y;
    if (p3.x < x_min) x_min = p3.x;
    if (p3.y < y_min) y_min = p3.y;
    Vec2D v12 = NVC_Vec2D_Subtract(p2, p1);
    Vec2D v23 = NVC_Vec2D_Subtract(p3, p2);
    Vec2D v31 = NVC_Vec2D_Subtract(p1, p3);
    for (int y = y_min; y <= y_max; ++y) {
        for (int x = x_min; x <= x_max; ++x) {
            if (0 <= x && x < pixels.width && 0 <= y && y < pixels.height) {
                Vec2D v1 = NVC_Vec2D_Subtract((Vec2D) { x, y }, p1);
                Vec2D v2 = NVC_Vec2D_Subtract((Vec2D) { x, y }, p2);
                Vec2D v3 = NVC_Vec2D_Subtract((Vec2D) { x, y }, p3);
                if (NVC_SIGN(float, NVC_Vec2D_Cross(v1, v12)) == NVC_SIGN(float, NVC_Vec2D_Cross(v2, v23)) && NVC_SIGN(float, NVC_Vec2D_Cross(v1, v12)) == NVC_SIGN(float, NVC_Vec2D_Cross(v3, v31))) {
                    NVC_Draw_Pixel(pixels, (Vec2D) { x, y }, color);

                }
            }
        }
    }
}

#endif // NEOVIN_C_
