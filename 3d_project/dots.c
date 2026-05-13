#include <stdint.h>
#define WIDTH 800
#define HEIGHT 600
#define SCALE_DOWN_FACTOR 10
#include "./tools/platform.c"

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

static uint32_t pixels[WIDTH*HEIGHT];
static uint32_t front[WIDTH*HEIGHT];
static uint32_t space[WIDTH*HEIGHT];
static uint32_t env[WIDTH*HEIGHT];
static float    z_buf[WIDTH*HEIGHT];
float angle = 0.f;

float get_angle() {
    return angle;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SQUARE_SIZE 1.

uint32_t *render(float dt)
{
    angle += 0.1 * M_PI * dt;

    NVC_Canvas_3D oc = NVC_CANVAS_3D(pixels, front, space, env, z_buf, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Envir_3D(oc, BACKGROUND_COLOR);
    NVC_Clear_Space_3D(oc);
    NVC_Clear_Front_3D(oc);
    NVC_Clear_Depth_3D(oc);

    Vec3D p0 = Vec3D(0.0f, 0.0f, 1 + (float)SQUARE_SIZE*sqrtf(3.0f)/2);
    Vec3D axis_x = Vec3D(1.0f, 0.0f, 0.0f);
    Vec3D axis_y = Vec3D(0.0f, 1.0f, 0.0f);
    Vec3D axis_z = Vec3D(0.0f, 0.0f, 1.0f);

    Vec3D v1 = Vec3D(p0.x - SQUARE_SIZE/2, p0.y - SQUARE_SIZE/2, p0.z - SQUARE_SIZE/2); uint32_t c1 = 0xFF000000;
    Vec3D v2 = Vec3D(p0.x + SQUARE_SIZE/2, p0.y - SQUARE_SIZE/2, p0.z - SQUARE_SIZE/2); uint32_t c2 = 0xFF0000FF;
    Vec3D v3 = Vec3D(p0.x + SQUARE_SIZE/2, p0.y - SQUARE_SIZE/2, p0.z + SQUARE_SIZE/2); uint32_t c3 = 0xFF00FFFF;
    Vec3D v4 = Vec3D(p0.x - SQUARE_SIZE/2, p0.y - SQUARE_SIZE/2, p0.z + SQUARE_SIZE/2); uint32_t c4 = 0xFF00FF00;
    Vec3D v5 = Vec3D(p0.x - SQUARE_SIZE/2, p0.y + SQUARE_SIZE/2, p0.z - SQUARE_SIZE/2); uint32_t c5 = 0xFFFF0000;
    Vec3D v6 = Vec3D(p0.x + SQUARE_SIZE/2, p0.y + SQUARE_SIZE/2, p0.z - SQUARE_SIZE/2); uint32_t c6 = 0xFFFF00FF;
    Vec3D v7 = Vec3D(p0.x + SQUARE_SIZE/2, p0.y + SQUARE_SIZE/2, p0.z + SQUARE_SIZE/2); uint32_t c7 = 0xFFFFFFFF;
    Vec3D v8 = Vec3D(p0.x - SQUARE_SIZE/2, p0.y + SQUARE_SIZE/2, p0.z + SQUARE_SIZE/2); uint32_t c8 = 0xFFFFFF00;
    NVC_ROTATE_POINT(&v1, p0, axis_y, angle); NVC_ROTATE_POINT(&v1, p0, axis_x, angle); NVC_ROTATE_POINT(&v1, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v2, p0, axis_y, angle); NVC_ROTATE_POINT(&v2, p0, axis_x, angle); NVC_ROTATE_POINT(&v2, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v3, p0, axis_y, angle); NVC_ROTATE_POINT(&v3, p0, axis_x, angle); NVC_ROTATE_POINT(&v3, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v4, p0, axis_y, angle); NVC_ROTATE_POINT(&v4, p0, axis_x, angle); NVC_ROTATE_POINT(&v4, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v5, p0, axis_y, angle); NVC_ROTATE_POINT(&v5, p0, axis_x, angle); NVC_ROTATE_POINT(&v5, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v6, p0, axis_y, angle); NVC_ROTATE_POINT(&v6, p0, axis_x, angle); NVC_ROTATE_POINT(&v6, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v7, p0, axis_y, angle); NVC_ROTATE_POINT(&v7, p0, axis_x, angle); NVC_ROTATE_POINT(&v7, p0, axis_z, angle);
    NVC_ROTATE_POINT(&v8, p0, axis_y, angle); NVC_ROTATE_POINT(&v8, p0, axis_x, angle); NVC_ROTATE_POINT(&v8, p0, axis_z, angle);

    NVC_Fill_Triangle_3D_C3(oc, v1, v2, v3, c1, c2, c3);
    NVC_Fill_Triangle_3D_C3(oc, v1, v4, v3, c1, c4, c3);
    
    NVC_Fill_Triangle_3D_C3(oc, v1, v2, v6, c1, c2, c6);
    NVC_Fill_Triangle_3D_C3(oc, v1, v5, v6, c1, c5, c6);

    NVC_Fill_Triangle_3D_C3(oc, v4, v1, v5, c4, c1, c5);
    NVC_Fill_Triangle_3D_C3(oc, v4, v8, v5, c4, c8, c5);

    NVC_Fill_Triangle_3D_C3(oc, v2, v3, v7, c2, c3, c7);
    NVC_Fill_Triangle_3D_C3(oc, v2, v6, v7, c2, c6, c7);

    NVC_Fill_Triangle_3D_C3(oc, v5, v6, v7, c5, c6, c7);
    NVC_Fill_Triangle_3D_C3(oc, v5, v8, v7, c5, c8, c7);

    NVC_Fill_Triangle_3D_C3(oc, v3, v7, v8, c3, c7, c8);
    NVC_Fill_Triangle_3D_C3(oc, v3, v4, v8, c3, c4, c8);

    NVC_Draw_Line_3D_Ex(oc, v1, v2, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v2, v3, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v3, v4, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v4, v1, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v1, v5, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v2, v6, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v3, v7, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v4, v8, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v5, v6, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v6, v7, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v7, v8, 2, 0xFF000000);
    NVC_Draw_Line_3D_Ex(oc, v8, v5, 2, 0xFF000000);

    // NVC_Draw_Line_3D_Ex(oc, v1, v3, 2, 0xFF000000);
    // NVC_Draw_Line_3D_Ex(oc, v1, v6, 2, 0xFF000000);
    // NVC_Draw_Line_3D_Ex(oc, v4, v5, 2, 0xFF000000);
    // NVC_Draw_Line_3D_Ex(oc, v2, v7, 2, 0xFF000000);
    // NVC_Draw_Line_3D_Ex(oc, v5, v7, 2, 0xFF000000);
    // NVC_Draw_Line_3D_Ex(oc, v3, v8, 2, 0xFF000000);

    NVC_Canvas oc_2d = NVC_CANVAS(oc.front, oc.width, oc.height, oc.stride);
    float font_size = 24;
    NVC_Text(oc_2d, "3D Grid Rendering E.X.", Vec2D(10, 10), NVC_default_font, font_size, 0xFFFFFFFF);

    NVC_Update_Canvas_3D(oc);

    return pixels;
}
