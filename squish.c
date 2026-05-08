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

#include "../thirdparty/stb_image_write.h"
#include "../thirdparty/stb_image.h"

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
    int tw, th;
    uint32_t *texture;
    texture = (uint32_t*) stbi_load(png_file_path, &tw, &th, NULL, 4);
    if (texture == NULL) {
        fprintf(stderr, "ERROR: could not read file %s: %s\n", png_file_path, strerror(errno));
    }

    NVC_Canvas oc = NVC_Make_Canvas(pixels, WIDTH, HEIGHT);
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float w = (float)tw/(1+0.5*sinf(angle));
    float h = (float)th*(1+0.5*sinf(angle));
    NVC_Canvas sub_oc;
    NVC_GetSubCanvas(&sub_oc, NVC_CANVAS(pixels, WIDTH, HEIGHT), Vec2D((float)WIDTH/2-w/2, HEIGHT-h), Vec2D(w, h));
    NVC_Copy(sub_oc, NVC_CANVAS(texture, tw, th));

    float font_size = 24;
    NVC_Text(oc, "Texture Rendering E.X.", Vec2D(-(float)WIDTH/2 + 10, -(float)HEIGHT/2 + 10), default_font, font_size, 0xFFFFFFFF);
    return pixels;
}

#if PLATFORM == RAYLIB_PLATFORM
#include <stdio.h>
#include <raylib.h>

#define return_defer(value) do { result = (value); goto defer; } while (0)

int main(void)
{
    // ---- 初始化窗口 ----
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "Texture");
    SetTargetFPS(60);

    // ---- 准备画布首帧数据 ----
    uint32_t *pixels = render(0.0f);  // 填充 oc.pixels

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
