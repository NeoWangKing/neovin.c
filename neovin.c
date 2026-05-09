// Copyright 2026 Neo Wang <3245938285@qq.com>
// MIT License

#ifndef NEOVIN_C_
#define NEOVIN_C_

#include <math.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef NEOVINCDEF
#define NEOVINCDEF static inline
#endif

// Anti-aliasing
#ifndef NVC_AA_RES
#define NVC_AA_RES 4
#endif
#define NVC_AA_PAD (1./NVC_AA_RES)

#ifndef NVC_CV_OY
#define NVC_CV_OY 0
#endif

#ifndef NVC_CV_OX
#define NVC_CV_OX 0
#endif

#define NVC_SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define NVC_SIGN(T, x) ((T)((x) > 0) - (T)((x) < 0))
#define NVC_ABS(T, x) (NVC_SIGN(T, x)*(x))

typedef struct {
    int width, height;
    const char *glyphs;
} NVC_Font;

#define DEFAULT_FONT_HEIGHT 8
#define DEFAULT_FONT_WIDTH 5
char default_font_glyphs[128][DEFAULT_FONT_HEIGHT][DEFAULT_FONT_WIDTH] = {
    [' '] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['!'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['"'] = {
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['#'] = {
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['$'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['%'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['&'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 1},
        {1, 0, 1, 1, 0},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['\''] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['('] = {
        {0, 0, 0, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0},
    },
    [')'] = {
        {1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['*'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['+'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    [','] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['-'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['.'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['/'] = {
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['0'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['1'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['2'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['3'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['4'] = {
        {0, 0, 0, 1, 1},
        {0, 0, 1, 0, 1},
        {0, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['5'] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['6'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['7'] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['8'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['9'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    [':'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    [';'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['<'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['='] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['>'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['?'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['@'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['['] = {
        {0, 1, 1, 1, 1},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['\\'] = {
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    [']'] = {
        {1, 1, 1, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['^'] = {
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['_'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['`'] = {
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['{'] = {
        {0, 0, 0, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['|'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['}'] = {
        {1, 1, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['~'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 1, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['a'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['b'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['c'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['d'] = {
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['e'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['f'] = {
        {0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['g'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
    },
    ['h'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['i'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['j'] = {
        {0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
    },
    ['k'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['l'] = {
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 1, 0, 0, 0},
        {0, 0, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['m'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['n'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['o'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['p'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
    },
    ['q'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
    },
    ['r'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['s'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['t'] = {
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['u'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['v'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['w'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['x'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['y'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
    },
    ['z'] = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['A'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['B'] = {
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['C'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['D'] = {
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['E'] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['F'] = {
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['G'] = {
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 0, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['H'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['I'] = {
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['J'] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['K'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 0, 0},
        {1, 0, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['L'] = {
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
    ['M'] = {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['N'] = {
        {1, 0, 0, 0, 1},
        {1, 1, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 0, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['O'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['P'] = {
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['Q'] = {
        {0, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0},
        {0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['R'] = {
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['S'] = {
        {0, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['T'] = {
        {1, 1, 1, 1, 1},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['U'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0},
    },
    ['V'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['W'] = {
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {1, 1, 0, 1, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['X'] = {
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0},
    },
    ['Y'] = {
        {1, 0, 0, 0, 1},
        {0, 1, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0},
    },
    ['Z'] = {
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0},
    },
};

static NVC_Font default_font = {
    .glyphs = &default_font_glyphs[0][0][0],
    .width = DEFAULT_FONT_WIDTH,
    .height = DEFAULT_FONT_HEIGHT,
};

typedef struct {
    int height;
    int width;
    int stride;
    uint32_t *pixels;
} NVC_Canvas;

#define NVC_CANVAS_NULL ((NVC_Canvas) {0})
#define NVC_PIXEL(oc, x, y) (oc).pixels[((int)(y)+NVC_CV_OY)*(oc).stride + (int)(x) + NVC_CV_OX]
#define NVC_CANVAS(data, w, h) (NVC_Canvas) { .height = (h), .width = (w), .stride = (w), .pixels = (data) }

NEOVINCDEF NVC_Canvas NVC_Make_Canvas(uint32_t *pixels, int width, int height)
{
    NVC_Canvas oc = {
        .pixels = pixels,
        .width = width,
        .height = height,
        .stride = width,
    };
    return oc;
}

typedef struct {
    float x;
    float y;
} Vec2D;
#define Vec2D(x, y) ((Vec2D){ (float)(x), (float)(y) })

typedef struct {
    float x;
    float y;
    float z;
} Vec3D;
#define Vec3D(x, y, z) ((Vec3D){ (float)(x), (float)(y), (float)(z) })

NEOVINCDEF void NVC_Rotate_Point(Vec3D *p, Vec3D refp, Vec3D axis, float angle)
{
    // 1. 计算相对向量
    float vx = p->x - refp.x;
    float vy = p->y - refp.y;
    float vz = p->z - refp.z;

    // 2. 归一化旋转轴
    float len_dir = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    if (len_dir < 0.0001f) return; // 轴退化，无法旋转
    float ux = axis.x / len_dir;
    float uy = axis.y / len_dir;
    float uz = axis.z / len_dir;

    // 3. 罗德里格斯旋转公式
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float one_minus_cos = 1.0f - cos_a;

    // 叉积 u × v
    float cx = uy * vz - uz * vy;
    float cy = uz * vx - ux * vz;
    float cz = ux * vy - uy * vx;

    // 点积 u · v
    float dot = ux * vx + uy * vy + uz * vz;

    // 组合三项
    float rx = vx * cos_a + cx * sin_a + ux * dot * one_minus_cos;
    float ry = vy * cos_a + cy * sin_a + uy * dot * one_minus_cos;
    float rz = vz * cos_a + cz * sin_a + uz * dot * one_minus_cos;

    // 4. 平移回原始参考点
    p->x = refp.x + rx;
    p->y = refp.y + ry;
    p->z = refp.z + rz;
}

NEOVINCDEF bool NVC_Is_In_CV(NVC_Canvas oc, Vec2D p)
{
    float ax = p.x+NVC_CV_OX;
    float ay = p.y+NVC_CV_OY;
    if (ax >= 0 && ax < oc.width && ay >= 0 && ay < oc.height) return true;
    return false;
}

NEOVINCDEF bool NVC_Normalize_Range(NVC_Canvas oc, Vec2D p, Vec2D s, int *x1, int *x2, int *y1, int *y2)
{
    float x_abs = p.x+NVC_CV_OX;
    float y_abs = p.y+NVC_CV_OY;
    *x1 = (int)x_abs;
    *y1 = (int)y_abs;

    // Convert the rectangle to 2-points representation
    *x2 = *x1 + NVC_SIGN(float, s.x)*(NVC_ABS(float, s.x) - 1);
    if (*x1 > *x2) NVC_SWAP(int, *x1, *x2);
    *y2 = *y1 + NVC_SIGN(float, s.y)*(NVC_ABS(float, s.y) - 1);
    if (*y1 > *y2) NVC_SWAP(int, *y1, *y2);

    // Cull out invisible rectangle
    if (*x1 >= (int) oc.width) return false;
    if (*x2 < 0) return false;
    if (*y1 >= (int) oc.height) return false;
    if (*y2 < 0) return false;

    // Clamp the rectangle to the boundaries
    if (*x1 < 0) *x1 = 0;
    if (*x2 >= (int) oc.width) *x2 = (int) oc.width - 1;
    if (*y1 < 0) *y1 = 0;
    if (*y2 >= (int) oc.height) *y2 = (int) oc.height - 1;

    *x1 -= (int)NVC_CV_OX;
    *x2 -= (int)NVC_CV_OX;
    *y1 -= (int)NVC_CV_OY;
    *y2 -= (int)NVC_CV_OY;

    return true;
}

NEOVINCDEF void NVC_GetSubCanvas(NVC_Canvas *dst, NVC_Canvas src, Vec2D p, Vec2D s)
{
    // 处理负尺寸，翻转方向
    if (s.x < 0) { p.x += s.x; s.x = -s.x; }
    if (s.y < 0) { p.y += s.y; s.y = -s.y; }

    // 裁剪到源画布内
    float x_abs = p.x+NVC_CV_OX;
    float y_abs = p.y+NVC_CV_OY;
    int x0 = (int)x_abs;
    int y0 = (int)y_abs;
    int x1 = x0 + (int)s.x;
    int y1 = y0 + (int)s.y;

    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > src.width)  x1 = src.width;
    if (y1 > src.height) y1 = src.height;

    dst->width  = x1 - x0;
    dst->height = y1 - y0;
    dst->stride = src.stride;          // 保持源画布的步幅
    dst->pixels   = &src.pixels[y0 * src.stride + x0];
}

NEOVINCDEF float NVC_Vec2D_Length(Vec2D vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y);
}

NEOVINCDEF float NVC_Vec2D_Angle(Vec2D vec)
{
    return atan2f(vec.y, vec.x);
}

NEOVINCDEF Vec2D NVC_Vec2D_Plus(Vec2D vec1, Vec2D vec2)
{
    return Vec2D(vec1.x + vec2.x, vec1.y + vec2.y);
}

NEOVINCDEF Vec2D NVC_Vec2D_Minus(Vec2D vec1, Vec2D vec2)
{
    return Vec2D(vec1.x - vec2.x, vec1.y - vec2.y);
}

NEOVINCDEF float NVC_Vec2D_Dot(Vec2D vec1, Vec2D vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y);
}

NEOVINCDEF Vec3D NVC_Vec2D_Cross(Vec2D vec1, Vec2D vec2)
{
    return Vec3D(0, 0, vec1.x*vec2.y - vec1.y*vec2.x);
}

// =============================================================

NEOVINCDEF float NVC_Vec3D_Length(Vec3D vec)
{
    return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

NEOVINCDEF Vec3D NVC_Vec3D_Plus(Vec3D vec1, Vec3D vec2)
{
    return Vec3D(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

NEOVINCDEF Vec3D NVC_Vec3D_Minus(Vec3D vec1, Vec3D vec2)
{
    return Vec3D(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

NEOVINCDEF float NVC_Vec3D_Dot(Vec3D vec1, Vec3D vec2)
{
    return (vec1.x*vec2.x + vec1.y*vec2.y + vec1.z*vec2.z);
}

NEOVINCDEF Vec3D NVC_Vec3D_Cross(Vec3D vec1, Vec3D vec2)
{
    return Vec3D(vec1.y*vec2.z - vec1.z*vec2.y, vec1.z*vec2.x - vec1.x*vec2.z, vec1.x*vec2.y - vec1.y*vec2.x);
}

typedef enum {
    COMP_RED = 0,
    COMP_GREEN,
    COMP_BLUE,
    COMP_ALPHA,
    COUNT_COMP
} COLOR;

NEOVINCDEF void NVC_Unpack_RGBA32(uint32_t color, uint8_t comp[COUNT_COMP])
{
    for (int i = 0; i < COUNT_COMP; ++i) {
        comp[i] = color&0xFF;
        color >>= 8;
    }
}

NEOVINCDEF uint32_t NVC_Pack_RGBA32(uint8_t comp[COUNT_COMP])
{
    uint32_t result = 0;
    for (int i = 0; i < COUNT_COMP; ++i) {
        result |= comp[i]<<(8*i);
    }
    return result;
}

NEOVINCDEF void NVC_Modi_Color(uint32_t *color, COLOR index, uint8_t value)
{
    uint8_t comp[COUNT_COMP];
    NVC_Unpack_RGBA32(*color, comp);
    comp[index] = value;
    *color = NVC_Pack_RGBA32(comp);
}

NEOVINCDEF void NVC_Transparent_Color(uint32_t *color, float alpha)
{
    uint8_t comp[COUNT_COMP];
    NVC_Unpack_RGBA32(*color, comp);
    comp[COMP_ALPHA] *= alpha;
    *color = NVC_Pack_RGBA32(comp);
}

NEOVINCDEF void NVC_Bright_Color(uint32_t *color, float rate)
{
    if (rate < 0.0f) rate = 0.0f;
    uint8_t comp[COUNT_COMP];
    NVC_Unpack_RGBA32(*color, comp);
    for (int i = 0; i < COMP_ALPHA; ++i) {
        float scaled = (float)comp[i] * rate;
        if (scaled > 255.0f) scaled = 255.0f;
        if (scaled < 0.0f)   scaled = 0.0f;
        comp[i] = (uint8_t)scaled;
    }
    *color = NVC_Pack_RGBA32(comp);
}

NEOVINCDEF uint32_t NVC_Mix_Color_Alpha(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    NVC_Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    NVC_Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = comp_t[COMP_ALPHA] + comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA])/255;
    if (comp_f[COMP_ALPHA] == 0) return 0x00000000;

    for (int i = 0; i < COMP_ALPHA; ++i) {
        uint32_t sum = (uint32_t)comp_t[i]*comp_t[COMP_ALPHA]*255
                     + (uint32_t)comp_b[i]*comp_b[COMP_ALPHA]*(255 - comp_t[COMP_ALPHA]);
        comp_f[i] = (uint8_t)(sum/ (255*comp_f[COMP_ALPHA]));
    }

    return NVC_Pack_RGBA32(comp_f);
}

NEOVINCDEF uint32_t NVC_Blend_Color_Normal(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    NVC_Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    NVC_Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i];
    }

    return NVC_Pack_RGBA32(comp_f);
}

NEOVINCDEF uint32_t NVC_Blend_Color_Darken(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    NVC_Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    NVC_Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i];
        if (comp_b[i] < comp_t[i]) comp_f[i] = comp_b[i];
    }

    return NVC_Pack_RGBA32(comp_f);
}

NEOVINCDEF uint32_t NVC_Blend_Color_Multiply(uint32_t color_b, uint32_t color_t)
{
    uint8_t comp_b[COUNT_COMP];
    NVC_Unpack_RGBA32(color_b, comp_b);

    uint8_t comp_t[COUNT_COMP];
    NVC_Unpack_RGBA32(color_t, comp_t);

    uint8_t comp_f[COUNT_COMP];

    comp_f[COMP_ALPHA] = 255;
    for (int i = 0; i < COMP_ALPHA; ++i) {
        comp_f[i] = comp_t[i] * comp_b[i] / 255;
    }

    return NVC_Pack_RGBA32(comp_f);
}

NEOVINCDEF void NVC_Set_Pixel(NVC_Canvas oc, Vec2D p, uint32_t color)
{
    // 将相对坐标转换为绝对坐标进行边界检查
    int ax = (int)p.x + NVC_CV_OX;
    int ay = (int)p.y + NVC_CV_OY;
    if (ax < 0 || ax >= oc.width || ay < 0 || ay >= oc.height) return;
    // 使用 NVC_PIXEL 进行写入
    NVC_PIXEL(oc, p.x, p.y) = color;
}

NEOVINCDEF void NVC_Draw_Pixel(NVC_Canvas oc, Vec2D p, uint32_t color)
{
    int ax = (int)p.x + NVC_CV_OX;
    int ay = (int)p.y + NVC_CV_OY;
    if (ax < 0 || ax >= oc.width || ay < 0 || ay >= oc.height) return;
    uint32_t* pixel = &NVC_PIXEL(oc, p.x, p.y);
    *pixel = NVC_Mix_Color_Alpha(*pixel, color);
}

NEOVINCDEF void NVC_Set_Background(NVC_Canvas oc, uint32_t color)
{
    for (int y = -NVC_CV_OY; y < oc.height-NVC_CV_OY; ++y) {
        for (int x = 0-NVC_CV_OX; x < oc.width-NVC_CV_OX; ++x) {
            NVC_Set_Pixel(oc, Vec2D(x, y), color);
        }
    }
}

NEOVINCDEF void NVC_Fill_Background(NVC_Canvas oc, uint32_t color)
{
    for (int y = -NVC_CV_OY; y < oc.height-NVC_CV_OY; ++y) {
        for (int x = 0-NVC_CV_OX; x < oc.width-NVC_CV_OX; ++x) {
            NVC_Draw_Pixel(oc, Vec2D(x, y), color);
        }
    }
}

NEOVINCDEF void NVC_Fill_Rectangle(NVC_Canvas oc, Vec2D p, Vec2D s, uint32_t color)
{
    float x_min = p.x;
    float x_max = x_min + s.x;
    if (x_min > x_max) NVC_SWAP(float, x_min, x_max);
    float y_min = p.y;
    float y_max = y_min + s.y;
    if (y_min > y_max) NVC_SWAP(float, y_min, y_max);

    int x1, y1, x2, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);

    const float d = 0.5f;

    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            if (y > y_min + d && y < y_max - d && x > x_min + d && x < x_max - d) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (ay >= y_min && ay <= y_max && ax >= x_min && ax <= x_max) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Rectangle(NVC_Canvas oc, Vec2D p, Vec2D s, float thick, uint32_t color)
{
    // 标准化矩形，计算外边界（边框外侧）和内边界（边框内侧）
    float x0 = p.x, x1 = p.x + s.x;
    if (x0 > x1) NVC_SWAP(float, x0, x1);
    float y0 = p.y, y1 = p.y + s.y;
    if (y0 > y1) NVC_SWAP(float, y0, y1);

    // 外矩形（边框外侧，扩展了线宽的一半）
    float outer_x0 = x0 - thick * 0.5f;
    float outer_y0 = y0 - thick * 0.5f;
    float outer_x1 = x1 + thick * 0.5f;
    float outer_y1 = y1 + thick * 0.5f;

    // 内矩形（边框内侧，即原始矩形区域）
    float inner_x0 = x0 + thick * 0.5f;
    float inner_y0 = y0 + thick * 0.5f;
    float inner_x1 = x1 - thick * 0.5f;
    float inner_y1 = y1 - thick * 0.5f;

    // 裁剪到画布
    int bx1, bx2, by1, by2;
    NVC_Normalize_Range(oc, Vec2D(outer_x0, outer_y0),
                        Vec2D(outer_x1 - outer_x0, outer_y1 - outer_y0),
                        &bx1, &bx2, &by1, &by2);

    for (int y = by1; y <= by2; ++y) {
        for (int x = bx1; x <= bx2; ++x) {
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD * 0.5f + sy * NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD * 0.5f + sx * NVC_AA_PAD;
                    // 子像素在外矩形内部，且不在内矩形内部 → 属于边框
                    if (ax >= outer_x0 && ax <= outer_x1 && ay >= outer_y0 && ay <= outer_y1) {
                        if (!(ax >= inner_x0 && ax <= inner_x1 && ay >= inner_y0 && ay <= inner_y1)) {
                            count++;
                        }
                    }
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, (float)count / (NVC_AA_RES * NVC_AA_RES));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Fill_Circle(NVC_Canvas oc, Vec2D p, float r, uint32_t color)
{
    r = NVC_ABS(float ,r);
    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(p.x-r-1, p.y-r-1), Vec2D(2*r+4, 2*r+4), &x1, &x2, &y1, &y2);

    const float d = 2*0.7071f;

    for (int y = y1; y <= y2; ++y) {
        float dy = y - p.y;
        for (int x = x1; x <= x2; ++x) {
            float dx = x - p.x;
            float dist = dx*dx + dy*dy;
            if (dist >= (r + d)*(r + d)) continue;
            if (dist <= (r - d)*(r - d)) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                float ady = ay - p.y;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    float adx = ax - p.x;
                    float adist = adx*adx + ady*ady;
                    if (adist <= r*r) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Point(NVC_Canvas oc, Vec2D p, float r, uint32_t color)
{
    NVC_Fill_Circle(oc, p, r, color);
}

NEOVINCDEF void NVC_Draw_Circle(NVC_Canvas oc, Vec2D p, float r, float thick, uint32_t color)
{
    float r_out = NVC_ABS(float ,r) + thick*0.5f;
    float r_in = r_out - thick;
    if (r_in > r_out) NVC_SWAP(float, r_in, r_out);
    if (r_in <= 0) {
        NVC_Fill_Circle(oc, p, r, color);
        return;
    }

    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(p.x-r_out-1, p.y-r_out-1), Vec2D(2*r_out+4, 2*r_out+4), &x1, &x2, &y1, &y2);

    const float d = 2*0.7071f;

    for (int y = y1; y <= y2; ++y) {
        float dy = y - p.y;
        for (int x = x1; x <= x2; ++x) {
            float dx = x - p.x;
            float dist = dx*dx + dy*dy;
            if (dist >= (r_out + d)*(r_out + d) || dist <= (r_in - d)*(r_in - d)) continue;
            if (dist <= (r_out - d)*(r_out - d) && dist >= (r_in + d)*(r_in + d)) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                float ady = ay - p.y;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    float adx = ax - p.x;
                    float adist = adx*adx + ady*ady;
                    if (adist <= r_out*r_out && adist >= r_in*r_in) count += 1;
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Line(NVC_Canvas oc, Vec2D p1, Vec2D p2, uint32_t color)
{
    int x1 = (int)p1.x;
    int y1 = (int)p1.y;
    int x2 = (int)p2.x;
    int y2 = (int)p2.y;

    int dx = NVC_ABS(int, x2 - x1);
    int dy = -NVC_ABS(int, y2 - y1);
    int sx = NVC_SIGN(int, x2 - x1);
    int sy = NVC_SIGN(int, y2 - y1);
    int err = dx + dy;

    while (1) {
        // 只在画布范围内画点
        NVC_Draw_Pixel(oc, Vec2D(x1, y1), color);

        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

NEOVINCDEF float NVC_Dist_To_Line(Vec2D p, Vec2D p1, Vec2D p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float len = sqrtf(dx*dx + dy*dy);
    float ux = dx / len;
    float uy = dy / len;
    float v1x = p.x - p1.x;
    float v1y = p.y - p1.y;
    float v2x = p.x - p2.x;
    float v2y = p.y - p2.y;
    float dot1 = v1x*ux + v1y*uy;
    float dot2 = v2x*ux + v2y*uy;
    if (dot1 < 0) {
        return v1x*v1x + v1y*v1y;
    }
    if (dot2 > 0) {
        return v2x*v2x + v2y*v2y;
    }
    float nx = v1x - ux*dot1;
    float ny = v1y - uy*dot1;
    return nx*nx + ny*ny;
}

NEOVINCDEF void NVC_Draw_Line_Ex(NVC_Canvas oc, Vec2D p1, Vec2D p2, float thick, uint32_t color)
{
    float half = thick * 0.5;
    int x_min = (int)floorf(fminf(p1.x, p2.x) - half);
    int y_min = (int)floorf(fminf(p1.y, p2.y) - half);
    int x_max = (int)ceilf(fmaxf(p1.x, p2.x) + half);
    int y_max = (int)ceilf(fmaxf(p1.y, p2.y) + half);

    const float d = 2*0.7071f;

    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);

    for (int x = x1; x <= x2; ++x) {
        for (int y = y1; y <= y2; ++y) {
            if (NVC_Dist_To_Line(Vec2D(x+0.5f, y+0.5f), p1, p2) > (half+d)) continue;
            if (NVC_Dist_To_Line(Vec2D(x+0.5f, y+0.5f), p1, p2) < (half-d)) {
                NVC_Draw_Pixel(oc, Vec2D(x, y), color);
                continue;
            }
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (NVC_Dist_To_Line(Vec2D(ax, ay), p1, p2) <= half) {
                        count += 1;
                    }
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
    
}

NEOVINCDEF bool NVC_IS_IN_TRIANGLE(Vec2D p, Vec2D p1, Vec2D p2, Vec2D p3)
{
    float v12x = p2.x - p1.x;
    float v12y = p2.y - p1.y;
    float v23x = p3.x - p2.x;
    float v23y = p3.y - p2.y;
    float v31x = p1.x - p3.x;
    float v31y = p1.y - p3.y;
    float v1x = p.x - p1.x;
    float v1y = p.y - p1.y;
    float v2x = p.x - p2.x;
    float v2y = p.y - p2.y;
    float v3x = p.x - p3.x;
    float v3y = p.y - p3.y;
    float cross1 = v1x*v12y - v1y*v12x;
    float cross2 = v2x*v23y - v2y*v23x;
    float cross3 = v3x*v31y - v3y*v31x;
    return (cross1*cross2 >= 0 && cross2*cross3 >= 0);
}

NEOVINCDEF void NVC_Fill_Triangle(NVC_Canvas oc, Vec2D p1, Vec2D p2, Vec2D p3, uint32_t color)
{
    int x_max = (int)p1.x;
    int y_max = (int)p1.y;
    if (p2.x > x_max) x_max = p2.x;
    if (p2.y > y_max) y_max = p2.y;
    if (p3.x > x_max) x_max = p3.x;
    if (p3.y > y_max) y_max = p3.y;
    int x_min = (int)p1.x;
    int y_min = (int)p1.y;
    if (p2.x < x_min) x_min = p2.x;
    if (p2.y < y_min) y_min = p2.y;
    if (p3.x < x_min) x_min = p3.x;
    if (p3.y < y_min) y_min = p3.y;
    int x1, x2, y1, y2;
    NVC_Normalize_Range(oc, Vec2D(x_min, y_min), Vec2D(x_max - x_min, y_max - y_min), &x1, &x2, &y1, &y2);
    for (int y = y1; y <= y2; ++y) {
        for (int x = x1; x <= x2; ++x) {
            int count = 0;
            for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                float ay = y + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD;
                for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                    float ax = x + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD;
                    if (NVC_IS_IN_TRIANGLE(Vec2D(ax, ay), p1, p2, p3)) {
                        count += 1;
                    }
                }
            }
            if (count > 0) {
                uint32_t pixel_color = color;
                NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
            }
        }
    }
}

NEOVINCDEF void NVC_Draw_Triangle(NVC_Canvas oc, Vec2D p1, Vec2D p2, Vec2D p3, float thick, uint32_t color)
{
    NVC_Draw_Line_Ex(oc, p1, p2, thick, color);
    NVC_Draw_Line_Ex(oc, p2, p3, thick, color);
    NVC_Draw_Line_Ex(oc, p3, p1, thick, color);
}

NEOVINCDEF void NVC_Text(NVC_Canvas oc, const char *text, Vec2D tp, NVC_Font font, float ts, uint32_t color)
{
    for (int i = 0; *text; ++i, ++text) {
        int gx = tp.x + i*(ts/font.height)*(font.width+1);
        int gy = tp.y;
        const char *glyph = &font.glyphs[(*text)*sizeof(char)*font.width*font.height];
        for (int dy = 0; (float)dy < ts; ++dy) {
            int y = gy + dy;
            for (int dx = 0; (float)dx < ts*font.width/font.height; ++dx) {
                int x = gx + dx;
                int count = 0;
                for (int sy = 0; sy < NVC_AA_RES; ++sy) {
                    float ry = (dy + NVC_AA_PAD*0.5 + sy*NVC_AA_PAD)*font.height/ts;
                    for (int sx = 0; sx < NVC_AA_RES; ++sx) {
                        float rx = (dx + NVC_AA_PAD*0.5 + sx*NVC_AA_PAD)*font.height/ts;
                        if (rx >= font.width || ry >= font.height) continue;
                        if (glyph[(int)ry*font.width + (int)rx]) count += 1;
                    }
                }
                if (count > 0) {
                    uint32_t pixel_color = color;
                    NVC_Transparent_Color(&pixel_color, ((float)count/(NVC_AA_RES*NVC_AA_RES)));
                    NVC_Draw_Pixel(oc, Vec2D(x, y), pixel_color);
                }
            }
        }
    }
}

NEOVINCDEF void NVC_Copy(NVC_Canvas dst, NVC_Canvas src)
{
    for (int y = 0; y < dst.height; ++y) {
        for (int x = 0; x < dst.width; ++x) {
            int ax = src.width*x/dst.width;
            int ay = src.height*y/dst.height;
            NVC_PIXEL(dst, x, y) = NVC_PIXEL(src, ax, ay);
        }
    }
}

#endif // NEOVIN_C_

// TODO: Font loadingp
// TODO: Texture loadingp
// TODO: 3D Drawing
