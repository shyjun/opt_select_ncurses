#include <assert.h>
#include <errno.h>
#include <ncurses.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opt_select_ncurses_lib.h"

char *options[MAX_OPTIONS]; // Array to store options
int num_options = 0; // Number of options
char prompt[MAX_OPTION_STR_LENGTH]; // Buffer to store the prompt
int selected[MAX_OPTIONS] = { 0 }; // Array to track selected options

int multi_select_enabled = 0; // Multi-select disabled by default
WINDOW *menu_win;
int highlight = 1;
char input_buffer[MAX_INPUT_LENGTH] = { 0 }; // Buffer to store input

int input_length = 0; // Length of the current input in the buffer
int g_pressed = 0; // State to track if 'g' was pressed
int ch;
int grep_mode = 0;
regex_t regex;
int ctrl_p = 0;
int ctrl_n = 0;

void
udp_dbg(const char *fmt, ...);

void
set_udp_port(int port);

void set_highlight(int val)
{
    highlight = val;
}

int get_highlight()
{
    return highlight;
}

void set_multi_select_enabled(int val)
{
    multi_select_enabled = val;
}

int get_multi_select_enabled()
{
    return multi_select_enabled;
}

int get_num_options()
{
    return num_options;
}

int get_selected_flag(int idx)
{
    return selected[idx];
}

void set_prompt(char *str)
{
    // Remove newline character if present
    str[strcspn(str, "\n")] = '\0';
    strcpy(prompt, str);
}

char *get_prompt()
{
    return prompt;
}

void init_options_array()
{
    // Allocate memory for each option (assuming max length per option is 255)
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        options[i] = malloc(MAX_OPTION_STR_LENGTH * sizeof(char));
        if (options[i] == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
    }
}

void add_option(char *option_str)
{
    // Remove newline character if present
    option_str[strcspn(option_str, "\n")] = '\0';
    strcpy(options[num_options], option_str);
    num_options++;
    udp_dbg("num_options=%d,cur_option=%s\n", num_options,
            options[num_options - 1]);
}

char *get_option(int idx)
{
    return options[idx];
}

// Function to calculate the maximum width needed for the window
int
calculate_max_width()
{
    int max_width = 0;
    int i;

    max_width = snprintf(NULL, 0, "%s", prompt);
    for (i = 0; i < num_options; ++i) {
        int item_width;
        if (multi_select_enabled) {
            item_width = snprintf(NULL, 0, "[%2d] * %s", i + 1, options[i]);
        } else {
            item_width = snprintf(NULL, 0, "[%2d] %s", i + 1, options[i]);
        }
        if (item_width > max_width) {
            max_width = item_width;
        }
        udp_dbg("%d: strlen=%d, len=%d, max_width=%d, str=%s\n", i,
            (int)strlen(options[i]), item_width, max_width, options[i]);
    }

    return max_width + 4; // Add extra space for borders and padding
}

// Function to display the options with a border, serial numbers, and a prompt
void
display_menu()
{
    int i;

    // Clear the window
    werase(menu_win);

    // Draw the border
    box(menu_win, 0, 0);

    // Print the prompt at the top
    mvwprintw(menu_win, 1, 2, "%s", prompt);

    // Print an extra newline after the prompt
    mvwprintw(menu_win, 2, 2, " ");

    // Print the options inside the border with right-aligned serial numbers
    for (i = 0; i < num_options; ++i) {
        if (multi_select_enabled == 1) {
            if ((highlight == i + 1) || (selected[i])) {
                wattron(menu_win, A_REVERSE); // Highlight the selected option
            }

            // Right-align the numbers in the brackets and indicate selected
            // options with '*'
            if (selected[i]) {
                mvwprintw(menu_win, i + 3, 2, "[%2d] * %s", i + 1, options[i]);
            } else {
                mvwprintw(menu_win, i + 3, 2, "[%2d]   %s", i + 1, options[i]);
            }
        } else {
            if (highlight == i + 1) {
                wattron(menu_win, A_REVERSE); // Highlight the selected option
            }

            // Right-align the numbers in the brackets
            mvwprintw(menu_win, i + 3, 2, "[%2d] %s", i + 1, options[i]);
        }
        wattroff(menu_win, A_REVERSE); // Remove the highlight
    }

    if (grep_mode == 1) {
        /* if (input_length > 0) */
        {
            char grep_buff[300];
            sprintf(grep_buff, "Grep:/%s", input_buffer);
            mvwprintw(menu_win, i + 3, 2, "%s", grep_buff);
        }
    }

    refresh();
    // Refresh the window to show changes
    wrefresh(menu_win);
    curs_set(0); // Hide the cursor
}

