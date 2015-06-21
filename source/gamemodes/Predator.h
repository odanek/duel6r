//
// Created by marcellus on 21.6.2015.
//

#ifndef DUEL6R_INVISIBLEMAN_H
#define DUEL6R_INVISIBLEMAN_H

#include "../GameMode.h"

namespace Duel6
{

    /**
     * PLOT:
     * Each round one player is randomly picked as an invisible man. Only his gun is visible.
     * All other people has to hunt him down as a team.
     * Invisible man: - invisible
     *                - fast
     *                - has submachine gun
     *
     * Others: - shotguns
     */
    class Predator : public GameMode
    {

    private:
        int predatorIndex;
        PlayerEventListener *eventListener = nullptr;
    public:
        static const std::string&PREDATOR_TEAM()
        {
            static std::string team("Predator");
            return team;
        }
        static const std::string& HUNTERS_TEAM()
        {
            static std::string team("Marines");
            return team;
        }

    public:
        Predator(){};
        std::string getName()
        {
            return "Predator";
        }
        void initialize(World *world, Duel6::Game *game);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool checkRoundOver(World *world, std::vector<Player*> &alivePlayers);
    };
}
#endif //DUEL6R_INVISIBLEMAN_H
