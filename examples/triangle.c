#include <stdint.h>

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1

#define WIDTH 800
#define HEIGHT 600
#include "neovin.c"

#define ROWS 6
#define COLS 8
#define CELL_HEIGHT 100
#define CELL_WIDTH 100

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

static uint32_t pixels[WIDTH*HEIGHT];
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

    angle += M_PI*dt;

    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    Vec2D p1 = { 400, 100 };
    Vec2D p2 = { 100, 300 };
    Vec2D p3 = { 750, 550 };
    Vec2D center = { (float)WIDTH/2, (float)HEIGHT/2 };
    rotate_point(&p1, center, angle);
    rotate_point(&p2, center, angle);
    rotate_point(&p3, center, angle);
    NVC_Fill_Triangle(oc, p1, p2, p3, 0xFF2020AA);

    Vec2D p4 = { 400, 300 };
    NVC_Fill_Circle(oc, p4, 200, 0x66AA2020);

    return pixels;
}

#if PLATFORM == RAYLIB_PLATFORM
#include <stdio.h>
#include <raylib.h>

int main(void)
{
    // ---- 初始化窗口 ----
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "triangle");
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

        // 1. 用 NeoVinC 绘制新一帧到内存
        render(dt);

        // 2. 上传内存像素到 GPU 纹理
        UpdateTexture(tex, pixels);

        // 3. 渲染纹理到窗口
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);
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
