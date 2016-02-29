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

#include "Level.h"
#include "json/JsonParser.h"

namespace Duel6
{
	Level::Level(const std::string& path, bool mirror, const Block::Meta& blockMeta)
		: blockMeta(blockMeta)
	{
		load(path, mirror);
	}

	void Level::load(const std::string& path, bool mirror)
	{
		levelData.clear();
		waterLevel = -1;
		Json::Parser parser;
		Json::Value root = parser.parse(path);

		width = root.get("width").asInt();
		height = root.get("height").asInt();

		Int32 blockCount = width * height;
		Json::Value blocks = root.get("blocks");
		levelData.resize(blockCount);
		for (Size i = 0; i < blocks.getLength(); i++)
		{
			levelData[i] = blocks.get(i).asInt();
		}

		if (mirror)
		{
			mirrorLevelData();
		}
		waterBlock = findWaterType();
	}

	void Level::mirrorLevelData()
	{
		for (Int32 y = 0; y < height; y++)
		{
			for (Int32 x = 0; x < width / 2; x++)
			{
				std::swap(levelData[y * width + x], levelData[y * width + width - 1 - x]);
			}
		}
	}

	void Level::raiseWater()
	{
		if(waterLevel < getHeight() - 1)
		{
			waterLevel++;
			for(Int32 x = 0; x < getWidth(); x++)
			{
				if(!isWall(x, waterLevel, false))
				{
					setBlock(waterBlock, x, waterLevel);
				}
			}
		}
	}

	Uint16 Level::findWaterType() const
	{
		for (Int32 y = 0; y < getHeight(); y++)
		{
			for (Int32 x = 0; x < getWidth(); x++)
			{
				if (isWater(x, y))
				{
					return getBlock(x, y);
				}
			}
		}

		static Uint16 waterBlocks[] = { 4, 16, 33 };
		return waterBlocks[rand() % 3];
	}

	Water Level::getWaterType(Int32 x, Int32 y) const
	{
		if (isWater(x, y))
		{
			Uint16 block = getBlock(x, y);
			if (block == 4)
			{
				return Water::BLUE;
			}
			else if (block == 16)
			{
				return Water::RED;
			}
			else if (block == 33)
			{
				return Water::GREEN;
			}
		}

		return Water::NONE;
	}
}