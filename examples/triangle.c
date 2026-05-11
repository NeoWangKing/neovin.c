#include <stdint.h>

// #define PLATFORM 2

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1
#define TERM_PLATFORM 2

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

    Vec2D p1 = { 400, 100 };
    Vec2D p2 = { 100, 300 };
    Vec2D p3 = { 750, 550 };
    Vec2D center = { (float)WIDTH/2, (float)HEIGHT/2 };
    rotate_point(&p1, center, angle);
    rotate_point(&p2, center, angle);
    rotate_point(&p3, center, angle);
    // NVC_Fill_Triangle(oc, p1, p2, p3, 0xFF2020AA);
    NVC_Fill_Triangle_C3(oc, p1, p2, p3, 0xFF2020AA, 0xFF20AA20, 0xFFAA2020);

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
#elif PLATFORM == TERM_PLATFORM

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#define SCALE_DOWN_FACTOR 5
static_assert(WIDTH%SCALE_DOWN_FACTOR == 0, "WIDTH must be divisible by the SCALE_DOWN_FACTOR");
#define SCALED_DOWN_WIDTH (WIDTH/SCALE_DOWN_FACTOR)
static_assert(HEIGHT%SCALE_DOWN_FACTOR == 0, "HEIGHT must be divisible by the SCALE_DOWN_FACTOR");
#define SCALED_DOWN_HEIGHT (HEIGHT/SCALE_DOWN_FACTOR)

char char_canvas[SCALED_DOWN_WIDTH*SCALED_DOWN_HEIGHT];

char color_to_char(uint32_t pixel)
{
    float r = NVC_Red(pixel);
    float g = NVC_Green(pixel);
    float b = NVC_Blue(pixel);
    float a = NVC_Alpha(pixel);

    float luminance = 0.299f * r + 0.587f * g + 0.114f * b;
    luminance = luminance * (a / 255.0f);

    char table[] = "           ......,,,:::;;++rr**zzssTTvvJ7(|Fi{C}fI31tlunneoZ5Yxjyaa2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
    int n = sizeof(table) - 1;
    int index = (int)((luminance * n) / 255.0f);
    if (index < 0) index = 0;
    if (index > n) index = n;
    return table[index];
}

uint32_t compress_pixels_chunk(NVC_Canvas oc)
{
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 0;

    for (int y = 0; y < oc.height; ++y) {
        for (int x = 0; x < oc.width; ++x) {
            r += NVC_Red(NVC_PIXEL(oc, x, y));
            g += NVC_Green(NVC_PIXEL(oc, x, y));
            b += NVC_Blue(NVC_PIXEL(oc, x, y));
            a += NVC_Alpha(NVC_PIXEL(oc, x, y));
        }
    }

    r /= oc.width*oc.height;
    g /= oc.width*oc.height;
    b /= oc.width*oc.height;
    a /= oc.width*oc.height;

    return NVC_RGBA((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
}

void compress_pixels(uint32_t *pixels)
{
    NVC_Canvas oc = NVC_Canvas(pixels, WIDTH, HEIGHT, WIDTH);
    for (int y = 0; y < SCALED_DOWN_HEIGHT; ++y) {
        for (int x = 0; x < SCALED_DOWN_WIDTH; ++x) {
            NVC_Canvas soc = NVC_Make_SubCanvas(oc,
                    Vec2D(x*SCALE_DOWN_FACTOR, y*SCALE_DOWN_FACTOR),
                    Vec2D(SCALE_DOWN_FACTOR, SCALE_DOWN_FACTOR));
            char_canvas[y*SCALED_DOWN_WIDTH + x] = color_to_char(compress_pixels_chunk(soc));
        }
    }
}

int main(void)
{
    for (;;) {
        compress_pixels(render(1.f/60.f));
        for (int y = 0; y < SCALED_DOWN_HEIGHT; ++y) {
            for (int x = 0; x < SCALED_DOWN_WIDTH; ++x) {
                putc(char_canvas[y*SCALED_DOWN_WIDTH + x], stdout);
                putc(char_canvas[y*SCALED_DOWN_WIDTH + x], stdout);
            }
            putc('\n', stdout);
        }

        usleep(1000*1000/60);
        printf("\033[%dA", SCALED_DOWN_HEIGHT);
        printf("\033[%dD", SCALED_DOWN_WIDTH);
    }
    return 0;
}
#elif PLATFORM == WASM_PLATFORM
// Do nothing
#else
#error "Unknown platform"
#endif // PLATFORM
