/*
 * NetClient.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */
#include "Peer.h"
#include "NetClient.h"
#include "../Game.h"
#include "master/protocol.h"
#define CHANNELS 254 //todo extract for usage in NetHost
namespace Duel6 {
    namespace net {

        NetClient::NetClient(ClientGameProxy &clientGameProxy, ServerGameProxy &serverGameProxy, Console &console)
            : Service(clientGameProxy, serverGameProxy), console(console) {
            initNetHost();
        }

        NetClient::~NetClient() {
            stop();
        }

        void NetClient::initNetHost() {
            nethost = enet_host_create(nullptr,
                          4 /* only allow 4 outgoing connections */,
                          CHANNELS /* allow up 254 channels to be used */,
                          0 /* assume any amount of incoming bandwidth */,
                          0 /* assume any amount of outgoing bandwidth */);
                      if (nethost == nullptr) {
                          D6_THROW(Exception, "Failed to start net client  \(^^)/");
                      }


                      // To make connection to master server work
                      serviceHost.reset(nethost);
                      state = ServiceState::STARTING;
        }
        void NetClient::requestServerList(masterserver::serverListReceivedCallback_t callback){
            masterServerProxy.requestServerList(serviceHost.get(), callback);
        }

        void NetClient::requestNATPunch(const enet_uint32 host, const enet_uint16 port) {
            masterServerProxy.connectNatToServer(serviceHost.get(), host, port);
        }

        void NetClient::connect(Game &game, const std::string &host,
                                const Duel6::net::port_t port) {
//            if (state != ServiceState::UNINITIALIZED) {
//                D6_THROW(Exception, "connecting client that is not uninitialized");
//            }
            setGameReference(game);
            this->host = host;
            this->port = port;
            start(nethost);
        }

        void NetClient::onStarting() {
            ENetAddress address;
            int res =  enet_address_set_host(&address, host.c_str());
            if(res<0){
                return ;
            }
            address.port = port;
            ENetPeer * enetpeer = enet_host_connect(serviceHost.get(), &address, CHANNELS, static_cast<unsigned int>(REQUEST_TYPE::GAME_CONNECTION));
            if(enetpeer == nullptr){
                return;
            }
            peer = std::make_unique<Peer>(*clientGameProxy,
                *serverGameProxy,
                enetpeer,
                serviceHost.get());
        }

        void NetClient::onStopping() {
            if (peer.get() == nullptr) {
                return;
            }
            peer->disconnect();
            stopped();
        }

        void NetClient::onStopped() {
            //state = ServiceState::STOPPED;
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

            requestStop();
        }

        void NetClient::onTearDown() {
            tearDown();
            initNetHost();
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
