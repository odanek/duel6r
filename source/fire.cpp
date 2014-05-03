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

#define D6_FIRES        2

namespace Duel6
{
	struct d6FIRE_s
	{
		float   X;
		float   Y;
		int     FN;
		int     Fr;
		bool    A;
	};

	myUINT      d6FT[D6_FIRES] = { 11, 15 };
	int         d6FiresCount;
	d6FIRE_s    *d6Fire = NULL;
	short       d6FAnm[D6_FIRES][20] =
	{
		{ 12, 20, 13, 20, 12, 20, 13, 20, 12, 20, 13, 20, 12, 20, 13, 20, 14, 100, -1, 0 },
		{ 16, 20, 17, 20, 16, 20, 17, 20, 16, 20, 17, 20, 16, 20, 17, 20, 18, 100, -1, 0 }
	};

	void FIRE_Init(void)
	{
		int     i, j;

		for (i = 0; i < D6_FIRES; i++)
			for (j = 3; j < 4; j++)
			{
				glBindTexture(GL_TEXTURE_2D, d6World.Anm.TexGlNum[d6FT[i] + j]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
	}

	void FIRE_Free(void)
	{
		d6FiresCount = 0;
		MY_Free(d6Fire);
	}

	void FIRE_Find(void)
	{
		int     i, j, n = 0;

		FIRE_Free();

		for (i = 0; i < d6World.Faces; i++)
			for (j = 0; j < D6_FIRES; j++)
				if (d6World.Face[i].MinTex == (int)d6FT[j])
					d6FiresCount++;

		if (!d6FiresCount)
			return;

		d6Fire = D6_MALLOC(d6FIRE_s, d6FiresCount);

		for (i = 0; i < d6World.Faces; i++)
			for (j = 0; j < D6_FIRES; j++)
				if (d6World.Face[i].MinTex == (int)d6FT[j])
				{
					d6Fire[n].X = d6World.Vertex[i << 2].X;
					d6Fire[n].Y = d6World.Vertex[i << 2].Y;
					d6Fire[n].A = false;
					d6Fire[n].FN = i;
					d6Fire[n].Fr = j;
					n++;
				}
	}

	void FIRE_Check(Float32 X, Float32 Y, Float32 d)
	{
		d6FIRE_s    *f;
		float       vzd;
		int         i;

		X -= 0.5f;
		Y += 0.5f;

		for (i = 0; i < d6FiresCount; i++)
			if (!d6Fire[i].A)
			{
				vzd = (float)sqrt(D6_SQR(X - d6Fire[i].X) + D6_SQR(Y - d6Fire[i].Y));

				if (vzd < d)
				{
					f = &d6Fire[i];

					f->A = true;
					d6World.Face[f->FN].MinTex = 0;
					d6World.Face[f->FN].MaxTex = 0;
					d6World.Face[f->FN].NowTex = 0;

					Sprite fireSprite(d6FAnm[f->Fr], d6World.Anm.TexGlNum);
					fireSprite.setPosition(f->X, f->Y, 0.75f)
						.setLooping(AnimationLooping::OnceAndStop);
					d6SpriteList.addSprite(fireSprite);
				}
			}
	}
}