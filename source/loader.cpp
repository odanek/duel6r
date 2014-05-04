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
	static d6WORLD  *w;
	static d6LEVEL  *l;

	/*
	===================
	Vraci zda je na dane pozici blok
	1 - neni
	0 - je
	===================
	*/
	static int LOADER_Block(int i, int j, int wr)
	{
		if (i < 0 || i >= l->SizeX || j < 0 || j >= l->SizeY)
			return 1;

		i = w->Anm.Znak[l->Data[j * l->SizeX + i]];

		if (i == D6_ANM_F_BLOCK)
			return 0;

		if (i == D6_ANM_F_WATER && wr == 1)
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
	static void LOADER_SetVertex(int n, int x, int y, float z)
	{
		w->Vertex[n].X = (float)x;
		w->Vertex[n].Y = (float)y;
		w->Vertex[n].Z = z;
		w->Vertex[n].U = (float)(((n % 4) - ((n % 4) / 2)) % 2) * 0.99f;
		w->Vertex[n].V = (float)((n % 4) / 2) * 0.99f;
		w->Vertex[n].Flags = D6_FLAG_NONE;
	}

	/*
	===================
	Prida Face od bloku
	===================
	*/
	static void LOADER_AddFace(int pos, int x1, int y1, int z1,
		int x2, int y2, int z2,
		int x3, int y3, int z3,
		int x4, int y4, int z4, int *blc)
	{
		d6FACE  *f = &w->Face[*blc / 4];

		f->MinTex = l->Data[pos];
		f->MaxTex = f->MinTex + w->Anm.Anim[f->MinTex];
		f->NowTex = f->MinTex;

		LOADER_SetVertex(*blc, x1, y1, (float)z1);
		LOADER_SetVertex(*blc + 1, x2, y2, (float)z2);
		LOADER_SetVertex(*blc + 2, x3, y3, (float)z3);
		LOADER_SetVertex(*blc + 3, x4, y4, (float)z4);
		*blc += 4;
	}

	/*
	===================
	Prida blok
	===================
	*/
	static void LOADER_AddBlock(int pos, int i, int j, int *blc)
	{
		int rj = w->Level.SizeY - j;

		LOADER_AddFace(pos, i, j, 1, i + 1, j, 1, i + 1, j - 1, 1, i, j - 1, 1, blc);
#ifdef D6_RENDER_BACKS
		LOADER_AddFace (pos, i + 1, j, 0, i, j, 0, i, j - 1, 0, i + 1, j - 1, 0, blc);
#endif

		if (LOADER_Block(i - 1, rj, 0))
			LOADER_AddFace(pos, i, j, 0, i, j, 1, i, j - 1, 1, i, j - 1, 0, blc);
		if (LOADER_Block(i + 1, rj, 0))
			LOADER_AddFace(pos, i + 1, j, 1, i + 1, j, 0, i + 1, j - 1, 0, i + 1, j - 1, 1, blc);
		if (LOADER_Block(i, rj - 1, 0))
			LOADER_AddFace(pos, i, j, 1, i, j, 0, i + 1, j, 0, i + 1, j, 1, blc);
		if (LOADER_Block(i, rj + 1, 0))
			LOADER_AddFace(pos, i, j - 1, 1, i + 1, j - 1, 1, i + 1, j - 1, 0, i, j - 1, 0, blc);
	}

	/*
	============================
	Prida blok vody
	============================
	*/
	static void LOADER_AddWaterBlock(int pos, int i, int j, int *wtr)
	{
		int rj = w->Level.SizeY - j;

		LOADER_AddFace(pos, i, j, 1, i + 1, j, 1, i + 1, j - 1, 1, i, j - 1, 1, wtr);
		LOADER_AddFace(pos, i + 1, j, 0, i, j, 0, i, j - 1, 0, i + 1, j - 1, 0, wtr);

		if (LOADER_Block(i, rj - 1, 1) != 2)
		{
			LOADER_AddFace(pos, i, j, 1, i, j, 0, i + 1, j, 0, i + 1, j, 1, wtr);
			w->Vertex[*wtr - 12].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 11].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 8].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 7].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 4].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 3].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 2].Flags = D6_FLAG_FLOW;
			w->Vertex[*wtr - 1].Flags = D6_FLAG_FLOW;
		}
	}

	/*
	===================
	Prida face od sprite
	===================
	*/
	static void LOADER_AddSprite(int pos, int i, int j, float z, int *spr)
	{
		d6FACE  *f = &w->Face[*spr / 4];

		f->MinTex = l->Data[pos];
		f->MaxTex = f->MinTex + w->Anm.Anim[f->MinTex];
		f->NowTex = f->MinTex;

		LOADER_SetVertex(*spr, i, j, z);
		LOADER_SetVertex(*spr + 1, i + 1, j, z);
		LOADER_SetVertex(*spr + 2, i + 1, j - 1, z);
		LOADER_SetVertex(*spr + 3, i, j - 1, z);
		*spr += 4;

		if (w->Anm.Znak[f->MinTex] == D6_ANM_F_WFALL && j > 1)
			if (w->Anm.Znak[l->Data[(l->SizeY - j + 1) * l->SizeX + i]] == D6_ANM_F_WATER)
			{
				w->Vertex[*spr - 2].Flags = D6_FLAG_FLOW;
				w->Vertex[*spr - 1].Flags = D6_FLAG_FLOW;
			}
	}

	/*
	===================
	Vyplni d6WORLD struct
	zjisti pocty bloku, facu, spritu....
	===================
	*/
	static void LOADER_Process(std::vector<Bonus>& bonuses)
	{
		int i, j, k, *anm, spr, blc, pos, wtr;

		w->Sprites = 0;
		w->Blocks = 0;
		w->Waters = 0;
		anm = w->Anm.Znak;
		pos = 0;

		for (j = 0; j < l->SizeY; j++)
		{
			for (i = 0; i < l->SizeX; i++, pos++)
			{
				switch (anm[l->Data[pos]])
				{
				case D6_ANM_F_NOTHING:
					for (k = 0; k < D6_BONUS_COUNT; k++)
						if (l->Data[pos] == d6BonusArt[k])
						{
							bonuses.push_back(Bonus(i, j, l->Data[pos]));
						}
					break;
				case D6_ANM_F_WFALL:
					w->Waters++;
					break;
				case D6_ANM_F_WATER:
					w->Waters += 2;
					if (LOADER_Block(i, j - 1, 1) != 2)
						w->Waters++;
					break;
				case D6_ANM_F_BLOCK:
#ifdef D6_RENDER_BACKS
					w->Blocks += 2;
#else
					w->Blocks += 1;
#endif
					w->Blocks += LOADER_Block(i - 1, j, 0);
					w->Blocks += LOADER_Block(i + 1, j, 0);
					w->Blocks += LOADER_Block(i, j + 1, 0);
					w->Blocks += LOADER_Block(i, j - 1, 0);
					break;
				case D6_ANM_F_FRBC:
					w->Sprites += 2;
					break;
				default:
					w->Sprites++;
					break;
				}
			}
		}

		w->Faces = w->Sprites + w->Blocks + w->Waters;
		w->Vertex = D6_MALLOC(d6VERTEX, w->Faces << 2);
		w->Face = D6_MALLOC(d6FACE, w->Faces);
		pos = 0;
		blc = 0;
		spr = w->Blocks << 2;
		wtr = (w->Blocks + w->Sprites) << 2;

		for (j = l->SizeY; j > 0; j--)
			for (i = 0; i < l->SizeX; i++, pos++)
				switch (anm[l->Data[pos]])
			{
				case D6_ANM_F_NOTHING:
					break;
				case D6_ANM_F_WFALL:
					LOADER_AddSprite(pos, i, j, 0.75, &wtr);
					break;
				case D6_ANM_F_WATER:
					LOADER_AddWaterBlock(pos, i, j, &wtr);
					break;
				case D6_ANM_F_BLOCK:
					LOADER_AddBlock(pos, i, j, &blc);
					break;
				case D6_ANM_F_FRBC:
					LOADER_AddSprite(pos, i, j, 1.0, &spr);
					LOADER_AddSprite(pos, i, j, 0.0, &spr);
					break;
				case D6_ANM_F_FRONT:
					LOADER_AddSprite(pos, i, j, 1.0, &spr);
					break;
				case D6_ANM_F_BACK:
					LOADER_AddSprite(pos, i, j, 0.0, &spr);
					break;
				case D6_ANM_F_3FRONT:
					LOADER_AddSprite(pos, i, j, 0.75, &spr);
					break;
				case D6_ANM_F_3BACK:
					LOADER_AddSprite(pos, i, j, 0.25, &spr);
					break;
				default:
					MY_Err(MY_ErrDump(MY_L("APP00028|LOADER_LoadWorld chyba: Neznamy typ %d"), anm[l->Data[pos]]));
					break;
			}
	}

	/*
	==================================================
	Optimalizace
	==================================================
	*/
	void LOADER_Optimize(void)
	{
		int     i, j, k, cur_tex, cur_fc;

		for (i = 0; i < w->Faces; i++)
		{
			cur_tex = w->Face[i].NowTex;
			cur_fc = i + 1;

			for (j = i + 1; j < w->Faces; j++)
			{
				if (w->Face[j].NowTex == cur_tex)
				{
					if (j != cur_fc)
					{
						// Swap
						d6FACE tmpf = w->Face[cur_fc];
						w->Face[cur_fc] = w->Face[j];
						w->Face[j] = tmpf;

						for (k = 0; k < 4; k++)
						{
							// Swap
							d6VERTEX tmpv = w->Vertex[cur_fc * 4 + k];
							w->Vertex[cur_fc * 4 + k] = w->Vertex[j * 4 + k];
							w->Vertex[j * 4 + k] = tmpv;
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
	void LOADER_LoadWorld(const std::string& path, d6WORLD *world, bool mirror, std::vector<Bonus>& bonuses)
	{
		int         x, y, b;
		myFile_s    *f;

		w = world;
		l = &w->Level;

		if (w->Vertex != NULL) MY_Free(w->Vertex);
		if (w->Face != NULL) MY_Free(w->Face);
		if (l->Data != NULL) MY_Free(l->Data);

		f = MY_FOpen(path.c_str(), 0, "rb", true);
		MY_FRead(&l->SizeX, 4, 1, f);
		MY_FRead(&l->SizeY, 4, 1, f);
		MY_FSeek(f, 12, SEEK_SET);
		l->Size = l->SizeX * l->SizeY;
		l->Data = D6_MALLOC(myWORD, l->Size);
		MY_FRead(l->Data, 2, l->Size, f);
		MY_FClose(&f);

		if (mirror)
			for (y = 0; y < l->SizeY; y++)
				for (x = 0; x < l->SizeX / 2; x++)
				{
					b = l->Data[y * l->SizeX + x];
					l->Data[y * l->SizeX + x] = l->Data[y * l->SizeX + l->SizeX - 1 - x];
					l->Data[y * l->SizeX + l->SizeX - 1 - x] = b;
				}

		LOADER_Process(bonuses);
		LOADER_Optimize();
	}
}