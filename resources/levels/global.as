/**
 * Author Frantisek Vevekra
 * 2017
 */

// #include "we can include other files"

// Executed right after the map is loaded
void mapLoaded(Level@ level){
	// some water for refreshments
	level.raiseWater();
	level.raiseWater();
	level.raiseWater();
	//prints to standard output
	print("Script> mapLoaded done!\n");
}

// Executed each game tick for each player
void playerThink(Player@ player){
	//	player.die();  UNCOMMENT AND SEE WHAT HAPPENS 
}