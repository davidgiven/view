#include <stdint.h>
typedef uint16_t addr_t;
/* Test for justify_edit_buffer (justification) */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef uint16_t addr_t;

extern uint8_t ram[65536];
extern uint8_t justifying_flag;
extern uint8_t ruler_left_stop;
extern uint8_t ruler_right_stop;
extern uint8_t column_position, justify_gap_count;
extern uint8_t scratch_index, screen_row, screen_column, temp_save;
extern uint8_t print_xpos;
extern uint8_t input_buffer_offset;
extern uint8_t* current_format_line_ptr;
extern uint8_t* current_line_ptr;
extern uint8_t* edit_buffer_base; // was ptr1
extern uint8_t* scratch_scan_ptr; // was tmp89
extern uint8_t output_buffer[132];
extern uint8_t input_buffer[68];

void justify_edit_buffer(void);

#define BUFFER_ADDR 0x0548 /* RAM_EDIT_BUFFER */

static int test_failures;

#define ASSERT(cond, msg)                      \
    do                                         \
    {                                          \
        if (!(cond))                           \
        {                                      \
            fprintf(stderr, "FAIL %s\n", msg); \
            test_failures++;                   \
        }                                      \
    } while (0)

static void setup_edit_buffer(const char* text)
{
    memset(ram, 0, sizeof(ram));
    int len = strlen(text);
    for (int i = 0; i < len; i++)
        ram[BUFFER_ADDR + i] = (uint8_t)text[i];
    for (int i = len; i < 200; i++)
        ram[BUFFER_ADDR + i] = 0x10;
}

static void init_globals(const char* text, uint8_t jf, uint8_t rstop)
{
    setup_edit_buffer(text);
    current_format_line_ptr = &ram[BUFFER_ADDR];
    current_line_ptr = &ram[BUFFER_ADDR];
    edit_buffer_base = &ram[BUFFER_ADDR]; // was ptr1

    justifying_flag = jf;
    ruler_left_stop = 0;
    ruler_right_stop = rstop;

    column_position = justify_gap_count = 0;
    scratch_index = screen_row = screen_column = temp_save = 0;
    scratch_scan_ptr = NULL; // was tmp89
    print_xpos = 4;
    input_buffer_offset = 0;
    memset(output_buffer, 0, sizeof(output_buffer));
    memset(input_buffer, 0, sizeof(input_buffer));
}

static void run_justify(const char* text, uint8_t rstop)
{
    init_globals(text, 0, rstop);
    justify_edit_buffer();

    int orig_len = strlen(text);
    int buf_len = 0;
    for (int i = 0; i < 132; i++)
    {
        if (ram[BUFFER_ADDR + i] == 0x10)
            break;
        buf_len++;
    }

    int total_extra = 0;
    for (int i = 0; i < justify_gap_count; i++)
        total_extra += input_buffer[i];

    ASSERT(justify_gap_count > 1, "justification found word gaps");
    ASSERT(buf_len > orig_len, "buffer expanded beyond original text");
    ASSERT(total_extra > 0, "extra spaces were distributed");
}

int main(void)
{
    test_failures = 0;

    run_justify("The quick brown fox jumps over", 40);

    run_justify("The quick brown fox jumps over", 30);

    {
        init_globals("The quick brown fox jumps over", 0xFF, 40);
        justify_edit_buffer();
        ASSERT(justify_gap_count == 0, "justification skipped when flag != 0");
    }

    if (test_failures)
        printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
