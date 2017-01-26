#include<ncurses.h>
#include<time.h>
#include<sys/time.h>  //used for getting the time of day
#include<stdlib.h>
#include<stdio.h>
#include "player.h"
#include "constants.h"
#include "invader.h"
#include "game.h"
#include "graphics.h"



extern struct Player p;

long int score;	/* score could be in the thousands, so ensure enough space to hold it */
int status;	/* used for operations in the game, e.g. if the game is paused, if the game is over
		  This status maps to the constants specified in the header file */
int level;	/* at run time, this will be set to zero */

int gameOverTick;	/* initially set to 0 */
int welcomeScreenTick;	/* initially set to 0 */
int printMessage;

void drawScreen();
void printWelcomeScreen();

/* This function will initialise the level by reseting all attributes of the game
   Called at WelcomeScreen
 */
void initLevel()
{
    playerReset();
    invaderReset();
    level = 0;
    score = 0;
    welcomeScreenTick = 0;
    gameOverTick=0;
    drawScreen();
}



/**** Main Function ****/
int main() {

    /* The following will initialise our ncurses mode */
    initscr();		    /* start ncurses mode */
    cbreak();		    /* disable the terminal driver buffer. want to get a character immediately */
    nodelay(stdscr, TRUE);  /* we do not want a delay on the screen */
    curs_set(0);	    /* disable the cursor. make it invisible */
    keypad(stdscr, TRUE);   /* set the keyboard to true to be used */
    noecho();		    /* disable echo, we don't want to keep updating coordinates that don't need changing */


    /* This section is concerned with setting up a time frame so the game runs smoothly
       http://www.koonsolo.com/news/dewitters-gameloop/
     */
    struct timeval tv;	      /* declare a struct for the time value, allocate tv as the time value */
    gettimeofday(&tv, NULL);  /* start the time of day, by getting the system time of the computer */

    /* get the next_game_tick by converting tv into ms */
    long int next_game_tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    int loops;

    initPlayer();
    initInvaders();
    status = PRINT_GAME_WELCOMESCREEN;
    printMessage = 1;
    int game_is_running = 1;

    while( game_is_running ) {
        loops = 0;  /* declare a loop, local to the frame rate function */
        gettimeofday(&tv, NULL);  /* call time function again, will have changed by now */
	/* set the current time to ms */
        long int curr_game_tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	/* Check that the current ms that have elapsed is more than the tick previously defined
	   earlier and that the number of loops is not more than the max frame skip */
        while (curr_game_tick > next_game_tick && loops < MAX_FRAMESKIP) { /* MF = 5 */
            get_user_input();
            update_game();
            next_game_tick += SKIP_TICKS; /* will set next_game_tick to 20 */
            gettimeofday(&tv, NULL);  /* need to call time function again, will have changed again */
            curr_game_tick = tv.tv_sec * 1000 + tv.tv_usec / 1000;  /* and reset the current game tick */
            loops++;
        }
        display_game(); /* will take on the current FPS */
    }
    endwin(); /* end curses mode, game is not running */
    return 0;
} /*** end of MAIN FUNCTION ***/




/* The following uses a switch statement to control the state of the game through user input */
void get_user_input(){
    int ch = getch(); /* get the user input */
    switch (status){
        case GAME_PAUSED:
            mvprintw(SCREEN_HEIGHT-1,1, "game paused");	/* let the user know the game is paused */
            if (ch == 'p') {	/* when user presses p to continue the game after the pause */
                mvprintw(SCREEN_HEIGHT-1,1, "           ");   /* delete the prompt to the user */
                status = GAME_LOOP;   /* call the game loop function below when user 'un-pauses' */
            }
            break;

	case GAME_OVER:
            break; // don't do anything, game is over

        case GAME_WELCOMESCREEN:
            if (ch == ' ') {  /* when user presses spacebar at welcome screen */
                clear();      /* clear the screen */
                initLevel();  /* start the game by calling initLevel function */
                status = GAME_NEXTLEVEL;  /* call the nextlevel functionbelow */
            }
            break;

        default:  /* when none of the above, go to default to get user input */
	/* switch statement to check user input */
        switch(ch){
            case KEY_DOWN: /* cannot move player down */
                break;
            case KEY_UP:  /* cannot move player up */
                break;
            case KEY_LEFT:
                movePlayerLeft();
                break;
            case KEY_RIGHT:
                movePlayerRight();
                break;
            case 'p':
                status = GAME_PAUSED;
                break;
            case 'q':
                status = GAME_EXIT;
                break;
            case 'w':
                status = GAME_NEXTLEVEL;
                break;
            case ' ':
                firePlayerMissile();
                break;
        }
    }
}

