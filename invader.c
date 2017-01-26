#include "invader.h"
#include "constants.h"
#include "player.h"
#include "graphics.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

/* Create instance of a new Invader, called 'inv' and give it row and column atrributes from the constants.h file */
struct Invader inv[INVADERS_ROWS][INVADERS_COLUMNS];
/* Create instance of a new Invader, called 'superInvader' */
struct Invader superInvader;
/* Pass in the the player instance/type */
extern struct Player p;
/* Pass in the status instance/type */
extern int status;
/* Pass in the score instance/type */
extern long int score;
/* Pass in the level instance/type */
extern int level;

/*** GLOBAL VARIABLES ***/
int direction;		  /* default set to 1, moving to the right */
int left_edge_invaders;   /* default set to 7, left-boundary */
int right_edge_invaders;  /* default set to 72, right-bounrday */

int invader_speed;	  /* default set to 50 */
int invader_speedtick;	  /* default set to 0 */
int missile_speed;	  /* default set to 3 */
int missile_speedtick;	  /* default set to 0 */

int curr_num_of_invader_missile; /* default set to 0 */
int count;  /* variable that will keep count of invaders who are alive */

int superInvaderTick;	  /* default set to 0 */
int timeForSuperInvader;  /* random time */
int superInvaderSpeed;	  /* default set to 10 */


/* This function will:
   1: Change the direction of the formation of invaders
   2: Check if the invader should fire a missile, and if so, actually fire a missle
   3: Check the direction and pass it up to 1
*/
void updateInvaders(){
    int i, j;
    /* if its time to update invader */
    /* when invader_speedtick == 3 (invader_speed) */
    if ((++invader_speedtick % invader_speed) == 0) {
        /* update invaders one by one */
        for (i = 0; i<INVADERS_ROWS; i++)
            for (j = 0; j<INVADERS_COLUMNS; j++) {
                /* only update invaders if they are alive*/
                if (inv[i][j].alive) {
                    /* first delete current invader from scene */
                    mvprintw(inv[i][j].posX, inv[i][j].posY-1, "   ");
                    /* depending on the direction invader is going, update the position*/
                    switch (direction) {
			/* When invaders are moving to the right */
                        case 1:
                            if (inv[i][j].posY + 2 < SCREEN_WIDTH)  /* check that we have not gone too far to the right */
                                inv[i][j].posY++;   /* update position by 1 */
                        break;
			/* When invaders are moving to the left */
                        case -1:
                            if (inv[i][j].posY - 2 > 0)	/* check that we have not gone too far to the left */
                                inv[i][j].posY--;   /* update position by -1 */
                        break;
                    }
                    /* now check if this invader should fire a missile*/
                    if (curr_num_of_invader_missile < MAX_NUMBER_OF_INVADERS_MISSILES) /* max number is 3, specified in constants */
                        if (inv[i][j].missileFired == 0 && (rand()%100) <= 3) {	/* set a random time to fire a missle */
			    curr_num_of_invader_missile++;	      /* increase the count of the missiles */
			    inv[i][j].missileFired = 1;		      /* missile is being fired, Boolean */
			    inv[i][j].missileX = inv[i][j].posX + 1;  /* move the missile towards bottom of screen, towards player */
			    inv[i][j].missileY = inv[i][j].posY;      /* no need to change y position of missile */
			}
		  }
            }
        /* check to see if we need to push one more row down */
        left_edge_invaders = left_edge_invaders + direction;
        right_edge_invaders = right_edge_invaders + direction;

        /* this is for direction */
        if (left_edge_invaders == 0 || right_edge_invaders == SCREEN_WIDTH) {
            direction = direction * -1;

            for (i = 0; i<INVADERS_ROWS; i++)
                for (j = 0; j<INVADERS_COLUMNS; j++) {

		    /* this will move the invader formation down the screen */
                    if (inv[i][j].alive) {  /* check that the invaders are alive */
                        inv[i][j].posX++;   /* move the invaders down the screen */

                        if (inv[i][j].posX == SCREEN_HEIGHT - 2)  /* the invaders have reached the bottom of the screen */
                            status = GAME_OVER;
                        }
                }
            }
        }
    /* if invader has a missile no matter if he is alive or dead, update missile position*/
    /* when missile_speedtick == 3 (missile_speed) */
    if ((++missile_speedtick % missile_speed) == 0){
        for (i = 0; i < INVADERS_ROWS; i++)
            for (j = 0; j < INVADERS_COLUMNS; j++) {
                if (inv[i][j].missileFired == 1) {  /* if there is a missile running */
                        mvprintw(inv[i][j].missileX, inv[i][j].missileY, " ");	/* clear the last position it was in */
                        if (checkInvaderMissile(inv[i][j])) { /* if checkInvaderMissile returns 1 */
                            inv[i][j].missileX ++;	      /* move the position of the invader missile down the screen, towards player */
                        } else {
                            inv[i][j].missileFired = 0;	    /* if there is no missile fired */
                            curr_num_of_invader_missile--;  /* decrement the number of missiles invader has */
                        }
                    }
                }
            }
    /* Statements that will display a Super Invader to the screen */
    /* when superInvaderTick == the random time for a SuperInvader */
    if (++superInvaderTick % timeForSuperInvader == 0) {
        if (superInvader.alive == 0) {
	      superInvader.alive = 1; /* superInvader is alive and on screen */
	      superInvader.posX = 2;  /* top */
	      superInvader.posY = -4; /* left */
	      superInvader.invaderlook = "#-#-#";
	      printStatus("WATCH OUT!!! There is SUPER INVADER COMMING"); /* relay warning message to user */
            }
    }
    /* Statements that will clear the last position of the Super Invader and move it along to the right */
    /* move SuperInvader along by 10 ticks */
    if (superInvaderTick % superInvaderSpeed == 0) {
        if (superInvader.alive) {
            mvprintw(superInvader.posX, superInvader.posY, "     ");  /* clear the current superInvader */
            if (superInvader.posY + 5 < SCREEN_WIDTH){	      /* and as long as their is space to the right, superInvader is 5 chars */
		  superInvader.posY = superInvader.posY + 1;  /* move the superInvader to the right by 1 */
            }
            else {
                superInvader.alive = 0;	  /* there is no superInvader alive */
                superInvaderTick = 0;	  /* reset the frame */
                timeForSuperInvader = 500 + rand() % 500; /* reset the random time in which it will appear next */
            }
        }
    }
} /*** End of updateInvaders() ***/



