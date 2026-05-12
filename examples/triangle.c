#define WIDTH 800
#define HEIGHT 600
#define SCALE_DOWN_FACTOR 10
#include "./tools/platform.c"

#define ROWS 6
#define COLS 8
#define CELL_HEIGHT 100
#define CELL_WIDTH 100

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

static uint32_t pixels[WIDTH*HEIGHT];
float angle = 0.f;
float acx = 0;
float acy = 0;
float vcx = 200;
float vcy = 200;
float pcx = 400;
float pcy = 300;

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
    Vec2D vec = NVC_Vec2D_Minus(*p, p0);
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
    NVC_Canvas oc = NVC_Make_Canvas(pixels, WIDTH, HEIGHT, WIDTH);

    angle += 0.5*M_PI*dt;

    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    Vec3D p1 = { 400, 100, 0 };
    Vec3D p2 = { 100, 300, 0 };
    Vec3D p3 = { 750, 550, 0 };
    Vec3D center = { (float)WIDTH/2, (float)HEIGHT/2, 0 };
    NVC_ROTATE_POINT(&p1, center, Vec3D(0, 0, 1), angle);
    NVC_ROTATE_POINT(&p2, center, Vec3D(0, 0, 1), angle);
    NVC_ROTATE_POINT(&p3, center, Vec3D(0, 0, 1), angle);
    // NVC_Fill_Triangle(oc, p1, p2, p3, 0xFF2020AA);
    NVC_Fill_Triangle_C3(oc, Vec2D(p1.x, p1.y), Vec2D(p2.x, p2.y), Vec2D(p3.x, p3.y), 0xFF2020AA, 0xFF20AA20, 0xFFAA2020);

    float radius = 150;
    pcx += vcx*dt;
    pcy += vcy*dt;
    if (pcx <= radius) { pcx = radius; vcx *= -1; }
    if (pcy <= radius) { pcy = radius; vcy *= -1; }
    if (pcx >= WIDTH-radius) { pcx = WIDTH-radius; vcx *= -1; }
    if (pcy >= HEIGHT-radius) { pcy = HEIGHT-radius; vcy *= -1; }
    vcx += acx*dt;
    vcy += acy*dt;
    NVC_Fill_Circle(oc, Vec2D(pcx, pcy), radius, 0x66AA2020);

    return pixels;
}
