
#include "MasterServer.h"

namespace masterserver {

    void MasterServer::requestServerList(ENetHost *host, serverListReceivedCallback_t callback){
        connect(host, REQUEST_TYPE::CLIENT_REQUEST_SERVERLIST);
        onConnectCallback = [](){

        };
        onServerListReceivedCallback = [=, this](serverlist_t & serverlist){
            callback(serverlist);
            disconnect();
            onServerListReceivedCallback = [](serverlist_t &){};
        };
    }
}