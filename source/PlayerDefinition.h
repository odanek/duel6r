#ifndef SOURCE_PLAYERDEFINTION_H_
#define SOURCE_PLAYERDEFINTION_H_
#include "Person.h"
#include "PlayerSkinColors.h"
#include "PlayerSounds.h"
#include "input/PlayerControls.h"
namespace Duel6 {

    class PlayerDefinition {
    private:
        Person &person;
        PlayerSkinColors colors;
        const PlayerSounds &sounds;
        const PlayerControls &controls;
        Int32 team;
        Int32 playerId;
        Int32 clientId;
        Int32 clientLocalId;
        public:
        Int32 playerPos;
        PlayerDefinition(Person &person, const PlayerSkinColors &colors, const PlayerSounds &sounds,
                         const PlayerControls &controls,
                         Int32 team, Int32 playerId,
                         Int32 clientId, Int32 clientLocalId)
            : person(person),
              colors(colors),
              sounds(sounds),
              controls(controls),
              team(team),
              playerId(playerId),
              clientId(clientId),
              clientLocalId(clientLocalId) {
        }

        Person& getPerson() const {
            return person;
        }

        PlayerSkinColors& getColors() {
            return colors;
        }

        const PlayerSkinColors& getColors() const {
            return colors;
        }

        const PlayerSounds& getSounds() const {
            return sounds;
        }

        const PlayerControls& getControls() const {
            return controls;
        }

        Int32 getTeam() const {
            return team;
        }

        Int32 getClientId() const {
            return clientId;
        }

        Int32 getClientLocalId() const {
            return clientLocalId;
        }

        Int32 getPlayerId() const {
            return playerId;
        }

        void setPlayerId(Int32 playerId) {
            this->playerId = playerId;
        }
    };
}

#endif /* SOURCE_DEFINTION_H_ */
