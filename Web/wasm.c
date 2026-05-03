#define NEOVIN_C_IMPLEMENTATION
#include "../neovin.c"
#include <stdint.h>

#define WIDTH 800
#define HEIGHT 600

#define ROWS 6
#define COLS 8
#define CELL_HEIGHT 100
#define CELL_WIDTH 100

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

NVC_Canvas pixels;
float angle = 0.f;

float get_angle() {
    return angle;
}

float sqrtf(float x);
float atan2f(float x, float y);
float cosf(float x);
float sinf(float x);

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float lerpf(float a, float b, float t)
{
    return a + (b - a)*t;
}

void rotate_point(Vec2D *p, Vec2D p0, float angle)
{
    Vec2D vec = NVC_Vec2D_Subtract(*p, p0);
    // float mag = NVC_Vec2D_Len(vec);
    // float dir = NVC_Vec2D_Angle(vec) + angle;
    float mag = sqrtf(vec.x*vec.x + vec.y*vec.y);
    float dir = atan2f(vec.y, vec.x) + angle;
    // *p = (Vec2D){ .x = cosf(dir)*mag + p0.x, .y = sinf(dir)*mag + p0.y };
    p->x = cosf(dir)*mag + p0.x;
    p->y = sinf(dir)*mag + p0.y;
}

uint32_t *render(float dt)
{
    static uint32_t data[WIDTH*HEIGHT];

    angle += 2*M_PI*dt;

    pixels.width = WIDTH;
    pixels.height = HEIGHT;
    pixels.stride = WIDTH;
    pixels.data = data;

    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    Vec2D p1 = { 400, 100 };
    Vec2D p2 = { 100, 300 };
    Vec2D p3 = { 750, 550 };
    Vec2D center = { (float)WIDTH/2, (float)HEIGHT/2 };
    rotate_point(&p1, center, angle);
    rotate_point(&p2, center, angle);
    rotate_point(&p3, center, angle);
    NVC_Fill_Triangle(pixels, p1, p2, p3, 0xFF2020AA);

    return data;
}
