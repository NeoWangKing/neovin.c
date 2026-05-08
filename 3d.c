#include <stdint.h>
#include <stdlib.h>

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1

#define WIDTH 800
#define HEIGHT 600
#define NVC_CV_OX (int)(WIDTH/2)
#define NVC_CV_OY (int)(HEIGHT/2)
#define NVC_3D_IMPLEMENTATION
#include "neovin.c"

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

// 假设 Vec3D 已定义，且 math.h 已包含
NEOVINCDEF void rotate_point(Vec3D *p, Vec3D p0, Vec3D dir, float angle) {
    // 1. 计算相对向量
    float vx = p->x - p0.x;
    float vy = p->y - p0.y;
    float vz = p->z - p0.z;

    // 2. 归一化旋转轴
    float len_dir = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    if (len_dir < 0.0001f) return; // 轴退化，无法旋转
    float ux = dir.x / len_dir;
    float uy = dir.y / len_dir;
    float uz = dir.z / len_dir;

    // 3. 罗德里格斯旋转公式
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float one_minus_cos = 1.0f - cos_a;

    // 叉积 u × v
    float cx = uy * vz - uz * vy;
    float cy = uz * vx - ux * vz;
    float cz = ux * vy - uy * vx;

    // 点积 u · v
    float dot = ux * vx + uy * vy + uz * vz;

    // 组合三项
    float rx = vx * cos_a + cx * sin_a + ux * dot * one_minus_cos;
    float ry = vy * cos_a + cy * sin_a + uy * dot * one_minus_cos;
    float rz = vz * cos_a + cz * sin_a + uz * dot * one_minus_cos;

    // 4. 平移回原始参考点
    p->x = p0.x + rx;
    p->y = p0.y + ry;
    p->z = p0.z + rz;
}

#define GRID_COUNT 10
#define GRID_PAD 1.f/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define FOV 1

uint32_t *render(float dt)
{
    angle += 0.5 * M_PI * dt;

    NVC_Canvas oc = NVC_Make_Canvas(pixels, WIDTH, HEIGHT);
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
                rotate_point(&p, p0, axis_y, angle);
                rotate_point(&p, p0, axis_z, angle);
                rotate_point(&p, p0, axis_x, angle);

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

                NVC_Point(oc, Vec2D(p.x/2*WIDTH/p.z, p.y/2*HEIGHT/p.z), 5.0f*FOV / p.z, tmp_color);

            }
        }
    }
    float font_size = 24;
    NVC_Text(oc, "3d Rendering E.X.", Vec2D(-(float)WIDTH/2 + 10, -(float)HEIGHT/2 + 10), default_font, font_size, 0xFFFFFFFF);
    return pixels;
}

#if PLATFORM == RAYLIB_PLATFORM
#include <stdio.h>
#include <raylib.h>

int main(void)
{
    // ---- 初始化窗口 ----
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "3d");
    SetTargetFPS(60);

    // ---- 准备画布首帧数据 ----
    render(0.0f);  // 填充 oc.pixels

    // ---- 创建纹理（从首帧内存数据）----
    Image Frame = {
        .data = pixels,
        .width = WIDTH,
        .height = HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D tex = LoadTextureFromImage(Frame);
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    // ---- 主循环 ----
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        render(dt);

        UpdateTexture(tex, pixels);

        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle source = { 0, 0, WIDTH, HEIGHT };
        Rectangle dest   = { 0, 0, GetScreenWidth(), GetScreenHeight() };
        DrawTexturePro(tex, source, dest, (Vector2){0,0}, 0, WHITE);
        EndDrawing();
    }

    // ---- 清理 ----
    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
#elif PLATFORM == WASM_PLATFORM
// Do nothing
#else
#error "Unknown platform"
#endif // PLATFORM
