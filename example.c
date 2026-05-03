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

NVC_Canvas pixels = {0};

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
            uint32_t pixel = pixels.data[i];
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

bool blank_example(const char *file_path) {
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool rectangles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if ((x+y)%2) {
                NVC_Fill_Rectangle(pixels,
                        (Vec2D){ x*CELL_WIDTH, y*CELL_HEIGHT },
                        (Vec2D){ CELL_WIDTH, CELL_HEIGHT },
                        FOREGROUND_COLOR);
            } else {
                NVC_Fill_Rectangle(pixels,
                        (Vec2D){ x*CELL_WIDTH, y*CELL_HEIGHT },
                        (Vec2D){ CELL_WIDTH, CELL_HEIGHT },
                        BACKGROUND_COLOR);
            }
        }

    }

    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool circles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            float u = (float)x / COLS;
            float v = (float)y / ROWS;
            float t = (u + v)/2;

            float radius = CELL_WIDTH;
            if (CELL_HEIGHT < radius) radius = CELL_HEIGHT;
            NVC_Fill_Circle(pixels,
                            (Vec2D){ x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT + (float)CELL_HEIGHT/2 },
                            lerpf(0, radius/2, t),
                            FOREGROUND_COLOR);
        }
    }

    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool lines_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    NVC_Draw_Line(pixels, (Vec2D){ 0, 0 },            (Vec2D){ WIDTH, HEIGHT },   0xFF0000FF);
    NVC_Draw_Line(pixels, (Vec2D){ 0, HEIGHT },       (Vec2D){ WIDTH, 0 },        0xFF0000FF);

    NVC_Draw_Line(pixels, (Vec2D){ 0, 0 },            (Vec2D){ (float)WIDTH/2, HEIGHT }, 0xFF00FF00);
    NVC_Draw_Line(pixels, (Vec2D){ 0, HEIGHT },       (Vec2D){ (float)WIDTH/2, 0 },      0xFF00FF00);

    NVC_Draw_Line(pixels, (Vec2D){ (float)WIDTH/2, 0 },      (Vec2D){ WIDTH, HEIGHT },   0xFF00FF00);
    NVC_Draw_Line(pixels, (Vec2D){ (float)WIDTH/2, HEIGHT }, (Vec2D){ WIDTH, 0 },        0xFF00FF00);

    NVC_Draw_Line(pixels, (Vec2D){ 0, (float)HEIGHT/2 },     (Vec2D){ WIDTH, (float)HEIGHT/2 }, 0xFFFFFF00);
    NVC_Draw_Line(pixels, (Vec2D){ (float)WIDTH/2, 0 },      (Vec2D){ (float)WIDTH/2, HEIGHT }, 0xFFFFFF00);

    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool triangles_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            NVC_Fill_Triangle(pixels,
                    (Vec2D) { x*CELL_WIDTH + (float)CELL_WIDTH/2, y*CELL_HEIGHT },
                    (Vec2D) { x*CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT },
                    (Vec2D) { x*CELL_WIDTH + CELL_WIDTH, y*CELL_HEIGHT + CELL_HEIGHT },
                    FOREGROUND_COLOR);
        }

    }

    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

bool alpha_blending_example(const char *file_path)
{
    // 0xAABBGGRR
    NVC_Fill_Background(pixels, BACKGROUND_COLOR);

    NVC_Fill_Background(pixels, BACKGROUND_COLOR);
    NVC_Fill_Rectangle(pixels, (Vec2D) { 0, 0 }, (Vec2D) { (float)WIDTH*2/3, (float)HEIGHT*2/3 }, 0x880000FF);
    NVC_Fill_Rectangle(pixels, (Vec2D) { (float)WIDTH, (float)HEIGHT }, (Vec2D) { (float)-WIDTH*2/3, (float)-HEIGHT*2/3 }, 0x22FF0000);


    if (!stbi_write_png(file_path, pixels.width, pixels.height, 4, pixels.data, pixels.width*sizeof(uint32_t))) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    printf("INFO: save file %s\n", file_path);
    return true;
}

int main(void)
{
    pixels.width = WIDTH;
    pixels.height = HEIGHT;
    pixels.stride = WIDTH;
    pixels.data = (uint32_t*)malloc(sizeof(uint32_t) * 800 * 600);

    if (!blank_example("examples/blank.png")) return -1;
    if (!rectangles_example("examples/rectangles.png")) return -1;
    if (!circles_example("examples/circles.png")) return -1;
    if (!lines_example("examples/lines.png")) return -1;
    if (!triangles_example("examples/triangles.png")) return -1;
    if (!alpha_blending_example("examples/alpha_blending.png")) return -1;
    return 0;
}
