#include "matrix_rain.h"
#include "screen.h" // for draw_filled_rect(), display functions
#include "font5x7.h"
#include <stdlib.h>
#include "color_scheme.h"

#define NUM_COLUMNS 40
#define CHAR_WIDTH 6
#define SCREEN_HEIGHT 320
#define SCREEN_WIDTH 240

typedef struct
{
    int y;
    char c;
} FallingChar;

static FallingChar rain[NUM_COLUMNS];

void draw_char5x7(char c, int x, int y, uint16_t color, uint16_t bg)
{
    if (c < 32 || c > 126)
        return;
    const uint8_t *bitmap = font5x7[c - 32];

    for (int col = 0; col < 5; col++)
    {
        uint8_t line = bitmap[col];
        for (int row = 0; row < 7; row++)
        {
            int pixel_x = x + col;
            int pixel_y = y + row;
            uint16_t px_color = (line & (1 << row)) ? color : bg;
            draw_filled_rect(pixel_x, pixel_y, 1, 1, px_color);
        }
    }
}

void init_rain(void)
{
    for (int i = 0; i < NUM_COLUMNS; i++)
    {
        rain[i].y = rand() % SCREEN_HEIGHT;
        rain[i].c = 32 + rand() % (126 - 32);
    }
}

void update_rain(void)
{
    for (int i = 0; i < NUM_COLUMNS; i++)
    {
        int x = i * CHAR_WIDTH;
        int y = rain[i].y;

        // Clear old character
        draw_filled_rect(x, y, CHAR_WIDTH, 7, 0x0000); // black

        // Move down
        rain[i].y += 8;
        if (rain[i].y > SCREEN_HEIGHT)
        {
            rain[i].y = 0;
            rain[i].c = 32 + rand() % (126 - 32);
        }

        // Draw new
        draw_char5x7(rain[i].c, x, rain[i].y, 0x07E0, 0x0000); // green
    }
}
