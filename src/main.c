/**
 * View - main entry point.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "document.h"
#include "globals.h"
#include "view.h"

#if defined(TEST_HARNESS)
/**
 * Parse an unsigned long command-line value and validate it.
 * @param arg argument string from optarg
 * @param name option name for error messages (without leading dashes)
 * @param out on success, receives the parsed value
 * @return true on success, false on failure (error already printed)
 */
static bool parse_ulong_option(
    const char* arg, const char* name, unsigned long* out)
{
    if (arg[0] == '\0' || arg[0] == '-')
    {
        fprintf(stderr, "invalid --%s value '%s'\n", name, arg);
        return false;
    }
    char* endptr;
    unsigned long value = strtoul(arg, &endptr, 0);
    if (endptr == arg || *endptr != '\0' || value == 0)
    {
        fprintf(stderr, "invalid --%s value '%s'\n", name, arg);
        return false;
    }
    *out = value;
    return true;
}
#endif

/**
 * Prints the supported command-line syntax.
 * @param program executable name from argv[0]
 */
static void print_usage(const char* program)
{
#if defined(TEST_HARNESS)
    fprintf(stderr,
        "Usage: %s [--ram=size] [--rulers=count] [filename]\n",
        program);
#else
    fprintf(stderr, "Usage: %s [filename]\n", program);
#endif
}

static const char* initial_filename = NULL;

/**
 * Callback invoked by run_view() to load the initial file.
 * Copies the filename into input_buffer and invokes load_cmd().
 */
static void initial_load_callback(void)
{
    size_t filename_length = strlen(initial_filename);

    if (filename_length >= MAX_COMMAND_LENGTH)
        filename_length = MAX_COMMAND_LENGTH - 1;
    memcpy(input_buffer, initial_filename, filename_length);
    input_buffer[filename_length] = 0x0d;
    scan_state_t scan = {0};

    load_cmd(&scan);
    run_cli();
}

/**
 * Program entry point.
 * Allocates document memory and enters VIEW.
 * @param argc argument count
 * @param argv argument vector
 * @return exit status
 */
int main(int argc, char* argv[])
{
    size_t ram_size = 655360;

    static struct option long_options[] = {
#if defined(TEST_HARNESS)
        {"ram",    required_argument, 0, 0},
        {"rulers", required_argument, 0, 0},
#endif
        {0,        0,                 0, 0}
    };

    int option;
    int option_index = 0;
    while ((option = getopt_long(
                argc, argv, "", long_options, &option_index)) != -1)
    {
        switch (option)
        {
#if defined(TEST_HARNESS)
            case 0:
            {
                const char* option_name = long_options[option_index].name;
                unsigned long value;
                if (!parse_ulong_option(optarg, option_name, &value))
                    return 1;
                if (strcmp(option_name, "ram") == 0)
                    ram_size = (size_t)value;
                else
                    ruler_index_size = (size_t)value;
                break;
            }
#endif

            case '?':
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (optind < argc)
    {
        if (argc - optind > 1)
        {
            print_usage(argv[0]);
            return 1;
        }
        initial_filename = argv[optind];
    }

    ram = malloc(ram_size);
    if (!ram)
    {
        perror("malloc");
        return 1;
    }
    himem = ram + ram_size - 1;

    ruler_index = calloc(ruler_index_size, sizeof(uint8_t*));
    if (!ruler_index)
    {
        perror("calloc");
        return 1;
    }

    run_view(initial_filename != NULL ? initial_load_callback : NULL);
    return 0;
}
