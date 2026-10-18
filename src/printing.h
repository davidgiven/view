#ifndef PRINTING_H
#define PRINTING_H

#include "globals.h"
#include "document.h"

typedef struct printer_driver
{
    void (*print_char)(uint8_t cur_ch);
    void (*printer_on)(void);
    void (*printer_off)(void);
    void (*printer_microspace)(void);
    void (*printer_getflags)(uint8_t* idx, uint8_t* pos);
} printer_driver_t;

/** A two-letter formatting command and its handler. */
typedef struct formatting_command
{
    char name[2];
    void (*callback)(void);
} formatting_command_t;

extern const formatting_command_t* lookup_formatting_command(void);
extern void prepare_printer_driver(void);
extern void print_document(scan_state_t* scan);
extern void stop_printing(void);
extern void write_byte_to_memory(uint8_t** cursor, uint8_t cur_ch);
extern void write_cr_to_memory(uint8_t** cursor);

#endif
