#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define MAX_OPTIONS 100
#define MAX_INPUT_LENGTH 3  // Maximum length for numeric input (e.g., "12" or "123")

char *options[MAX_OPTIONS];  // Array to store options
int num_options = 0;  // Number of options
char prompt[256];  // Buffer to store the prompt
int selected[MAX_OPTIONS] = {0};  // Array to track selected options
int multi_select_enabled = 0;  // Multi-select disabled by default

// Function to calculate the maximum width needed for the window
int calculate_max_width() {
    int max_width = 0;
    int i;

    max_width = snprintf(NULL, 0, "%s", prompt);
    for (i = 0; i < num_options; ++i) {
        int item_width;
        if(multi_select_enabled) {
            item_width = snprintf(NULL, 0, "[%2d] * %s", i + 1, options[i]);
        } else {
            item_width = snprintf(NULL, 0, "[%2d] %s", i + 1, options[i]);
        }
        if (item_width > max_width) {
            max_width = item_width;
        }
    }

    return max_width + 4;  // Add extra space for borders and padding
}

// Function to display the options with a border, serial numbers, and a prompt
void display_menu(WINDOW *menu_win, int highlight) {
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
        if(multi_select_enabled == 1) {
            if ((highlight == i + 1) || (selected[i])) {
                wattron(menu_win, A_REVERSE);  // Highlight the selected option
            }

            // Right-align the numbers in the brackets and indicate selected options with '*'
            if (selected[i]) {
                mvwprintw(menu_win, i + 3, 2, "[%2d] * %s", i + 1, options[i]);
            } else {
                mvwprintw(menu_win, i + 3, 2, "[%2d]   %s", i + 1, options[i]);
            }
        } else {
            if (highlight == i + 1) {
                wattron(menu_win, A_REVERSE);  // Highlight the selected option
            }

            // Right-align the numbers in the brackets
            mvwprintw(menu_win, i + 3, 2, "[%2d] %s", i + 1, options[i]);

        }
        wattroff(menu_win, A_REVERSE);  // Remove the highlight
    }

    refresh();
    // Refresh the window to show changes
    wrefresh(menu_win);
    curs_set(0);  // Hide the cursor
}

