#include "constants.h"
#include "player.h"
#include "invader.h"
#include <ncurses.h>

/* This simple function will relay messages to the user to the bottom-left of the screen
   these includes, prompts such as "game paused" and when a user defeats an invader/superInvader
*/
void printStatus(char *status) {
    mvprintw(SCREEN_HEIGHT-1, 1, "                                                                    ");
    mvprintw(SCREEN_HEIGHT-1, 1, status);
}
