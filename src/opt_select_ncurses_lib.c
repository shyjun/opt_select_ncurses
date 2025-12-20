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
#include <ncurses.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "opt_select_ncurses_lib.h"

#define START_ROW ( \
    1 /* top box line */ + \
    1 /* prompt */ + \
    1 /* empty line */ \
)

#define END_ROW 1 /* bottom box line */

#define COLS_EXTRA 4

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

int startidx = 0;
int max_width;

#define TAB_WIDTH 8
#define BUF_LEN 512
char cur_line_buf[BUF_LEN];
char cur_line_buf_temp[BUF_LEN+10];

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

void correct_line_end(char *str)
{
    size_t pos = strcspn(str, "\n");
    str[pos] = '\0';       // OK: str is caller-owned and always null-terminated
}

void set_prompt(const char *str)
{
    correct_line_end((char *)str);      // modifies str in place (documented)

    size_t max = sizeof(prompt) - 1;    // leave room for '\0'
    size_t len = strlen(str);

    if (len > max) {
        // Copy only what fits
        memcpy(prompt, str, max - 3);
        prompt[max - 3] = '.';
        prompt[max - 2] = '.';
        prompt[max - 1] = '.';
        prompt[max] = '\0';
    } else {
        memcpy(prompt, str, len + 1);
    }
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
    correct_line_end(option_str);
    strcpy(options[num_options], option_str);
    num_options++;
    udp_dbg("num_options=%d,cur_option=%s\n", num_options,
            options[num_options - 1]);
}

char *get_option(int idx)
{
    return options[idx];
}

int count_tabs(const char *str)
{
    if (str == NULL) {
        return 0;
    }

    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\t') {
            count++;
        }
    }
    return count;
}

// Function to calculate the maximum width needed for the window
int
calculate_max_width()
{
    int width = 0;
    int i;
    int tabs;

    width = snprintf(NULL, 0, "%s", prompt);
    for (i = 0; i < num_options; ++i) {
        int item_width;
        if (multi_select_enabled) {
            item_width = snprintf(NULL, 0, "[%2d] * %s", i + 1, options[i]);
        } else {
            item_width = snprintf(NULL, 0, "[%2d] %s", i + 1, options[i]);
        }
        tabs = count_tabs(options[i]);
        item_width += 7*tabs;

        assert(item_width < (int)sizeof(cur_line_buf));

        if(item_width+COLS_EXTRA > COLS) {
            // original value will be assigned by caller
            return COLS+1;
        }

        if (item_width > width) {
            width = item_width;
        }
        udp_dbg("%d: strlen=%d, len=%d, width=%d, str=%s\n", i,
            (int)strlen(options[i]), item_width, width, options[i]);
    }

    return width + COLS_EXTRA; // Add extra space for borders and padding
}

