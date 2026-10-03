/**
 * View - main entry point.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "view.h"

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
    unsigned long v = strtoul(arg, &endptr, 0);
    if (endptr == arg || *endptr != '\0' || v == 0)
    {
        fprintf(stderr, "invalid --%s value '%s'\n", name, arg);
        return false;
    }
    *out = v;
    return true;
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
        {"ram",    required_argument, 0, 0},
        {"rulers", required_argument, 0, 0},
        {0,        0,                 0, 0}
    };

    int opt;
    int option_index = 0;
    while (
        (opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1)
    {
        switch (opt)
        {
            case 0:
                if (strcmp(long_options[option_index].name, "ram") == 0)
                {
                    unsigned long v;
                    if (!parse_ulong_option(optarg, "ram", &v))
                        return 1;
                    ram_size = (size_t)v;
                }
                else if (strcmp(long_options[option_index].name, "rulers") == 0)
                {
                    unsigned long v;
                    if (!parse_ulong_option(optarg, "rulers", &v))
                        return 1;
                    ruler_index_size = (size_t)v;
                }
                break;

            case '?':
            default:
                fprintf(stderr,
                    "Usage: %s [--ram=size] [--rulers=count]\n",
                    argv[0]);
                return 1;
        }
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

    run_view();
    return 0;
}
