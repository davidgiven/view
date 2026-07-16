/* Test for justify_edit_buffer (justification) */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef uint16_t addr_t;

extern uint8_t ram[65536];
extern uint8_t a, x, y, flags;
extern uint8_t justifying_flag;
extern uint8_t ruler_left_stop;
extern uint8_t ruler_right_stop;
extern uint8_t l0039, l0042, l0043, l0044, l0045, l0046;
extern uint8_t l0081, l0082, l0083, l0084;
extern uint8_t print_xpos;
extern uint8_t input_buffer_offset;
extern addr_t current_edit_line_ptr;
extern addr_t current_format_line_ptr;
extern addr_t current_line_ptr;
extern addr_t ptr1;
extern uint8_t tmp8, tmp9;
extern uint8_t output_buffer[132];
extern uint8_t input_buffer[68];

void justify_edit_buffer(void);

#define BUFFER_ADDR 0x8000

static int test_failures;

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s\n", msg); \
        test_failures++; \
    } else { \
        printf("  ok %s\n", msg); \
    } \
} while(0)

static void setup_edit_buffer(const char *text) {
    memset(ram, 0, sizeof(ram));
    int len = strlen(text);
    for (int i = 0; i < len; i++)
        ram[BUFFER_ADDR + i] = (uint8_t)text[i];
    for (int i = len; i < 200; i++)
        ram[BUFFER_ADDR + i] = 0x10;
}

static void run_justify(const char *text, uint8_t rstop) {
    setup_edit_buffer(text);
    current_edit_line_ptr = BUFFER_ADDR;
    current_format_line_ptr = BUFFER_ADDR;
    current_line_ptr = BUFFER_ADDR;
    ptr1 = BUFFER_ADDR;

    justifying_flag = 0;
    ruler_left_stop = 0;
    ruler_right_stop = rstop;

    l0039 = l0042 = l0046 = 0;
    l0081 = l0082 = l0083 = l0084 = 0;
    tmp8 = tmp9 = 0;
    print_xpos = 4;
    input_buffer_offset = 0;
    a = x = y = flags = 0;
    memset(output_buffer, 0, sizeof(output_buffer));
    memset(input_buffer, 0, sizeof(input_buffer));

    justify_edit_buffer();

    int orig_len = strlen(text);
    int buf_len = 0;
    for (int i = 0; i < 132; i++) {
        if (ram[BUFFER_ADDR + i] == 0x10) break;
        buf_len++;
    }

    int total_extra = 0;
    for (int i = 0; i < l0046; i++)
        total_extra += input_buffer[i];

    printf("  gaps=%d buf=%d->%d extra_distrib=%d\n",
           l0046, orig_len, buf_len, total_extra);

    ASSERT(l0046 > 1, "justification found word gaps");
    ASSERT(buf_len > orig_len, "buffer expanded beyond original text");
    ASSERT(total_extra > 0, "extra spaces were distributed");
}

int main(void) {
    test_failures = 0;
    printf("justify tests:\n\n");

    printf("Test 1: justification with room to expand\n");
    run_justify("The quick brown fox jumps over", 40);

    printf("\nTest 2: exact fit (no expansion expected)\n");
    run_justify("The quick brown fox jumps over", 30);

    printf("\nTest 3: overriding justifying_flag (should skip)\n");
    {
        setup_edit_buffer("The quick brown fox jumps over");
        current_edit_line_ptr = BUFFER_ADDR;
        current_format_line_ptr = BUFFER_ADDR;
        current_line_ptr = BUFFER_ADDR;
        ptr1 = BUFFER_ADDR;
        justifying_flag = 0xFF;  /* non-zero → should skip */
        ruler_left_stop = 0;
        ruler_right_stop = 40;
        l0039 = l0042 = l0046 = 0;
        l0081 = l0082 = l0083 = l0084 = 0;
        tmp8 = tmp9 = 0;
        print_xpos = 4;
        input_buffer_offset = 0;
        a = x = y = flags = 0;
        memset(output_buffer, 0, sizeof(output_buffer));
        memset(input_buffer, 0, sizeof(input_buffer));
        justify_edit_buffer();
        printf("  l0046=%d", l0046);
        ASSERT(l0046 == 0, "justification skipped when flag != 0");
    }

    printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
