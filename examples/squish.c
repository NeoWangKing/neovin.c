#define WIDTH 800
#define HEIGHT 600
#define SCALE_DOWN_FACTOR 10
#include "./tools/platform.c"

// #include "./imgs/neowang.c"
#include "./imgs/amiya.c"

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
    angle += 6 * M_PI * dt;

    NVC_Texture texture;
    texture.pixels = amiya_data;
    texture.width = amiya_width;
    texture.height = amiya_height;
    texture.stride = amiya_width;
    // texture.data = neowang_data;
    // texture.width = neowang_width;
    // texture.height = neowang_height;
    // texture.stride = neowang_width;


    NVC_Canvas oc = NVC_CANVAS(pixels, WIDTH, HEIGHT, WIDTH);
    NVC_Fill_Background(oc, BACKGROUND_COLOR);

    float w = (float)WIDTH/(1+0.2*sinf(angle));
    float h = (float)HEIGHT*(1+0.2*sinf(angle));
    w /= 1.5;
    h /= 1.5;
    float x = (float)WIDTH/2 - w/2;
    float y = (float)HEIGHT/2 - h/2;
    NVC_Draw_Texture(oc, texture, Vec2D(x, y), Vec2D(w, h));

    float font_size = 24;
    NVC_Text(oc, "Texture Rendering E.X.", Vec2D(10, 10), NVC_default_font, font_size, 0xFFFFFFFF);
    return pixels;
}
