/**
 * View - main entry point.
 */

#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "view.h"

/**
 * Program entry point.
 * Allocates document memory and enters VIEW.
 * @param argc argument count (unused)
 * @param argv argument vector (unused)
 * @return exit status
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    ram = malloc(655360);
    if (!ram)
    {
        perror("malloc");
        return 1;
    }
    himem = ram + 655360 - 1;

    run_view();
    return 0;
}
