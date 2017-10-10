/**
 * Author Frantisek Vevekra
 * 2017
 */
// #include "we can include other files"
//bool played = false;
//Sample s= {SOUND.loadSample('sound/game/water-red.wav')};

float targetx = 0;
float targety = 0;
float targetPosx = 0;
float targetPosy = 0;
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
array<PathSegment@> segments;

PathSegment@ createPathSegment(uint id, uint l, uint r, int y){
	PathSegment @handle = PathSegment(id, l, r, y);
	segments.insertLast(handle);
	return @handle;
}

grid<bool> matrix; // TODO bool -> distance
array<array<PathSegment@> > adjacencyList;
grid<bool> walls;

grid<PathSegment@> segmentMap;

uint width;
uint height;
Level @level;

uint segmentCounter = 0;
PathSegment@ zero = createPathSegment(0,0,0,0);
PathSegment@ getSegmentForBlock(uint x, int y){
	uint uy = y;
	int height_1 = height - 1;
	if(!level.isWall(x, y, true)){
		return zero;
	}
	if(x == 0 && y == height_1){
		return createPathSegment(++segmentCounter , x, x, y);
	}

	if(y == height_1){
		if(level.isWall(x-1, uy, true)){
			PathSegment@ prev = segmentMap[x-1, uy];
			if(prev.y == y)
			{
				prev.r = x;
				return prev;
			}
		}
		return createPathSegment(++segmentCounter, x, x, y);
	}


	if(x == 0){
		if(level.isWall(x, y + 1, true)){
			PathSegment@ prev = segmentMap[x, y + 1];
			return prev;
		}
		return PathSegment(++segmentCounter, x, x, y);
	}
	
	if(level.isWall(x, y + 1, true)){
		PathSegment@ prev = segmentMap[x, y + 1];
		return prev;
	}
	if(level.isWall(x - 1, y, true)){
		PathSegment@ prev = segmentMap[x - 1, y];
		if(prev.y == y)
		{
			prev.r = x;
			return prev;
		}
	}

	return createPathSegment(++segmentCounter, x, x, y);
}

// Executed right after the map is loaded
void mapLoaded(Level@ currentLevel){
	@level = currentLevel;

	// some water for refreshments
	//	level.raiseWater();

	width = currentLevel.getWidth();
	height = currentLevel.getHeight();
	//prints to standard output
	print("Script> mapLoaded done! \n");
	print("Calculating navigation graph! \n");
	walls.resize(width, height);
	segmentMap.resize(width, height);
	
	for(int y = height - 1; y >= 0; y--){
		for(uint x = 0; x < width; x++){
			walls[x,y] = currentLevel.isWall(x, y, true);
//			print(' '  + ((walls[x,y])?1:0) + '|');
			@segmentMap[x,y] = getSegmentForBlock(x,y);
			uint id = segmentMap[x,y].id;
			if(id < 10) print(' '); 
			if(@segmentMap[x,y] != @zero){
				print(' '  + id + ' |');
			} else print('   |');

		}
		print('\n');
	}
	uint segmentCount = segments.length();
	matrix.resize(segmentCount, segmentCount);
	adjacencyList.resize(segmentCount);
	
	
	for(uint sy = 0; sy < segmentCount ; ++sy ){
		for(uint sx = 0; sx < segmentCount ; ++sx ){
			matrix[sx, sy] = false;
		}
	}
	for(uint s = 1; s < segmentCount ; ++s ){
		PathSegment@ ps;
		@ps = @segments[s];
		uint l = ps.l;
		uint r = ps.r;
		uint y = ps.y;
		uint srcId = ps.id;
		if(l > 0){
			findAccessibleSegments(l - 1, y, srcId);
		}
		if(r < width - 1){
			findAccessibleSegments(r + 1, y, srcId);
		}
		
		print(' ' + segments[s].id + ':' + segments[s].y + '/' + segments[s].l + '/' + segments[s].r );
		
	}
	print('\n');
	print('    |');
	for(uint sx = 0; sx < segments.length() ; ++sx ){
		if(sx < 10) print(' '); 
		print(' '  + sx + ' |');
	}		
	print('\n');
	for(uint sy = 0; sy < segments.length() ; ++sy ){
		if(sy < 10) print(' '); 
		print(' '  + sy + ' |');	
		for(uint sx = 0; sx < segments.length() ; ++sx ){
			string val = matrix[sx, sy] ? '1' : ' ';
			print(' '  + val + '  |');			
		}		
		print('\n');
	}	

	print('\n');	
	print("Navigation graph calculated! \n");

	//s = SOUND.loadSample('sound/game/water-red.wav');
	AStar(3,2);
	console.print("Output to quake console\n");
}

