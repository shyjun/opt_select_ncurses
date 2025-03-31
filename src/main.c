#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opt_select_ncurses_lib.h"

char *in_file = NULL;
char *out_file = NULL;
int from_pipe = 0;

// Function to load options from a file
void
load_options()
{
    FILE *file, *pipe_write_file;

    if (from_pipe == 1) {

        // Open the specified output file for writing
        pipe_write_file = fopen(out_file, "w");
        if (pipe_write_file == NULL) {
            perror("Error opening output file");
            exit(1);
        }

        char buffer[1024];
        size_t bytes_read;

        // Read from stdin (piped input) and write to the output file
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
            fwrite(buffer, 1, bytes_read, pipe_write_file);
        }

        // Close the output file
        fclose(pipe_write_file);

        // open the file for reading
        file = fopen(out_file, "r");
    } else {
        file = fopen(in_file, "r");
    }

    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char prompt_str[MAX_OPTION_STR_LENGTH]; // Buffer to store the prompt
    // Read the first line as the prompt
    if (fgets(prompt_str, sizeof(prompt_str), file) == NULL) {
        perror("Error reading prompt from file");
        exit(EXIT_FAILURE);
    }

    set_prompt(prompt_str);

    udp_dbg("prompt:%s\n", get_prompt());

    init_options_array();

    char cur_option[MAX_OPTION_STR_LENGTH]; // Buffer to store the prompt
    // Read options from the file
    while (
        get_num_options() < MAX_OPTIONS && fgets(cur_option, MAX_OPTION_STR_LENGTH, file)) {
        add_option(cur_option);
    }

    fclose(file);
}

void
usage(char *app_name)
{
    fprintf(stderr,
        "Usage: %s [in_file=<input_file>] [out_file=<output_file>] "
        "[multi_select=yes] [default=<value>] [from_pipe=yes/no] "
        "[udp_dbg_port=<udp_dbg_server_port>] [-h  for help]\n",
        app_name);
}

int
main(int argc, char *argv[])
{
    char *default_value = NULL;
    int multi_select = 0;
    int default_selected_idx
        = 0; // when loading, this should be the default selected

    // Iterate through each argument
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return 1;
        } else if (strncmp(argv[i], "in_file=", 8) == 0) {
            in_file = argv[i]
                + 8; // Assign pointer to the file name after "in_file="
        } else if (strncmp(argv[i], "out_file=", 9) == 0) {
            out_file = argv[i]
                + 9; // Assign pointer to the file name after "out_file="
        } else if (strncmp(argv[i], "multi_select=", 13) == 0) {
            if (strcmp(argv[i] + 13, "yes") == 0) {
                multi_select = 1;
            }
        } else if (strncmp(argv[i], "udp_dbg_port=", 13) == 0) {
            default_value = argv[i] + 13; // Assign pointer to the default value
            set_udp_port(atoi(default_value));
        } else if (strncmp(argv[i], "default=", 8) == 0) {
            default_value = argv[i] + 8; // Assign pointer to the default value
            default_selected_idx = atoi(default_value);
        } else if (strncmp(argv[i], "from_pipe=", 10) == 0) {
            if (strcmp(argv[i] + 10, "yes") == 0) {
                from_pipe = 1;
            }
        }
    }

    // Additional Checks
    if (from_pipe) {
        // If from_pipe is enabled, set the default out_file if not specified
        if (!out_file) {
            out_file
                = "/tmp/opt_select_ncurses_out_file"; // Set default out_file
        }
    } else {
        // If in_file and out_file are both NULL, handle the logic accordingly
        if (!in_file) {
            fprintf(stderr, "Error: 'in_file' must be provided\n");
            usage(argv[0]);
            return 1;
        }
        if (!out_file) {
            fprintf(stderr, "Error: 'out_file' must be provided\n");
            usage(argv[0]);
            return 1;
        }
    }

    set_multi_select_enabled(multi_select);

    // Load options from the specified file
    load_options();

    if (default_selected_idx != 0) {
        set_highlight(default_selected_idx);
    }

    run_opt_select_ncurses();

    char *selected_filename = out_file;

    // Save selected options to the specified file
    FILE *selected_file = fopen(selected_filename, "w");
    if (!selected_file) {
        perror("Error opening selected file");
        exit(EXIT_FAILURE);
    }

    int retval = -EINVAL;
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
                fprintf(selected_file, "%s%s", separator, get_option(i));
            }
        }

        if (num_selected == 0) {
            if (get_multi_select_enabled()) {
                /* multi select enabled, but nothing selected.
                 * consider current cursor item as selected
                 */
                fprintf(selected_file, "%s", get_option(get_highlight()-1));
            } else {
                /* shouldn't happen .! */
                assert(0);
            }
        }

        // return success
        retval = 0;
    }

    fclose(selected_file);

    clean_up_opt_select_ncurses();

    return retval;
}
