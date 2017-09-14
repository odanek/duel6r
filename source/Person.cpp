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

#include "Person.h"

namespace Duel6 {
    Person &Person::reset() {
        shots = 0;
        hits = 0;
        kills = 0;
        wins = 0;
        games = 0;
        penalties = 0;
        timeAlive = 0;
        totalGameTime = 0;
        totalDamage = 0;
        return *this;
    }

    Json::Value Person::toJson() const {
        Json::Value json = Json::Value::makeObject();
        json.set("name", Json::Value::makeString(getName()));
        json.set("shots", Json::Value::makeNumber(shots));
        json.set("hits", Json::Value::makeNumber(hits));
        json.set("kills", Json::Value::makeNumber(kills));
        json.set("wins", Json::Value::makeNumber(wins));
        json.set("penalties", Json::Value::makeNumber(penalties));
        json.set("games", Json::Value::makeNumber(games));
        json.set("timeAlive", Json::Value::makeNumber(timeAlive));
        json.set("totalGameTime", Json::Value::makeNumber(totalGameTime));
        json.set("totalDamage", Json::Value::makeNumber(totalDamage));
        return json;
    }

    Person Person::fromJson(const Json::Value &json) {
        Person person;
        person.name = json.get("name").asString();
        person.shots = json.get("shots").asInt();
        person.hits = json.get("hits").asInt();
        person.kills = json.get("kills").asInt();
        person.wins = json.get("wins").asInt();
        person.penalties = json.get("penalties").asInt();
        person.games = json.get("games").asInt();
        person.timeAlive = json.get("timeAlive").asInt();
        person.totalGameTime = json.get("totalGameTime").asInt();
        person.totalDamage = json.getOrDefault("totalDamage", Json::Value::makeNumber(0)).asInt();
        return person;
    }
}