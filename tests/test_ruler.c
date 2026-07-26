#include <stdint.h>
typedef uint16_t addr_t;
/* Test for create_default_ruler */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

extern uint8_t ram[65536];
extern uint8_t a, x, y, flags;
extern addr_t tmp01;
#define tmp0 (*((uint8_t*)&tmp01))
#define tmp1 (*((uint8_t*)&tmp01 + 1))
extern uint8_t screen_maxcolumn;
extern void create_default_ruler(uint16_t ruler_addr);

static int test_failures;

#define ASSERT_EQ(expected, actual, fmt, msg)                  \
    do                                                         \
    {                                                          \
        if ((expected) != (actual))                            \
        {                                                      \
            fprintf(stderr,                                    \
                "FAIL %s: expected " fmt " but got " fmt "\n", \
                msg,                                           \
                (expected),                                    \
                (actual));                                     \
            test_failures++;                                   \
        }                                                      \
    } while (0)

int main(void)
{
    test_failures = 0;

    uint16_t ruler_addr = 0x8000;
    flags = 0;
    screen_maxcolumn = 79;
    memset(ram, 0, sizeof(ram));

    create_default_ruler(ruler_addr);

    int ruler_len = screen_maxcolumn - 6;
    for (int i = 0; i < ruler_len; i++)
    {
        uint8_t expected;
        if ((i & 7) == 7)
            expected = '*';
        else
            expected = '.';
        char desc[64];
        snprintf(desc, sizeof(desc), "ruler byte %d", i);
        ASSERT_EQ(expected, ram[ruler_addr + i], "%c", desc);
    }
    ASSERT_EQ('<', ram[ruler_addr + ruler_len], "%c", "ruler '<' at pos 74");

    if (test_failures)
        printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
