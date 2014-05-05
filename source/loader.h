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

#include <vector>
#include "Bonus.h"
#include "WaterType.h"

#define D6_MALLOC(t,s)      (t *) MY_Alloc (sizeof (t) * (s))

//#define D6_RENDER_BACKS

#define D6_ANM_F_NOTHING    0x00
#define D6_ANM_F_BLOCK      0x01
#define D6_ANM_F_WATER      0x02
#define D6_ANM_F_FRONT      0x03
#define D6_ANM_F_BACK       0x04
#define D6_ANM_F_FRBC       0x05
#define D6_ANM_F_3FRONT     0x06
#define D6_ANM_F_3BACK      0x07
#define D6_ANM_F_WFALL      0x08

#define D6_FLAG_NONE        0x00
#define D6_FLAG_FLOW        0x01

namespace Duel6
{
	struct Vertex
	{
		Float32 X;
		Float32 Y;
		Float32 Z;
		Float32 U;
		Float32 V;
		Int32 Flags;
	};

	struct Face
	{
		Int32 nowTex;
		Int32 minTex;
		Int32 maxTex;
	};

	class Level
	{
	private:
		Int32 sizeX;
		Int32 sizeY;
		std::vector<Uint16> data;

	public:
		void load(const std::string& path, std::vector<Int32>& elevatorData);				
		Level& mirror();

		Int32 getSizeX() const
		{
			return sizeX;
		}

		Int32 getSizeY() const
		{
			return sizeY;
		}
		
		Uint16 getBlock(Int32 x, Int32 y)
		{
			return data[y * sizeX + x];
		}

		Uint16 getBlock(Size offset)
		{
			return data[offset];
		}

		bool isInside(Int32 x, Int32 y)
		{
			return (x >= 0 && x < sizeX && y >= 0 && y < sizeY);
		}

	private:
		Level& loadElevators(myFile_s *f, std::vector<Int32>& elevatorData);
	};

	class World
	{
	private:
		struct Block
		{
			Int32 type;
			Int32 anim;
		};
		Level level;

	public:
		Size Blocks;
		Size Sprites;
		Size Waters;
		std::vector<Vertex> vertexes;
		std::vector<Face> faces;
		std::vector<Block> blocks;
		std::vector<GLuint> blockTextures;
		Float32 animWait;

	public:
		World()
		{}

		void init(const std::string& textureFile, const std::string blockDataFile);
		void deInit();
		void loadLevel(const std::string& path, bool mirror, std::vector<Bonus>& bonuses, std::vector<Int32>& elevatorData);

		Int32 getSizeX() const
		{
			return level.getSizeX();
		}

		Int32 getSizeY() const
		{
			return level.getSizeY();
		}

		bool isWater(Int32 x, Int32 y)
		{
			return getBlockTypeSafe(x, y) == D6_ANM_F_WATER;
		}

		bool isWall(Int32 x, Int32 y)
		{
			return getBlockTypeSafe(x, y) == D6_ANM_F_BLOCK;
		}

		WaterType getWaterType(Int32 x, Int32 y)
		{
			if (isWater(x, y))
			{
				return (level.getBlock(x, y) == 4) ? WaterType::Blue : WaterType::Red;
			}

			return WaterType::None;
		}

	private:
		void process(std::vector<Bonus>& bonuses);
		void optimize();
		void addSprite(int pos, int i, int j, float z, int *spr);
		void setVertex(int n, int x, int y, float z);
		void addFace(int pos, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, int x4, int y4, int z4, int *blc);
		void addBlock(int pos, int i, int j, int *blc);
		void addWaterBlock(int pos, int i, int j, int *wtr);
		int block(int i, int j, int wr);

		Int32 getBlockType(Int32 x, Int32 y)
		{
			return blocks[level.getBlock(x, y)].type;
		}

		Int32 getBlockTypeSafe(Int32 x, Int32 y)
		{
			return level.isInside(x, y) ? getBlockType(x, y) : D6_ANM_F_BLOCK;
		}
	};
}

#endif