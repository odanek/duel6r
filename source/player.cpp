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

#include <stdlib.h>
#include "project.h"
#include "anims.h"

static  d6KEYBOARD_s    d6Keyboard[12] =
{
    // 1
    { APP_INP_KEY | SDLK_LEFT,  APP_INP_KEY | SDLK_RIGHT, APP_INP_KEY | SDLK_UP,
      APP_INP_KEY | SDLK_DOWN,  APP_INP_KEY | SDLK_RCTRL, APP_INP_KEY | SDLK_RSHIFT},
    // 2
    { APP_INP_KEY | SDLK_a,     APP_INP_KEY | SDLK_d,     APP_INP_KEY | SDLK_w,
      APP_INP_KEY | SDLK_s,     APP_INP_KEY | SDLK_q,     APP_INP_KEY | SDLK_e},
    // 3
    { APP_INP_KEY | SDLK_h,     APP_INP_KEY | SDLK_k,     APP_INP_KEY | SDLK_u,
      APP_INP_KEY | SDLK_j,     APP_INP_KEY | SDLK_o,     APP_INP_KEY | SDLK_l },
    // 4
    { APP_INP_KEY | SDLK_KP1,       APP_INP_KEY | SDLK_KP3,   APP_INP_KEY | SDLK_KP5,
      APP_INP_KEY | SDLK_KP_ENTER,  APP_INP_KEY | SDLK_KP0,   APP_INP_KEY | SDLK_KP_PERIOD },
    // 5
    { APP_INP_JOY1 | APP_INP_JOY_LEFT, APP_INP_JOY1 | APP_INP_JOY_RIGHT,
      APP_INP_JOY1 | APP_INP_JOY_BUT1, APP_INP_JOY1 | APP_INP_JOY_DOWN,
      APP_INP_JOY1 | APP_INP_JOY_BUT2, APP_INP_JOY1 | APP_INP_JOY_BUT3 },
    // 6
    { APP_INP_JOY2 | APP_INP_JOY_LEFT, APP_INP_JOY2 | APP_INP_JOY_RIGHT,
      APP_INP_JOY2 | APP_INP_JOY_BUT1, APP_INP_JOY2 | APP_INP_JOY_DOWN,
      APP_INP_JOY2 | APP_INP_JOY_BUT2, APP_INP_JOY2 | APP_INP_JOY_BUT3 },
    // 7
    { APP_INP_JOY3 | APP_INP_JOY_LEFT, APP_INP_JOY3 | APP_INP_JOY_RIGHT,
      APP_INP_JOY3 | APP_INP_JOY_BUT1, APP_INP_JOY3 | APP_INP_JOY_DOWN,
      APP_INP_JOY3 | APP_INP_JOY_BUT2, APP_INP_JOY3 | APP_INP_JOY_BUT3 },
    // 8
    { APP_INP_JOY4 | APP_INP_JOY_LEFT, APP_INP_JOY4 | APP_INP_JOY_RIGHT,
      APP_INP_JOY4 | APP_INP_JOY_BUT1, APP_INP_JOY4 | APP_INP_JOY_DOWN,
      APP_INP_JOY4 | APP_INP_JOY_BUT2, APP_INP_JOY4 | APP_INP_JOY_BUT3 },
    // 9
    { APP_INP_JOY5 | APP_INP_JOY_LEFT, APP_INP_JOY5 | APP_INP_JOY_RIGHT,
      APP_INP_JOY5 | APP_INP_JOY_BUT1, APP_INP_JOY5 | APP_INP_JOY_DOWN,
      APP_INP_JOY5 | APP_INP_JOY_BUT2, APP_INP_JOY5 | APP_INP_JOY_BUT3 },
    // 10
    { APP_INP_JOY6 | APP_INP_JOY_LEFT, APP_INP_JOY6 | APP_INP_JOY_RIGHT,
      APP_INP_JOY6 | APP_INP_JOY_BUT1, APP_INP_JOY6 | APP_INP_JOY_DOWN,
      APP_INP_JOY6 | APP_INP_JOY_BUT2, APP_INP_JOY6 | APP_INP_JOY_BUT3 },
    // 11
    { APP_INP_JOY7 | APP_INP_JOY_LEFT, APP_INP_JOY7 | APP_INP_JOY_RIGHT,
      APP_INP_JOY7 | APP_INP_JOY_BUT1, APP_INP_JOY7 | APP_INP_JOY_DOWN,
      APP_INP_JOY7 | APP_INP_JOY_BUT2, APP_INP_JOY7 | APP_INP_JOY_BUT3 },
    // 12
    { APP_INP_JOY8 | APP_INP_JOY_LEFT, APP_INP_JOY8 | APP_INP_JOY_RIGHT,
      APP_INP_JOY8 | APP_INP_JOY_BUT1, APP_INP_JOY8 | APP_INP_JOY_DOWN,
      APP_INP_JOY8 | APP_INP_JOY_BUT2, APP_INP_JOY8 | APP_INP_JOY_BUT3 }
};

