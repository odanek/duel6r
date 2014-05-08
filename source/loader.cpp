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

#include "project.h"

namespace Duel6
{
	void World::loadLevelData(const std::string& path, std::vector<Int32>& elevatorData)
	{
		levelData.clear();

		myFile_s* f = MY_FOpen(path.c_str(), 0, "rb", true);
		MY_FRead(&width, 4, 1, f);
		MY_FRead(&height, 4, 1, f);
		MY_FSeek(f, 12, SEEK_SET);
		levelData.resize(width * height);
		MY_FRead(&levelData[0], 2, levelData.size(), f);
		loadElevators(f, elevatorData);
		MY_FClose(&f);
	}

	void World::loadElevators(myFile_s* f, std::vector<Int32>& elevatorData)
	{
		// The rest of file is elevator data
		g_app.con->printf(MY_L("APP00026|...Nahravam vytahy - "));
		while (!MY_FEof(f))
		{
			Int32 data;
			MY_FRead(&data, 4, 1, f);
			elevatorData.push_back(data);
		}

		if (elevatorData.empty())
		{
			elevatorData.push_back(0);
		}

		g_app.con->printf(MY_L("APP00027|%d vytahu\n"), elevatorData[0]);
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

	/*
	===================
	Prida blok
	===================
	*/
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

	/*
	============================
	Prida blok vody
	============================
	*/
	void World::addWater(FaceList& faceList, const Block& block, Int32 x, Int32 y)
	{
		bool topWater = !isWater(x, y + 1);
		Int32 flowFlag = topWater ? D6_FLAG_FLOW : D6_FLAG_NONE;

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
				.addVertex(Vertex(0, x, y + 1, 1, D6_FLAG_FLOW))
				.addVertex(Vertex(1, x, y + 1, 0, D6_FLAG_FLOW))
				.addVertex(Vertex(2, x + 1, y + 1, 0, D6_FLAG_FLOW))
				.addVertex(Vertex(3, x + 1, y + 1, 1, D6_FLAG_FLOW));
		}
	}

	/*
	===================
	Prida face od sprite
	===================
	*/
	void World::addSprite(FaceList& faceList, const Block& block, Int32 x, Int32 y, Float32 z)
	{
		Float32 fx = Float32(x), fy = Float32(y);
		bool bottomWaterfall = (block.getType() == D6_ANM_F_WFALL && isWater(x, y - 1));
		Int32 flowFlag = bottomWaterfall ? D6_FLAG_FLOW : D6_FLAG_NONE;

		faceList.addFace(Face(block))
			.addVertex(Vertex(0, fx, fy + 1, z))
			.addVertex(Vertex(1, fx + 1, fy + 1, z))
			.addVertex(Vertex(2, fx + 1, fy, z, flowFlag))
			.addVertex(Vertex(3, fx, fy, z, flowFlag));
	}

	void World::findBonuses(std::vector<Bonus>& bonuses)
	{
		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				Int32 blockIndex = getBlock(x, y);
				const Block& block = blockMeta[blockIndex];

				if (block.getType() == D6_ANM_F_NOTHING)
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

	void World::addWallFaces()
	{
		walls.clear();

		for (Int32 y = 0; y < getSizeY(); y++)
		{
			for (Int32 x = 0; x < getSizeX(); x++)
			{
				Int32 blockIndex = getBlock(x, y);
				const Block& block = blockMeta[blockIndex];

				if (block.getType() == D6_ANM_F_BLOCK)
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
				Int32 blockIndex = getBlock(x, y);
				const Block& block = blockMeta[blockIndex];

				if (block.getType() == D6_ANM_F_FRBC)
				{
					addSprite(sprites, block, x, y, 1.0f);
					addSprite(sprites, block, x, y, 0.0f);
				}
				else if (block.getType() == D6_ANM_F_FRONT)
				{
					addSprite(sprites, block, x, y, 1.0f);
				}
				else if (block.getType() == D6_ANM_F_BACK)
				{
					addSprite(sprites, block, x, y, 0.0f);
				}
				else if (block.getType() == D6_ANM_F_3FRONT)
				{
					addSprite(sprites, block, x, y, 0.75f);
				}
				else if (block.getType() == D6_ANM_F_3BACK)
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
				Int32 blockIndex = getBlock(x, y);
				const Block& block = blockMeta[blockIndex];

				if (block.getType() == D6_ANM_F_WFALL)
				{
					addSprite(water, block, x, y, 0.75);
				}
				else if (block.getType() == D6_ANM_F_WATER)
				{
					addWater(water, block, x, y);
				}
			}
		}

		water.optimize();
	}

	void FaceList::optimize()
	{
		for (Size i = 0; i < faces.size(); i++)
		{
			Uint32 curTexture = faces[i].nowTex;
			Size curFace = i + 1;

			for (Size j = i + 1; j < faces.size(); j++)
			{
				if (faces[j].nowTex == curTexture && j != curFace)
				{
					std::swap(faces[curFace], faces[j]);

					for (Size k = 0; k < 4; k++)
					{
						std::swap(vertexes[curFace * 4 + k], vertexes[j * 4 + k]);
					}

					curFace++;
				}
			}
		}
	}

	void FaceList::render()
	{
		if (faces.empty())
		{
			return;
		}

		const std::vector<GLuint>& textures = d6World.blockTextures;
		GLuint curTexture = textures[faces[0].nowTex];
		Size first = 0, count = 0;

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &vertexes[0].X);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertexes[0].U);

