#ifndef WASM_PLATFORM
#define WASM_PLATFORM 0
#endif
#ifndef RAYLIB_PLATFORM
#define RAYLIB_PLATFORM 1
#endif
#ifndef TERM_PLATFORM
#define TERM_PLATFORM 2
#endif

#ifndef WIDTH
#define WIDTH 800
#endif
#ifndef HEIGHT
#define HEIGHT 600
#endif

#include "neovin.c"

#pragma once
#include <stdint.h>
uint32_t *render(float dt);


#if PLATFORM == RAYLIB_PLATFORM

#include <raylib.h>

#define return_defer(value) do { result = (value); goto defer; } while (0)

#include <stdio.h>
#include <errno.h>
#include <stdint.h>


int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "Texture");
    SetTargetFPS(60);

    uint32_t *pixels = render(0.0f);

    Image Frame = {
        .data = pixels,
        .width = WIDTH,
        .height = HEIGHT,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };
    Texture2D tex = LoadTextureFromImage(Frame);
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

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

    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
#elif PLATFORM == TERM_PLATFORM

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#ifndef SCALE_DOWN_FACTOR
#define SCALE_DOWN_FACTOR 5
#endif
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
    NVC_Canvas oc = NVC_CANVAS(pixels, WIDTH, HEIGHT, WIDTH);
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
