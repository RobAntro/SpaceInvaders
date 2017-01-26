#ifndef PLAYERH

#include <stdio.h>

struct Player {
	int posX;	  // horizontal position of player
	int posY;	  // vertical position of player
	int missileFired; // 0: missile not running; 1: missile running
	int missileX;	  // horizontal position of missile
	int missileY;	  // vertical position of missile
	int missilespeed; // missilespeed
	int missilecurrenttick; //missilecurrenttick, determents in what frame to move the missile
	char *playerlook;   // look of the player current look
    int num_of_lives;
};

/* Declare player functions to include in other files for compilation */
void initPlayer();
void movePlayerLeft();
void movePlayerRight();
void displayPlayer();
void firePlayerMissile();
void updatePlayer();
int checkPlayerMissile();
int checkPlayerHit();
void playerReset();
int checkIfPlayerIsDead();
void updatePlayerLives();
void updateScore();

#define PLAYERH
#endif

