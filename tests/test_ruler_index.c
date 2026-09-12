#include <stdint.h>
typedef uint16_t addr_t;
/* Test for ruler index push/pop */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "globals.h"

typedef uint16_t addr_t;

extern uint8_t ram[655360];
extern uint8_t* scratch_line_ptr; // was tmp01
extern int ruler_index_ptr;
extern uint8_t* oshwm;
extern uint8_t* ruler_index[RULER_INDEX_SIZE];
extern uint8_t* current_ruler_ptr;
extern uint8_t ruler_left_stop;
extern uint8_t ruler_right_stop;
extern uint8_t status_line_needs_redrawing_flag;
extern uint8_t ruler_buffer_len;
extern uint8_t screen_maxcolumn;

void push_onto_ruler_index(uint8_t* ptr);
void pop_from_ruler_index(void);
void create_default_ruler(uint8_t* ruler_addr);

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
        oshwm = &ram[0x0800];
        ruler_index_ptr = 10;
        status_line_needs_redrawing_flag = 0;
        ruler_left_stop = ruler_right_stop = 0;

        push_onto_ruler_index(&ram[ruler1_addr - 3]);

        ASSERT_EQ(9,
            ruler_index_ptr,
            "%d",
            "ruler_index_ptr decremented from 10 to 9");
        ASSERT_EQ((unsigned int)ruler1_addr,
            (unsigned int)(current_ruler_ptr - &ram[0]),
            "0x%04x",
            "current_ruler_ptr points to ruler1");
        ASSERT_EQ(ruler_left_stop, 10, "%d", "ruler_left_stop = 10");
        ASSERT_EQ(ruler_right_stop, 70, "%d", "ruler_right_stop = 70");
    }

    {
        push_onto_ruler_index(&ram[ruler2_addr - 3]);

        ASSERT_EQ(8, ruler_index_ptr, "%d", "ruler_index_ptr decremented to 8");
        ASSERT_EQ((unsigned int)ruler2_addr,
            (unsigned int)(current_ruler_ptr - &ram[0]),
            "0x%04x",
            "current_ruler_ptr points to ruler2");
        ASSERT_EQ(ruler_left_stop, 5, "%d", "ruler_left_stop = 5");
        ASSERT_EQ(ruler_right_stop, 60, "%d", "ruler_right_stop = 60");
    }

    {
        pop_from_ruler_index();

        ASSERT_EQ(
            9, ruler_index_ptr, "%d", "ruler_index_ptr incremented back to 9");
        ASSERT_EQ((unsigned int)ruler1_addr,
            (unsigned int)(current_ruler_ptr - &ram[0]),
            "0x%04x",
            "current_ruler_ptr restored to ruler1");
        ASSERT_EQ(ruler_left_stop, 10, "%d", "ruler_left_stop restored to 10");
        ASSERT_EQ(
            ruler_right_stop, 70, "%d", "ruler_right_stop restored to 70");
    }

    {
        ruler_index[10] = &ram[0];
        pop_from_ruler_index();

        ASSERT_EQ(10, ruler_index_ptr, "%d", "ruler_index_ptr back to 10");
    }

    {
        oshwm = &ram[0x0800];
        ruler_index_ptr = 10;
        push_onto_ruler_index(&ram[ruler1_addr - 3]);

        uint8_t* stored = ruler_index[9];
        ASSERT_EQ((unsigned int)(ruler1_addr - 3),
            (unsigned int)(stored - &ram[0]),
            "0x%04x",
            "index stores (ruler_addr - 3) at ruler_index[9]");
    }

    {
        oshwm = &ram[0x0800];
        ruler_index_ptr = 10;
        ruler_left_stop = ruler_right_stop = 0;
        screen_maxcolumn = 79;

        addr_t ruler_addr = 0x7200;
        create_default_ruler(&ram[ruler_addr]);

        push_onto_ruler_index(&ram[ruler_addr - 3]);

        ASSERT_EQ(9, ruler_index_ptr, "%d", "ruler_index_ptr = 9");
        ASSERT_EQ(0, ruler_left_stop, "%d", "default ruler left_stop = 0");
        ASSERT_EQ(73, ruler_right_stop, "%d", "default ruler right_stop = 73");
    }

    if (test_failures)
        printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
