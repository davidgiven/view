#include "document.h"
#include "view.h"
#include "editor.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/** Checks document addressing, marker write-back and multi-line wrapping. */
int main(void)
{
    ram = malloc(256 * 1024);
    assert(ram != NULL);
    current_format_line = (uint8_t*)&current_line_buffer;
    memset(current_line_buffer.text, 0x10, sizeof(current_line_buffer.text));
    memcpy(current_line_buffer.text, "hello", 5);

    const size_t line_offsets[] = {0, 65534, 65536, 70000};
    const uint8_t prefixes[] = {0, COMMAND_PREFIX, RULER_PREFIX};
    for (size_t line = 0; line < ARRAY_SIZE(line_offsets); line++)
    {
        current_line_ptr = ram + line_offsets[line];
        for (size_t prefix = 0; prefix < ARRAY_SIZE(prefixes); prefix++)
        {
            current_line_buffer.prefix_byte = prefixes[prefix];
            size_t header_length =
                prefixes[prefix] == 0 ? 0 : offsetof(line_t, text);
            xpos = 3;
            uint8_t* expected = current_line_ptr + header_length + xpos;
            assert(get_cursor_document_address() == expected);
            set_marker_to_here(0);
            assert(markers_array[0] == expected);

            xpos = 100;
            expected = current_line_ptr + header_length + 5;
            assert(get_cursor_document_address() == expected);
            set_marker_to_here(0);
            assert(markers_array[0] == expected);
        }
    }
    // Both markers must survive write-back at and above the 64 KiB boundary.
    const size_t write_offsets[] = {65534, 70000};
    for (size_t line = 0; line < ARRAY_SIZE(write_offsets); line++)
    {
        memset(&pointer_array, 0, sizeof(pointer_array));
        current_line_ptr = ram + write_offsets[line];
        memcpy(current_line_ptr, "hello\r", 7);
        top = current_line_ptr + 6;
        current_line_buffer.prefix_byte = 0;
        edit_line_len = 5;
        edit_buffer_unpacked_flag = true;
        edit_buffer_contains_ruler_flag = false;
        edit_buffer_dirty_flag = false;
        markers_array[0] = current_line_buffer.text + 2;
        markers_array[1] = current_line_buffer.text + 2;
        assert(!write_line_back_to_document());
        assert(markers_array[0] == current_line_ptr + 2);
        assert(markers_array[1] == current_line_ptr + 2);
    }

    // A zero second address byte must not stop wrapping after the first split.
    memset(&pointer_array, 0, sizeof(pointer_array));
    himem = ram + 256 * 1024 - 1;
    uintptr_t aligned =
        ((uintptr_t)(ram + 70000) + 0xffff) & ~(uintptr_t)0xffff;
    uint8_t* preceding_break = (uint8_t*)aligned;
    *preceding_break = '\r';
    uint8_t* long_line = preceding_break + 1;
    memset(long_line, 'a', 500);
    long_line[500] = '\r';
    long_line[501] = 0;
    top = long_line + 501;
    split_line_at_wrap(long_line);
    size_t line_length = 0;
    size_t text_length = 0;
    for (uint8_t* ch = long_line; ch < top; ch++)
    {
        if (*ch == '\r')
        {
            assert(line_length <= MAX_LINE_LENGTH + 1);
            line_length = 0;
        }
        else
        {
            assert(*ch == 'a');
            line_length++;
            text_length++;
        }
    }
    assert(text_length == 500 && line_length == 0);

    free(ram);
    return 0;
}
