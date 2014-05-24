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

#ifndef DUEL6_LOADER_H
#define DUEL6_LOADER_H

#include <string>
#include <vector>
#include "FaceList.h"
#include "Bonus.h"
#include "WaterType.h"
#include "WaterList.h"

namespace Duel6
{
	class BlockData
	{
	private:
		std::vector<Block> meta;
		std::vector<GLuint> textures;

	private:
		Size loadBlockTextures(const std::string& path);
		void loadBlockMeta(const std::string& path, Size blocks);

	public:
		~BlockData()
		{
			freeTextures();
		}

		void init(const std::string& textureFile, const std::string& blockMetaFile);
		
		const std::vector<Block>& getMeta() const
		{
			return meta;
		}

		const std::vector<GLuint>& getTextures() const
		{
			return textures;
		}

		void freeTextures();
	};

	class World
	{
	private:
		const BlockData& blockData;

		Int32 width;
		Int32 height;
		std::vector<Uint16> levelData;

		FaceList walls;
		FaceList sprites;
		FaceList water;
		
		Float32 animationSpeed;
		Float32 animWait;

		Float32 waveHeight;
		WaterList floatingVertexes;

	public:
		World(const BlockData& blockData, Float32 animationSpeed, Float32 waveHeight)
			: blockData(blockData), walls(blockData.getTextures()), sprites(blockData.getTextures()), 
			water(blockData.getTextures()), animationSpeed(animationSpeed), waveHeight(waveHeight)
		{}

		void loadLevel(const std::string& path, bool mirror);
		void findBonuses(std::vector<Bonus>& bonuses);
		void prepareFaces();

		void update(Float32 elapsedTime);

		FaceList& getWalls()
		{
			return walls;
		}

		const FaceList& getWalls() const
		{
			return walls;
		}

		FaceList& getSprites()
		{
			return sprites;
		}

		const FaceList& getSprites() const
		{
			return sprites;
		}

		FaceList& getWater()
		{
			return water;
		}

		const FaceList& getWater() const
		{
			return water;
		}

		Int32 getSizeX() const
		{
			return width;
		}

		Int32 getSizeY() const
		{
			return height;
		}

		bool isWater(Int32 x, Int32 y) const
		{
			return isInside(x, y) ? getBlockMeta(x, y).is(Block::Water) : false;
		}

		bool isWall(Int32 x, Int32 y, bool outside) const
		{
			return isInside(x, y) ? getBlockMeta(x, y).is(Block::Wall) : outside;
		}

		WaterType getWaterType(Int32 x, Int32 y) const
		{
			if (isWater(x, y))
			{
				return (getBlock(x, y) == 4) ? WaterType::Blue : WaterType::Red;
			}

			return WaterType::None;
		}

	private:
		void mirrorLevelData();
		void loadElevators(myFile_s* f, bool mirror);

		void addWallFaces();
		void addSpriteFaces();
		void addWaterFaces();
		void addWall(FaceList& faceList, const Block& block, Int32 x, Int32 y);
		void addWater(FaceList& faceList, const Block& block, Int32 x, Int32 y);
		void addSprite(FaceList& faceList, const Block& block, Int32 x, Int32 y, Float32 z);

		/** Group faces with the same texture together so that they can be rendered in batches */
		void optimize(FaceList& faceList);

		bool isInside(Int32 x, Int32 y) const
		{
			return (x >= 0 && x < width && y >= 0 && y < height);
		}

		Uint16 getBlock(Int32 x, Int32 y) const
		{
			return levelData[(height - y - 1) * width + x];
		}

		const Block& getBlockMeta(Int32 x, Int32 y) const
		{
			return blockData.getMeta()[getBlock(x, y)];
		}
	};
}

#endif