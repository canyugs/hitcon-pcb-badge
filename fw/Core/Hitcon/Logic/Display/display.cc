#include "display.h"
#include <Logic/Display/editor.h>
#include <Logic/Display/font.h>
#include <string.h>

static uint8_t display_buf[DISPLAY_HEIGHT][DISPLAY_WIDTH];

static int display_mode;
static int
    display_current_frame; // will be updated when display_get_frame is called
static hitcon::TextEditorDisplay *text_editor_display;
static int display_orientation = 0; // 0: normal, 1: upside down

// TODO: use union to save memory if we want to store other info for other modes
struct {
  uint8_t buf[DISPLAY_HEIGHT * DISPLAY_SCROLL_MAX_COLUMNS];
  int first_frame;
  int cols;
  int speed;
} display_scroll_info;

void get_scroll_frame(uint8_t *buf, int frame) {
  /**
   * The content will scroll from right to left, and the first frame of the
   * scrolling is an empty screen.
   *
   * In the following illustration:
   * s = speed
   * T = (DISPLAY_WIDTH + cols + 1) * speed
   * (which is the period of the scrolling)
   *
   *                      +------------------------------------+
   * scroll buffer        |                                    |
   *                      +------------------------------------+
   * frame = [0, s)  +---+
   * display buffer  |   |
   *                 +---+
   * frame = [s, 2s)  +---+
   * display buffer   |   |
   *                  +---+
   * frame = [2s, 3s)  +---+
   * display buffer    |   |
   *                   +---+
   * ...
   * frame = [T - 3s, T - 2s)                                 +---+
   * display buffer                                           |   |
   *                                                          +---+
   * frame = [T - 2s, T - s)                                   +---+
   * display buffer                                            |   |
   *                                                           +---+
   * frame = [T - s, T)                                         +---+
   * display buffer                                             |   |
   *                                                            +---+
   */

  int total_width = DISPLAY_WIDTH + display_scroll_info.cols + 1;
  int period = total_width * display_scroll_info.speed;
  int x_at_frame0 = -DISPLAY_WIDTH;
  int current_x = x_at_frame0 + (frame - display_scroll_info.first_frame) %
                                    period / display_scroll_info.speed;

  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      buf[y * DISPLAY_WIDTH + x] =
          (0 <= current_x + x && current_x + x < display_scroll_info.cols)
              ? display_scroll_info
                    .buf[y * display_scroll_info.cols + current_x + x]
              : 0;
    }
  }
}

void display_init() {
  display_mode = DISPLAY_MODE_BLANK;
  memset(display_buf, 0, sizeof(display_buf));
}

void display_toggle_orientation() {
  display_orientation = 1 - display_orientation;
}

void display_get_frame(uint8_t *buf, int frame) {
  switch (display_mode) {
  case DISPLAY_MODE_BLANK:
    memset(buf, 0, sizeof(display_buf));
    break;

  case DISPLAY_MODE_FIXED:
    memcpy(buf, display_buf, sizeof(display_buf));
    break;

  case DISPLAY_MODE_SCROLL:
    get_scroll_frame(buf, frame);
    break;

  case DISPLAY_MODE_TEXT_EDITOR:
    text_editor_display->draw(buf, frame);
    break;
  }

  if (display_orientation) {
    uint8_t tmp[DISPLAY_HEIGHT][DISPLAY_WIDTH];
    for (int y = 0; y < DISPLAY_HEIGHT; y++) {
      for (int x = 0; x < DISPLAY_WIDTH; x++) {
        tmp[y][x] = buf[sizeof(tmp) - 1 - (y * DISPLAY_WIDTH + x)];
      }
    }
    memcpy(buf, tmp, sizeof(tmp));
  }

  display_current_frame = frame;
}

void display_set_mode_blank() {
  display_mode = DISPLAY_MODE_BLANK;
  memset(display_buf, 0, sizeof(display_buf));
}

void display_set_mode_fixed(uint8_t *buf) {
  display_mode = DISPLAY_MODE_FIXED;
  memcpy(display_buf, buf, sizeof(display_buf));
}

void display_set_mode_scroll(uint8_t *buf, int cols, int speed) {
  display_mode = DISPLAY_MODE_SCROLL;
  display_scroll_info.first_frame = display_current_frame;
  display_scroll_info.cols = cols;
  display_scroll_info.speed = speed;
  memcpy(display_scroll_info.buf, buf, DISPLAY_HEIGHT * cols);
}

void display_set_mode_scroll_text(const char *text, int speed) {
  uint8_t buf[DISPLAY_HEIGHT][DISPLAY_SCROLL_MAX_COLUMNS];
  int len = strlen(text);
  for (int i = 0; i < len && i * CHAR_WIDTH < DISPLAY_SCROLL_MAX_COLUMNS; ++i) {
    render_char(buf, text[i], i * CHAR_WIDTH, 0, DISPLAY_SCROLL_MAX_COLUMNS,
                DISPLAY_HEIGHT);
  }
  int cols = (len * CHAR_WIDTH > DISPLAY_SCROLL_MAX_COLUMNS)
                 ? DISPLAY_SCROLL_MAX_COLUMNS
                 : len * CHAR_WIDTH;

  uint8_t buf_1d[DISPLAY_HEIGHT * DISPLAY_SCROLL_MAX_COLUMNS];
  for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
    for (int x = 0; x < cols; ++x) {
      buf_1d[y * cols + x] = buf[y][x];
    }
  }

  display_set_mode_scroll(buf_1d, cols, speed);
}

void display_set_mode_scroll_text(const char *text) {
  display_set_mode_scroll_text(text, DISPLAY_SCROLL_DEFAULT_SPEED);
}

void display_set_mode_editor(hitcon::TextEditorDisplay *editor) {
  display_mode = DISPLAY_MODE_TEXT_EDITOR;
  text_editor_display = editor;
}
