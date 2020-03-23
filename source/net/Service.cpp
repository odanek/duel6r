/*
 * Service.cpp
 *
 *  Created on: Mar 19, 2020
 *      Author: fanick
 */

#include "Service.h"
#include "binarystream/BinaryStream.h"
#include "Message.h"
#include "Object.h"
#include "Peer.h"
namespace Duel6::net {

    void Service::stopNow() {

    }

    void Service::onPeerReceived(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel) {
        {
            binarystream bs(packet->data, packet->dataLength);
            MessageType type;
            if(!(bs >> type)){
                return;
            }

            switch (type) {
            case MessageType::OBJECT: {
                ObjectType type;
                if (!(bs >> type)){
                    return;
                }
                PeerRef * ref = (PeerRef *) peer->data;
                ref->peer->handle(type, bs);
               // onPeerHandle(peer, type, bs);
                break;
            }
            case MessageType::EVENT: {
                EventType type;

                if (!(bs >> type)){
                    return;
                }
                PeerRef * ref = (PeerRef *) peer->data;
                ref->peer->handle(type, bs);
             //   onPeerHandle(peer, type, bs);
                break;
            }
            case MessageType::MAX_COUNT:
                break;
            }
        }

    }
}

