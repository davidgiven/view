#ifndef CLI_H
#define CLI_H

#include "globals.h"

extern uint8_t execute_cli_command(uint8_t a);
extern void start_printing(void);
extern void clear_cmd(void);
extern void run_cli(void);
extern void cli_handler_impl(void);

#endif
