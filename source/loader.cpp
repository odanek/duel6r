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
	void Level::load(const std::string& path, std::vector<Int32>& elevatorData)
	{
		data.clear();

		myFile_s* f = MY_FOpen(path.c_str(), 0, "rb", true);
		MY_FRead(&sizeX, 4, 1, f);
		MY_FRead(&sizeY, 4, 1, f);
		MY_FSeek(f, 12, SEEK_SET);
		data.resize(sizeX * sizeY);
		MY_FRead(&data[0], 2, data.size(), f);
		loadElevators(f, elevatorData);
		MY_FClose(&f);
	}

	Level& Level::loadElevators(myFile_s *f, std::vector<Int32>& elevatorData)
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
		return *this;
	}

	Level& Level::mirror()
	{
		for (Int32 y = 0; y < sizeY; y++)
		{
			for (Int32 x = 0; x < sizeX / 2; x++)
			{
				Uint16 block = data[y * sizeX + x];
				data[y * sizeX + x] = data[y * sizeX + sizeX - 1 - x];
				data[y * sizeX + sizeX - 1 - x] = block;
			}
		}

		return *this;
	}

	/*
	===================
	Vraci zda je na dane pozici blok
	1 - neni
	0 - je
	===================
	*/
	int World::block(int i, int j, int wr)
	{
		if (!level.isInside(i, j))
		{
			return 1;
		}

		Int32 type = getBlockType(i, j);

		if (type == D6_ANM_F_BLOCK)
			return 0;

		if (type == D6_ANM_F_WATER && wr == 1)
			return 2;

		return 1;
	}

	/*
	===================
	Prida jeden vertex
	N % 4 -   0   1   2   3
	Nastaveni U   - 0.0 1.0 1.0 0.0
	Nastaveni V   - 0.0 0.0 1.0 1.0
	===================
	*/
	void World::setVertex(int n, int x, int y, float z)
	{
		vertexes[n].X = (float)x;
		vertexes[n].Y = (float)y;
		vertexes[n].Z = z;
		vertexes[n].U = (float)(((n % 4) - ((n % 4) / 2)) % 2) * 0.99f;
		vertexes[n].V = (float)((n % 4) / 2) * 0.99f;
		vertexes[n].Flags = D6_FLAG_NONE;
	}

	/*
	===================
	Prida Face od bloku
	===================
	*/
	void World::addFace(int pos, int x1, int y1, int z1,
		int x2, int y2, int z2,
		int x3, int y3, int z3,
		int x4, int y4, int z4, int *blc)
	{
		Face& f = faces[*blc / 4];

		f.minTex = level.getBlock(pos);
		f.maxTex = f.minTex + blocks[f.minTex].anim;
		f.nowTex = f.minTex;

		setVertex(*blc, x1, y1, (float)z1);
		setVertex(*blc + 1, x2, y2, (float)z2);
		setVertex(*blc + 2, x3, y3, (float)z3);
		setVertex(*blc + 3, x4, y4, (float)z4);
		*blc += 4;
	}

	/*
	===================
	Prida blok
	===================
	*/
	void World::addBlock(int pos, int i, int j, int *blc)
	{
		int rj = level.getSizeY() - j;

		addFace(pos, i, j, 1, i + 1, j, 1, i + 1, j - 1, 1, i, j - 1, 1, blc);
#ifdef D6_RENDER_BACKS
		addFace (pos, i + 1, j, 0, i, j, 0, i, j - 1, 0, i + 1, j - 1, 0, blc);
#endif

		if (block(i - 1, rj, 0))
			addFace(pos, i, j, 0, i, j, 1, i, j - 1, 1, i, j - 1, 0, blc);
		if (block(i + 1, rj, 0))
			addFace(pos, i + 1, j, 1, i + 1, j, 0, i + 1, j - 1, 0, i + 1, j - 1, 1, blc);
		if (block(i, rj - 1, 0))
			addFace(pos, i, j, 1, i, j, 0, i + 1, j, 0, i + 1, j, 1, blc);
		if (block(i, rj + 1, 0))
			addFace(pos, i, j - 1, 1, i + 1, j - 1, 1, i + 1, j - 1, 0, i, j - 1, 0, blc);
	}

	/*
	============================
	Prida blok vody
	============================
	*/
	void World::addWaterBlock(int pos, int i, int j, int *wtr)
	{
		int rj = level.getSizeY() - j;

		addFace(pos, i, j, 1, i + 1, j, 1, i + 1, j - 1, 1, i, j - 1, 1, wtr);
		addFace(pos, i + 1, j, 0, i, j, 0, i, j - 1, 0, i + 1, j - 1, 0, wtr);

		if (block(i, rj - 1, 1) != 2)
		{
			addFace(pos, i, j, 1, i, j, 0, i + 1, j, 0, i + 1, j, 1, wtr);
			vertexes[*wtr - 12].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 11].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 8].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 7].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 4].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 3].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 2].Flags = D6_FLAG_FLOW;
			vertexes[*wtr - 1].Flags = D6_FLAG_FLOW;
		}
	}

	/*
	===================
	Prida face od sprite
	===================
	*/
	void World::addSprite(int pos, int i, int j, float z, int *spr)
	{
		Face& f = faces[*spr / 4];

		f.minTex = level.getBlock(pos);
		f.maxTex = f.minTex + blocks[f.minTex].anim;
		f.nowTex = f.minTex;

		setVertex(*spr, i, j, z);
		setVertex(*spr + 1, i + 1, j, z);
		setVertex(*spr + 2, i + 1, j - 1, z);
		setVertex(*spr + 3, i, j - 1, z);
		*spr += 4;

		if (blocks[f.minTex].type == D6_ANM_F_WFALL && j > 1)
		{
			if (blocks[level.getBlock(i, level.getSizeY() - j + 1)].type == D6_ANM_F_WATER)
			{
				vertexes[*spr - 2].Flags = D6_FLAG_FLOW;
				vertexes[*spr - 1].Flags = D6_FLAG_FLOW;
			}
		}
	}

	/*
	===================
	Vyplni d6WORLD struct
	zjisti pocty bloku, facu, spritu....
	===================
	*/
	void World::process(std::vector<Bonus>& bonuses)
	{
		int i, j, k, spr, blc, pos, wtr;

		Sprites = 0;
		Blocks = 0;
		Waters = 0;
		pos = 0;

		for (j = 0; j < level.getSizeY(); j++)
		{
			for (i = 0; i < level.getSizeX(); i++, pos++)
			{
				Int32 bl = level.getBlock(pos);
				switch (blocks[bl].type)
				{
				case D6_ANM_F_NOTHING:
					for (k = 0; k < D6_BONUS_COUNT; k++)
					{
						if (bl == d6BonusArt[k])
						{
							bonuses.push_back(Bonus(i, j, bl));
						}
					}
					break;
				case D6_ANM_F_WFALL:
					Waters++;
					break;
				case D6_ANM_F_WATER:
					Waters += 2;
					if (block(i, j - 1, 1) != 2)
					{
						Waters++;
					}
					break;
				case D6_ANM_F_BLOCK:
#ifdef D6_RENDER_BACKS
					Blocks += 2;
#else
					Blocks += 1;
#endif
					Blocks += block(i - 1, j, 0);
					Blocks += block(i + 1, j, 0);
					Blocks += block(i, j + 1, 0);
					Blocks += block(i, j - 1, 0);
					break;
				case D6_ANM_F_FRBC:
					Sprites += 2;
					break;
				default:
					Sprites++;
					break;
				}
			}
		}

		Size numFaces = Sprites + Blocks + Waters;
		vertexes.resize(numFaces << 2);
		faces.resize(numFaces);
		pos = 0;
		blc = 0;
		spr = Blocks << 2;
		wtr = (Blocks + Sprites) << 2;

		for (j = level.getSizeY(); j > 0; j--)
		{
			for (i = 0; i < level.getSizeX(); i++, pos++)
			{
				Int32 bl = level.getBlock(pos);
				switch (blocks[bl].type)
				{
				case D6_ANM_F_NOTHING:
					break;
				case D6_ANM_F_WFALL:
					addSprite(pos, i, j, 0.75, &wtr);
					break;
				case D6_ANM_F_WATER:
					addWaterBlock(pos, i, j, &wtr);
					break;
				case D6_ANM_F_BLOCK:
					addBlock(pos, i, j, &blc);
					break;
				case D6_ANM_F_FRBC:
					addSprite(pos, i, j, 1.0, &spr);
					addSprite(pos, i, j, 0.0, &spr);
					break;
				case D6_ANM_F_FRONT:
					addSprite(pos, i, j, 1.0, &spr);
					break;
				case D6_ANM_F_BACK:
					addSprite(pos, i, j, 0.0, &spr);
					break;
				case D6_ANM_F_3FRONT:
					addSprite(pos, i, j, 0.75, &spr);
					break;
				case D6_ANM_F_3BACK:
					addSprite(pos, i, j, 0.25, &spr);
					break;
				default:
					MY_Err(MY_ErrDump(MY_L("APP00028|LOADER_LoadWorld chyba: Neznamy typ %d"), blocks[bl].type));
					break;
				}
			}
		}
	}

	/*
	==================================================
	Optimalizace
	==================================================
	*/
	void World::optimize()
	{
		int     k, cur_tex, cur_fc;

		for (Size i = 0; i < faces.size(); i++)
		{
			cur_tex = faces[i].nowTex;
			cur_fc = i + 1;

			for (Size j = i + 1; j < faces.size(); j++)
			{
				if (faces[j].nowTex == cur_tex)
				{
					if (j != cur_fc)
					{
						// Swap
						Face tmpf = faces[cur_fc];
						faces[cur_fc] = faces[j];
						faces[j] = tmpf;

						for (k = 0; k < 4; k++)
						{
							// Swap
							Vertex tmpv = vertexes[cur_fc * 4 + k];
							vertexes[cur_fc * 4 + k] = vertexes[j * 4 + k];
							vertexes[j * 4 + k] = tmpv;
						}
					}
					cur_fc++;
				}
			}
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
		level.load(path, elevatorData);
		if (mirror)
		{
			level.mirror();
		}

		vertexes.clear();
		faces.clear();
		animWait = 0;

		process(bonuses);
		optimize();
	}

	void World::init(const std::string& textureFile, const std::string blockDataFile)
	{
		myFile_s        *f;
		myKh3info_s     ki;

		g_app.con->printf(MY_L("APP00056|Nahravam textury urovne\n"));
		MY_KH3Open(textureFile.c_str());
		MY_KH3GetInfo(&ki);
		g_app.con->printf(MY_L("APP00057|...Soubor %s obsahuje %lu textur\n"), textureFile.c_str(), ki.picts);

		g_app.con->printf(MY_L("APP00058|Nahravam animacni data (%s)\n"), blockDataFile.c_str());
		blocks.clear();		
		f = MY_FOpen(blockDataFile.c_str(), 0, "rb", true);
		for (Size i = 0; i < ki.picts; i++)
		{
			Block block;
			MY_FRead(&block.anim, 4, 1, f);
			MY_FRead(&block.type, 4, 1, f);
			blocks.push_back(block);

		}
		MY_FClose(&f);

		blockTextures.resize(ki.picts);
		for (Size i = 0; i < ki.picts; i++)
		{
			blockTextures[i] = UTIL_LoadKH3Texture(textureFile.c_str(), i, false);
		}
		MY_KH3Close();
	}

	void World::deInit()
	{
		glDeleteTextures(blockTextures.size(), &blockTextures[0]);
		blockTextures.clear();
	}
}