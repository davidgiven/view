#ifndef CLI_H
#define CLI_H

#include "globals.h"

extern void execute_cli_command(uint8_t a, struct scan_state* scan);
extern void start_printing(void);
extern void clear_cmd(void);
extern void run_cli(void);
extern void cli_handler_impl(void);

#endif
