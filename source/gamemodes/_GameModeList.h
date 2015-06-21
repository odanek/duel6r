//
// Created by marcellus on 16.6.2015.
//

#ifndef DUEL6R_GAMEMODELIST_H
#define DUEL6R_GAMEMODELIST_H

#include <vector>
#include "../GameMode.h"
#include "DeathMatch.h"
#include "TeamDeathMatch.h"
#include "Predator.h"

// TODO: some cleanup after end of an app? consult with Ondrej
std::vector<Duel6::GameMode*> GAME_MODES = {
        new Duel6::DeathMatch(),
        new Duel6::TeamDeathMatch(2, false),
        new Duel6::TeamDeathMatch(2, true),
        new Duel6::TeamDeathMatch(3, false),
        new Duel6::TeamDeathMatch(3, true),
        new Duel6::Predator()
};



#endif //DUEL6R_GAMEMODELIST_H
