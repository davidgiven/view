#ifndef PRINTING_H
#define PRINTING_H

#include "globals.h"

// Format-command indices as returned by lookup_formatting_command(): each
// two-letter code from commands_table in table order.
enum formatting_command
{
    FORMATTING_COMMAND_CE = 0,
    FORMATTING_COMMAND_RJ = 1,
    FORMATTING_COMMAND_DF = 2,
    FORMATTING_COMMAND_DH = 3,
    FORMATTING_COMMAND_DM = 4,
    FORMATTING_COMMAND_EM = 5,
    FORMATTING_COMMAND_SR = 6,
    FORMATTING_COMMAND_PE = 7,
    FORMATTING_COMMAND_TM = 8,
    FORMATTING_COMMAND_BM = 9,
    FORMATTING_COMMAND_PL = 10,
    FORMATTING_COMMAND_TS = 11,
    FORMATTING_COMMAND_FO = 12,
    FORMATTING_COMMAND_HE = 13,
    FORMATTING_COMMAND_HT = 14,
    FORMATTING_COMMAND_HM = 15,
    FORMATTING_COMMAND_FM = 16,
    FORMATTING_COMMAND_LM = 17,
    FORMATTING_COMMAND_LS = 18,
    FORMATTING_COMMAND_OP = 19,
    FORMATTING_COMMAND_EP = 20,
    FORMATTING_COMMAND_LJ = 21,
    FORMATTING_COMMAND_PB = 22,
    NO_FORMATTING_COMMAND = -1
};

// Functions defined in printing.c, called from view.c
extern bool execute_formatting_command(enum formatting_command x);
extern void prepare_printer_driver(void);
extern void stop_printing(void);

#endif