void findAccessibleSegments(uint x, uint y, uint srcId){
//	array<string> case1 = {'o.d',
//						   'w.w'};
	const uint MAX_VERTICAL_DIST = 2;
	//don't compute for top level row of blocks, check for passability of the edge
	if( y < height - 1 && !walls[x, y + 1]){ 
		while(y > 0 && !walls[x, y]){
			PathSegment@ src = @segments[srcId];
			if(srcId != src.id){
				print (' srcId!=src ' + srcId + ' ' + src.id + '\n'); 
			}
			if(walls[x, y - 1]){
				PathSegment@ hit = @segmentMap[x, y - 1];
				uint targetId = hit.id;
				matrix[srcId, targetId] = true;
				adjacencyList[srcId].insertLast(hit);

				if(src.y - hit.y <= MAX_VERTICAL_DIST){
					matrix[targetId, srcId] = true; //TODO May add unreachable segments
					adjacencyList[targetId].insertLast(src); //TODO May add unreachable segments
				}
			}
			if(x > 0 && !walls[x - 1, y] && walls[x - 1, y - 1]){
				PathSegment@ hit = @segmentMap[x - 1, y - 1];
				uint targetId = hit.id;
				matrix[srcId, targetId] = true;
				adjacencyList[srcId].insertLast(hit);

				if(src.y - hit.y <= MAX_VERTICAL_DIST){
					matrix[targetId, srcId] = true; //TODO May add unreachable segments
					adjacencyList[targetId].insertLast(src); //TODO May add unreachable segments
				}
			}
			if(x < width - 1 && !walls[x + 1, y] && walls[x + 1, y - 1]){
				PathSegment@ hit = @segmentMap[x + 1, y - 1];
				uint targetId = hit.id;
				matrix[srcId, targetId] = true;
				adjacencyList[srcId].insertLast(hit);

				if(src.y - hit.y <= MAX_VERTICAL_DIST){
					matrix[targetId, srcId] = true; //TODO May add unreachable segments
					adjacencyList[targetId].insertLast(src); //TODO May add unreachable segments
				}
			}

			//TODO Ground (for maps not having ground level
			
			//TODO Search upwards
			
			
			//TODO Search to the sides
			y--;
		}
	}
}
PathSegment@[] AStar(uint src, uint dst){
	PathSegment@[] openSet = {segments[src]};
	
	if(src == 0 || dst == 0){ //TODO
		return openSet;
	}
	
	uint segmentsLength =  segments.length();
	// for quick presence check
	bool[] openSet_bool(segmentsLength, false);
	// for quick presence check
	bool[] closedSet_bool(segmentsLength, false);
	
	uint[] gScore(segmentsLength, 0);
	uint[] fScore(segmentsLength, 0); 
	uint[] parent(segmentsLength, 0);
	openSet_bool[src] = true;

	fScore[src] = 0;
	
	while(openSet.length() > 0){
		PathSegment@ current = popCheapestSegment(openSet, fScore);
	//	print(' current: id: '+current.id+'l: '+current.l+'r: '+current.r+'y: ' +current.y+'\n');
		
		if(current.id == dst){
			return reconstructPath(parent, current);
		}

		openSet_bool[current.id] = false;
		closedSet_bool[current.id] = true;
		
		PathSegment@[] neighbours = adjacencyList[current.id];
		
		for(uint n = 0; n < neighbours.length(); n++){
			PathSegment@ neighbour = neighbours[n];
	//		print(' neighbour: id: '+neighbour.id+'l: '+neighbour.l+'r: '+neighbour.r+'y: ' +neighbour.y+'\n');
			if(closedSet_bool[neighbour.id]){
				continue;
			}
			uint tentative_gScore = gScore[current.id] + dist_between(current, neighbour);
			if(!openSet_bool[neighbour.id]){
				openSet.insertLast(neighbour);
				openSet_bool[neighbour.id] = true;
			} else if(tentative_gScore >= gScore[neighbour.id]){
				continue;
			}
			parent[neighbour.id] = current.id;
			gScore[neighbour.id] = tentative_gScore;
			uint dist2 =   dist_between(neighbour, segments[dst]);
		//	print(' dist2 ' + dist2 + ' dst: ' + dst + ' \n');
			fScore[neighbour.id] = tentative_gScore + dist_between(neighbour, segments[dst]);
		//	print(' fscore['+neighbour.id+'] = ' + fScore[neighbour.id] + ' \n');
		}
	}
//	print('path not found\n');
	
	PathSegment@[] empty;
	return empty;
}
// Sort of works
// TODO Account for direction of travel
uint dist_between(PathSegment@ current, PathSegment@ neighbour){
	uint c1 = (current.l + current.r) / 2;
	uint c2 = (neighbour.l + neighbour.r) / 2;
	int c3 = c1 - c2;
	uint c4 = c3 ** 2;
	int c5 = (current.y - neighbour.y);
	uint c6 = c5 ** 2;
	//print( 'c1: ' + c1 + ' c2: ' + c2 + ' c3: ' + c3 + ' c4: ' + c4 + ' c5: ' + c5 + ' c6: ' + c6 + '\n');
	
	return c4 + ((current.y - neighbour.y) ** 2);
}

