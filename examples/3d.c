#define WIDTH 800
#define HEIGHT 600
#include "./tools/platform.c"

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

static uint32_t pixels[WIDTH*HEIGHT];
float angle = 0.f;

float get_angle() {
    return angle;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define GRID_COUNT 10
#define GRID_PAD 1.f/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define FOV 1

uint32_t *render(float dt)
{
    angle += 0.5 * M_PI * dt;

    NVC_Canvas oc = NVC_Make_Canvas(pixels, WIDTH, HEIGHT, WIDTH);
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    uint32_t color = 0xFF2020AA;
    uint32_t tmp_color = color;

    // 修正：使用复合字面量，确保 z 被初始化
    Vec3D p0 = { 0.0f, 0.0f, FOV + (float)GRID_SIZE*sqrtf(2.0f)/2 };
    Vec3D axis_x = { 1.0f, 0.0f, 0.0f };
    Vec3D axis_y = { 0.0f, 1.0f, 0.0f };
    Vec3D axis_z = { 0.0f, 0.0f, 1.0f };

    for (int cz = GRID_COUNT - 1; cz >= 0; --cz) {
        for (int cy = 0; cy < GRID_COUNT; ++cy) {
            for (int cx = 0; cx < GRID_COUNT; ++cx) {
                uint8_t r = 255*(float)cx/(GRID_COUNT - 1);
                uint8_t g = 255*(float)cy/(GRID_COUNT - 1);
                uint8_t b = 255*(float)cz/(GRID_COUNT - 1);
                float x = p0.x - (float)GRID_SIZE/2 + cx * GRID_PAD;
                float y = p0.y - (float)GRID_SIZE/2 + cy * GRID_PAD;
                float z = p0.z - (float)GRID_SIZE/2 + cz * GRID_PAD;
                
                Vec3D p = { x, y, z };
                NVC_ROTATE_POINT(&p, p0, axis_y, angle);
                NVC_ROTATE_POINT(&p, p0, axis_x, angle);
                NVC_ROTATE_POINT(&p, p0, axis_z, angle);

                uint8_t comp[COUNT_COMP];
                comp[COMP_RED] = r;
                comp[COMP_GREEN] = g;
                comp[COMP_BLUE] = b;
                comp[COMP_ALPHA] = 255;
                tmp_color = NVC_Pack_RGBA32(comp);
                NVC_Bright_Color(&tmp_color, 1.5);

                // 防止 z 变负或为零
                if (p.z <= 0.0f) continue;
                if (p.z <= FOV) NVC_Transparent_Color(&tmp_color, p.z/FOV);
                if (p.z > FOV) NVC_Bright_Color(&tmp_color, 1.0f/(1 + 2*(p.z - FOV)));
                if (p.z > FOV) NVC_Transparent_Color(&tmp_color, 1.0f/(1 + 2*(p.z - FOV)));

                NVC_Point(oc, Vec2D(p.x/2*WIDTH/p.z + (float)WIDTH/2, p.y/2*HEIGHT/p.z + (float)HEIGHT/2), 5.0f*FOV / p.z, tmp_color);
            }
        }
    }

    float font_size = 24;
    NVC_Text(oc, "3d Rendering E.X.", Vec2D(10, 10), NVC_default_font, font_size, 0xFFFFFFFF);
    return pixels;
}
