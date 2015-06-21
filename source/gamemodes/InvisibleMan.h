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
    class InvisibleMan : public GameMode
    {

    public:
        InvisibleMan(){};
        std::string getName()
        {
            return "Deathmatch";
        }
        void initialize(World *world);
        void preparePlayer(Player *player, Int32 playerIndex, std::vector<Player> &allPlayers);
        bool roundIsOver(Duel6::World world);
    };
}
#endif //DUEL6R_INVISIBLEMAN_H
