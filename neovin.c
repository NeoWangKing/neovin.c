#ifndef NEOVIN_C_
#define NEOVIN_C_

#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#ifndef NEOVINCDEF
#define NEOVINCDEF static inline
#endif

// Anti-aliasing
#ifndef NVC_AA_RES
#define NVC_AA_RES 4
#endif
#define NVC_AA_PAD (1./NVC_AA_RES)

#define NVC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define NVC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define NVC_ABS(T, x) (NVC_SIGN(T, x)*(x))
#define Vec2D(x, y) ((Vec2D){ (float)(x), (float)(y) })

typedef struct {
    int height;
    int width;
    int stride;
    uint32_t *pixels;
} NVC_Canvas;

#define NVC_CANVAS_NULL ((NVC_Canvas) {0})
#define NVC_PIXEL(oc, x, y) (oc).pixels[(y)*(oc).stride + (x)]

NEOVINCDEF NVC_Canvas NVC_Make_Canvas(uint32_t *pixels, int width, int height)
{
    NVC_Canvas oc = {
        .pixels = pixels,
        .width = width,
        .height = height,
        .stride = width,
    };
    return oc;
}

typedef struct {
    float x;
    float y;
} Vec2D;

NEOVINCDEF bool NVC_Normalize_Range(NVC_Canvas oc, Vec2D position, Vec2D size, int *x1, int *x2, int *y1, int *y2)
{
    *x1 = (int)position.x;
    *y1 = (int)position.y;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + NVC_SIGN(float, size.x)*(NVC_ABS(float, size.x) - 1);
    if (*x1 > *x2) NVC_SWAP(int, *x1, *x2);
    *y2 = *y1 + NVC_SIGN(float, size.y)*(NVC_ABS(float, size.y) - 1);
    if (*y1 > *y2) NVC_SWAP(int, *y1, *y2);

    // Cull out invisible rectangle
    if (*x1 >= (int) oc.width) return false;
    if (*x2 < 0) return false;
    if (*y1 >= (int) oc.height) return false;
    if (*y2 < 0) return false;

    // Clamp the rectangle to the boundaries
    if (*x1 < 0) *x1 = 0;
    if (*x2 >= (int) oc.width) *x2 = (int) oc.width - 1;
    if (*y1 < 0) *y1 = 0;
    if (*y2 >= (int) oc.height) *y2 = (int) oc.height - 1;

    return true;
}

NEOVINCDEF void NVC_GetSubCanvas(NVC_Canvas *dst, NVC_Canvas src, Vec2D pos, Vec2D size)
{
    // 处理负尺寸，翻转方向
    if (size.x < 0) { pos.x += size.x; size.x = -size.x; }
    if (size.y < 0) { pos.y += size.y; size.y = -size.y; }

    // 裁剪到源画布内
    int x0 = (int)pos.x;
    int y0 = (int)pos.y;
    int x1 = x0 + (int)size.x;
    int y1 = y0 + (int)size.y;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > src.width)  x1 = src.width;
    if (y1 > src.height) y1 = src.height;

    dst->width  = x1 - x0;
    dst->height = y1 - y0;
    dst->stride = src.stride;          // 保持源画布的步幅
    dst->pixels   = &src.pixels[y0 * src.stride + x0];
}

NEOVINCDEF float NVC_Vec2D_Length(Vec2D vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

NEOVINCDEF float NVC_Vec2D_Angle(Vec2D vec)
{
    return atan2f(vec.y, vec.x);
}

NEOVINCDEF Vec2D NVC_Vec2D_Plus(Vec2D vec1, Vec2D vec2)
{
    return Vec2D(vec1.x + vec2.x, vec1.y + vec2.y);
}

NEOVINCDEF Vec2D NVC_Vec2D_Minus(Vec2D vec1, Vec2D vec2)
{
    return Vec2D(vec1.x - vec2.x, vec1.y - vec2.y);
}

NEOVINCDEF float NVC_Vec2D_Dot(Vec2D vec1, Vec2D vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y);
}

