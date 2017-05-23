/**
 * Author Frantisek Vevekra
 * 2017
 */
// #include "we can include other files"
//bool played = false;
//Sample s= {SOUND.loadSample('sound/game/water-red.wav')};
float targetx = 0;
float targety = 0;


uint8 Left   = 0b00000001;
uint8 Right  = 0b00000010;
uint8 Up     = 0b00000100;
uint8 Down   = 0b00001000;
uint8 Fire   = 0b00010000;
uint8 Pick   = 0b00100000;
uint8 Status = 0b01000000;


uint FlagDead = 0x01;
uint FlagPick = 0x02;
uint FlagKnee = 0x04;
uint FlagLying = 0x08;
uint FlagMoveDown = 0x10;
uint FlagMoveLeft = 0x40;
uint FlagMoveRight = 0x80;
uint FlagMoveUp = 0x100;
uint FlagHasGun = 0x200;
uint FlagGhost = 0x400;
uint FlagDoubleJumpDebounce = 0x800;
uint FlagDoubleJump = 0x1000;
float roundTime = 0;
int roundNumber = 0;
int counter = 0;
// Executed right after the map is loaded
void mapLoaded(Level@ level){
	// some water for refreshments
	//	level.raiseWater();
	//prints to standard output
	print("Script> mapLoaded done!\n");


	//s = SOUND.loadSample('sound/game/water-red.wav');

	console.print("Output to quake console\n");
}
// Executed each frame
void roundUpdate(Round@ round, float  elapsedTime){
	int currentRound = round.getRoundNumber();
	if (currentRound != roundNumber){
		roundNumber = currentRound;
		roundTime = 0;
	} else {
		roundTime += elapsedTime;
	}
}
// Executed each game tick for each player
void playerThink(Player@ player, uint & in id ){

	if(id == 0){
		targetx = player.position.x;
		targety = player.position.y;

		player.ammo = 100;
		player.life = 100;
	} else {
		float xdiff = player.position.x - targetx;
		float ydiff = player.position.y - targety;

		float xthresh = 15 + id - 5;
		float ythresh = 4 + id - 5;

		if(xdiff * xdiff < xthresh && ydiff * ydiff < ythresh && roundTime > 5){
			player.setControl(Fire);
		}
		if(player.position.x < targetx + id - 5){
			player.setControl(Right);
		} else {
			player.setControl(Left);
		}
		int itsJumpTime = roundTime % id;
		if(player.position.y < targety + id - 5 || itsJumpTime == 0){
			player.setControl(Up);
		} else {
			//player.setControl(Down);
		}
	}

	//	player.die();  UNCOMMENT AND SEE WHAT HAPPENS 
}