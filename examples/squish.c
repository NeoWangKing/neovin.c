#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

// #define PLATFORM 2

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1
#define TERM_PLATFORM 2

#define WIDTH 800
#define HEIGHT 600
#include "neovin.c"

// #if PLATFORM != RAYLIB_PLATFORM
// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #define STB_IMAGE_IMPLEMENTATION
// #endif
//
// #include "thirdparty/stb_image_write.h"
// #include "thirdparty/stb_image.h"

// #include "neowang.c"
#include "amiya.c"

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

float angle = 0.f;

float get_angle() {
    return angle;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

uint32_t *render(float dt)
{
    static uint32_t pixels[WIDTH*HEIGHT];
    angle += 4 * M_PI * dt;

    // const char *png_file_path = "NeoWangKing.png";
    // NVC_Texture texture;
    // texture.data = (uint32_t*) stbi_load(png_file_path, &texture.width, &texture.height, NULL, 4);
    // if (texture.data == NULL) {
    //     fprintf(stderr, "ERROR: could not read file %s: %s\n", png_file_path, strerror(errno));
    // }
    
    NVC_Texture texture;
    texture.data = amiya_data;
    texture.width = amiya_width;
    texture.height = amiya_height;


    NVC_Canvas oc = NVC_Canvas(pixels, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float w = (float)texture.width/(1+0.2*sinf(angle));
    float h = (float)texture.height*(1+0.2*sinf(angle));
    float x = (float)WIDTH/2 - w/2;
    float y = (float)HEIGHT/2 - h/2;
    NVC_Draw_Texture(oc, texture, Vec2D(x, y), Vec2D(w, h));

    float font_size = 24;
    NVC_Text(oc, "Texture Rendering E.X.", Vec2D(10, 10), NVC_default_font, font_size, 0xFFFFFFFF);
    return pixels;
}

#if PLATFORM == RAYLIB_PLATFORM
#include <stdio.h>
#include <raylib.h>

#define return_defer(value) do { result = (value); goto defer; } while (0)

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
    int r = NVC_Red(pixel);
    int g = NVC_Green(pixel);
    int b = NVC_Blue(pixel);
    // TODO: brightness should take into account tranparency as well
    int bright = r;
    if (bright < g) bright = g;
    if (bright < b) bright = b;

    char table[] = " .:a@#";
    int n = sizeof(table) - 1;
    return table[bright*n/256];
}

uint32_t compress_pixels_chunk(NVC_Canvas oc)
{
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0;

    for (int y = -NVC_CV_OY; y < oc.height - NVC_CV_OY; ++y) {
        for (int x = -NVC_CV_OX; x < oc.width - NVC_CV_OX; ++x) {
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

    return NVC_RGBA(r, g, b, a);
}

void compress_pixels(uint32_t *pixels)
{
    NVC_Canvas oc = NVC_Canvas(pixels, WIDTH, HEIGHT, WIDTH);
    for (int y = 0; y < SCALED_DOWN_HEIGHT; ++y) {
        for (int x = 0; x < SCALED_DOWN_WIDTH; ++x) {
            NVC_Canvas soc = NVC_Make_SubCanvas(oc,
                    Vec2D(x*SCALE_DOWN_FACTOR - NVC_CV_OX, y*SCALE_DOWN_FACTOR - NVC_CV_OY),
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
