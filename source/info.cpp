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

#define D6_INFO_WAIT        5
#define D6_INFO_MAX         10

namespace Duel6
{
	struct d6INFO_s
	{
		Player  *From;
		float   Wait;
		char    Text[100];
	};

	static d6INFO_s     d6Info[D6_MAX_PLAYERS][D6_INFO_MAX];
	static int          d6InfoStart[D6_MAX_PLAYERS], d6InfoCount[D6_MAX_PLAYERS];
	static bool         d6ShowRanking = false;

	void INFO_Init(void)
	{
		int     i;

		for (i = 0; i < D6_MAX_PLAYERS; i++)
		{
			d6InfoStart[i] = 0;
			d6InfoCount[i] = 0;
		}
	}

	static void INFO_Scroll(int i)
	{
		d6InfoStart[i] = (d6InfoStart[i] + 1) % D6_INFO_MAX;
		d6InfoCount[i]--;
	}

	void INFO_Add(Player& player, const char *tex, ...)
	{
		int     i = player.State.I, j = i, n;
		char    texP[100];
		va_list argptr;

		va_start(argptr, tex);
		vsprintf(texP, tex, argptr);
		va_end(argptr);

		if (d6ScreenMode == ScreenMode::FullScreen)
			i = 0;

		if (++d6InfoCount[i] > D6_INFO_MAX)
			INFO_Scroll(i);

		n = (d6InfoStart[i] + d6InfoCount[i] - 1) % D6_INFO_MAX;
		d6Info[i][n].From = &player;
		d6Info[i][n].Wait = D6_INFO_WAIT * (int)APP_FPS_SPEED;
		strcpy(d6Info[i][n].Text, texP);
	}

	void INFO_ShowRankingSwap(void)
	{
		d6ShowRanking = !d6ShowRanking;
	}

	void INFO_MoveAll(void)
	{
		int         i, j;

		for (i = 0; i < D6_MAX_PLAYERS; i++)
			for (j = 0; j < d6InfoCount[i]; j++)
			{
				float *wait = &d6Info[i][(d6InfoStart[i] + j) % D6_INFO_MAX].Wait;
				*wait -= g_app.frame_interval;
				if (*wait <= 0)
				{
					INFO_Scroll(i);
					j--;
				}
			}
	}

	void INFO_DrawAll(void)
	{
		d6VIEW_s    *w;
		char        msg[200];
		int         sH, x, y;

		if (d6ScreenMode == ScreenMode::FullScreen)
		{
			if (d6Playing > 4)
				sH = g_vid.cl_height - 50;
			else
				sH = g_vid.cl_height - 20;
		}
		else
			sH = g_vid.cl_height / 2 - 24;

		CO_FontColor(255, 255, 0);

		for (Size i = 0; i < D6_MAX_PLAYERS; i++)
		{
			for (int j = 0; j < d6InfoCount[i]; j++)
			{
				w = &d6Player[i]->View;
				int n = (d6InfoStart[i] + j) % D6_INFO_MAX;
				x = w->X + 4;
				y = w->Y + sH - (j << 4);

				if (d6ScreenMode == ScreenMode::FullScreen)
					sprintf(msg, "%s : %s", d6Info[i][n].From->State.PH->Name, d6Info[i][n].Text);
				else
					sprintf(msg, "%s", d6Info[i][n].Text);

				glColor4f(0, 0, 1, 0.7f);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin(GL_QUADS);
				glVertex2i(x, y + 15);
				glVertex2i(x + 8 * strlen(msg), y + 15);
				glVertex2i(x + 8 * strlen(msg), y + 1);
				glVertex2i(x, y + 1);
				glEnd();
				glDisable(GL_BLEND);

				CO_FontPrintf(x, y, msg);
			}
		}

		if (d6ShowRanking && d6ScreenMode == ScreenMode::FullScreen)
		{
			char    rank_name[D6_MAX_PLAYERS][20];
			int     best, n_max = 0, rank_points[D6_MAX_PLAYERS];

			for (Size i = 0; i < d6Playing; i++)
			{
				strcpy(rank_name[i], d6Player[i]->State.PH->Name);
				rank_points[i] = d6Player[i]->State.PH->Kills + d6Player[i]->State.PH->Wins;
				n_max = MY_Max(n_max, 5 + int(strlen(rank_name[i])));
			}

			for (Size i = 0; i < d6Playing; i++)
			{
				Size n = 0;
				best = rank_points[0];

				for (Size j = 0; j < d6Playing - i; j++)
				{
					if (rank_points[j] > best)
					{
						n = j;
						best = rank_points[j];
					}
				}

				x = d6Player[0]->View.X + g_vid.cl_width - 8 * n_max - 3;
				y = d6Player[0]->View.Y + sH - (i << 4);

				glColor4f(0, 0, 1, 0.7f);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBegin(GL_QUADS);
				glVertex2i(x, y + 15);
				glVertex2i(x + 8 * n_max, y + 15);
				glVertex2i(x + 8 * n_max, y + 1);
				glVertex2i(x, y + 1);
				glEnd();
				glDisable(GL_BLEND);

				CO_FontPrintf(x, y, rank_name[n]);
				CO_FontPrintf(x + 8 * (n_max - 5), y, "|%4d", rank_points[n]);

				if (n < d6Playing - i - 1)
				{
					strcpy(rank_name[n], rank_name[d6Playing - i - 1]);
					rank_points[n] = rank_points[d6Playing - i - 1];
				}
			}
		}
	}
}