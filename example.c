#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "neovin.c"

#define WIDTH 800
#define HEIGHT 600
#define ROWS 6
#define COLS 8
#define CELL_HEIGHT 100
#define CELL_WIDTH 100

#define BACKGROUND_COLOR 0xFF181818
#define FOREGROUND_COLOR 0xFF5050FF



static uint32_t pixels[HEIGHT*WIDTH];

float lerpf(float a, float b, float t)
{
    return a + (b - a)*t;
}

bool blank_example(void)
{
    // 0xAABBGGRR
    NVC_Fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    const char *file_path = "blank.ppm";
    Errno err = NVC_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
    if (err) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    return true;
}

bool rectangle_example(void)
{
    // 0xAABBGGRR
    NVC_Fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if ((x+y)%2) {
                NVC_Fill_Rectangle(pixels, WIDTH, HEIGHT, x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, FOREGROUND_COLOR);
            } else {
                NVC_Fill_Rectangle(pixels, WIDTH, HEIGHT, x*CELL_WIDTH, y*CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT, BACKGROUND_COLOR);
            }
        }

    }

    const char *file_path = "rectangle.ppm";
    Errno err = NVC_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
    if (err) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    return true;
}

bool circle_example(void)
{
    // 0xAABBGGRR
    NVC_Fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            float u = (float)x / COLS;
            float v = (float)y / ROWS;
            float t = (u + v)/2;

            float radius = CELL_WIDTH;
            if (CELL_HEIGHT < radius) radius = CELL_HEIGHT;
            NVC_Fill_Circle(pixels, WIDTH, HEIGHT,
                            x*CELL_WIDTH + CELL_WIDTH/2, y*CELL_HEIGHT + CELL_HEIGHT/2, lerpf(0, radius/2, t),
                            FOREGROUND_COLOR);
        }
    }

    const char *file_path = "circle.ppm";
    Errno err = NVC_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
    if (err) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    return true;
}

bool lines_example(void)
{
    // 0xAABBGGRR
    NVC_Fill(pixels, WIDTH, HEIGHT, BACKGROUND_COLOR);

    NVC_Draw_Line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, 0xFF0000FF);
    NVC_Draw_Line(pixels, WIDTH, HEIGHT, 0, HEIGHT, WIDTH, 0, 0xFF0000FF);

    NVC_Draw_Line(pixels, WIDTH, HEIGHT, 0, 0, WIDTH/2, HEIGHT, 0xFF00FF00);
    NVC_Draw_Line(pixels, WIDTH, HEIGHT, 0, HEIGHT, WIDTH/2, 0, 0xFF00FF00);

    NVC_Draw_Line(pixels, WIDTH, HEIGHT, WIDTH/2, 0, WIDTH, HEIGHT, 0xFF00FF00);
    NVC_Draw_Line(pixels, WIDTH, HEIGHT, WIDTH/2, HEIGHT, WIDTH, 0, 0xFF00FF00);

    NVC_Draw_Line(pixels, WIDTH, HEIGHT, 0, HEIGHT/2, WIDTH, HEIGHT/2, 0xFFFF0000);
    NVC_Draw_Line(pixels, WIDTH, HEIGHT, WIDTH/2, 0, WIDTH/2, HEIGHT, 0xFFFF0000);

    const char *file_path = "lines.ppm";
    Errno err = NVC_save_to_ppm_file(pixels, WIDTH, HEIGHT, file_path);
    if (err) {
        fprintf(stderr, "ERROR: could not save file %s: %s\n", file_path, strerror(errno));
        return false;
    }

    return true;
}

int main(void)
{
    if (!blank_example()) return -1;
    if (!rectangle_example()) return -1;
    if (!circle_example()) return -1;
    if (!lines_example()) return -1;
    return 0;
}
