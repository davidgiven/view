#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ---- global state (same layout as view.c) ---- */
uint8_t a, x, y, flags;
uint8_t tmp6, tmp7, tmp8, tmp9, l0083;
static void (*number_callback)(void);

/* ---- captured output for testing ---- */
static char output_buf[32];
static int output_len;

static void test_callback(void) {
    if (output_len < (int)sizeof(output_buf) - 1)
        output_buf[output_len++] = (char)a;
}

static void reset_output(void) {
    output_len = 0;
    memset(output_buf, 0, sizeof(output_buf));
    number_callback = test_callback;
}

/* ---- units-under-test (transcribed from view.c) ---- */
static void render_number_to_callback(void) {
    tmp6 = a;
    tmp7 = y;
    uint16_t value = (uint16_t)tmp8 | ((uint16_t)tmp9 << 8);
    char buf[6];
    snprintf(buf, sizeof(buf), "%u", (unsigned int)value);
    for (char *p = buf; *p; p++) {
        a = *p - '0';
        a |= 0x30;
        number_callback();
    }
}

/* ---- helpers ---- */
static void render_number(uint16_t value) {
    tmp8 = (uint8_t)(value & 0xff);
    tmp9 = (uint8_t)(value >> 8);
    a = 0;
    y = 0;
    flags = 0;
    reset_output();
    render_number_to_callback();
}

static int test_failures;

#define ASSERT_EQ(expected, actual, fmt, msg) do { \
    if ((expected) != (actual)) { \
        fprintf(stderr, "FAIL %s: expected " fmt " but got " fmt "\n", msg, (expected), (actual)); \
        test_failures++; \
    } else { \
        printf("  ok %s\n", msg); \
    } \
} while(0)

/* ---- tests ---- */
static void test_render_66(void) {
    render_number(66);
    ASSERT_EQ(2, output_len, "%d", "66 length");
    ASSERT_EQ('6', output_buf[0], "%c", "66 digit 0");
    ASSERT_EQ('6', output_buf[1], "%c", "66 digit 1");
}

static void test_render_0(void) {
    render_number(0);
    ASSERT_EQ(1, output_len, "%d", "0 length");
    ASSERT_EQ('0', output_buf[0], "%c", "0 digit 0");
}

static void test_render_1(void) {
    render_number(1);
    ASSERT_EQ(1, output_len, "%d", "1 length");
    ASSERT_EQ('1', output_buf[0], "%c", "1 digit 0");
}

static void test_render_10000(void) {
    render_number(10000);
    ASSERT_EQ(5, output_len, "%d", "10000 length");
    ASSERT_EQ('1', output_buf[0], "%c", "10000 digit 0");
    ASSERT_EQ('0', output_buf[1], "%c", "10000 digit 1");
    ASSERT_EQ('0', output_buf[2], "%c", "10000 digit 2");
    ASSERT_EQ('0', output_buf[3], "%c", "10000 digit 3");
    ASSERT_EQ('0', output_buf[4], "%c", "10000 digit 4");
}

static void test_render_65535(void) {
    render_number(65535);
    ASSERT_EQ(5, output_len, "%d", "65535 length");
    ASSERT_EQ('6', output_buf[0], "%c", "65535 digit 0");
    ASSERT_EQ('5', output_buf[1], "%c", "65535 digit 1");
    ASSERT_EQ('5', output_buf[2], "%c", "65535 digit 2");
    ASSERT_EQ('3', output_buf[3], "%c", "65535 digit 3");
    ASSERT_EQ('5', output_buf[4], "%c", "65535 digit 4");
}

int main(void) {
    test_failures = 0;

    printf("render_number_to_callback tests:\n");
    test_render_0();
    test_render_1();
    test_render_66();
    test_render_10000();
    test_render_65535();

    printf("\n%d failure(s)\n", test_failures);
    return test_failures ? 1 : 0;
}
