#include <math.h>
#define SCALE_DOWN_FACTOR 10
#include "../tools/platform.c"
#include <stdint.h>

uint32_t pixels[WIDTH*HEIGHT];
uint32_t front[WIDTH*HEIGHT];
uint32_t space[WIDTH*HEIGHT];
uint32_t env[WIDTH*HEIGHT];
float    z_buf[WIDTH*HEIGHT];
#define BACKGROUND_COLOR 0xFF181818

bool project_3d_2d(Vec3D v3, Vec2D *v2)
{
    if (v3.z<=0) return false;
    v2->x = (1.f + v3.x/v3.z)*WIDTH/2;
    v2->y = (1.f - v3.y/v3.z)*HEIGHT/2;
    return true;
}

float t = 0;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

uint32_t *render(float dt)
{
    t += dt;
    float angle = t;

    NVC_Canvas_3D oc = NVC_CANVAS_3D(pixels, front, space, env, z_buf, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Envir_3D(oc, BACKGROUND_COLOR);
    NVC_Clear_Front_3D(oc);
    NVC_Clear_Space_3D(oc);
    NVC_Clear_Depth_3D(oc);

    // float z = 1.5 + 0.5*sinf(t);
    float z = 1.5;
    // float dy = 0.1*sinf(10*t);
    float dy = 0;
    Vec3D a = Vec3D(-0.5, -0.5+dy, z);
    Vec3D b = Vec3D(0.5, -0.5+dy, z);
    Vec3D c = Vec3D(0, 0.5+dy, z);
    Vec3D rp = Vec3D(0, 0, z);
    NVC_ROTATE_POINT(&a, rp, Vec3D(0,1,0), angle);
    NVC_ROTATE_POINT(&b, rp, Vec3D(0,1,0), angle);
    NVC_ROTATE_POINT(&c, rp, Vec3D(0,1,0), angle);

    // NVC_Point_3D(oc, a, 10, 0xFF1818FF);
    // NVC_Point_3D(oc, b, 10, 0xFF1818FF);
    // NVC_Point_3D(oc, c, 10, 0xFF1818FF);
    NVC_Fill_Triangle_3D_C3(oc, a, b, c, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000);
    // NVC_Draw_Triangle_3D(oc, a, b, c, 2, 0xFF000000);

    NVC_ROTATE_POINT(&a, rp, Vec3D(0,1,0), M_PI/2);
    NVC_ROTATE_POINT(&b, rp, Vec3D(0,1,0), M_PI/2);
    NVC_ROTATE_POINT(&c, rp, Vec3D(0,1,0), M_PI/2);

    // NVC_Point_3D(oc, a, 10, 0xFF1818FF);
    // NVC_Point_3D(oc, b, 10, 0xFF1818FF);
    // NVC_Point_3D(oc, c, 10, 0xFF1818FF);
    NVC_Fill_Triangle_3D_C3(oc, a, b, c, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000);
    // NVC_Draw_Triangle_3D(oc, a, b, c, 2, 0xFF000000);


    NVC_Canvas oc_2d = NVC_CANVAS(front, WIDTH, HEIGHT, WIDTH);
    float font_size = 24;
    NVC_Text(oc_2d, "3d triangles Rendering E.X.", Vec2D(10, 10), NVC_default_font, font_size, 0xFFFFFFFF);

    NVC_Update_Canvas_3D(oc);

    return pixels;
}