void
regex_find()
{
    int first_match_idx = -1;
    int last_match_idx;
    int found_highlight = -1;
    int prev_highlight = -1;

    // Compile the regular expression
    int ret = regcomp(&regex, input_buffer, REG_EXTENDED);
    if (ret != 0) {
        printf("Could not compile regex\n");
        exit(1);
    }

    udp_dbg("pattern=%s\n", input_buffer);
    int i;
    for (i = 0; i < num_options; ++i) {
        // Execute the regular expression
        ret = regexec(&regex, options[i], 0, NULL, 0);
        if (ret == 0) {
            if (first_match_idx == -1) {
                first_match_idx = i;
            }
            last_match_idx = i;
            udp_dbg("%d:num_options=%d The text matches the "
                    "pattern.highlight=%d,prev_highlight=%d\n",
                i, num_options, highlight, prev_highlight);
            if (ctrl_n == 1) {
                if (i
                    <= highlight - 1) { // item before current highlighted item
                    continue;
                }
            }
            if (ctrl_p == 1) {
                if (highlight - 1 == i) { // current highlighted item
                    if (prev_highlight != -1) {
                        found_highlight = prev_highlight;
                    }
                }
                prev_highlight = i + 1;
                udp_dbg("%d: else ctrl_p.highlight=%d, prev_highlight=%d\n", i,
                    highlight, prev_highlight);
                continue;
            }
            set_highlight(i + 1);
            break;
        } else if (ret == REG_NOMATCH) {
            udp_dbg("%d: The text does not match the pattern.\n", i);
        } else {
            char error_message[100];
            regerror(ret, &regex, error_message, sizeof(error_message));
            udp_dbg("Regex match failed: %s\n", error_message);
        }
    }
    if ((ctrl_p == 1) && (first_match_idx != -1)) {
        udp_dbg("ctrl+p end: i=%d:num_options=%d "
                ".highlight=%d,prev_highlight=%d,found_highlight=%d\n",
            i, num_options, highlight, prev_highlight, found_highlight);
        if (i == num_options) {
            if (found_highlight != -1) {
                set_highlight(found_highlight);
            } else {
                set_highlight(last_match_idx + 1);
            }
        } else {
            set_highlight(prev_highlight);
        }
    }
    if ((ctrl_n == 1) && (i == num_options)) {
        // last entry and still ctrl+n is pressed..
        set_highlight(first_match_idx+1);
    }

    assert(highlight >= 0);

    // Free the compiled regular expression
    regfree(&regex);
}

int
handle_grep_mode()
{
    ctrl_p = 0;
    ctrl_n = 0;

    if (ch == 27) { // ESC
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        grep_mode = 0;
        return true;
    }

    udp_dbg("before: grep_mode:ch:%d,input_length=%d\n", ch, input_length);

    if ((ch == 8) || (ch == 263)) { // BackSpace
        ch = 0;
        if (input_length == 0) {
            input_buffer[input_length] = '\0'; // Null-terminate the buffer
        } else {
            input_length--;
            udp_dbg("BS: input_length=%d", input_length);
        }
        input_buffer[input_length] = '\0'; // Null-terminate the buffer
    } else if (ch == 32) { // Space key to select/unselect
        if (!multi_select_enabled) {
            memset(selected, 0,
                sizeof(
                    selected)); // no multi select support. clear all selection
        }
        selected[highlight - 1] = !selected[highlight - 1]; // Toggle selection
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 10) { // Enter key
        if (!multi_select_enabled) {
            memset(selected, 0,
                sizeof(
                    selected)); // no multi select support. clear all selection
            selected[highlight - 1]
                = !selected[highlight - 1]; // Toggle selection
            return false;
        } else {
            selected[highlight - 1]
                = !selected[highlight - 1]; // Toggle selection
        }
        udp_dbg("Grep mode: Enter key");
    } else if (ch == 16) { // ctrl+p : prev
        udp_dbg("Grep mode: ctrl+p key");
        ctrl_p = 1;
        ctrl_n = 0;
    } else if (ch == 14) { // ctrl+n : next
        udp_dbg("Grep mode: ctrl+n key");
        ctrl_p = 0;
        ctrl_n = 1;
    } else if (ch == '/') { // already in grep mode.
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
    } else {
        if (input_length < MAX_INPUT_LENGTH - 1) {
            input_buffer[input_length++] = ch; // Add digit to the buffer
            input_buffer[input_length] = '\0'; // Null-terminate the buffer
        }
    }

    udp_dbg("after: grep_mode:ch:%d,input_length=%d\n", ch, input_length);

    regex_find();

    // always continue
    return true;
}

void update_num()
{
    input_buffer[input_length++] = ch; // Add digit to the buffer
    input_buffer[input_length] = '\0'; // Null-terminate the buffer
}

