/*
 * opt_select_ncurses - Ncurses-based option selection utility
 *
 * Copyright (c) 2025, Shyju N
 * Email: n.shyju@gmail.com
 *
 * Licensed under the BSD 3-Clause License.
 * See the LICENSE file in the project root for full license information.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opt_select_ncurses_lib.h"

char *in_file = NULL;
char *out_file = NULL;
int from_pipe = 0;

void
usage(char *app_name)
{
    fprintf(stderr,
        "Usage: %s [in_file=<input_file>] [out_file=<output_file>] "
        "[multi_select=yes] [default=<value>] [from_pipe=yes/no] "
        "[udp_dbg_port=<udp_dbg_server_port>] [-h  for help]\n",
        app_name);
}

int main()
{

    set_udp_port(8050);

    init_options_array();
    set_prompt("Please select a number:");

    add_option("1");
    add_option("2");
    add_option("3");
    add_option("4");
    add_option("5");

    set_multi_select_enabled(1);
    set_highlight(3);

    run_opt_select_ncurses();

    int num_selected = 0;
    if (get_last_ch() != 27) {
        // we are here because of Enter. not because of Esc.
        for (int i = 0; i < get_num_options(); ++i) {
            char *separator;
            separator = ",";
            if (get_selected_flag(i)) {
                if (num_selected++ == 0) {
                    separator = "";
                }
                printf("%s%s", separator, get_option(i));
            }
        }

        if (num_selected == 0) {
            if (get_multi_select_enabled()) {
                /* multi select enabled, but nothing selected.
                 * consider current cursor item as selected
                 */
                printf("%s", get_option(get_highlight()-1));
            } else {
                /* shouldn't happen .! */
                assert(0);
            }
        }
    }

    clean_up_opt_select_ncurses();

    return 0;
}
