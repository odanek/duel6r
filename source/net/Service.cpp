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

    void Service::poll(Uint32 timeout) {
        if (state == ServiceState::UNINITIALIZED || state == ServiceState::STOPPED) {
            return;
        }
        ENetEvent event;
        ENetHost *netHost = serviceHost.get();
        while (enet_host_service(netHost, &event, timeout) > 0) {
            switch (event.type) {
            case ENetEventType::ENET_EVENT_TYPE_NONE:
                break;
            case ENetEventType::ENET_EVENT_TYPE_CONNECT:
                onPeerConnected(event.peer);
                break;
            case ENetEventType::ENET_EVENT_TYPE_DISCONNECT:
                onPeerDisconnected(event.peer, event.data);
                break;
            case ENetEventType::ENET_EVENT_TYPE_RECEIVE:
                onPeerReceived(event.peer, event.packet, event.channelID);
                enet_packet_destroy(event.packet);
                break;
            }
            if (state == ServiceState::UNINITIALIZED) {
                break;
            }
        }
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