NEOVINCDEF float NVC_Vec2D_Cross(Vec2D vec1, Vec2D vec2)
{
    return (vec1.x*vec2.y - vec1.y*vec2.x);
}

typedef enum {
    COMP_RED = 0,
    COMP_GREEN,
    COMP_BLUE,
    COMP_ALPHA,
    COUNT_COMP
} COLOR;

NEOVINCDEF void Unpack_RGBA32(uint32_t color, uint8_t comp[COUNT_COMP])
{
    for (int i = 0; i < COUNT_COMP; ++i) {
        comp[i] = color&0xFF;
        color >>= 8;
    }
}

NEOVINCDEF uint32_t Pack_RGBA32(uint8_t comp[COUNT_COMP])
{
    uint32_t result = 0;
    for (int i = 0; i < COUNT_COMP; ++i) {
        result |= comp[i]<<(8*i);
    }
    return result;
}

NEOVINCDEF void Modi_Color(uint32_t *color, COLOR index, uint8_t value)
{
    uint8_t comp[COUNT_COMP];
    Unpack_RGBA32(*color, comp);
    comp[index] = value;
    *color = Pack_RGBA32(comp);
}

NEOVINCDEF void Transparent_Color(uint32_t *color, float alpha)
{
    uint8_t comp[COUNT_COMP];
    Unpack_RGBA32(*color, comp);
    comp[COMP_ALPHA] *= alpha;
    *color = Pack_RGBA32(comp);
}

NEOVINCDEF uint32_t NVC_Mix_Color_Alpha(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = comp_t[COMP_ALPHA] + comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA])/255;
    if (comp_f[COMP_ALPHA] == 0) return 0x00000000;

    for (int i = 0; i < COMP_ALPHA; ++i) {
        uint32_t sum = (uint32_t)comp_t[i]*comp_t[COMP_ALPHA]*255
                     + (uint32_t)comp_b[i]*comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA]);
        comp_f[i] = (uint8_t)(sum/ (255*comp_f[COMP_ALPHA]));
    }

    return Pack_RGBA32(comp_f);
}

NEOVINCDEF uint32_t NVC_Blend_Color_Normal(uint32_t color_b, uint32_t color_t)
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

NEOVINCDEF uint32_t NVC_Blend_Color_Darken(uint32_t color_b, uint32_t color_t)
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

NEOVINCDEF uint32_t NVC_Blend_Color_Multiply(uint32_t color_b, uint32_t color_t)
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

NEOVINCDEF void NVC_Set_Pixel(NVC_Canvas oc, Vec2D p, uint32_t color)
{
    int x = (int)p.x;
    int y = (int)p.y;
    if (x < 0 || x >= oc.width) return;
    if (y < 0 || y >= oc.height) return;
    NVC_PIXEL(oc, x, y) = color;
}

NEOVINCDEF void NVC_Draw_Pixel(NVC_Canvas oc, Vec2D p, uint32_t color)
{
    int x = (int)p.x;
    int y = (int)p.y;
    if (x < 0 || x >= oc.width) return;
    if (y < 0 || y >= oc.height) return;
    NVC_PIXEL(oc, x, y) = NVC_Mix_Color_Alpha(NVC_PIXEL(oc, x, y), color);
}

NEOVINCDEF void NVC_Set_Background(NVC_Canvas oc, uint32_t color)
{
    for (int y = 0; y < oc.height; ++y) {
        for (int x = 0; x < oc.width; ++x) {
            NVC_Set_Pixel(oc, Vec2D(x, y), color);
        }
    }
}

NEOVINCDEF void NVC_Fill_Background(NVC_Canvas oc, uint32_t color)
{
    for (int y = 0; y < oc.height; ++y) {
        for (int x = 0; x < oc.width; ++x) {
            NVC_Draw_Pixel(oc, Vec2D(x, y), color);
        }
    }
}

