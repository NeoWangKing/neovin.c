#include <stdint.h>
#include <stdlib.h>

// #define PLATFORM 2

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1
#define TERM_PLATFORM 2

#define WIDTH 800
#define HEIGHT 600
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
                NVC_Rotate_Point(&p, p0, axis_y, angle);
                NVC_Rotate_Point(&p, p0, axis_x, angle);
                NVC_Rotate_Point(&p, p0, axis_z, angle);

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
