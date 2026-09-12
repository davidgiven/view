#ifndef MACRO_H
#define MACRO_H

#include "globals.h"

/**
 * Handles the DM formatting command to define a macro.
 */
void dm_fmt_cmd(void);

/**
 * Reports a nested macro call error and aborts printing.
 */
void nested_macro_error(void);

/**
 * Prepares the next line for printing, handling macro expansion.
 *
 * @param read_limit upper bound for reading document data
 * @param macro_cursor pointer to the current macro cursor, updated as macros
 * execute
 * @return pointer to the prepared line, or NULL when no more output remains
 */
uint8_t* prepare_output_line(uint8_t* read_limit, uint8_t** macro_cursor);

#endif
