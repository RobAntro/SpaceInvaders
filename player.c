#include "player.h"
#include "constants.h"
#include "invader.h"
#include <ncurses.h>

struct Player p;	    /* create instance of a new Player type, called 'p' */
extern struct Invader inv;  /* include the invader instance/type, 'inv' */
extern long int score;	/* get pre-defined score from game.c */
extern int level;	/* get pre-defined level from game.c */


/* This function will initialise player attributes 
   by declaring values using the player structure template 
*/
void initPlayer(){
    p.posX = SCREEN_HEIGHT - 2;	/* think of posX as relating to rows, the bottom of the screen will be at
				 maximum Y value, so subtract 2, to set the player near the bottom */

    p.posY = SCREEN_WIDTH / 2;	/* think of posY as relating to a column, the middle of the screen will be
				 at the middle of the screen_width, divided by 2, to set player at center */
    p.playerlook = "/-\\";
    p.missileFired = 0;
    p.missilespeed = 3;
    p.missilecurrenttick = 0;
    p.num_of_lives = 3;
}


/* This function will update the player's missile */
void updatePlayer() {
    if (p.missileFired == 1) {	/* if the missle is running. i.e. if user is pressing spacebar */
	/* when missilecurrenttick == 3 (missilespeed) */
        if ((++p.missilecurrenttick % p.missilespeed) == 0){
            mvprintw(p.missileX, p.missileY, " ");  /* print a blank space */

            if (checkPlayerMissile()) {	/* if checkPlayerMissile returns 1 */
                p.missileX--;		/* change missile position */
            } 
	      else {		      /* if checkPlayerMissile returns 0 */ 
                p.missileFired = 0;   /* remove it from the screen, set missile to false */
                p.missilecurrenttick = 0; /* reset the missile tick */
            }
        }
    }
}


/* Function to allow user to move player to the left of the screen */
void movePlayerLeft() {
    if (p.posY - 2 > 0) { /* checks that player has not gone to far to the left, we need 2 spaces to keep player on screen */
	  mvprintw(p.posX, p.posY-1, "   ");  /* if true, it will print an empty space, deleting the player */
	  p.posY --;	  /* remember to decrement, as we are going to the left */
        }
}


/* Function to allow user to move player to the right of the screen */
void movePlayerRight(){	  
    if (p.posY + 2 < SCREEN_WIDTH) {  /* checks that player has not gone to far to the right, need 2 spaces to keep player on screen */
	  mvprintw(p.posX, p.posY-1, "   ");  /* if true, it will print an empty space, deleting the player */
	  p.posY++;     // remember to increment, as we are going to the right
        }
}


/* Function that will print the player and the player's missile */
void displayPlayer() {
    if (p.num_of_lives)	/* as long as number of lives >0 */
	  mvprintw(p.posX, p.posY-1, p.playerlook); /* print the player */
    if (p.missileFired == 1) {	/* as long as user presses spacebar, see the firePlayerMissile() below */
	  mvprintw(p.missileX, p.missileY, "|");	  /* print the missile */
    }
 }


/* Function that will set missileFired to true and change the position of the player's missile */
void firePlayerMissile(){
    if (p.missileFired == 0) { 
	  p.missileFired = 1; /* set missile being fired to true, 1 */
	  p.missilecurrenttick = 0;   /* set frame to 0 */
	  p.missileX = p.posX - 1;    /* has to move 'back-up' the screen */
	  p.missileY = p.posY;	      /* don't need to change Y (x) position */
        }
}


/* Function that checks:
   1: Did player's missile hit the invader? - return 0
  OR
   2: Did it hit the SuperInvader? - return 0
  OR
   3: Is the player missile in second row? -  return 0; we need to remove it
*/
int checkPlayerMissile() {
    if (checkInvaderHit() || checkSuperInvaderHit() || p.missileX == 2)
        return 0;
    return 1; /* nothing has happened with the missile, it continues up towards the invaders */
}


/* This function will determine if the invader's missile has hit the player's ship */
int checkPlayerHit(struct Invader inv){
    for (int j = -1; j<2; j++) {
        if ( (inv.missileX == (p.posX)) && ((inv.missileY == (p.posY+j)))) { /* should the invader missile reach the player's position */
	      mvprintw(p.posX, p.posY-1, "   ");	/* clear the part of the player's ship that was hit */
	      p.num_of_lives--;	    /* player has lost a life */
	      updatePlayerLives();  /* need to update this lost life to the status bar */
            return 1;
            }
    }
    return 0;
}


/* Function that will reset player and any player missiles */
void playerReset(){
    if (p.missileFired == 1) {
		mvprintw(p.missileX, p.missileY, " ");	/* clear any player missiles */
	}
	mvprintw(p.posX, p.posY-1, "   ");	/* clear old position of player */
    initPlayer();   /* go back to intialise function declared earlier */
}



int checkIfPlayerIsDead(){
    return !p.num_of_lives;
}


/* Function that will update number of lives player has left to the user
   To retain respect to original game, lives will be represented as the player cannon
   mprintw() function will print the player's lives 1 space apart (19+4*i)
 */
void updatePlayerLives() {
    mvprintw(1, 11, "Lives:              ");
    for (int i = 0; i<p.num_of_lives; i++) {
        mvprintw(1, 19+4*i, "/-\\ "); /* need to print horizontally leaving a space */
    }
}


/* Function that will take the score and continuously update it to the user */
void updateScore() {
    char buffer[30];  // remember, score is declared as a long int
    sprintf(buffer, "Score: %ld", score);
    mvprintw(1, 60, buffer);  // print at top right
}
