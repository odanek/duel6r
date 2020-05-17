/*
 * NetClient.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include "Peer.h"
#include "NetClient.h"
#include "../Game.h"
namespace Duel6 {
    namespace net {

        NetClient::NetClient(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy, Console &console)
            : Service(clientGameProxy, serverGameProxy), console(console) {
        }

        NetClient::~NetClient() {
            stop();
        }

        void NetClient::connect(Game &game, const std::string &host,
                                const Duel6::net::port_t port) {
            if (state != ServiceState::UNINITIALIZED) {
                D6_THROW(Exception, "connecting client that is not uninitialized");
            }
            setGameReference(game);
            this->host = host;
            this->port = port;
            ENetHost *nethost = enet_host_create(nullptr,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
            if (nethost == nullptr) {
                D6_THROW(Exception, "Failed to start net client  \(^^)/");
            }

            start(nethost);
        }

        void NetClient::onStarting() {
            ENetAddress address;
            enet_address_set_host(&address, host.c_str());
            address.port = port;
            peer = std::make_unique<Peer>(*clientGameProxy,
                *serverGameProxy,
                enet_host_connect(serviceHost.get(), &address, 2, 42),
                serviceHost.get());
        }

        void NetClient::onStopping() {
            if (peer.get() == nullptr) {
                return;
            }
            peer->disconnect();
        }

        void NetClient::onStopped() {
            clientGameProxy->netStopped();
            peer.reset();
        }

        void NetClient::onPeerConnected(ENetPeer *me) {
            if (!peer->onConnected(me)) {
                return;
            }
            started();
        }

        void NetClient::onPeerDisconnected(ENetPeer *me, enet_uint32 reason) {
            if (!peer->onDisconnected(me, reason)) {
                return;
            }

            stopped();
        }

        void NetClient::onConnectionLost() {
            console.printLine("NetClient::onConnectionLost");
        }
        void NetClient::recordPeerNetStats(ENetPeer *me) {
            if(game == nullptr){
                return;
            }
            auto & netstat = game->netstat;
            const auto & enetPeer = peer->getEnetPeer();
            netstat.inBandwidth = enetPeer.incomingBandwidth;
            netstat.outBandwidth = enetPeer.outgoingBandwidth;
            netstat.inThrottleEpoch = enetPeer.incomingBandwidthThrottleEpoch;
            netstat.outThrottleEpoch = enetPeer.outgoingBandwidthThrottleEpoch;
            netstat.inDataTotal = enetPeer.incomingDataTotal;
            netstat.outDataTotal = enetPeer.outgoingDataTotal;
            netstat.packetLoss = enetPeer.packetLoss;
            netstat.packetLossVariance = enetPeer.packetLossVariance;
            netstat.packetThrottle = enetPeer.packetThrottle;
            netstat.packetThrottleLimit = enetPeer.packetThrottleLimit;
            netstat.packetThrottleCounter = enetPeer.packetThrottleCounter;
            netstat.packetThrottleInterval = enetPeer.packetThrottleInterval;
            netstat.lastRTT = enetPeer.lastRoundTripTime;
            netstat.lowestRTT = enetPeer.lowestRoundTripTime;
            netstat.lastRTTVariance = enetPeer.lastRoundTripTimeVariance;
            netstat.highestRTTVariance = enetPeer.highestRoundTripTimeVariance;
            netstat.rtt = enetPeer.roundTripTime;
            netstat.rttVariance = enetPeer.roundTripTimeVariance;
            netstat.mtu = enetPeer.mtu;
            netstat.windowSize = enetPeer.windowSize;
            netstat.totalWaitingData = enetPeer.totalWaitingData;

        }
    } /* namespace net */
} /* namespace Duel6 */