/* This function will continously update the game state by:
    1: Going to the next level of the game
    2: Looping the game and checking if the player has been defeated or invaders have been defeated
    3: Pausing the game
    4: Run the game_over function and display the welcomescreen to the user
    5: Exiting the game
    6: Updating the welcomescreen frame by calling printWelcomeScreen()
*/
void update_game(){
    switch (status){
	/* This case will execute when 
	   1: The user presses the spacebar and begins the game
	   2: The invaders have been defeated
	   3: Continue to the GAME_LOOP	
	*/
        case GAME_NEXTLEVEL:
            level++;
            invaderReset();
            drawScreen();
            status = GAME_LOOP;
	/* notice there is no break, continues onto the following ... */

	/* This case checks if the player has been defeated, if so, signal GAME_OVER status
	   and also checks if all the invaders have been defeated, if so, signal NEXTLEVEL status
	   and will update both the player and the invaders 
	*/
        case GAME_LOOP:
            if (checkIfPlayerIsDead()) 
                status = GAME_OVER;
            if (checkInvadersCount() == 0)
                status = GAME_NEXTLEVEL;
            updatePlayer();
            updateInvaders();
            break;

        case GAME_PAUSED:
            break;
        case GAME_OVER:
            printStatus("GAME OVER!!! The Aliens won this time, but the war is not over");
	    /* when gameOverTick == 200 */
            if (++gameOverTick % 200 == 0){
                status = PRINT_GAME_WELCOMESCREEN;  // go back to welcome screen
                gameOverTick = 0;		    // reset gameOverTick
            }
            break;
        case GAME_EXIT:
            exit(0);  // quit the program
            break;
        case PRINT_GAME_WELCOMESCREEN:
            clear();  // clear the current screen
            printWelcomeScreen();   // call to printWelcomeScreen below
            status = GAME_WELCOMESCREEN;  // set the status
            break;
        case GAME_WELCOMESCREEN:
	    /* when welcomeScreenTick == 50 */
            if (++welcomeScreenTick % 50 == 0) {
		/* The prompt for the user, will keep flashing between printMessage 0 and 1, hence why we need a tick */
                if (printMessage == 1) {
                    mvprintw(19, 10, "                                                 ");  /* print a blank between frames */
                    printMessage = 0;
                    }
                else {
                    mvprintw(19, 10, "PRESS SPACEBAR TO START THE GAME!!!!");
                    printMessage = 1;
                }
            }
            break;
    }
}

/* This function will switch between the GAME_LOOP (the game is running)
   and the GAME_WELCOMESCREEN by the use of the ncurses refresh() function
*/
void display_game() {
    switch (status){
        case GAME_LOOP:
            displayPlayer();
            displayInvader();
            break;
        case GAME_WELCOMESCREEN:
            break;
    }
    refresh();
}

/* Function that will:
   1: Update the score
   2: Print the number of levels remaining
   3: Update the player lives
*/ 
void drawScreen() {
    updateScore();
    char buffer[30];
    sprintf(buffer, "Level: %d", level);
    mvprintw(1, 1, buffer);
    updatePlayerLives();
}


/* Function will use ASCII art to display welcome screen to the user */
void printWelcomeScreen(){
mvprintw(3, 2, "                       ####  ####   ###    ####   ####");
mvprintw(4, 2, "                     #      #   #  #   #  #      #");
mvprintw(5, 2, "                    #      #   #  #   #  #      #");
mvprintw(6, 2, "                   ####   ####   #####  #      #####");
mvprintw(7, 2, "                      #  #      #   #  #      #");
mvprintw(8, 2, "                     #  #      #   #  #      #");
mvprintw(9, 2, "                ####   #      #   #   ####   ####");

mvprintw(11, 2, "       #  #   #  #   #   ###   ####   #####  ####    ####");
mvprintw(12, 2, "      #  #   #  #   #  #   #  #   #  #      #   #  #");
mvprintw(13, 2, "     #  ##  #  #   #  #   #  #   #  #      #   #  #");
mvprintw(14, 2, "    #  # # #  #   #  #####  #   #  #####  ####   ####");
mvprintw(15, 2, "   #  #  ##  #   #  #   #  #   #  #      # #        #");
mvprintw(16, 2, "  #  #   #   # #   #   #  #   #  #      #  #       #");
mvprintw(17, 2, " #  #   #    #    #   #  ####   #####  #   #  ####");
mvprintw(19, 10, "PRESS SPACEBAR TO START THE GAME!!!!");
}