void initInvaders() {
    direction = 1; /* Default, Boolean; move to the right of the screen */
    left_edge_invaders = 7;   /* Specifies left-boundary of the invaders */
    right_edge_invaders = 72; /* Specifies right-boundary of the invaders */

    invader_speed = 50;
    invader_speedtick = 0;
    missile_speed = 5;
    missile_speedtick = 0;

    superInvaderTick = 0;
    timeForSuperInvader = 500 + rand() % 500;
    superInvaderSpeed = 10;   /* superInvader will have higher speed than normal ones */

    curr_num_of_invader_missile = 0; 
    count = INVADERS_ROWS * INVADERS_COLUMNS; /* total number of invaders */
    /* Here, we will set the invaders into a row and column configuration */
    for (int i = 0; i<INVADERS_ROWS; i++)
        for (int j = 0; j<INVADERS_COLUMNS; j++) {
            inv[i][j].posX = 3+i;   /* print invaders down the screen */
            inv[i][j].posY = 8+7*j; /* print invaders across the screen, leaving a space */
            inv[i][j].invaderlook = "o-o";
            inv[i][j].alive = 1;
            inv[i][j].missileFired = 0;
        }
}


/* Function that will display the invaders, the invaders missile(s) and the superInvader */
void displayInvader() {
    for (int i = 0; i<INVADERS_ROWS; i++)
        for (int j = 0; j<INVADERS_COLUMNS; j++) {
            if (inv[i][j].alive)  /* as long as the invader is alive */
                mvprintw(inv[i][j].posX, inv[i][j].posY-1, inv[i][j].invaderlook); /* print it */
            if (inv[i][j].missileFired == 1) {	/* should the invader fire a missile */
                mvprintw(inv[i][j].missileX, inv[i][j].missileY, "*");	/* print it */
        }
    }
    if (superInvader.alive) { /* should superInvader be alive */
        mvprintw(superInvader.posX, superInvader.posY, superInvader.invaderlook); /* print it to the screen */
    }
 }


