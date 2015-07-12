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

#include "TeamDeathMatch.h"
#include "../Game.h"
#include "TeamDeathMatchPlayerEventListener.h"

namespace Duel6
{
	namespace
	{
		// Predefined teams as a temporary solution
		std::vector<std::string> TEAMS = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};
		std::vector<Color> COLORS = { Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255), Color(219, 216, 241), Color(115,137,186)};
	}

	void TeamDeathMatch::preparePlayer(Player& player, Int32 playerIndex, std::vector<Player>& allPlayers)
	{
		Int32 teamSize = std::max(0, (int)allPlayers.size() / teamsCount);
		Int32 playerTeam = std::min(playerIndex / teamSize, teamsCount - 1);

		// todo: add support for custom teams in future versions
		std::string& teamName = TEAMS[playerTeam];
		Color teamColor = COLORS[playerTeam];

		player.setTeam(teamName);
		player.setOverlay(teamColor);
		player.setEventListener(*eventListener);
	}


	bool TeamDeathMatch::checkRoundOver(World& world, std::vector<Player*>& alivePlayers)
	{
		if (alivePlayers.size() == 0)
		{
			for (const Player& player : world.getPlayers())
			{
				world.getMessageQueue().add(player, D6_L("End of round - no winner"));
			}
			return true;
		}

		bool oneTeamRemaining = true;
		std::string lastAliveTeam = "";

		for (Player *player : alivePlayers)
		{
			if (lastAliveTeam == "")
			{
				lastAliveTeam = player->getTeam();
			}
			else if (lastAliveTeam != player->getTeam())
			{
				oneTeamRemaining = false;
				break;
			}
		}

		if (oneTeamRemaining)
		{
			for(Player& player : world.getPlayers())
			{
				if (player.hasTeam(lastAliveTeam))
				{
					world.getMessageQueue().add(player, Format("Team {0} won!") << lastAliveTeam);
					player.getPerson().addWins(1);
				}
			}
		}

		return oneTeamRemaining;
	}

	void TeamDeathMatch::initialize(World& world, Game& game)
	{
		eventListener = std::make_unique<TeamDeathMatchPlayerEventListener>(world.getMessageQueue(), game.getSettings(), friendlyFire);
	}
}
