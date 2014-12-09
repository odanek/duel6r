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

#include "World.h"
#include "Util.h"
#include "ElevatorList.h"
#include "TextureManager.h"
#include "Format.h"
#include "Lang.h"
#include "Globals.h"
#include "json/Json.h"

namespace Duel6
{
	void World::loadBlockMeta(const std::string& path)
	{
		blockMeta.clear();		
		File file(path, "rb");

		Size blocks = File::getSize(path) / 8;
		for (Size i = 0; i < blocks; ++i)
		{
			Int32 blockAnimations;			
			file.read(&blockAnimations, 4, 1);

			Int32 blockType;
			file.read(&blockType, 4, 1);

			blockMeta.push_back(Block(i, (Block::Type)blockType, blockAnimations));
		}
		file.close();
	}

	void World::loadLevel(const std::string& path, Size background, bool mirror)
	{
		backgroundTexture = d6TextureManager.get(D6_TEXTURE_BCG_KEY)[background];
		levelData.clear();

		Json::Parser parser;
		const Json::Value& root = parser.parse(path);

		width = root.get("width").asInt();
		height = root.get("height").asInt();
		
		Int32 blockCount = width * height;
		const Json::Value& blocks = root.get("blocks");
		levelData.resize(blockCount);
		for (Size i = 0; i < blocks.getLength(); i++)
		{
			levelData[i] = blocks.get(i).asInt();
		}

		ELEV_Clear();
		Size elevators = root.get("elevators").getLength();
		for (Size i = 0; i < elevators; i++)
		{
			Elevator elevator;
			const Json::Value& points = root.get("elevators").get(i).get("controlPoints");
			for (Size j = 0; j < points.getLength(); j++)
			{
				Int32 x = points.get(j).get("x").asInt();
				Int32 y = points.get(j).get("y").asInt();
				elevator.addControlPoint(Elevator::ControlPoint(mirror ? width - 1 - x : x, height - y));
			}
			ELEV_Add(elevator);
		}

		if (mirror)
		{
			mirrorLevelData();
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