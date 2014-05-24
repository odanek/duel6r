/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "core/co_core.h"
#include "World.h"
#include "Util.h"
#include "ElevatorList.h"

namespace Duel6
{
	void BlockData::init(const std::string& textureFile, const std::string& blockMetaFile)
	{
		Size blocks = loadBlockTextures(textureFile);
		loadBlockMeta(blockMetaFile, blocks);
	}

	void BlockData::loadBlockMeta(const std::string& path, Size blocks)
	{
		g_app.con->printf(MY_L("APP00058|Nahravam animacni data (%s)\n"), path.c_str());
		
		meta.clear();		
		myFile_s* f = MY_FOpen(path.c_str(), 0, "rb", true);
		for (Size i = 0; i < blocks; i++)
		{
			Int32 blockAnimations;			
			MY_FRead(&blockAnimations, 4, 1, f);

			Int32 blockType;
			MY_FRead(&blockType, 4, 1, f);

			meta.push_back(Block(meta.size(), (Block::Type)blockType, blockAnimations));
		}
		MY_FClose(&f);
	}

	Size BlockData::loadBlockTextures(const std::string& path)
	{		
		g_app.con->printf(MY_L("APP00056|Nahravam textury urovne\n"));		
		
		myKh3info_s ki;
		MY_KH3Open(path.c_str());		
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00057|...Soubor %s obsahuje %lu textur\n"), path.c_str(), ki.picts);

		textures.resize(ki.picts);
		for (Size i = 0; i < ki.picts; i++)
		{
			textures[i] = Util::loadKH3Texture(path, i, false);
		}
		MY_KH3Close();

		return ki.picts;
	}

	void BlockData::freeTextures()
	{
		if (!textures.empty())
		{
			glDeleteTextures(textures.size(), &textures[0]);
			textures.clear();
		}
	}

	void World::loadLevel(const std::string& path, bool mirror)
	{
		levelData.clear();

		myFile_s* f = MY_FOpen(path.c_str(), 0, "rb", true);
		MY_FRead(&width, 4, 1, f);
		MY_FRead(&height, 4, 1, f);
		MY_FSeek(f, 12, SEEK_SET);
		levelData.resize(width * height);
		MY_FRead(&levelData[0], 2, levelData.size(), f);

		if (mirror)
		{
			mirrorLevelData();
		}

		loadElevators(f, mirror);
		MY_FClose(&f);
	}

	void World::loadElevators(myFile_s* f, bool mirror)
	{
		// The rest of file is elevator data
		g_app.con->printf(MY_L("APP00026|...Nahravam vytahy - "));

		std::vector<Int32> elevatorData;
		while (!MY_FEof(f))
		{
			Int32 data;
			MY_FRead(&data, 4, 1, f);
			elevatorData.push_back(data);
		}

		Int32 elevators = elevatorData.empty() ? 0 : elevatorData[0];
		g_app.con->printf(MY_L("APP00027|%d vytahu\n"), elevators);

		ELEV_Clear();

		auto edata = std::next(elevatorData.begin());
		while (elevators-- > 0)
		{
			Elevator elevator;
			
			Int32 controlPoints = edata[0];
			elevator.addControlPoint(Elevator::ControlPoint(mirror ? width - 1 - edata[1] : edata[1], height - edata[2]));
			edata += 3;

			while (controlPoints-- > 0)
			{
				elevator.addControlPoint(Elevator::ControlPoint(mirror ? width - 1 - edata[0] : edata[0], height - edata[1]));
				edata += 2;
			}

			ELEV_Add(elevator);
		}
	}

	void World::mirrorLevelData()
	{
		for (Int32 y = 0; y < height; y++)
		{
			for (Int32 x = 0; x < width / 2; x++)
			{
				std::swap(levelData[y * width + x], levelData[y * width + width - 1 - x]);
			}
		}
	}

	void World::prepareFaces()
	{
		animWait = 0;
		addWallFaces();
		addSpriteFaces();
		addWaterFaces();

		floatingVertexes.build(water, waveHeight);
	}