// Function to load options from a file
void load_options(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the first line as the prompt
    if (fgets(prompt, sizeof(prompt), file) == NULL) {
        perror("Error reading prompt from file");
        exit(EXIT_FAILURE);
    }

    // Remove newline character if present
    prompt[strcspn(prompt, "\n")] = '\0';

    // Allocate memory for each option (assuming max length per option is 255)
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        options[i] = malloc(256 * sizeof(char));
        if (options[i] == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
    }

    // Read options from the file
    num_options = 0;
    while (num_options < MAX_OPTIONS && fgets(options[num_options], 256, file)) {
        // Remove newline character if present
        options[num_options][strcspn(options[num_options], "\n")] = '\0';
        num_options++;
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <options_file> <selected_file> [multi_select=yes]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 4 && strcmp(argv[3], "multi_select=yes") == 0) {
        multi_select_enabled = 1;
    }

    WINDOW *menu_win;
    int ch;
    int highlight = 1;
    char input_buffer[MAX_INPUT_LENGTH] = {0};  // Buffer to store numeric input
    int input_length = 0;  // Length of the current input in the buffer
    int g_pressed = 0;  // State to track if 'g' was pressed

    // Load options from the specified file
    load_options(argv[1]);
    const char *selected_filename = argv[2];  // Filename for saving the selected option

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Clear the screen before creating the window
    clear();

    // Calculate the maximum width needed for the window
    int max_width = calculate_max_width();
    int height = num_options + 4;  // Increased height for prompt, extra line, and options
    int start_y = 0;  // Set start_y to 0 to position the window at the top of the screen
    int start_x = 0;  // Set start_x to 0 to position the window at the left edge of the screen
    menu_win = newwin(height, max_width, start_y, start_x);

    // Display the menu immediately
    display_menu(menu_win, highlight);

    int select_done = 0;
    while ((ch = getch()) != 27) {  // Press 'Esc' to quit (ASCII 27)
        if (ch >= '0' && ch <= '9') {  // Check if the key is a digit
            if (input_length < MAX_INPUT_LENGTH - 1) {
                input_buffer[input_length++] = ch;  // Add digit to the buffer
                input_buffer[input_length] = '\0';  // Null-terminate the buffer

                // Convert input buffer to an integer
                int input_number = atoi(input_buffer);

                // Validate and set highlight if input is within range
                if (input_number >= 1 && input_number <= num_options) {
                    highlight = input_number;
                }

                // If the number is a single digit and valid, highlight immediately
                if (input_length == 1 && input_number >= 1 && input_number <= num_options) {
                    highlight = input_number;
                    input_length = 0;  // Clear the input buffer after immediate selection
                    input_buffer[0] = '\0';
                }
            }
        } else if (ch == 'k' || ch == KEY_UP || ch == 16) {  // Key 'k' or UP or CTRL+p for up
            if (highlight == 1) {
                highlight = num_options;
            } else {
                --highlight;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 'j' || ch == KEY_DOWN || ch == 14) {  // Key 'j' or DOWN or CTRL+n for down
            if (highlight == num_options) {
                highlight = 1;
            } else {
                ++highlight;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 4) {  // Ctrl+D
            highlight += 4;
            if (highlight > num_options) {
                highlight = num_options;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 21) {  // Ctrl+U
            highlight -= 4;
            if (highlight < 1) {
                highlight = 1;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == KEY_NPAGE) {  // Page Down key
            highlight += 8;
            if (highlight > num_options) {
                highlight = num_options;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == KEY_PPAGE) {  // Page Up key
            highlight -= 8;
            if (highlight < 1) {
                highlight = 1;
            }
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 'G') {  // 'G' key to go to the last item
            highlight = num_options;
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 'g') {  // 'g' key to start sequence for 'gg'
            if (g_pressed) {  // If 'g' was already pressed
                highlight = 1;  // Go to the first item
            }
            g_pressed = 1;  // Set the state to indicate 'g' was pressed
        } else if (ch == 32) {  // Space key to select/unselect
            if (!multi_select_enabled) {
                memset(selected, 0, sizeof(selected));  // no multi select support. clear all selection
            }
            selected[highlight - 1] = !selected[highlight - 1];  // Toggle selection
            input_length = 0;  // Clear the input buffer
            input_buffer[0] = '\0';
            g_pressed = 0;
        } else if (ch == 10) {  // Enter key
            if (!multi_select_enabled) {
                memset(selected, 0, sizeof(selected));  // no multi select support. clear all selection
                selected[highlight - 1] = !selected[highlight - 1];  // Toggle selection
            }
            select_done = 1;
            break;  // Exit loop if single selection mode
        }

        display_menu(menu_win, highlight);
    }

    if (!multi_select_enabled) {
        //while loading itself it will highlight and select 1 entry;
        select_done = 1;
    }

    // Save selected options to the specified file
    FILE *selected_file = fopen(selected_filename, "w");
    if (!selected_file) {
        perror("Error opening selected file");
        exit(EXIT_FAILURE);
    }

    int num_selected = 0;
    for (int i = 0; i < num_options; ++i) {
        char *separator;
        separator = ",";
        if (selected[i]) {
            if(num_selected++ == 0) {
                separator = "";
            }
            fprintf(selected_file, "%s%s", separator, options[i]);
        }
    }

    fclose(selected_file);

    // Free allocated memory for options
    for (int i = 0; i < MAX_OPTIONS; ++i) {
        free(options[i]);
    }

    // End ncurses mode
    endwin();

    int retval = EINVAL;

    if(select_done == 1) {
        retval = 0;
    }

    return retval;
}