void
printify_line()
{
    int i = 0, num_tabs = 0;
    char *src = cur_line_buf;
    char *dst = cur_line_buf_temp;
    while(src[i]) {
        if (*src == '\t') {
            int spaces = TAB_WIDTH;
            num_tabs ++;
            while (spaces--) {
                *dst++ = ' ';
            }
        } else if(!isprint(*src)) {
            *dst++='.';
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';

    int len;
    len = strlen(cur_line_buf_temp);
    int max_len = max_width-COLS_EXTRA;
    udp_dbg("max_width=%d,COLS_EXTRA=%d,max_len=%d,len=%d,num_tabs=%d,COLS=%d,LINES=%d\n",
            max_width,COLS_EXTRA,max_len,len,num_tabs,COLS,LINES);
    if(len > max_len) {
        cur_line_buf_temp[max_len-1] = '.';
        cur_line_buf_temp[max_len-2] = '.';
        cur_line_buf_temp[max_len-3] = '.';
        cur_line_buf_temp[max_len] = 0;
    }
    strcpy(cur_line_buf, cur_line_buf_temp);
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

    int extra_lines = START_ROW+END_ROW;
    int total_lines = LINES - extra_lines;
    startidx = (highlight-1) / total_lines;
    startidx =  startidx * total_lines;

    udp_dbg("display_menu: total_lines=%d,highlight=%d,startidx=%d,LINES:=%d,COLS=%d,num_options=%d\n",
            total_lines,highlight,startidx,LINES,COLS,num_options);

    // Print the options inside the border with right-aligned serial numbers
    for (i=0; (startidx+i) < num_options; ++i) {
        if(i+extra_lines==LINES) {
            break;
        }
        if (multi_select_enabled == 1) {
            if ((highlight == i+startidx+1) || (selected[startidx+i])) {
                wattron(menu_win, A_REVERSE); // Highlight the selected option
            }

            // Right-align the numbers in the brackets and indicate selected options with '*'
            char mark = ' ';
            if (selected[startidx+i]) {
                mark = '*';
            }
            sprintf(cur_line_buf, "[%2d] %c %s", startidx+i+1, mark, options[startidx+i]);
        } else {
            sprintf(cur_line_buf, "[%2d] %s", startidx+i+ 1, options[startidx+i]);

            if (highlight == startidx+i+1) {
                wattron(menu_win, A_REVERSE); // Highlight the selected option
            }
        }

        udp_dbg("printify:i=%d,startidx=%d,highlight=%d\n", i,startidx,highlight);
        printify_line();

        // Right-align the numbers in the brackets
        mvwprintw(menu_win, i + START_ROW, 2, "%s", cur_line_buf);

        wattroff(menu_win, A_REVERSE); // Remove the highlight
    }

    if (grep_mode == 1) {
        // if (input_length > 0)
        {
            char grep_buff[300];
            sprintf(grep_buff, "Grep:/%s", input_buffer);
            mvwprintw(menu_win, i + START_ROW, 2, "%s", grep_buff);
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
    int last_match_idx = 0;
    int found_highlight = -1;
    int prev_highlight = -1;
    int current_highlight = highlight;

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

    if(highlight == 0) {
        // idx became 0. restore it back to current
        highlight = current_highlight;
    }

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
        udp_dbg("key=UP\n");
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
        udp_dbg("key=DOWN\n");
        if (highlight == num_options) {
            set_highlight(1);
        } else {
            set_highlight(highlight+1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 4) { // Ctrl+D
        udp_dbg("key=CTRL+D\n");
        set_highlight(highlight+4);
        if (highlight > num_options) {
            set_highlight(num_options);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 21) { // Ctrl+U
        udp_dbg("key=CTRL+U\n");
        set_highlight(highlight-4);
        if (highlight < 1) {
            set_highlight(1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == KEY_NPAGE) { // Page Down key
        udp_dbg("key=PG_DOWN\n");
        set_highlight(highlight+8);
        if (highlight > num_options) {
            set_highlight(num_options);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == KEY_PPAGE) { // Page Up key
        udp_dbg("key=PG_UP\n");
        set_highlight(highlight-8);
        if (highlight < 1) {
            set_highlight(1);
        }
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 'G') { // 'G' key to go to the last item
        udp_dbg("key=G\n");
        set_highlight(num_options);
        input_length = 0; // Clear the input buffer
        input_buffer[0] = '\0';
        g_pressed = 0;
    } else if (ch == 'g') { // 'g' key to start sequence for 'gg'
        udp_dbg("key=g\n");
        if (g_pressed) { // If 'g' was already pressed
            set_highlight(1); // Go to the first item
        }
        g_pressed = 1; // Set the state to indicate 'g' was pressed
    } else if (ch == 32) { // Space key to select/unselect
        udp_dbg("key=space\n");
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
        udp_dbg("key=enter\n");
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
    set_tabsize(TAB_WIDTH);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Clear the screen before creating the window
    clear();

    // Calculate the maximum width needed for the window
    max_width = calculate_max_width();
    if(max_width > COLS) {
        max_width = COLS;
    }
    int height = num_options
        + 4; // Increased height for prompt, extra line, and options
    if(height > LINES) {
        height = LINES;
    }
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

    // End ncurses mode
    endwin();

}
