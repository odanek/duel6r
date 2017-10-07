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

#include "ShotList.h"
#include "World.h"
#include "Weapon.h"
#include "Player.h"

namespace Duel6 {
    ShotList::ShotList() {}

    void ShotList::addShot(ShotPointer &&shot) {
        shots.push_back(std::forward<ShotPointer>(shot));
    }

    void ShotList::update(World &world, Float32 elapsedTime) {
        auto iter = shots.begin();

        while (iter != shots.end()) {
            Shot &shot = *iter->get();
            if (!shot.update(elapsedTime, world)) {
                iter = shots.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    void ShotList::forEach(std::function<bool(const Shot &)> handler) const {
        for (auto &shotPointer : shots) {
            if (!handler(*shotPointer)) {
                break;
            }
        }
    }

    void ShotList::forEach(std::function<bool(Shot &)> handler) {
        for (auto &shotPointer : shots) {
            if (!handler(*shotPointer)) {
                break;
            }
        }
    }
}