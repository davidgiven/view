#include <stdint.h>
typedef uint16_t addr_t;
/* Test for ruler index push/pop */
#include <stdint.h>
#include <string.h>
#include <stdio.h>

typedef uint16_t addr_t;

extern uint8_t ram[65536];
extern uint8_t a, x, y, flags;
extern addr_t tmp01;
#define tmp0 (*((uint8_t*)&tmp01))
#define tmp1 (*((uint8_t*)&tmp01 + 1))
extern uint8_t ruler_index_ptr;
extern addr_t oshwm;
extern addr_t current_ruler_ptr;
extern uint8_t ruler_left_stop;
extern uint8_t ruler_right_stop;
extern uint8_t status_line_needs_redrawing_flag;
extern uint8_t l003a;
extern uint8_t screen_maxcolumn;

void push_onto_ruler_index(uint8_t y, addr_t tmp01);
void pop_from_ruler_index(void);
void create_default_ruler(uint16_t ruler_addr);

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

    screen_maxcolumn = 79;
    memset(ram, 0, sizeof(ram));

    addr_t ruler1_addr = 0x7000;
    for (int i = 0; i < 78; i++)
        ram[ruler1_addr + i] = '.';
    ram[ruler1_addr + 10] = '>';
    ram[ruler1_addr + 70] = '<';
    ram[ruler1_addr + 78] = 0x0d;

    addr_t ruler2_addr = 0x7100;
    for (int i = 0; i < 78; i++)
        ram[ruler2_addr + i] = '.';
    ram[ruler2_addr + 5] = '>';
    ram[ruler2_addr + 60] = '<';
    ram[ruler2_addr + 78] = 0x0d;

    {
        oshwm = 0x0800;
        ruler_index_ptr = 0;
        status_line_needs_redrawing_flag = 0;
        a = x = y = flags = 0;
        ruler_left_stop = ruler_right_stop = 0;

        y = 0;
        push_onto_ruler_index(y, ruler1_addr - 3);

        ASSERT_EQ(0xfe,
            ruler_index_ptr,
            "%d",
            "ruler_index_ptr decremented from 0 to 0xfe");
        ASSERT_EQ(ruler1_addr,
            current_ruler_ptr,
            "0x%04x",
            "current_ruler_ptr points to ruler1");
        ASSERT_EQ(ruler_left_stop, 10, "%d", "ruler_left_stop = 10");
        ASSERT_EQ(ruler_right_stop, 70, "%d", "ruler_right_stop = 70");
    }

    {
        y = 0;
        push_onto_ruler_index(y, ruler2_addr - 3);

        ASSERT_EQ(
            0xfc, ruler_index_ptr, "%d", "ruler_index_ptr decremented to 0xfc");
        ASSERT_EQ(ruler2_addr,
            current_ruler_ptr,
            "0x%04x",
            "current_ruler_ptr points to ruler2");
        ASSERT_EQ(ruler_left_stop, 5, "%d", "ruler_left_stop = 5");
        ASSERT_EQ(ruler_right_stop, 60, "%d", "ruler_right_stop = 60");
    }

    {
        pop_from_ruler_index();

        ASSERT_EQ(0xfe,
            ruler_index_ptr,
            "%d",
            "ruler_index_ptr incremented back to 0xfe");
        ASSERT_EQ(ruler1_addr,
            current_ruler_ptr,
            "0x%04x",
            "current_ruler_ptr restored to ruler1");
        ASSERT_EQ(ruler_left_stop, 10, "%d", "ruler_left_stop restored to 10");
        ASSERT_EQ(
            ruler_right_stop, 70, "%d", "ruler_right_stop restored to 70");
    }

    {
        pop_from_ruler_index();

        ASSERT_EQ(0x00, ruler_index_ptr, "%d", "ruler_index_ptr back to 0");
    }

    {
        oshwm = 0x0800;
        ruler_index_ptr = 0;
        y = 0;
        push_onto_ruler_index(y, ruler1_addr - 3);

        uint8_t hi = ram[oshwm + 0xfe];
        uint8_t lo = ram[oshwm + 0xff];
        addr_t stored = (addr_t)(((addr_t)hi << 8) | lo);
        ASSERT_EQ(ruler1_addr - 3,
            stored,
            "0x%04x",
            "index stores (ruler_addr - 3) at oshwm+0xfe, oshwm+0xff");
    }

    {
        oshwm = 0x0800;
        ruler_index_ptr = 0;
        a = x = y = flags = 0;
        ruler_left_stop = ruler_right_stop = 0;
        screen_maxcolumn = 79;

        addr_t ruler_addr = 0x7200;
        create_default_ruler(ruler_addr);

        y = 0;
        push_onto_ruler_index(y, ruler_addr - 3);

        ASSERT_EQ(0xfe, ruler_index_ptr, "%d", "ruler_index_ptr = 0xfe");
        ASSERT_EQ(0, ruler_left_stop, "%d", "default ruler left_stop = 0");
        ASSERT_EQ(73, ruler_right_stop, "%d", "default ruler right_stop = 73");
    }

    if (test_failures)
        printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
