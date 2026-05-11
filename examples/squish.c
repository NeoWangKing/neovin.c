#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#define WASM_PLATFORM 0
#define RAYLIB_PLATFORM 1

#define WIDTH 800
#define HEIGHT 600
#include "neovin.c"

#if PLATFORM != RAYLIB_PLATFORM
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "thirdparty/stb_image_write.h"
#include "thirdparty/stb_image.h"

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

    const char *png_file_path = "NeoWangKing.png";
    NVC_Texture texture;
    texture.data = (uint32_t*) stbi_load(png_file_path, &texture.width, &texture.height, NULL, 4);
    if (texture.data == NULL) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", png_file_path, strerror(errno));
    }

    NVC_Canvas oc = NVC_Canvas(pixels, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float w = (float)texture.width/(1+0.5*sinf(angle));
    float h = (float)texture.height*(1+0.5*sinf(angle));
    float x = (float)WIDTH/2 - w/2;
    float y = (float)HEIGHT/2 - h/2;
    NVC_Draw_Texture(oc, texture, Vec2D(x, y), Vec2D(w, h));

    float font_size = 24;
    NVC_Text(oc, "Texture Rendering E.X.", Vec2D(-(float)WIDTH/2 + 10, -(float)HEIGHT/2 + 10), NVC_default_font, font_size, 0xFFFFFFFF);
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
#elif PLATFORM == WASM_PLATFORM
// Do nothing
#else
#error "Unknown platform"
#endif // PLATFORM
