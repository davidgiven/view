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
        {"ram", required_argument, 0, 0},
        {0,     0,                 0, 0}
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
                    if (optarg[0] == '\0' || optarg[0] == '-')
                    {
                        fprintf(stderr, "invalid --ram value '%s'\n", optarg);
                        return 1;
                    }
                    char* endptr;
                    unsigned long v = strtoul(optarg, &endptr, 0);
                    if (endptr == optarg || *endptr != '\0' || v == 0)
                    {
                        fprintf(stderr, "invalid --ram value '%s'\n", optarg);
                        return 1;
                    }
                    ram_size = (size_t)v;
                }
                break;

            case '?':
            default:
                fprintf(stderr, "Usage: %s [--ram=size]\n", argv[0]);
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

    run_view();
    return 0;
}
