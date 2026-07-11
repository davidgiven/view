/* Test for create_default_ruler */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern uint8_t ram[65536];
extern uint8_t a, x, y, flags;
extern uint8_t tmp0, tmp1;
extern uint8_t screen_maxcolumn;
extern void create_default_ruler(void);

static int test_failures;

#define ASSERT_EQ(expected, actual, fmt, msg) do { \
    if ((expected) != (actual)) { \
        fprintf(stderr, "FAIL %s: expected " fmt " but got " fmt "\n", msg, (expected), (actual)); \
        test_failures++; \
    } else { \
        printf("  ok %s\n", msg); \
    } \
} while(0)

int main(void) {
    test_failures = 0;
    printf("create_default_ruler tests:\n");
    printf("  screen_maxcolumn = %d\n", screen_maxcolumn);

    uint16_t ruler_addr = 0x8000;
    a = (uint8_t)(ruler_addr & 0xff);
    y = (uint8_t)(ruler_addr >> 8);
    flags = 0;
    screen_maxcolumn = 79;
    printf("  screen_maxcolumn = %d\n", screen_maxcolumn);
    printf("  a=0x%02x y=0x%02x ruler_addr=0x%04x\n", a, y, ruler_addr);
    memset(ram, 0, sizeof(ram));

    create_default_ruler();

    printf("  Final state: tmp0=0x%02x tmp1=0x%02x y=0x%02x flags=0x%02x\n", tmp0, tmp1, y, flags);
    printf("  Full ruler at 0x%04x:\n", ruler_addr);
    for (int i = 0; i < 80; i++) {
        uint8_t v = ram[ruler_addr + i];
        char ch = (v >= 32 && v < 127) ? (char)v : '.';
        printf("    [%2d] = 0x%02x (%c)\n", i, v, ch);
    }

    int ruler_len = screen_maxcolumn - 6;
    for (int i = 0; i < ruler_len; i++) {
        uint8_t expected;
        if ((i & 7) == 7) expected = '*';
        else expected = '.';
        char desc[64];
        snprintf(desc, sizeof(desc), "ruler byte %d", i);
        ASSERT_EQ(expected, ram[ruler_addr + i], "%c", desc);
    }
    ASSERT_EQ('<', ram[ruler_addr + ruler_len], "%c", "ruler '<' at pos 74");

    printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
