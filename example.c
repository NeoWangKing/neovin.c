#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./thirdparty/stb_image_write.h"

#define NEOVIN_C_IMPLEMENTATION
#include "neovin.c"

#define WIDTH 800
#define HEIGHT 600
#define ROWS 6
#define COLS 8
#define CELL_HEIGHT 100
#define CELL_WIDTH 100

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF

NVC_Canvas oc = {0};

float lerpf(float a, float b, float t)
{
    return a + (b - a)*t;
}

typedef int Errno;

#define return_defer(value) do { result = (value); goto defer; } while (0)

Errno NVC_save_to_ppm_file(NVC_Canvas pixels, const char *file_path)
{
    int result = 0;
    FILE *f = NULL;

    {
        f = fopen(file_path, "wb");
        if (f == NULL) return_defer(errno);

        fprintf(f, "P6\n%d %d 255\n", pixels.width, pixels.height);
        if (ferror(f)) return_defer(errno);

        for (int i = 0; i < pixels.width*pixels.height; ++i) {
            // 0xAABBGGRR
            uint32_t pixel = pixels.pixels[i];
            uint8_t bytes[3] = {
                (pixel>>(8*0))&0xFF,
                (pixel>>(8*1))&0xFF,
                (pixel>>(8*2))&0xFF,
            };
            fwrite(bytes, sizeof(bytes), 1, f);
            if (ferror(f)) return_defer(errno);
        }
    }

defer:
    if (f) fclose(f);
    return result;
}

