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
	class Vertex
	{
	public:
		Float32 X;
		Float32 Y;
		Float32 Z;
		Float32 U;
		Float32 V;
		Int32 Flags;

	public:
		Vertex(Size order, Float32 x, Float32 y, Float32 z, Int32 flags = D6_FLAG_NONE)
		{
			X = x;
			Y = y;
			Z = z;
			U = (order == 0 || order == 3) ? 0.0f : 0.99f;
			V = (order == 0 || order == 1) ? 0.0f : 0.99f;
			Flags = flags;
		}

		Vertex(Size order, Int32 x, Int32 y, Int32 z, Int32 flags = D6_FLAG_NONE)
			: Vertex(order, Float32(x), Float32(y), Float32(z), flags)
		{}
	};

	class Block
	{
	private:
		Uint32 texture;
		Int32 type;
		Int32 animationFrames;

	public:
		// TODO: BlockType
		Block(Uint32 texture, Int32 type, Int32 animationFrames)
			: texture(texture), type(type), animationFrames(animationFrames)
		{}

		Uint32 getTexture() const
		{
			return texture;
		}

		Int32 getType() const
		{
			return type;
		}

		Int32 getAnimationFrames() const
		{
			return animationFrames;
		}

		// is(BlockType)
	};

	class Face
	{
	public:
		Uint32 nowTex;
		Uint32 minTex;
		Uint32 maxTex;

	public:
		Face(const Block& block)
		{
			minTex = block.getTexture();
			maxTex = block.getTexture() + block.getAnimationFrames();
			nowTex = block.getTexture();
		}
	};

	class FaceList
	{
	private:
		std::vector<Vertex> vertexes;
		std::vector<Face> faces;

	public:
		FaceList& clear()
		{
			vertexes.clear();
			faces.clear();
			return *this;
		}

		FaceList& addVertex(const Vertex& vertex)
		{
			vertexes.push_back(vertex);
			return *this;
		}

		FaceList& addFace(const Face& face)
		{
			faces.push_back(face);
			return *this;
		}

		std::vector<Vertex>& getVertexes()
		{
			return vertexes;
		}

		std::vector<Face>& getFaces()
		{
			return faces;
		}

		void optimize();
		void render();
		void nextFrame();
	};

	class World
	{
	private:
		std::vector<Block> blockMeta;
		Int32 width;
		Int32 height;
		std::vector<Uint16> levelData;
		FaceList walls;
		FaceList sprites;
		FaceList water;
		Float32 animWait;

	public:
		std::vector<GLuint> blockTextures;

	public:
		World()
		{}

		void init(const std::string& textureFile, const std::string& blockMetaFile);
		void deInit();
		void loadLevel(const std::string& path, bool mirror, std::vector<Bonus>& bonuses, std::vector<Int32>& elevatorData);
		void update(Float32 elapsedTime);

		FaceList& getWalls()
		{
			return walls;
		}

		FaceList& getSprites()
		{
			return sprites;
		}

		FaceList& getWater()
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

		bool isWater(Int32 x, Int32 y)
		{
			return isInside(x, y) ? getBlockType(x, y) == D6_ANM_F_WATER : false;
		}

		bool isWall(Int32 x, Int32 y, bool outside)
		{
			return isInside(x, y) ? getBlockType(x, y) == D6_ANM_F_BLOCK : outside;
		}

		WaterType getWaterType(Int32 x, Int32 y)
		{
			if (isWater(x, y))
			{
				return (getBlock(x, y) == 4) ? WaterType::Blue : WaterType::Red;
			}

			return WaterType::None;
		}

	private:
		Size loadBlockTextures(const std::string& path);
		void loadBlockMeta(const std::string& path, Size blocks);

		void loadLevelData(const std::string& path, std::vector<Int32>& elevatorData);
		void loadElevators(myFile_s* f, std::vector<Int32>& elevatorData);
		void mirrorLevelData();

		void findBonuses(std::vector<Bonus>& bonuses);
		void addWallFaces();
		void addSpriteFaces();
		void addWaterFaces();
		void addWall(FaceList& faceList, const Block& block, Int32 x, Int32 y);
		void addWater(FaceList& faceList, const Block& block, Int32 x, Int32 y);
		void addSprite(FaceList& faceList, const Block& block, Int32 x, Int32 y, Float32 z);

		/** Group faces with the same texture together so that they can be rendered in batches */
		void optimize(FaceList& faceList);

		bool isInside(Int32 x, Int32 y)
		{
			return (x >= 0 && x < width && y >= 0 && y < height);
		}

		Uint16 getBlock(Int32 x, Int32 y)
		{
			return levelData[(height - y - 1) * width + x];
		}

		Int32 getBlockType(Int32 x, Int32 y)
		{
			return blockMeta[getBlock(x, y)].getType();
		}
	};
}

#endif