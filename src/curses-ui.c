#include "curses-ui.h"

void endwin_without_result(void) { endwin(); }

WINDOW *setup_window(void) {
    WINDOW *window = initscr();
    atexit(endwin_without_result);
    cbreak();
    noecho();
    keypad(stdscr, true);
    nonl();
    return window;
}

void ui_section_draw_header() {
    mvprintw(1, 2, "curses-firewall v0.1");
    mvprintw(
        3,
        2,
        (
            "Movement: wasd, hjkl, or Arrow Keys"
            "        Activate: Enter"
            "        Quit: q"
        )
    );
}

void ui_option_draw(
    struct ui_option const *option,
    enum ui_selection_state state,
    WINDOW *window
) {
    int x, y;
    getyx(window, y, x);
    if (state == ui_selection_state_active) {
        attron(A_REVERSE);
    }
    mvprintw(y, 4, "%s", option->title);
    if (state == ui_selection_state_active) {
        attroff(A_REVERSE);
    }
    mvprintw(y + 1, 6, "%s", option->description);
    move(y + 3, x);
}

void ui_section_draw(struct ui_section const *section, WINDOW *window) {
    box(window, 0, 0);
    mvprintw(5, 2, "%s", section->title);
    move(7, 0);
    struct ui_option const *options = section->options;
    size_t len = section->options_length;
    for (size_t i = 0; i < len; ++i) {
        ui_option_draw(
                options + i,
                (i == section->active_option_index)
                        ? ui_selection_state_active
                        : ui_selection_state_inactive,
                window
        );
    }
}

struct ui_state_change on_key(
        struct ui_section *section,
        int key,
        WINDOW *window
) {
    size_t active_index = section->active_option_index;
    size_t len = section->options_length;
    struct ui_state_change change;

    switch (key) {
    case 'w':
    case 'k':
    case KEY_UP:
        if (0 < active_index) {
            --section->active_option_index;
        }
        change.tag = ui_state_change_tag_noop;
        break;
    case 's':
    case 'j':
    case KEY_DOWN:
        if (active_index < (len - 1)) {
            ++section->active_option_index;
        }
        change.tag = ui_state_change_tag_noop;
        break;
    case 'e':
    case '\r':
    case KEY_ENTER:
        {
            struct ui_option const *option = section->options + active_index;
            change = section->options[active_index].activate(option, window);
        }
        break;
    case 'q':
        change.tag = ui_state_change_tag_exit_requested;
        break;
    default:
        change.tag = ui_state_change_tag_noop;
        break;
    }
    return change;
}

struct ui_state_change ui_chain_activate(
        struct ui_option const *option,
        WINDOW *window
) {
    informf("Manipulating the UI chain from %s", option->title);
    return (struct ui_state_change) {
        .tag = ui_state_change_tag_noop,
    };
}

struct ui_section make_chain_section(struct ui_section *parent) {
    static struct ui_option const options[]  = {
        {
            "Input",
            "View or manipulate the input chain.",
            ui_chain_activate,
        },
        {
            "Output",
            "View or manipulate the output chain.",
            ui_chain_activate,
        },
        {
            "Prerouting",
            "View or manipulate the prerouting chain.",
            ui_chain_activate,
        },
        {
            "Postrouting",
            "View or manipulate the postrouting chain.",
            ui_chain_activate,
        },
    };

    return (struct ui_section) {
        .title = "Chains",
        .options = options,
        .options_length = sizeof(options) / sizeof(options[0]),
        .active_option_index = 0,
        .parent = parent,
    };
}

struct ui_state_change ui_table_activate(struct ui_option const *option, WINDOW *window) {
    informf("Manipulating table from %s", option->title);
    return (struct ui_state_change) {
        .tag = ui_state_change_tag_noop,
    };
}

struct ui_section make_main_section(void) {
    static struct ui_option const options[] = {
        {
            "nat",
            "View or manipulate the nat table.",
            ui_table_activate,
        },
        {
            "filter",
            "View or manipulate the filter table.",
            ui_table_activate,
        },
        {
            "raw",
            "View or manipulate the raw table.",
            ui_table_activate,
        },
    };

    return (struct ui_section) {
        .title = "Tables",
        .options = options,
        .options_length = sizeof(options) / sizeof(options[0]),
        .active_option_index = 0,
        .parent = NULL,
    };
}

void start_ui_loop(WINDOW *window) {
    int entered;
    clear();
    ui_section_draw_header();

    struct ui_section main_section = make_main_section();
    struct ui_section chain_section = make_chain_section(&main_section);
    struct ui_section *current_section = &main_section;

    for (bool stop = false; !stop; ) {
        refresh();
        ui_section_draw(current_section, window);
        entered = getch();
        struct ui_state_change result = on_key(current_section, entered, window);
        switch (result.tag) {
        case ui_state_change_tag_exit_requested:
            stop = true;
            break;
        case ui_state_change_tag_go_back:
            if (current_section->parent != NULL) {
                current_section = current_section->parent;
            }
            break;
        case ui_state_change_tag_change_section:
            switch (result.destination_section) {
            case ui_section_tag_chains:
                break;
            case ui_section_tag_tables:
                break;
            default:
                dief(
                    "unknown ui_section_tag: %d",
                    result.destination_section
                );
            }
            break;
        case ui_state_change_tag_noop:
            break;
        default:
            dief("unknown ui_state_change_tag: %d", result.tag);
        }
    }
}