NEOVINCDEF void NVC_Fill_Rectangle(NVC_Canvas oc, Vec2D p, Vec2D s, uint32_t color)
{
    float x_min = p.x;
    float x_max = x_min + s.x;
    if (x_min > x_max) NVC_SWAP(float, x_min, x_max);
    float y_min = p.y;
    float y_max = y_min + s.y;
    if (y_min > y_max) NVC_SWAP(float, y_min, y_max);

    int x1, y1, x2, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);

    const float d = 0.5f;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            if (y > y_min + d && y < y_max - d && x > x_min + d && x < x_max - d) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (ay >= y_min && ay <= y_max && ax >= x_min && ax <= x_max) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Rectangle(NVC_Canvas oc, Vec2D p, Vec2D s, float thick, uint32_t color)
{
    float x_min = p.x;
    float x_max = x_min + s.x;
    if (x_min > x_max) NVC_SWAP(float, x_min, x_max);
    float y_min = p.y;
    float y_max = y_min + s.y;
    if (y_min > y_max) NVC_SWAP(float, y_min, y_max);
    x_min -= thick/2;
    y_min -= thick/2;
    x_max += thick/2;
    y_max += thick/2;

    int x1, y1, x2, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);

    const float d = 0.5f;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            if (y > y_min + d && y < y_max - d && x > x_min + d && x < x_max - d) {
                if (y < y_min + thick - d || y > y_max - thick + d || x < x_min + thick - d || x > x_max - thick + d) {
                    NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                    continue;
                }
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (ay >= y_min && ay <= y_max && ax >= x_min && ax <= x_max) {
                        if (ay <= y_min + thick || ay >= y_max - thick || ax <= x_min + thick || ax >= x_max - thick) count += 1;
                    }
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Fill_Circle(NVC_Canvas oc, Vec2D p, float r, uint32_t color)
{
    r = NVC_ABS(float ,r);
    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(p.x-r-1, p.y-r-1), Vec2D(2*r+4, 2*r+4), &x1, &x2, &y1, &y2);

    const float d = 2*0.7071f;

    for (int y = y1; y <= y2; ++y) {
        float dy = y - p.y;
        for (int x = x1; x <= x2; ++x) {
            float dx = x - p.x;
            float dist = dx*dx + dy*dy;
            if (dist >= (r + d)*(r + d)) continue;
            if (dist <= (r - d)*(r - d)) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                float ady = ay - p.y;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    float adx = ax - p.x;
                    float adist = adx*adx + ady*ady;
                    if (adist <= r*r) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Circle(NVC_Canvas oc, Vec2D p, float r, float thick, uint32_t color)
{
    float r_out = NVC_ABS(float ,r) + thick*0.5f;
    float r_in = r_out - thick;
    if (r_in > r_out) NVC_SWAP(float, r_in, r_out);
    if (r_in <= 0) {
        NVC_Fill_Circle(oc, p, r, color);
        return;
    }

    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(p.x-r_out-1, p.y-r_out-1), Vec2D(2*r_out+4, 2*r_out+4), &x1, &x2, &y1, &y2);

    const float d = 2*0.7071f;

    for (int y = y1; y <= y2; ++y) {
        float dy = y - p.y;
        for (int x = x1; x <= x2; ++x) {
            float dx = x - p.x;
            float dist = dx*dx + dy*dy;
            if (dist >= (r_out + d)*(r_out + d) || dist <= (r_in - d)*(r_in - d)) continue;
            if (dist <= (r_out - d)*(r_out - d) && dist >= (r_in + d)*(r_in + d)) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                float ady = ay - p.y;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    float adx = ax - p.x;
                    float adist = adx*adx + ady*ady;
                    if (adist <= r_out*r_out && adist >= r_in*r_in) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Line(NVC_Canvas oc, Vec2D p1, Vec2D p2, uint32_t color)
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
        if (0 <= x1 && x1 < oc.width && 0 <= y1 && y1 < oc.height)
            NVC_Draw_Pixel(oc, Vec2D(x1, y1), color);

        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

NEOVINCDEF bool NVC_IS_IN_TRIANGLE(Vec2D p, Vec2D p1, Vec2D p2, Vec2D p3)
{
    Vec2D v12 = NVC_Vec2D_Minus(p2, p1);
    Vec2D v23 = NVC_Vec2D_Minus(p3, p2);
    Vec2D v31 = NVC_Vec2D_Minus(p1, p3);
    Vec2D v1 = NVC_Vec2D_Minus(p, p1);
    Vec2D v2 = NVC_Vec2D_Minus(p, p2);
    Vec2D v3 = NVC_Vec2D_Minus(p, p3);
    return (NVC_SIGN(float, NVC_Vec2D_Cross(v1, v12)) == NVC_SIGN(float, NVC_Vec2D_Cross(v2, v23)) && NVC_SIGN(float, NVC_Vec2D_Cross(v1, v12)) == NVC_SIGN(float, NVC_Vec2D_Cross(v3, v31)));
}

NEOVINCDEF void NVC_Fill_Triangle(NVC_Canvas oc, Vec2D p1, Vec2D p2, Vec2D p3, uint32_t color)
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
    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (NVC_IS_IN_TRIANGLE(Vec2D(ax, ay), p1, p2, p3)) {
                        count += 1;
                    }
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Line_Ex(NVC_Canvas oc, Vec2D p1, Vec2D p2, float thick, uint32_t color)
{
    if (p1.x == p2.x && p1.y == p2.y) return;
    if (p1.x == p2.x) {
        float x = p1.x;
        float y1 = p1.y;
        float y2 = p2.y;
        if (y2 < y1) NVC_SWAP(float, y1, y2);
        NVC_Fill_Rectangle(oc, Vec2D(x-thick/2, y1), Vec2D(thick, y2-y1), color);
        return;
    }
    if (p1.y == p2.y) {
        float y = p1.y;
        float x1 = p1.x;
        float x2 = p2.x;
        if (x2 < x1) NVC_SWAP(float, x1, x2);
        NVC_Fill_Rectangle(oc, Vec2D(x1, y-thick/2), Vec2D(x2-x1, thick), color);
        return;
    }

    Vec2D dir = NVC_Vec2D_Minus(p2, p1);
    float len = NVC_Vec2D_Length(dir);
    // if (len < 0.001f) return;

    Vec2D u = { dir.x / len, dir.y / len };
    Vec2D n = { -u.y, u.x };

    Vec2D offset = Vec2D(n.x * thick * 0.5f, n.y * thick * 0.5f);
    Vec2D A = NVC_Vec2D_Plus(p1, offset);
    Vec2D B = NVC_Vec2D_Minus(p1, offset);
    Vec2D C = NVC_Vec2D_Plus(p2, offset);
    Vec2D D = NVC_Vec2D_Minus(p2, offset);

    NVC_Fill_Triangle(oc, A, B, D, color);
    NVC_Fill_Triangle(oc, A, C, D, color);
    NVC_Draw_Line(oc, A, D, color);
}

NEOVINCDEF void NVC_Draw_Triangle(NVC_Canvas oc, Vec2D p1, Vec2D p2, Vec2D p3, float thick, uint32_t color)
{
    NVC_Draw_Line_Ex(oc, p1, p2, thick, color);
    NVC_Draw_Line_Ex(oc, p2, p3, thick, color);
    NVC_Draw_Line_Ex(oc, p3, p1, thick, color);
}

#endif // NEOVIN_C_

// TODO: Anti_aliasing of line
// TODO: Anti_aliasing of triangle
// TODO: Draw line with arg: thick