bool save_as_png(const char *file_path)
{
    if (!stbi_write_png(file_path, oc.width, oc.height, 4, oc.pixels, oc.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool blank_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float font_size = 24;
    NVC_Text(oc, "Blank E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool rectangles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            float u = (float)x / COLS;
            float v = (float)y / ROWS;
            float t = (u + v)/2;
            if ((x+y)%2) {
                NVC_Fill_Rectangle(oc,
                        Vec2D(x*CELL_WIDTH+(float)CELL_WIDTH/2-(float)CELL_WIDTH*t/2, y*CELL_HEIGHT+(float)CELL_HEIGHT/2-(float)CELL_HEIGHT*t/2),
                        Vec2D(CELL_WIDTH*t, CELL_HEIGHT*t),
                        0xCC2020FF);
            } else {
                NVC_Draw_Rectangle(oc,
                        Vec2D(x*CELL_WIDTH+(float)CELL_WIDTH/2-(float)CELL_WIDTH*t/2, y*CELL_HEIGHT+(float)CELL_HEIGHT/2-(float)CELL_HEIGHT*t/2),
                        Vec2D(CELL_WIDTH*t, CELL_HEIGHT*t),
                        4,
                        0xCC20AA20);
            }
        }
    }

    float font_size = 24;
    NVC_Text(oc, "Rectangles E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool circles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    for (int i = 5; i > 0; --i) {
        NVC_Draw_Circle(oc,
                Vec2D((float)WIDTH/2, (float)HEIGHT/2),
                lerpf(0, (float)HEIGHT/2, (float)i/5),
                2,
                0xFF20FF20);
    }

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            float u = (float)x / COLS;
            float v = (float)y / ROWS;
            float t = (u + v)/2;

            float radius = CELL_WIDTH;
            uint32_t color = 0xFF5050FF;
            NVC_Transparent_Color(&color, t);
            if (CELL_HEIGHT < radius) radius = CELL_HEIGHT;
            if ((x+y)%2) {
                NVC_Fill_Circle(oc,
                        Vec2D(x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT + (float)CELL_HEIGHT/2),
                        lerpf(0, radius/2, t),
                        color);
            } else {
                NVC_Draw_Circle(oc,
                        Vec2D(x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT + (float)CELL_HEIGHT/2),
                        lerpf(0, radius/2, t),
                        2,
                        color);
            }
        }
    }

    float font_size = 24;
    NVC_Text(oc, "Circles E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool lines_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    NVC_Draw_Line_Ex(oc, Vec2D(0, 0), Vec2D(WIDTH, HEIGHT), 6, 0xFF0000FF);
    NVC_Draw_Line_Ex(oc, Vec2D(0, HEIGHT), Vec2D(WIDTH, 0), 6, 0xFF0000FF);

    NVC_Draw_Line_Ex(oc, Vec2D(0, 0), Vec2D((float)WIDTH/2, HEIGHT), 6, 0xFF00FF00);
    NVC_Draw_Line_Ex(oc, Vec2D(0, HEIGHT), Vec2D((float)WIDTH/2, 0), 6, 0xFF00FF00);

    NVC_Draw_Line_Ex(oc, Vec2D((float)WIDTH/2, 0), Vec2D(WIDTH, HEIGHT), 6, 0xFF00FF00);
    NVC_Draw_Line_Ex(oc, Vec2D((float)WIDTH/2, HEIGHT), Vec2D(WIDTH, 0), 6, 0xFF00FF00);

    NVC_Draw_Line_Ex(oc, Vec2D(0, (float)HEIGHT/2), Vec2D(WIDTH, (float)HEIGHT/2), 6, 0xFFFFFF00);
    NVC_Draw_Line_Ex(oc, Vec2D((float)WIDTH/2, 0), Vec2D((float)WIDTH/2, HEIGHT), 6, 0xFFFFFF00);

    float font_size = 24;
    NVC_Text(oc, "Lines E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool triangles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if ((x+y)%2) {
                NVC_Fill_Triangle(oc,
                        Vec2D(x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT),
                        Vec2D(x*CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT),
                        Vec2D(x*CELL_WIDTH + CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT),
                        FOREGROUND_COLOR);
            } else {
                NVC_Draw_Triangle(oc,
                        Vec2D(x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT),
                        Vec2D(x*CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT),
                        Vec2D(x*CELL_WIDTH + CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT),
                        1,
                        FOREGROUND_COLOR);
            }
        }
    }

    float font_size = 24;
    NVC_Text(oc, "Triangles E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool alpha_blending_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    NVC_Fill_Background(oc, BACKGROUND_COLOR);
    NVC_Fill_Rectangle(oc, Vec2D(0, 0), Vec2D((float)WIDTH*2/3, (float)HEIGHT*2/3), 0x880000FF);
    NVC_Fill_Rectangle(oc, Vec2D((float)WIDTH, (float)HEIGHT), Vec2D((float)-WIDTH*2/3, (float)-HEIGHT*2/3), 0x22FF0000);

    float font_size = 24;
    NVC_Text(oc, "Color Blending E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool subcanvas_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    NVC_Canvas sub_oc;

    NVC_GetSubCanvas(&sub_oc, oc, Vec2D((float)WIDTH/4, (float)HEIGHT/4), Vec2D((float)WIDTH/2, (float)HEIGHT/2));

    NVC_Fill_Background(sub_oc, 0xFF5050FF);
    NVC_Fill_Rectangle(sub_oc, Vec2D(0, 0), Vec2D((float)WIDTH/4, (float)HEIGHT/3), 0xFF2020AA);

    float font_size = 24;
    NVC_Text(oc, "Subcanvas E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool render_3d_example(const char *file_path)
{
    float PI = 3.14159265358979323846;
    float angle = PI / 6;

    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    int GRID_COUNT = 10;
    float GRID_PAD = 1.f/GRID_COUNT;
    float GRID_SIZE = ((GRID_COUNT - 1)*GRID_PAD);
    float FOV = 1;

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
    NVC_Text(oc, "3d Rendering E.X.", Vec2D(10, 10), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

bool text_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float font_size = 25;
    float line_pad = 5;
    NVC_Text(oc, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", Vec2D(0, 0), default_font, font_size, 0xFFFFFFFF);
    NVC_Text(oc, "abcdefghijklmnopqrstuvwxyz", Vec2D(0, font_size + line_pad), default_font, font_size, 0xFFFFFFFF);
    NVC_Text(oc, "1234567890 !\"#$%&'()*+,-./", Vec2D(0, 2*(font_size + line_pad)), default_font, font_size, 0xFFFFFFFF);
    NVC_Text(oc, ":;<=>?@[\\]^_`{|}~", Vec2D(0, 3*(font_size + line_pad)), default_font, font_size, 0xFFFFFFFF);

    return save_as_png(file_path);
}

int main(void)
{
    oc.width = WIDTH;
    oc.height = HEIGHT;
    oc.stride = WIDTH;
    oc.pixels = (uint32_t*)malloc(sizeof(uint32_t) * WIDTH * HEIGHT);

    if (!blank_example("examples/blank.png")) return -1;
    if (!rectangles_example("examples/rectangles.png")) return -1;
    if (!circles_example("examples/circles.png")) return -1;
    if (!lines_example("examples/lines.png")) return -1;
    if (!triangles_example("examples/triangles.png")) return -1;
    if (!alpha_blending_example("examples/alpha_blending.png")) return -1;
    if (!subcanvas_example("examples/subcanvas.png")) return -1;
    if (!render_3d_example("examples/3d.png")) return -1;
    if (!text_example("examples/text.png")) return -1;

    return 0;
}
