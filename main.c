#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#include <ncurses.h>

#include "src/curses-ui.h"
#include "src/utils.h"

int main(void) {
    WINDOW *window = setup_window();
    start_ui_loop(window);
    return EXIT_SUCCESS;
}
