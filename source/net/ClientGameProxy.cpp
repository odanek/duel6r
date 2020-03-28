/*
 * ClientGameProxy.cpp
 *
 *  Created on: Mar 21, 2020
 *      Author: fanick
 */

#include "ClientGameProxy.h"
#include "../Game.h"
#include "../PersonProfile.h"
namespace Duel6 {
    namespace net {

        ClientGameProxy::ClientGameProxy()
        {
            // TODO Auto-generated constructor stub

        }

        ClientGameProxy::~ClientGameProxy()
        {
            // TODO Auto-generated destructor stub
        }
        template<>
        void ClientGameProxy::handleEvent(GameState &e) {
            handle(e);
        }
        template<>
        void ClientGameProxy::handleEvent(NextRound &nr) {
            handle(nr);
        }
        void ClientGameProxy::handle(Player & p) {
            auto & position = p.position;
            game->players[p.id].setPosition(position.x, position.y, position.z);
        }

        void ClientGameProxy::handle(GameState & s) {
            for(auto & player: s.players){
                auto & position = player.position;
                auto & dst = game->players[player.id];
                if(!game->isServer) {
                 dst.setPosition(position.x, position.y, position.z);
                }
                dst.setControllerState(player.controls);
            }
        }

        void ClientGameProxy::handle(NextRound & nr) {
            game->onNextRound();
        }

        //this should only happen on the server side when the client called us with this request
        void ClientGameProxy::handle(Peer & peer, RequestGameState & r) {
            std::vector<Game::PlayerDefinition> playerDefinitions;
            playerDefinitions.reserve(r.connectingPlayers.size());
            for(auto & p: r.connectingPlayers){
                Person & person = persons.emplace_back(p.name, nullptr);
                PlayerSkinColors colors;
                colors.setHair(p.skin.hair);
                colors.setHeadBand(p.skin.headBand);
                int cid = 0;
                for(auto c: p.skin.colors){
                    if(cid > 10){
                        return;
                    }
                    Color color {c.red, c.green, c.blue, c.alpha};
                    colors.set((PlayerSkinColors::BodyPart)cid++, color);
                }
                playerDefinitions.emplace_back(
                    person,
                    colors,
                    PlayerSounds::makeDefault(game->getAppService().getSound()),
                    PlayerControlsManager::getNoop(),
                    p.team);
               // p.clientId

            }
            game->joinPlayers(playerDefinitions);

            sendGameState(peer, *game);
        }

        void ClientGameProxy::handle(ObjectBase &o){
            handleObject(o);
        }
        void ClientGameProxy::handle(EventBase &e){
            handleEvent(e);
        }

        RequestGameState ClientGameProxy::getRequestGameState() {
            RequestGameState r;

            r.connectingPlayers.reserve(game->getPlayers().size());
            for(auto & player : game->getPlayers()){
                if(!player.local){
                    continue;
                }
                RequestGameState::Player cp;
                RequestGameState::Skin & skin = cp.skin;
                cp.clientId = player.getId();
                const Person & person = player.getPerson();
                cp.name = person.getName();
                cp.team = player.getTeam();
                const PlayerSkinColors & colors = player.getSkin().getColors();
                skin.hair = (RequestGameState::Hair) colors.getHair();
                skin.headBand = colors.hasHeadBand();

                size_t i = 0;
                for(auto & c : skin.colors){
                    PlayerSkinColors::BodyPart bp = (PlayerSkinColors::BodyPart) i;
                    c.red = colors.get(bp).getRed();
                    c.green = colors.get(bp).getGreen();
                    c.blue = colors.get(bp).getBlue();
                    c.alpha = colors.get(bp).getAlpha();

                    i++;
                }
                r.connectingPlayers.push_back(cp);
            }
            return r;
        }

        void ClientGameProxy::nextRound() {
            game->onNextRound();
        }
    } /* namespace net */
} /* namespace Duel6 */
