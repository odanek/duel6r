/*
 * NetClient.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include "Peer.h"
#include "NetClient.h"
#include "../Game.h"
#include "../Format.h"
#include "master/protocol.h"
#define CHANNELS 254 //todo extract for usage in NetHost
namespace Duel6 {
    namespace net {

        NetClient::NetClient(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy, Duel6::Console &console)
            : Service(clientGameProxy, serverGameProxy), console(console) {
        }

        NetClient::~NetClient() {
            stop();
        }

        void NetClient::initNetHost() {
            ENetAddress localAddress;
            localAddress.port = ENET_PORT_ANY;

            enet_address_set_host(&localAddress, this->localIPAddress.c_str());
            nethost = enet_host_create(&localAddress,
                4 /* only allow 4 outgoing connections */,
                CHANNELS /* allow up 254 channels to be used */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
            if (nethost == nullptr) {
                D6_THROW(Exception, "Failed to start net client  \\(^^)/ ");
            }

            // To make connection to master server work
            serviceHost.reset(nethost);
            state = ServiceState::STARTING;
        }

        void NetClient::requestServerList(masterserver::serverListReceivedCallback_t callback) {
            masterServerProxy.requestServerList(serviceHost.get(), callback);
        }

        void NetClient::requestNATPunch(const enet_uint32 address, const enet_uint16 port,
                                        const enet_uint32 publicIPAddress,
                                        const enet_uint16 publicPort,
                                        const enet_uint32 networkLocalAddress,
                                        const enet_uint16 networklocalPort
                                        ) {
            auto localAddress = nethost->address;
            masterServerProxy.connectNatToServer(serviceHost.get(), address, port, localAddress.host, localAddress.port);
            NATTraversal(serviceHost.get()->socket, address, port);
            NATTraversal(serviceHost.get()->socket, networkLocalAddress, networklocalPort);
            if (publicIPAddress != 0 && publicPort != 0) {
                NATTraversal(serviceHost.get()->socket, publicIPAddress, publicPort);
            }
        }

        void NetClient::connect(Game &game, const std::string &host,
                                const Duel6::net::port_t port) {
            setGameReference(game);
            this->host = host;
            this->port = port;

            start(nethost);
        }

        void NetClient::onStarting() {
            ENetAddress address;
            int res = enet_address_set_host(&address, host.c_str());
            if (res < 0) {
                return;
            }
            address.port = port;
            ENetPeer *enetpeer = enet_host_connect(serviceHost.get(), &address, CHANNELS, static_cast<unsigned int>(REQUEST_TYPE::GAME_CONNECTION));
            if (enetpeer == nullptr) {
                return;
            }
            pendingConnectionRequests++;
            // 5 sec initial timeout when connecting from menu
            enet_peer_timeout(enetpeer, 100, 1000, 5000);
            enetpeer->data = new net::PeerRef { 1234, nullptr, false }; // this is probably superfluous (we check for nullptr in onPeerConnected
        }

        void NetClient::onStopping() {
            if (peer.get() != nullptr) {
                peer->disconnect();
            }
            stopped();
        }

        void NetClient::onStopped() {
            clientGameProxy->netStopped();
            pendingConnectionRequests = 0;
            peer.reset();
        }

        void NetClient::onPeerConnected(ENetPeer *me) {
            console.printLine(Format("NetHost::onPeerConnected address: {0}") << hostToIPaddress(me->address.host, me->address.port));
            if (peer) {
                return;
            }
            peer = std::make_unique<Peer>(*clientGameProxy,
                *serverGameProxy,
                me,
                serviceHost.get());

            if (!peer->onConnected(me)) {
                return;
            }

            started();
        }

        void NetClient::onPeerDisconnected(ENetPeer *me, enet_uint32 reason) {
            console.printLine(Format("NetHost::onPeerDisconnected address: {0}") << hostToIPaddress(me->address.host, me->address.port));
            if (peer) {
                if (peer->onDisconnected(me, reason)) {
                    requestStop();
                    return;
                }
            } else {
                pendingConnectionRequests--;
                if (pendingConnectionRequests <= 0) {
                    requestStop();
                }
            }
        }

        void NetClient::onTearDown() {
            tearDown();

            // start up right away to be able to connect to the master server
            initNetHost();
        }

        void NetClient::onConnectionLost() {
            console.printLine("NetClient::onConnectionLost");
        }

        void NetClient::recordPeerNetStats(ENetPeer *me) {
            if (game == nullptr) {
                return;
            }
            auto &netstat = game->netstat;
            const auto &enetPeer = peer->getEnetPeer();
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

        void NetClient::NATTraversal(ENetSocket s, const enet_uint32 address, const enet_uint16 port) {
            ENetAddress addressForNATPunch;
            addressForNATPunch.host = address;
            addressForNATPunch.port = port;
            ENetBuffer buffer;
            enet_uint32 stuff = 42;
            buffer.data = &stuff;
            buffer.dataLength = 4;
            // fingers crossed
#ifndef IP_TTL
    #define IP_TTL 7
#endif
            char ttl = 0;
            for (; ttl < 10; ttl++) { //TODO This is probably unnecessary
                setsockopt(s, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
                enet_socket_send(s, &addressForNATPunch, &buffer, 0);
            }
            ttl = 64;
            setsockopt(s, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
            enet_socket_send(s, &addressForNATPunch, &buffer, 0);
        }
    } /* namespace net */
} /* namespace Duel6 */
