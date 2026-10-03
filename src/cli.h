#ifndef CLI_H
#define CLI_H

#include "globals.h"
#include "document.h"

typedef enum
{
    CLI_CMD_OK,        /** Command parsed and processed. */
    CLI_CMD_NO_TARGET, /** No command given. */
    CLI_CMD_NO_STRING  /** Area empty or no search string. */
} cli_cmd_status_t;

extern void start_printing(void);
extern void clear_cmd(void);
extern void run_cli(void);
extern void cli_handler_impl(void);
extern void check_not_continuous_editing(void);
extern void file_error(void);
extern void file_not_found_error(void);
extern void zero_terminate_filename_buffer(void);
extern bool parse_optional_filename_from_command(scan_state_t* scan);

#endif
