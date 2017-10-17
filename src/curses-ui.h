#ifndef CURSES_UI_H
#define CURSES_UI_H

#include <stdbool.h>
#include <stdlib.h>

#include <ncurses.h>

#include "utils.h"

enum ui_section_tag {
    ui_section_tag_chains,
    ui_section_tag_tables
};

enum ui_state_change_tag {
    ui_state_change_tag_noop,
    ui_state_change_tag_change_section,
    ui_state_change_tag_exit_requested,
    ui_state_change_tag_go_back,
};

struct ui_state_change {
    enum ui_section_tag destination_section;
    enum ui_state_change_tag tag;
};

struct ui_option {
    char const *const title;
    char const *const description;

    struct ui_state_change (*const activate)(
        struct ui_option const *option,
        WINDOW *window
    );
};

struct ui_section {
    char const *const title;
    struct ui_option const *const options;
    size_t const options_length;
    size_t active_option_index;
    struct ui_section *const parent;
};

enum ui_selection_state {
    ui_selection_state_active,
    ui_selection_state_inactive,
};

void endwin_without_result(void);
WINDOW *setup_window(void);
void start_ui_loop(WINDOW *WINDOW);

#endif