	void World::findBonuses(std::vector<Bonus>& bonuses)
	{
		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				Int32 blockIndex = getBlock(x, y);
				const Block& block = blockData.getMeta()[blockIndex];

				if (block.is(Block::EmptySpace))
				{
					for (Size k = 0; k < D6_BONUS_COUNT; k++)
					{
						if (blockIndex == d6BonusArt[k])
						{
							bonuses.push_back(Bonus(x, y, blockIndex));
						}
					}
				}
			}
		}
	}

	void World::update(Float32 elapsedTime)
	{
		animWait += elapsedTime;
		if (animWait > animationSpeed)
		{
			animWait = 0;
			walls.nextFrame();
			sprites.nextFrame();
			water.nextFrame();
		}

		floatingVertexes.update(elapsedTime);
	}

	void World::addWallFaces()
	{
		walls.clear();

		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				const Block& block = getBlockMeta(x, y);

				if (block.is(Block::Wall))
				{
					addWall(walls, block, x, y);
				}
			}
		}

		walls.optimize();
	}

	void World::addSpriteFaces()
	{
		sprites.clear();

		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				const Block& block = getBlockMeta(x, y);

				if (block.is(Block::FrontAndBackSprite))
				{
					addSprite(sprites, block, x, y, 1.0f);
					addSprite(sprites, block, x, y, 0.0f);
				}
				else if (block.is(Block::FrontSprite))
				{
					addSprite(sprites, block, x, y, 1.0f);
				}
				else if (block.is(Block::BackSprite))
				{
					addSprite(sprites, block, x, y, 0.0f);
				}
				else if (block.is(Block::Front4Sprite))
				{
					addSprite(sprites, block, x, y, 0.75f);
				}
				else if (block.is(Block::Back4Sprite))
				{
					addSprite(sprites, block, x, y, 0.25f);
				}
			}
		}

		sprites.optimize();
	}

	void World::addWaterFaces()
	{
		water.clear();

		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				const Block& block = getBlockMeta(x, y);

				if (block.is(Block::Waterfall))
				{
					addSprite(water, block, x, y, 0.75);
				}
				else if (block.is(Block::Water))
				{
					addWater(water, block, x, y);
				}
			}
		}

		water.optimize();
	}

	void World::addWall(FaceList& faceList, const Block& block, Int32 x, Int32 y)
	{
		faceList.addFace(Face(block))
			.addVertex(Vertex(0, x, y + 1, 1))
			.addVertex(Vertex(1, x + 1, y + 1, 1))
			.addVertex(Vertex(2, x + 1, y, 1))
			.addVertex(Vertex(3, x, y, 1));


#ifdef D6_RENDER_BACKS
		faceList.addFace(Face(block))
			.addVertex(Vertex(0, x + 1, y + 1, 0))
			.addVertex(Vertex(1, x, y + 1, 0))
			.addVertex(Vertex(2, x, y, 0))
			.addVertex(Vertex(3, x + 1, y, 0));
#endif

		if (!isWall(x - 1, y, false))
		{
			faceList.addFace(Face(block))
				.addVertex(Vertex(0, x, y + 1, 0))
				.addVertex(Vertex(1, x, y + 1, 1))
				.addVertex(Vertex(2, x, y, 1))
				.addVertex(Vertex(3, x, y, 0));
		}
		if (!isWall(x + 1, y, false))
		{
			faceList.addFace(Face(block))
				.addVertex(Vertex(0, x + 1, y + 1, 1))
				.addVertex(Vertex(1, x + 1, y + 1, 0))
				.addVertex(Vertex(2, x + 1, y, 0))
				.addVertex(Vertex(3, x + 1, y, 1));
		}
		if (!isWall(x, y + 1, false))
		{
			faceList.addFace(Face(block))
				.addVertex(Vertex(0, x, y + 1, 1))
				.addVertex(Vertex(1, x, y + 1, 0))
				.addVertex(Vertex(2, x + 1, y + 1, 0))
				.addVertex(Vertex(3, x + 1, y + 1, 1));
		}
		if (!isWall(x, y - 1, false))
		{
			faceList.addFace(Face(block))
				.addVertex(Vertex(0, x, y, 1))
				.addVertex(Vertex(1, x + 1, y, 1))
				.addVertex(Vertex(2, x + 1, y, 0))
				.addVertex(Vertex(3, x, y, 0));
		}
	}

	void World::addWater(FaceList& faceList, const Block& block, Int32 x, Int32 y)
	{
		bool topWater = !isWater(x, y + 1);
		Vertex::Flag flowFlag = topWater ? Vertex::Flag::Flow : Vertex::Flag::None;

		faceList.addFace(Face(block))
			.addVertex(Vertex(0, x, y + 1, 1, flowFlag))
			.addVertex(Vertex(1, x + 1, y + 1, 1, flowFlag))
			.addVertex(Vertex(2, x + 1, y, 1))
			.addVertex(Vertex(3, x, y, 1));

		faceList.addFace(Face(block))
			.addVertex(Vertex(0, x + 1, y + 1, 0, flowFlag))
			.addVertex(Vertex(1, x, y + 1, 0, flowFlag))
			.addVertex(Vertex(2, x, y, 0))
			.addVertex(Vertex(3, x + 1, y, 0));

		if (topWater)
		{
			faceList.addFace(Face(block))
				.addVertex(Vertex(0, x, y + 1, 1, Vertex::Flag::Flow))
				.addVertex(Vertex(1, x, y + 1, 0, Vertex::Flag::Flow))
				.addVertex(Vertex(2, x + 1, y + 1, 0, Vertex::Flag::Flow))
				.addVertex(Vertex(3, x + 1, y + 1, 1, Vertex::Flag::Flow));
		}
	}

	void World::addSprite(FaceList& faceList, const Block& block, Int32 x, Int32 y, Float32 z)
	{
		Float32 fx = Float32(x), fy = Float32(y);
		bool bottomWaterfall = (block.is(Block::Waterfall) && isWater(x, y - 1));
		Vertex::Flag flowFlag = bottomWaterfall ? Vertex::Flag::Flow : Vertex::Flag::None;

		faceList.addFace(Face(block))
			.addVertex(Vertex(0, fx, fy + 1, z))
			.addVertex(Vertex(1, fx + 1, fy + 1, z))
			.addVertex(Vertex(2, fx + 1, fy, z, flowFlag))
			.addVertex(Vertex(3, fx, fy, z, flowFlag));
	}
}