//Works
PathSegment@ popCheapestSegment(PathSegment@[] & inout openSet , uint[] fScore){
	uint counter = 0;
	uint index = counter;
	PathSegment@ lowest = openSet[counter++];
	uint lowestFScore = fScore[lowest.id];

	while(counter < openSet.length()){
		PathSegment@ check = openSet[counter++];
		uint checkScore = fScore[check.id];
	//	print(' checkId ' + check.id + ' checkScore: ' + checkScore + ' \n');
		if(checkScore < lowestFScore){
			index = counter - 1;
			@lowest = check;
			lowestFScore = checkScore;
		}
	}
	openSet.removeAt(index);

	return @lowest;
}
// Works
PathSegment@[] reconstructPath(uint[] parent, PathSegment @ current){
	print('Path found!\n');
	PathSegment@[] path = {current};
	while(parent[current.id] != 0){
		@current = segments[parent[current.id]];
		path.insertLast(current);
	}
	
	path.reverse();
	print('' + path[0].id);
	for(uint p = 1; p < path.length(); p++){
		print(' -> ' + path[p].id );
	}
	print('\n');
	return path;

}


class Target{
	float posx;
	float posy;
	float targetPosx;
	float targetPosy;
	float pathx;
	float pathy;
	Target(){
		
	}
	Target(float posx, float posy, float targetPosx, float targetPosy){
		this.posx = posx;
		this.posy = posy;
		this.targetPosx = targetPosx;
		this.targetPosy = targetPosy;
	}
};

Target[]targets(16);

uint elapsedFrames = 0;
// Executed each frame
void roundUpdate(Round@ round, float  elapsedTime, uint frame){
	int currentRound = round.getRoundNumber();
	elapsedFrames = frame;

	if (currentRound != roundNumber){
		roundNumber = currentRound;
		roundTime = 0;
	} else {
		roundTime += elapsedTime;
	}

}
uint PlayerTargetId = 0;


