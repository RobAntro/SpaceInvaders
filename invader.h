#ifndef INVADERH
struct Invader {
	int posX;	  // horizontal position of invader
	int posY;	  // vertical position of invader
	int missileFired; // 0: missile not running; 1: missile running
	int missileX;	  // horizontal position of missile
	int missileY;	  // vertical position of missile
	char *invaderlook; // look of the invader, current look is o-o
	int alive; // is invader allive
};

/* Declare invader functions to include in other files for compilation */
void updateInvaders();
void initInvaders();
void displayInvader();
int checkInvaderHit();
int checkSuperInvaderHit();
int checkInvaderMissile(struct Invader inv);
void invaderReset();
int checkInvadersCount();

#define INVADERH
#endif
