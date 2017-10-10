/**
 * Author Frantisek Vevekra
 * 2017
 */

int playersAlive = 0;
uint elapsedFrames = 0;
uint roundNumber = 0;
float roundTime = 0;
float waterRiseCountDown = 100;

void roundUpdate(Round@ round, float elapsedTime, uint frame) {
	World @ w = round.getWorld();
	//w.setLevelBlock(0, 2, 2);
	//w.setLevelBlock(6, 2, 2);
	int currentRound = round.getRoundNumber();
	elapsedFrames = frame;

	if (currentRound != roundNumber) {
		roundNumber = currentRound;
		roundTime = 0;
	} else {
		roundTime += elapsedTime;
	}
	int alive = w.getPlayersAliveCount();
	//print('Alive:' + alive + '\n');
	if (playersAlive != alive) {
		playersAlive = alive;
		waterRiseCountDown = playersAlive;
	}

	if (playersAlive > 2) {
		waterRiseCountDown -= elapsedTime;
		if (waterRiseCountDown <= 0) {
			w.raiseWater();
			waterRiseCountDown = playersAlive;
		}
	}
	playersAlive = alive;
}

void playerThink(Player@ player, uint & in id ) {

}

uint r1 = 1;
uint r2 = 2;
uint Rand(uint mod) {
	uint t = r2;
	r2 = r1 + r2;
	r1 = t;

	return r2 % mod;
}
