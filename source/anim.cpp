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
	static int          anmStartVertex;
	static anmVERTEX_s  *anmVertex;
	static anmANM_s     anmAnm[ANM_MAX];

	void ANM_Init(anmVERTEX_s *first, int num)
	{
		int     i;

		anmVertex = first;
		anmStartVertex = num;

		for (i = 0; i < ANM_MAX; i++)
		{
			anmAnm[i].Flags = ANM_FLAG_NONE;
			anmAnm[i].I = i;
			anmVertex[(i << 2)].V = 0.0f;
			anmVertex[(i << 2) + 1].V = 0.0f;
			anmVertex[(i << 2) + 2].V = 1.0f;
			anmVertex[(i << 2) + 3].V = 1.0f;
		}
	}

	static void ANM_SetXYU(anmANM_s *a, float X, float Y, float Z)
	{
		anmVERTEX_s *v = &anmVertex[a->I << 2];

		a->X = X;
		a->Y = Y;
		a->Z = Z;

		v[0].X = X;
		v[0].Y = Y;
		v[0].Z = Z;
		v[0].U = (float)a->O;

		v[1].X = X + 1.0f;
		v[1].Y = Y;
		v[1].Z = Z;
		v[1].U = 1.0f - (float)a->O;

		v[2].X = X + 1.0f;
		v[2].Y = Y - 1.0f;
		v[2].Z = Z;
		v[2].U = 1.0f - (float)a->O;

		v[3].X = X;
		v[3].Y = Y - 1.0f;
		v[3].Z = Z;
		v[3].U = (float)a->O;
	}

	int ANM_Add(float X, float Y, float Z, int S, int loop, int O, short *pl, myUINT *tex, bool T)
	{
		int         i;
		anmANM_s    *a;

		for (i = 0; i < ANM_MAX; i++)
			if (!(anmAnm[i].Flags & ANM_FLAG_USED))
				break;

		if (i >= ANM_MAX)
			MY_Err(MY_L("APP00011|ANM_Add chyba: preteceni"));

		a = &anmAnm[i];
		a->Pos = 0;
		a->Del = 0;
		a->Sp = S;
		a->Pl = pl;
		a->Tex = tex;
		a->Loop = loop;
		a->O = O;
		a->Grow = 0;
		a->Flags = ANM_FLAG_USED | ANM_FLAG_DRAW;
		if (T)
		{
			a->Flags |= ANM_FLAG_TRANS;
			a->Alpha = 0.6f;
		}
		else
			a->Alpha = 1.0f;
		ANM_SetXYU(a, X, Y, Z);

		return i;
	}

	void ANM_Grow(int i, float grow)
	{
		anmAnm[i].Grow = grow;
	}

	void ANM_ReSet(int i, float X, float Y, int loop, int O, short *pl)
	{
		anmANM_s    *a;
		int         oldO;

		if (!ANM_CheckFlags(i, ANM_FLAG_USED))
			return;

		a = &anmAnm[i];
		oldO = a->O;
		if (O != -1)
			a->O = O;
		if (loop != -1)
			a->Loop = loop;
		if (pl != NULL && pl != a->Pl)
		{
			a->Pos = 0;
			a->Del = 0;
			a->Pl = pl;
			a->Flags &= ~ANM_FLAG_FINISHED;
		}
		if (X != a->X || Y != a->Y || a->O != oldO)
			ANM_SetXYU(a, X, Y, a->Z);
	}

	void ANM_SetAnm(int i, int p)
	{
		if (anmAnm[i].Flags & ANM_FLAG_USED)
		{
			anmAnm[i].Pos = 2 * p;
			anmAnm[i].Del = 0;
			anmAnm[i].Flags &= ~ANM_FLAG_FINISHED;
		}
	}

	int ANM_Pict(int num)
	{
		return anmAnm[num].Pl[anmAnm[num].Pos];
	}

	static void ANM_Move(anmANM_s *a)
	{
		a->Del += g_app.frame_interval;
		if (a->Del >= a->Pl[a->Pos + 1] * a->Sp)
		{
			a->Pos += 2;
			a->Del = 0;
			if (a->Pl[a->Pos] == -1)
			{
				a->Flags |= ANM_FLAG_FINISHED;

				switch (a->Loop)
				{
				case ANM_LOOP_ONEKILL: a->Flags = ANM_FLAG_NONE; break;
				case ANM_LOOP_FOREVER: a->Pos = 0; break;
				case ANM_LOOP_ONESTOP: a->Pos -= 2; break;
				}
			}
		}

		if (a->Grow > 0)
		{
			anmVERTEX_s *v = &anmVertex[a->I << 2];

			a->X -= a->Grow * g_app.frame_interval;
			a->Y += a->Grow * g_app.frame_interval;
			v[0].X -= a->Grow * g_app.frame_interval;
			v[0].Y += a->Grow * g_app.frame_interval;
			v[1].X += a->Grow * g_app.frame_interval;
			v[1].Y += a->Grow * g_app.frame_interval;
			v[2].X += a->Grow * g_app.frame_interval;
			v[2].Y -= a->Grow * g_app.frame_interval;
			v[3].X -= a->Grow * g_app.frame_interval;
			v[3].Y -= a->Grow * g_app.frame_interval;
		}
	}

	static void ANM_DrawTrans(bool t)
	{
		anmANM_s    *a;
		int         i, v;
		GLfloat     cur_col[4];

		glGetFloatv(GL_CURRENT_COLOR, cur_col);

		v = anmStartVertex;
		for (i = 0; i < ANM_MAX; i++, v += 4)
			if (ANM_CheckFlags(i, ANM_FLAG_TRANS) == t)
			{
				if (!ANM_CheckFlags(i, ANM_FLAG_USED | ANM_FLAG_DRAW))
					continue;

				a = &anmAnm[i];

				if (ANM_CheckFlags(i, ANM_FLAG_NO_DEPTH))
				{
					glDisable(GL_DEPTH_TEST);
				}

				glColor4f(cur_col[0], cur_col[1], cur_col[2], a->Alpha);
				glBindTexture(GL_TEXTURE_2D, a->Tex[a->Pl[a->Pos]]);
				glDrawArrays(GL_QUADS, v, 4);

				if (ANM_CheckFlags(i, ANM_FLAG_NO_DEPTH))
				{
					glEnable(GL_DEPTH_TEST);
				}
			}
		glColor4f(cur_col[0], cur_col[1], cur_col[2], 1.0f);
	}

	void ANM_MoveAll(void)
	{
		int     i;

		for (i = 0; i < ANM_MAX; i++)
			if (anmAnm[i].Flags & ANM_FLAG_USED)
				ANM_Move(&anmAnm[i]);
	}

	void ANM_RemoveFlags(int i, int f)
	{
		if (i >= 0 && i < ANM_MAX)
			anmAnm[i].Flags &= ~f;
	}

	void ANM_AddFlags(int i, int f)
	{
		if (i >= 0 && i < ANM_MAX)
			anmAnm[i].Flags |= f;
	}

	bool ANM_CheckFlags(int i, int f)
	{
		if (i >= 0 && i < ANM_MAX)
			if ((anmAnm[i].Flags & f) == f)
				return true;

		return false;
	}

	void ANM_SetAlpha(int i, float a)
	{
		if (i >= 0 && i < ANM_MAX)
		{
			if (a < 1)
				ANM_AddFlags(i, ANM_FLAG_TRANS);
			else
				ANM_RemoveFlags(i, ANM_FLAG_TRANS);
			anmAnm[i].Alpha = a;
		}
	}

	void ANM_SetTexture(int i, myUINT *tex)
	{
		if (i >= 0 && i < ANM_MAX && tex != NULL)
			anmAnm[i].Tex = tex;
	}

	void ANM_DrawAll(void)
	{
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_CULL_FACE);

		ANM_DrawTrans(false);

		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		ANM_DrawTrans(true);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}
}