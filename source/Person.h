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

#ifndef DUEL6_PERSON_H
#define DUEL6_PERSON_H

#include <stdio.h>
#include <string>
#include "Type.h"
#include "File.h"
#include "json/JsonValue.h"

namespace Duel6 {
    class PersonProfile;

    class Person {
    public:
        static constexpr Int32 defaultElo = 1000;

    private:
        std::string name;
        Int32 shots;
        Int32 hits;
        Int32 kills;
        Int32 deaths;
        Int32 assistances;
        Int32 wins;
        Int32 penalties;
        Int32 games;
        Int32 timeAlive;
        Int32 totalGameTime;
        Int32 totalDamage;
        Int32 assistedDamage;
        Int32 elo;
        Int32 eloTrend;
        Int32 eloGames;
        PersonProfile *profile;

    public:
        Person()
                : shots(0), hits(0), kills(0), deaths(0), assistances(0), wins(0), penalties(0), games(0), timeAlive(0), totalGameTime(0),
                  totalDamage(0), assistedDamage(0), elo(defaultElo), eloTrend(0), eloGames(0), profile(nullptr) {}

        explicit Person(const std::string &name, PersonProfile *profile)
                : Person() {
            this->name = name;
            this->profile = profile;
        }

        const std::string &getName() const {
            return name;
        }

        Int32 getShots() const {
            return shots;
        }

        void setShots(Int32 shots) {
            this->shots = shots;
        }

        Int32 getHits() const {
            return hits;
        }

        void setHits(Int32 hits) {
            this->hits = hits;
        }

        Int32 getKills() const {
            return kills;
        }

        void setKills(Int32 kills) {
            this->kills = kills;
        }

        Int32 getDeaths() const {
            return deaths;
        }

        void setDeaths(Int32 deaths) {
            this->deaths = deaths;
        }

        Int32 getAssistances() const {
            return assistances;
        }

        void setAssistances(Int32 assistances) {
            this->assistances = assistances;
        }

        Int32 getWins() const {
            return wins;
        }

        void setWins(Int32 wins) {
            this->wins = wins;
        }

        Int32 getPenalties() const {
            return penalties;
        }

        void setPenalties(Int32 penalties) {
            this->penalties = penalties;
        }

        Int32 getGames() const {
            return games;
        }

        void setAssistedDamage(Int32 assistedDamage) {
            this->assistedDamage = assistedDamage;
        }

        void setEloGames(Int32 eloGames) {
            this->eloGames = eloGames;
        }

        void setGames(Int32 games) {
            this->games = games;
        }

        void setTimeAlive(Int32 timeAlive) {
            this->timeAlive = timeAlive;
        }

        void setTotalDamage(Int32 totalDamage) {
            this->totalDamage = totalDamage;
        }

        void setTotalGameTime(Int32 totalGameTime) {
            this->totalGameTime = totalGameTime;
        }
        Int32 getAccuracy() const {
            return getShots() > 0 ? (getHits() * 100 / getShots()) : 0;
        }

        Int32 getTotalPoints() const {
            return getKills() + getWins() + getAssistances() - getPenalties();
        }

        Int32 getTimeAlive() const {
            return timeAlive;
        }

        Int32 getAliveRatio() const {
            return totalGameTime > 0 ? (timeAlive * 100) / totalGameTime : 0;
        }

        Int32 getTotalGameTime() const {
            return totalGameTime;
        }

        Int32 getTotalDamage() const {
            return totalDamage;
        }

        Int32 getAssistedDamage() const {
            return assistedDamage;
        }

        Int32 getElo() const {
            return elo;
        }

        Int32 getEloTrend() const {
            return eloTrend;
        }

        Int32 getEloGames() const {
            return eloGames;
        }

        Person &addShots(Int32 shots) {
            this->shots += shots;
            return *this;
        }

        Person &addHits(Int32 hits) {
            this->hits += hits;
            return *this;
        }

        Person &addKills(Int32 kills) {
            this->kills += kills;
            return *this;
        }

        Person &addDeaths(Int32 deaths) {
            this->deaths += deaths;
            return *this;
        }

        Person &addAssistances(Int32 assistances) {
            this->assistances += assistances;
            return *this;
        }

        Person &addPenalties(Int32 penalties) {
            this->penalties += penalties;
            return *this;
        }

        Person &addWins(Int32 wins) {
            this->wins += wins;
            return *this;
        }

        Person &addGames(Int32 games) {
            this->games += games;
            return *this;
        }

        Person &addTimeAlive(Int32 timeAlive) {
            this->timeAlive += timeAlive;
            return *this;
        }

        Person &addTotalGameTime(Int32 totalGameTime) {
            this->totalGameTime += totalGameTime;
            return *this;
        }

        Person &addDamageCaused(Int32 damageCaused) {
            this->totalDamage += damageCaused;
            return *this;
        }

        Person &addAssistedDamage(Int32 damageCaused) {
            this->assistedDamage += damageCaused;
            return *this;
        }

        Person &setElo(Int32 elo) {
            this->elo = elo;
            return *this;
        }

        Person &setEloTrend(Int32 trend) {
            this->eloTrend = trend;
            return *this;
        }

        Person &addEloGame() {
            this->eloGames++;
            return *this;
        }

        PersonProfile *getProfile() const {
            return profile;
        }

        Person &setProfile(PersonProfile *profile) {
            this->profile = profile;
            return *this;
        }

        bool hasHigherScoreThan(const Person &person) const {
            if (getTotalPoints() > person.getTotalPoints()) {
                return true;
            }
            if (getTotalPoints() < person.getTotalPoints()) {
                return false;
            }
            if (getWins() > person.getWins()) {
                return true;
            }
            if (getWins() < person.getWins()) {
                return false;
            }

            return getTotalDamage() > person.getTotalDamage();
        }

        Person &reset();

        Json::Value toJson() const;

        static Person fromJson(const Json::Value &json);

        static std::string getKillsToDeathsRatio(Int32 kills, Int32 deaths);
    };
}

#endif
