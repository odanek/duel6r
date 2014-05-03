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
	struct d6EXPLOSION_s
	{
		float       X;
		float       Y;
		float       Now;
		float       Max;
		GLubyte     Col[3];
	};

	static  GLuint          d6ExTexture;
	static  d6EXPLOSION_s   d6Expl[D6_MAX_EXPLOSIONS];
	static  int             d6Explosions;

	void EXPL_Load(void)
	{
		d6ExTexture = UTIL_LoadKH3Texture(D6_FILE_EXPLODE, 0, true);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void EXPL_Free(void)
	{
		glDeleteTextures(1, &d6ExTexture);
	}

	void EXPL_Init(void)
	{
		d6Explosions = 0;
	}

	void EXPL_Add(float x, float y, float s, float m, myDWORD c)
	{
		d6EXPLOSION_s   *e;

		if (d6Explosions >= D6_MAX_EXPLOSIONS)
			return;

		e = &d6Expl[d6Explosions++];
		e->X = x;
		e->Y = y;
		e->Now = s;
		e->Max = m;
		e->Col[0] = (GLubyte)((c & 0xFF0000) >> 16);
		e->Col[1] = (GLubyte)((c & 0x00FF00) >> 8);
		e->Col[2] = (GLubyte)c & 0x0000FF;
	}

	static void EXPL_Remove(int i)
	{
		d6Explosions--;
		if (i < d6Explosions)
			memcpy(&d6Expl[i], &d6Expl[d6Explosions], sizeof (d6EXPLOSION_s));
	}

	void EXPL_MoveAll(float elapsedTime)
	{
		int     i;

		for (i = 0; i < d6Explosions; i++)
		{
			d6Expl[i].Now += D6_EXPL_SPEED * elapsedTime;
			if (d6Expl[i].Now > d6Expl[i].Max)
			{
				EXPL_Remove(i);
				i--;
			}
		}
	}

	void EXPL_DrawAll(void)
	{
		d6EXPLOSION_s   *e;
		int             i;

		glEnable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, d6ExTexture);
		glBegin(GL_QUADS);

		for (i = 0; i < d6Explosions; i++)
		{
			e = &d6Expl[i];
			glColor3ubv(e->Col);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(e->X - e->Now, e->Y + e->Now, 0.6f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(e->X + e->Now, e->Y + e->Now, 0.6f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(e->X + e->Now, e->Y - e->Now, 0.6f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(e->X - e->Now, e->Y - e->Now, 0.6f);
		}

		glEnd();
		glColor3f(1, 1, 1);
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_DEPTH_TEST);
	}
}