int
handle_normal_mode()
{
    int input_number;
    if (ch == '/') { // switch to grep mode
        grep_mode = 1;
        input_length = 0;
        return true;
    }

    if (ch == 27)
        return false;

    if (ch >= '0' && ch <= '9') { // Check if the key is a digit
        udp_dbg("cur_ch=%d\n", ch);
        if (input_length < MAX_INPUT_LENGTH - 1) {

            update_num();
                                               //
            if ((ch == '0') && (input_length == 1)) {
                // 0 pressed.
                input_length = 0;
            }
            // Convert input buffer to an integer
            input_number = atoi(input_buffer);
            udp_dbg("before: cur_str=%s\n", input_buffer);
            udp_dbg("before: input_number=%d\n", input_number);

            // Validate and set highlight if input is within range
            if (input_number >= 1 && input_number <= num_options) {
                set_highlight(input_number);
            }

            // If the number is a single digit and valid, highlight immediately
            if (input_length == 1) {
                if (input_number >= 1) {
                    // first digit
                    set_highlight(input_number);
                } else {
                    // Not sure what !!
                    set_highlight(0);
                }
            } else {
                if (input_number <= num_options) {
                    set_highlight(input_number);
                }
                else {
                        input_length
                            = 0; // Clear the input buffer after immediate selection
                        input_buffer[0] = '\0';
                        update_num();
                        input_number = atoi(input_buffer);
                        set_highlight(input_number);
                }
            }
        }
        input_number = atoi(input_buffer);
        udp_dbg("after: cur_str=%s\n", input_buffer);
        udp_dbg("after: input_number=%d\n", input_number);
        udp_dbg("after: highlight=%d\n", highlight);
    } else if (ch == 'k' || ch == KEY_UP
        || ch == 16) { // Key 'k' or UP or CTRL+p for up
        if (highlight == 1) {
            set_highlight(num_options);
        } else {
            set_highlight(highlight-1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 'j' || ch == KEY_DOWN
        || ch == 14) { // Key 'j' or DOWN or CTRL+n for down
        if (highlight == num_options) {
            set_highlight(1);
        } else {
            set_highlight(highlight+1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 4) { // Ctrl+D
        set_highlight(highlight+4);
        if (highlight > num_options) {
            set_highlight(num_options);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 21) { // Ctrl+U
        set_highlight(highlight-4);
        if (highlight < 1) {
            set_highlight(1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == KEY_NPAGE) { // Page Down key
        set_highlight(highlight+8);
        if (highlight > num_options) {
            set_highlight(num_options);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == KEY_PPAGE) { // Page Up key
        set_highlight(highlight-8);
        if (highlight < 1) {
            set_highlight(1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 'G') { // 'G' key to go to the last item
        set_highlight(num_options);
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 'g') { // 'g' key to start sequence for 'gg'
        if (g_pressed) { // If 'g' was already pressed
            set_highlight(1); // Go to the first item
        }
        g_pressed = 1; // Set the state to indicate 'g' was pressed
    } else if (ch == 32) { // Space key to select/unselect
        if (!multi_select_enabled) {
            memset(selected, 0,
                sizeof(
                    selected)); // no multi select support. clear all selection
        }
        selected[highlight - 1] = !selected[highlight - 1]; // Toggle selection
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 10) { // Enter key
        if (!multi_select_enabled) {
            memset(selected, 0,
                sizeof(
                    selected)); // no multi select support. clear all selection
            selected[highlight - 1]
                = !selected[highlight - 1]; // Toggle selection
        }

        return false; // Exit loop if single selection mode
    }

    return true;
}

void
handle_keys()
{
    int again;
    do {
        ch = getch();
        if (grep_mode == 1) {
            again = handle_grep_mode();
        } else {
            again = handle_normal_mode();
        }
        display_menu();
    } while (again == true);
}

void clean_up_opt_select_ncurses()
{
    // Free allocated memory for options
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        free(options[i]);
    }
}

int get_last_ch()
{
    return ch;
}

void run_opt_select_ncurses()
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Clear the screen before creating the window
    clear();

    // Calculate the maximum width needed for the window
    int max_width = calculate_max_width();
    int height = num_options
        + 4; // Increased height for prompt, extra line, and options
    int start_y
        = 0; // Set start_y to 0 to position the window at the top of the screen
    int start_x = 0; // Set start_x to 0 to position the window at the left edge
                     // of the screen
    menu_win = newwin(height, max_width, start_y, start_x);

    // Display the menu immediately
    display_menu();

    // Reopen stdin as /dev/tty to read from the terminal
    freopen("/dev/tty", "r", stdin);

    handle_keys();
}