/* Function that will determine whether an invader has been hit by player's missile */
int checkInvaderHit(){
    for (int i = 0; i<INVADERS_ROWS; i++)
        for (int j = 0; j<INVADERS_COLUMNS; j++) {
            if (inv[i][j].alive) {
                for (int k = -1; k<2; k++) {
		    /* Remember invaders are 3 chars long, so need to determine where the player's missile hit the invader */
                    if ((p.missileX == (inv[i][j].posX)) && ((p.missileY == (inv[i][j].posY+k)))) {
                        mvprintw(inv[i][j].posX, inv[i][j].posY-1, "   ");  /* go back 3 spaces on the Y and delete the invader */
                        inv[i][j].alive = 0;  /* current invader has been defeated */
                        count--;	      /* number of invaders has been decreased */
                        score += 100 * level; /* add a score of 100 with regards to what level user is on */
                        updateScore();	      /* pass this new score into the updateScore() function in player.c */

			/* Switch statement that will prompt 'encouraging' messages to the user */
                        switch(rand()%3){
                            case 0:
                                printStatus("Great Shot! Just keep going.");
                                break;
                            case 1:
                                printStatus("Nuke them down, nuke them down!!!");
                                break;
                            case 2:
                                printStatus("Superb shot!!!");
                                break;
                        }
                        return 1; /* boolean for invader has been hit */
                    }
                }
            }
        }
    return 0;
}


/* Function that will determine whether an Super Invader has been hit by player's missile */
int checkSuperInvaderHit(){
    for (int k = 0; k<5; k++) {
	/* Rememeber Super Invaders are 5 chars long, so need to determine where the player's missile hit the invader */
        if ((p.missileX == (superInvader.posX)) && ((p.missileY == (superInvader.posY+k)))) {
            mvprintw(superInvader.posX, superInvader.posY, "     ");  /* go back 5 spaces on the Y and delete the super invader*/
            superInvader.alive = 0;   /* super invader has been defeated */
            score += 500 * level;     /* add a score of 500 with regards to the current level */
            updateScore();	      /* pass this new score into the updateScore() function in player.c */

	    /* Switch statement that will prompt 'encouraging' messages to the user */
            switch(rand()%3){
                case 0:
                    printStatus("Earth 1 Aliens 0!!!!!!!!!");
                    break;
                case 1:
                    printStatus("Thats the way to gooooo!!!!!!!");
                    break;
                case 2:
                    printStatus("We will destroy them, bravoooo !!!!");
                    break;
                }
            return 1; /* boolean for Super Invader has been hit */
        }
    }
    return 0;
}


/* Function that checks:
   1: Did invader's missile hit the player? - return 0
  OR
   2: Is the invader missile at the bottom of the screen? - return 0
*/
int checkInvaderMissile(struct Invader inv){
    if (checkPlayerHit(inv) || inv.missileX == SCREEN_HEIGHT-2)
        return 0;
    return 1;
}


/* Function that will reset invaders and any invader missiles currently on the screen */
void invaderReset(){
    for (int i = 0; i < INVADERS_ROWS; i++)
        for (int j = 0; j < INVADERS_COLUMNS; j++) {
            if (inv[i][j].missileFired == 1) {	/* if invader fires a missle */
                mvprintw(inv[i][j].missileX, inv[i][j].missileY, " ");	/* clear missile from the screen */
            }
            if (inv[i][j].alive)
                mvprintw(inv[i][j].posX, inv[i][j].posY-1, "   ");	// clear old position of invader
        }
    initInvaders(); /* call the initialise invaders function */
}


/* Function that checks how many invaders are currently alive */
int checkInvadersCount() {
    return count;
}
