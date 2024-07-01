#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

/**
 * The underlying buffer is a 2D array of uint8_t like this:
 *
 * buf[0][0] buf[0][1] ... buf[0][DISPLAY_WIDTH - 1]
 * buf[1][0] buf[1][1] ... buf[1][DISPLAY_WIDTH - 1]
 * ...
 * buf[DISPLAY_HEIGHT - 1][0] ... buf[DISPLAY_HEIGHT - 1][DISPLAY_WIDTH - 1]
 */

#define DISPLAY_HEIGHT 8 // fixed
#define DISPLAY_WIDTH 16 // adjustable, maybe 8, 12, 16

#define DISPLAY_MODE_BLANK 0
#define DISPLAY_MODE_FIXED 1
#define DISPLAY_MODE_SCROLL 2
#define DISPLAY_MODE_TEXT_EDITOR 3

#define DISPLAY_SCROLL_MAX_COLUMNS 100
#define DISPLAY_SCROLL_DEFAULT_SPEED 15

void display_init();

// Toggle the orientation of the display.
void display_toggle_orientation();

// Get the frame of the display at the given frame.
// The size of `buf` should be DISPLAY_HEIGHT * DISPLAY_WIDTH
void display_get_frame(uint8_t *buf, int frame);

void display_set_mode_blank();

// size of `buf` should be DISPLAY_HEIGHT * DISPLAY_WIDTH
void display_set_mode_fixed(uint8_t *buf);

// size of `buf` should be DISPLAY_HEIGHT * `cols`
// maximum `cols` is DISPLAY_SCROLL_MAX_COLUMNS
// `speed` means how many frames to move one pixel
void display_set_mode_scroll(uint8_t *buf, int cols, int speed);

// Rasterize `text` to the underlying display buffer.
// If the text is too long, the output will be truncated.
void display_set_mode_scroll_text(const char *text, int speed);
void display_set_mode_scroll_text(const char *text);

namespace hitcon {
class TextEditorDisplay;
}

// Set the display to the text editor mode. The logic is outsourced to the
// `TextEditorDisplay` class.
void display_set_mode_editor(hitcon::TextEditorDisplay *editor);

#endif