		for (Face& face : faces)
		{
			if (textures[face.nowTex] != curTexture)
			{
				glBindTexture(GL_TEXTURE_2D, curTexture);
				glDrawArrays(GL_QUADS, first, count);
				curTexture = textures[face.nowTex];
				first += count;
				count = 4;
			}
			else
			{
				count += 4;
			}
		}

		glBindTexture(GL_TEXTURE_2D, curTexture);
		glDrawArrays(GL_QUADS, first, count);
	}

	void FaceList::nextFrame()
	{
		for (Face& face : faces)
		{
			if (++face.nowTex > face.maxTex)
			{
				face.nowTex = face.minTex;
			}
		}
	}

	void World::update(Float32 elapsedTime)
	{
		if ((d6World.animWait += elapsedTime) > D6_ANM_SPEED)
		{
			d6World.animWait = 0;
			walls.nextFrame();
			sprites.nextFrame();
			water.nextFrame();
		}
	}

	/*
	===================
	Nahraje level
	pokud w->Vertex, w->Face, w->Level.Data nejsou NULL
	tak budou uvolneny
	===================
	*/
	void World::loadLevel(const std::string& path, bool mirror, std::vector<Bonus>& bonuses, std::vector<Int32>& elevatorData)
	{
		loadLevelData(path, elevatorData);
		if (mirror)
		{
			mirrorLevelData();
		}

		animWait = 0;

		addWallFaces();
		addSpriteFaces();
		addWaterFaces();
		findBonuses(bonuses);
	}

	void World::init(const std::string& textureFile, const std::string& blockMetaFile)
	{
		Size blocks = loadBlockTextures(textureFile);
		loadBlockMeta(blockMetaFile, blocks);
	}

	void World::loadBlockMeta(const std::string& path, Size blocks)
	{
		g_app.con->printf(MY_L("APP00058|Nahravam animacni data (%s)\n"), path.c_str());
		
		blockMeta.clear();		
		myFile_s* f = MY_FOpen(path.c_str(), 0, "rb", true);
		for (Size i = 0; i < blocks; i++)
		{
			Int32 blockAnimations;			
			MY_FRead(&blockAnimations, 4, 1, f);

			Int32 blockType;
			MY_FRead(&blockType, 4, 1, f);

			blockMeta.push_back(Block(blockMeta.size(), blockType, blockAnimations));
		}
		MY_FClose(&f);
	}

	Size World::loadBlockTextures(const std::string& path)
	{		
		g_app.con->printf(MY_L("APP00056|Nahravam textury urovne\n"));		
		
		myKh3info_s ki;
		MY_KH3Open(path.c_str());		
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00057|...Soubor %s obsahuje %lu textur\n"), path.c_str(), ki.picts);

		blockTextures.resize(ki.picts);
		for (Size i = 0; i < ki.picts; i++)
		{
			blockTextures[i] = UTIL_LoadKH3Texture(path.c_str(), i, false);
		}
		MY_KH3Close();

		return ki.picts;
	}

	void World::deInit()
	{
		glDeleteTextures(blockTextures.size(), &blockTextures[0]);
		blockTextures.clear();
	}
}