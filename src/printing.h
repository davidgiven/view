#ifndef PRINTING_H
#define PRINTING_H

#include "globals.h"

// Functions defined in printing.c, called from view.c
extern bool execute_formatting_command(uint8_t x);
extern void prepare_printer_driver(void);
extern void stop_printing(void);

#endif