////////////////////////////////////////////////////////////////////////////////////////
//
//          d6PLAYER_c
//
////////////////////////////////////////////////////////////////////////////////////////

int d6PLAYER_c::next_id = 0;

d6PLAYER_c::d6PLAYER_c (void)
{
    Textures = 0;
    Texture = NULL;
    Camera = new mycam_c;
    Camera->resize (false, (mval_t) g_vid.gl_fov, float (g_vid.cl_width) / g_vid.cl_height);
    MY_RegMem (Camera, sizeof (mycam_c));
    Camera->rotate (180.0, 0.0, 0.0);
    State.I = next_id++;
    Keys = &d6Keyboard[State.I];
    Skin ();
}

d6PLAYER_c::~d6PLAYER_c (void)
{
    glDeleteTextures (Textures, Texture);
    MY_Free (Texture);
    MY_UnregMem (Camera);
    delete Camera;
}

/*
==================================================
Nahraje a naskinuje textury
==================================================
*/
void d6PLAYER_c::Skin (void)
{
    myKh3info_s     ki;
    myWORD          *hcData;
    myBYTE          *tcData;
    int             i, j, c_index, pos, img_size;

    // Nejdrive uvolni stary skin
    if (Texture != NULL)
        MY_Free (Texture);

    // Nacti novy
    g_app.con->printf (MY_L("APP00049|Inicializace hrace - nahravam textury\n"));

    if (MY_KH3Open (D6_FILE_PLAYER) != MY_OK)
        MY_Err (MY_ErrDump(MY_L("APP00091|Nepodarilo se otevrit soubor %s s texturami postav"), D6_FILE_PLAYER));

    MY_KH3GetInfo (&ki);
    g_app.con->printf (MY_L("APP00050|...Soubor %s obsahuje %d textur\n"), D6_FILE_PLAYER, ki.picts);

    Textures = ki.picts;
    Texture = D6_MALLOC (myUINT, Textures);
    img_size = ki.sizex * ki.sizey;
    hcData = (myWORD *) MY_Alloc (img_size << 1);
    tcData = (myBYTE *) MY_Alloc (img_size << 2);

    for (i = 0; i < Textures; i++)
    {
        MY_KH3Load (i, hcData);
        pos = 0;

        for (j = 0; j < img_size; j++)
        {
            // Get color from skin
            switch (hcData[j])
            {
                case 65504: c_index = 0; break;     // Vlasy
                case 57024: c_index = 1; break;     // Vlasy - spodni tmava cast
                case 21   : c_index = 2; break;     // Obrys tela
                case 31   : c_index = 3; break;     // Triko
                case 1440 : c_index = 4; break;     // Obrys rukou
                case 2016 : c_index = 5; break;     // Rukavy
                case 34816: c_index = 6; break;     // Kalhoty
                case 46496: c_index = 7; break;     // Boty
                default: c_index = -1;
            }

            if (c_index == -1)
            {
                tcData[pos++] = (((hcData[j] >> 11) & 0x1F) * 255) / 31;
                tcData[pos++] = (((hcData[j] >> 5) & 0x3F) * 255) / 63;
                tcData[pos++] = ((hcData[j] & 0x1F) * 255) / 31;
            }
            else
            {
                tcData[pos++] = (d6PlayerSkin[State.I][c_index] >> 16) & 0xff;
                tcData[pos++] = (d6PlayerSkin[State.I][c_index] >> 8) & 0xff;
                tcData[pos++] = d6PlayerSkin[State.I][c_index] & 0xff;
            }

            // Alpha hodnota - pouziva se pri viditelnosti a pruhlednosti mist s 0
            tcData[pos++] = (!hcData[j]) ? 0 : 0xff;
        }

        glGenTextures (1, &Texture[i]);
        glBindTexture (GL_TEXTURE_2D, Texture[i]);
        glTexImage2D (GL_TEXTURE_2D, 0, 4, ki.sizex, ki.sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, tcData);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    MY_Free (hcData);
    MY_Free (tcData);
    MY_KH3Close ();
}

void d6PLAYER_c::SetControls (int n)
{
    if (n >= 0 && n <= 12)
        Keys = &d6Keyboard[n];
}

void d6PLAYER_c::SetView (int x, int y, int w, int h)
{
    View.X = x;
    View.Y = y;
    View.Width = w;
    View.Height = h;
}

void d6PLAYER_c::Left (void)
{
    if (State.Speed >= -D6_PLAYER_MAX_SPEED)
         State.Flags |= D6_FLAG_REQ_LEFT;
    if ((State.Speed -= g_app.frame_interval) < -D6_PLAYER_MAX_SPEED)
        State.Speed = -D6_PLAYER_MAX_SPEED;
    if (State.Speed < 0)
        State.O = 0;
}

void d6PLAYER_c::Right (void)
{
    if (State.Speed <= D6_PLAYER_MAX_SPEED)
         State.Flags |= D6_FLAG_REQ_RIGHT;
    if ((State.Speed += g_app.frame_interval) > D6_PLAYER_MAX_SPEED)
        State.Speed = D6_PLAYER_MAX_SPEED;
    if (State.Speed > 0)
        State.O = 1;
}

void d6PLAYER_c::Jump (void)
{
    if (!State.J)
        if (KONTR_CanJump (this))
            State.J = 90;
}

void d6PLAYER_c::Fall (void)
{
    if (!State.J && !State.Speed)
        State.Flags |= D6_FLAG_KNEE;
    else
        if (State.J > 0 && State.J < 180)
            State.J = 180;
}

void d6PLAYER_c::Pick (void)
{
    if (!State.J && !State.Speed && State.Elev == -1)
        BONUS_Pick (&State);
}

void d6PLAYER_c::MakeMove (void)
{
    float   sp;

    if (State.Flags & D6_FLAG_INWATER)
        sp = 0.67f;
    else
        sp = 1.0f;

    if (State.SD)
        sp *= 0.5;

    if (State.Bonus == D6_BONUS_SPEED)
        sp *= 1.43f;

    sp *= g_app.frame_interval;
    State.Elev = -1;

    if (State.J > 0)
    {
        if ((State.J += 2 * g_app.frame_interval) > 270.0)
            State.J = 270.0f;

        State.Y += d6Sin[int(State.J)] * D6_PLAYER_JUMP_SPEED * sp;

        if (State.J < 180.0f)
            KONTR_Kontr (this, 1);
        else
            KONTR_Kontr (this, 2);
    }
    else
        KONTR_Kontr (this, 3);

    if (State.Speed != 0)
    {
        if (!(State.Flags & D6_FLAG_REQ_RIGHT) && State.Speed > 0)
            if ((State.Speed -= g_app.frame_interval) < 0)
                State.Speed = 0;
        if (!(State.Flags & D6_FLAG_REQ_LEFT) && State.Speed < 0)
            if ((State.Speed += g_app.frame_interval) > 0)
                State.Speed = 0;

        State.X += State.Speed * D6_PLAYER_ACCEL * sp;
        KONTR_Kontr (this, 4);
    }

    ELEV_MoveMan (&State);

    State.Flags &= ~(D6_FLAG_REQ_RIGHT | D6_FLAG_REQ_LEFT);
}

void d6PLAYER_c::CheckKeys (void)
{
    if ((State.Flags & (D6_FLAG_DEAD | D6_FLAG_PICK)) != 0)
        return;

    if (!(State.Flags & D6_FLAG_KNEE))
    {
        if (CO_InpIsPressed (Keys->Left)) Left ();
        if (CO_InpIsPressed (Keys->Right)) Right ();
        if (CO_InpIsPressed (Keys->Up)) Jump ();
        if (CO_InpIsPressed (Keys->Pick)) Pick ();
    }

    if (CO_InpIsPressed (Keys->Shoot)) WPN_Shoot (&State);
    State.Flags &= ~D6_FLAG_KNEE;
    if (CO_InpIsPressed (Keys->Down)) Fall ();
}

void d6PLAYER_c::SetAnm (void)
{
    float   ad = 0.0;
    short   *a;

    if (State.Flags & D6_FLAG_DEAD)
    {
        if (State.Flags & D6_FLAG_LYING)
            ANM_ReSet (State.A, State.X, State.Y, -1, State.O, d6LAnim);
        else
            ANM_ReSet (State.A, State.X, State.Y, -1, State.O, d6NAnim);

        return;
    }

    if (State.Flags & D6_FLAG_PICK)
        a = d6PAnim;
    else if (State.J)
        a = d6JAnim;
    else if (State.Flags & D6_FLAG_KNEE)
    {
        a = d6DAnim;
        ad = 0.2f;
    }
    else if (!State.Speed)
        a = d6SAnim;
    else
        a = d6WAnim;

    ANM_ReSet (State.A, State.X, State.Y, -1, State.O, a);
    ANM_ReSet (State.GA, State.X, State.Y - ad, -1, State.O, NULL);

    if (State.Flags & D6_FLAG_PICK)
        if (ANM_CheckFlags (State.A, ANM_FLAG_FINISHED))
        {
            ANM_AddFlags (State.GA, ANM_FLAG_DRAW);
            State.Flags &= ~D6_FLAG_PICK;
        }
}

void d6PLAYER_c::PrepareCam (void)
{
    float   fovX, fovY, mZ, dX = 0.0, dY = 0.0;
    d6LEVEL *l = &d6World.Level;

    CamPos.Pos.x = l->SizeX / 2.0f;
    CamPos.Pos.y = l->SizeY / 2.0f;

    fovY = (float) tan (MM_D2R (g_vid.gl_fov) / 2.0f);
    fovX = g_vid.gl_aspect * fovY;

    if (d6ZoomMode == D6_ZM_FULL)
    {
        if (l->SizeX > g_vid.gl_aspect * l->SizeY)
            dX = (float) l->SizeX;
        else
            dY = (float) l->SizeY;
    }
    else if (l->SizeX > l->SizeY)
    {
        if (d6ZoomBlc > l->SizeY)
            dX = (float) l->SizeY;
        else
            dX = (float) d6ZoomBlc;
    }
    else
    {
        if (d6ZoomBlc > l->SizeX)
            dX = (float) l->SizeX;
        else
            dX = (float) d6ZoomBlc;
    }

    if (dX == 0.0)
    {
        mZ = dY / (2.0f * fovY);
        dX = 2.0f * fovX * mZ;
    }
    else
    {
        mZ = dX / (2.0f * fovX);
        dY = 2.0f * fovY * mZ;
    }

    CamPos.Pos.z = mZ + 1.0f;
    CamPos.Left = CamPos.Pos.x - dX / 2.0f;
    CamPos.Right = CamPos.Pos.x + dX / 2.0f;
    CamPos.Down = CamPos.Pos.y - dY / 2.0f;
    CamPos.Up = CamPos.Pos.y + dY / 2.0f;
    CamPos.TolX = (dX * D6_CAM_TOLPER_X) / 200.0f;
    CamPos.TolY = (dY * D6_CAM_TOLPER_Y) / 200.0f;

    Camera->setpos (CamPos.Pos);
}

void d6PLAYER_c::UpdateCam (void)
{
    d6LEVEL *l = &d6World.Level;
    float   mX = 0.0, mY = 0.0, X, Y;

    if (d6ZoomMode == D6_ZM_FULL)
        return;

    X = State.X + 0.5f;
    Y = State.Y - 0.5f;

    if (X < CamPos.Pos.x - CamPos.TolX)
    {
        mX = X - (CamPos.Pos.x - CamPos.TolX);
        if (CamPos.Left + mX < 0.0f)
            mX = -CamPos.Left;
    }
    else if (X > CamPos.Pos.x + CamPos.TolX)
    {
        mX = X - (CamPos.Pos.x + CamPos.TolX);
        if (CamPos.Right + mX > (float) l->SizeX)
            mX = (float) l->SizeX - CamPos.Right;
    }
    if (Y < CamPos.Pos.y - CamPos.TolY)
    {
        mY = Y - (CamPos.Pos.y - CamPos.TolY);
        if (CamPos.Down + mY < 0.0f)
            mY = -CamPos.Down;
    }
    else if (Y > CamPos.Pos.y + CamPos.TolY)
    {
        mY = Y - (CamPos.Pos.y + CamPos.TolY);
        if (CamPos.Up + mY > (float) l->SizeY)
            mY = (float) l->SizeY - CamPos.Up;
    }

    if (mX != 0.0)
    {
        CamPos.Left += mX;
        CamPos.Right += mX;
        CamPos.Pos.x += mX;
    }
    if (mY != 0.0)
    {
        CamPos.Up += mY;
        CamPos.Down += mY;
        CamPos.Pos.y += mY;
    }
    if (mX != 0.0 || mY != 0.0)
        Camera->setpos (CamPos.Pos);
}

void d6PLAYER_c::Hit (float pw, d6SHOT_s *s, bool hit)
{
    if (State.Bonus == D6_BONUS_INVUL)
        return;
    if (!State.Life && ((State.Flags & D6_FLAG_LYING) == 0))
        return;
    if (!State.Life && s == NULL)
        return;

    if (s != NULL && hit)
        if (s->WD->Blood)
            EXPL_Add (State.X + 0.3f + (rand () % 40) * 0.01f, s->Y - 0.15f, 0.2f, 0.5f, 0xFF0000);

    if (!State.Life)
    {
        if (s->WD->ExplC && hit)
        {
            State.Flags &= ~D6_FLAG_LYING;
            EXPL_Add (State.X + 0.5f, State.Y - 0.7f, 0.5f, 1.2f, s->WD->ExplC);
        }
        return;
    }

    State.Life -= pw;
    if (hit && s != NULL)
        d6Player[s->FromPlayer]->State.PH->Hits++;

    if (State.Life < 1)
    {
        State.Life = 0;
        State.Flags |= D6_FLAG_DEAD | D6_FLAG_LYING;
        ANM_ReSet (State.A, State.X, State.Y, ANM_LOOP_ONESTOP, -1, NULL);
        ANM_RemoveFlags (State.GA, ANM_FLAG_ALL);
        if (s != NULL)
        {
            if (s->X < State.X)
                State.O = 0;
            else
                State.O = 1;

            if (State.I != s->FromPlayer)
            {
                d6Player[s->FromPlayer]->State.PH->Kills++;
                INFO_Add (State.I, MY_L("APP00051|Jsi mrtvy - zabil te %s"), d6Player[s->FromPlayer]->State.PH->Name);
                INFO_Add (s->FromPlayer, MY_L("APP00052|Zabil jsi hrace %s"), State.PH->Name);
            }
            else
                INFO_Add (State.I, MY_L("APP00053|Jsi mrtvy"));

            if (s->WD->ExplC && hit)
            {
                State.Flags &= ~D6_FLAG_LYING;
                EXPL_Add (State.X + 0.5f, State.Y - 0.5f, 0.5f, 1.2f, s->WD->ExplC);
            }
        }
        else
            INFO_Add (State.I, MY_L("APP00054|Jsi mrtvy"));

        SOUND_PlaySample (D6_SND_DEAD);

        // Pridej lezici zbran
        if (!State.J && (s == NULL || !s->WD->ExplC || !hit))
        {
            int x1 = int (State.X + 0.2f), x2 = int (State.X + 0.8f);
            int y = d6World.Level.SizeY - int (State.Y - 0.5f) - 1;

            if (D6_BlockZ (x1, y + 1) == D6_ANM_F_BLOCK && D6_BlockZ (x1, y) != D6_ANM_F_BLOCK)
                BONUS_AddDeadManGun (x1, y, &State);
            else
                if (D6_BlockZ (x2, y + 1) == D6_ANM_F_BLOCK && D6_BlockZ (x2, y) != D6_ANM_F_BLOCK)
                    BONUS_AddDeadManGun (x2, y, &State);
        }
    }
    else if (hit)
        SOUND_PlaySample (D6_SND_HIT);
}