uint pathDst;
uint pathSrc;
uint dstX;
uint dstY;
float rallyX;
float rallyY;
bool rally = false;
bool cont = false; 
// Executed each game tick for each player
void playerThink(Player@ player, uint & in id ){
	uint z;
	float rand = Rand(10);
	Target @ target = targets[PlayerTargetId]; // intended target for this player
	Target @ victim = targets[id]; // this player's position for other players to pick


	
	if(roundTime < 0.5){
		player.ammo = 100;
		player.life = 100;		
	}
	//return;
	
	// Player 0 - human tester
	if(id == 0){
//		player.ammo = 100;
//		player.life = 100;
	
	// give the human player a moment to visually find himself on the screen
	} else if(roundTime > 0.5){
		uint new_dstY = target.targetPosy + 0.1;
		while(--new_dstY >= 0 && walls[target.targetPosx, new_dstY] == false);
		uint new_dstX = target.targetPosx;
		uint srcY;
		uint currY = player.position.y + 0.1 ;
		uint currX = player.position.y + 0.5;
		while(--currY >= 0 && walls[currX, currY] == false);

		
 		if( walls[currX, currY] && (new_dstX != dstX || new_dstY != dstY || cont) && walls[target.targetPosx, new_dstY] ){
 				cont = false;
				dstX = new_dstX;
				dstY = new_dstY;
				PathSegment@ dst = segmentMap[dstX, dstY];
				uint new_pathDst = dst.id;
				uint new_pathSrc = segmentMap[currX, currY].id;
				print('spot X\n');
				if(new_pathDst != pathDst || new_pathSrc != pathSrc){
					pathDst = new_pathDst;
					pathSrc = new_pathSrc;
					print ('looking for path ' + pathSrc + ' -> ' + pathDst + ' dsty: ' + dstY + '\n');
					PathSegment@[] path = AStar(pathSrc, pathDst);
					
					if(path.length > 1){
						PathSegment@ current = path[0];
						PathSegment@ next = path[1];
						print (' id: ' + next.id + ' l: ' + next.l + ' r: ' + next.r + ' y: ' + next.y + ' target.x:' + target.pathx + ' target.y: ' +target.pathy+ ' \n');

							
						if(current.y > next.y){
							print('case C\n');
							if(current.r <= next.r){
								print('case C.1\n');
								target.pathx = current.r + 1;
								target.pathy = next.y;
							} else if( current.l >= next.l){
								print('case C.2\n');
								target.pathx = current.l-1;
								target.pathy = next.y;
							}
							// upwards
						} else if(current.y < next.y){
							if(current.r > next.r){
								print('case 1\n');
								target.pathx = next.r;
								target.pathy = next.y + 1;
								rallyX = next.r + 1;
								rallyY = current.y + 1;
								rally = true;

							} else if( current.l < next.l ){
								print('case 2\n');
								target.pathx = next.l;
								target.pathy = next.y + 1;
								rallyX = next.l - 1;
								rallyY = current.y + 1;
								rally = true;
							}
						} else{
							if(current.r <= next.l ){
								print('case A\n');
								target.pathx = next.r;
								target.pathy = next.y;
							} else
							if(current.l >= next.r ){
								print('case B\n');
								target.pathx = next.l;
								target.pathy = next.y;
							}
						}
						
//						else {
//							target.pathx = (next.l + next.r) / 2;
//							target.pathy = next.y;
//						}
					} else {
						target.pathx = target.posx;
						target.pathy = target.posy;
					}

					print (' length:' + path.length + ' x:' + target.pathx + ' y: ' +target.pathy+'\n');
				}
		}
		float pathX;
		float pathY;
		
 		if(rally){
 			pathX = rallyX;
 			pathY = rallyY;
			float tgtdistx = (player.position.x - rallyX) ** 2;
			float tgtdisty = (player.position.y - rallyY) ** 2;
			if(tgtdistx < 0.1 && tgtdisty < 0.1){
				print('case 2.1\n');
				pathX = target.pathx;
				pathY = target.pathy;
				rally = false;
				pathSrc = 0;

			}
 		} else{
 			pathX = target.pathx;
 			pathY = target.pathy;
 			float tgtdistx = (player.position.x - pathX) ** 2;
			float tgtdisty = (player.position.y - pathY) ** 2;
			if(tgtdistx < 0.1 && tgtdisty < 0.1 || tgtdisty > 5 ){
				cont = true;
			}
 		}
		
		float xdiff = player.position.x - target.posx;
		float ydiff = player.position.y - target.posy;
		float xposdiff = player.position.x - target.targetPosx;
		float yposdiff = player.position.y - target.targetPosy;
		float xthresh = 100;
		float ythresh = 1 + rand;

		if(player.position.x <  pathX){
			player.setControl(Right);
		} else {
			player.setControl(Left);
		}
		int itsJumpTime = Rand(100);
		float jumpDiff = pathY - player.position.y;
//		print('jumpDiff: ' + jumpDiff + '\n');
		if( (jumpDiff > 0.5 && jumpDiff < 2.3)){
			player.setControl(Up);
		}
		if(itsJumpTime == 1){
		//	player.setControl(Down);
		}
		if(xposdiff * xposdiff < xthresh && yposdiff * yposdiff < 0.2 && roundTime > 2 ){
			if(xposdiff > 0.1){
				//player.setControl(Left);
				if(player.getOrientation() == Orientation::Left)
				{
					//player.setControl(Fire);
				}

			}
			if(xposdiff < -0.1){
				//player.setControl(Right);
				if(player.getOrientation() == Orientation::Right)
				{
				//	player.setControl(Fire);
				}
			}
			
		}

	}
	PlayerTargetId = id;
//	if(player.hasFlag(FlagDead) || rand < 4){
//		return;
//	}

	if(elapsedFrames % 60 == 0){
		if(player.getOrientation() == Orientation::Left)
		{
			victim.posx = player.position.x + 2;
		} else{
			victim.posx = player.position.x - 2;
		}
	}
	victim.targetPosx = player.position.x;
	victim.targetPosy = player.position.y;
	victim.posy = player.position.y;

	//	player.die();  UNCOMMENT AND SEE WHAT HAPPENS 

}



uint r1 = 1;
uint r2 = 2;
uint Rand(uint mod){
	uint t = r2;
	r2 = r1 + r2;
	r1 = t;
	
	return r2 % mod;
}