/**
 * Author Frantisek Vevekra
 * 2017
 */

// #include "we can include other files"

// Executed right after the map is loaded
Sample s;
void mapLoaded(Level@ level){
	// some water for refreshments
	level.raiseWater();
	level.raiseWater();
	level.raiseWater();
	//prints to standard output
	print("Script> mapLoaded done!\n");


	s = SOUND.loadSample('sound/game/game-over.wav');
	//prints to in-game console
	console.print("Output to quake console\n");
}

// Executed each game tick for each player
void playerThink(Player@ player, int & in id ){
	//	player.die();  UNCOMMENT AND SEE WHAT HAPPENS 
}