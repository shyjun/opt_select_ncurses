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

#pragma once

#define MAX_OPTIONS 4999
#define MAX_INPUT_LENGTH                                                       \
    100 // Maximum length for numeric input (e.g., "12" or "123")
#define MAX_OPTION_STR_LENGTH 256

void
udp_dbg(const char *fmt, ...);

void
set_udp_port(int port);

void set_highlight(int val);

int get_highlight();

void set_multi_select_enabled(int val);

int get_multi_select_enabled();

int get_num_options();

int get_selected_flag(int idx);

void set_prompt(const char *str);

char *get_prompt();

void init_options_array();

void add_option(char *option_str);

int get_last_ch();

char *get_option(int idx);

void clean_up_opt_select_ncurses();

void run_opt_select_ncurses();
