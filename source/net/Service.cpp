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

    void Service::requestStop() {
        stopRequested = true;
    }
    void Service::flush() {
        enet_host_flush(serviceHost.get());
    }
    void Service::poll(Uint32 timeout) {
        if (!(state == ServiceState::STARTED || state == ServiceState::STARTING || state == ServiceState::STOPPING)) {
            return;
        }
        ENetEvent event;
        ENetHost *netHost = serviceHost.get();
        while (enet_host_service(netHost, &event, timeout) > 0) {
            timeout = 0;
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
        if(stopRequested){
            stop();
        }
    }
    void Service::onPeerReceived(ENetPeer *peer, ENetPacket *packet, enet_uint8 channel) {
        recordPeerNetStats(peer);
        binarystream bs(packet->data, packet->dataLength);
        MessageType type = MessageType::MAX_COUNT;
        if (!(bs >> type)) {
            D6_THROW(Exception, "Service::onPeerReceived");
        }
        switch (type) {
        case MessageType::OBJECT: {
            ObjectType objectType;
            if (!((bs >> objectType) == true)) {
                D6_THROW(Exception, "Cannot deserialize MessageType::OBJECT");
            }
            PeerRef *ref = (PeerRef*) peer->data;
            ref->peer->handle(objectType, bs);
            break;
        }
        case MessageType::EVENT: {
            EventType eventType;
            if (!(bs >> eventType) == true) {
                D6_THROW(Exception, "Cannot deserialize MessageType::EVENT");
            }
            PeerRef *ref = (PeerRef*) peer->data;
            ref->peer->handle(eventType, bs);
            break;
        }
        case MessageType::MAX_COUNT:
            default:
            D6_THROW(Exception, "Unexpected deserialization")
            ;
            break;
        }
    }
}

