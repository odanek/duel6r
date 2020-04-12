/*
 * Net.cpp
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#include "Net.h"
#include <enet/enet.h>
#include <iostream> //TODO odjebnout

namespace Duel6 {
    namespace net {

        Net::Net()
        {
            // TODO Auto-generated constructor stub

        }
        void Net::deinitialize(){
            if(Net::initialized){
                enet_deinitialize();
            }
            Net::initialized = false;
        }
        bool Net::initialize(){
            if(Net::initialized){
                return true;
            }
            if (enet_initialize() != 0)
            {
                std::cerr << "An error occurred while initializing ENet.\n";
                return false;
            }
            Net::initialized = true;
            return true;
        }

        Net::~Net()
        {
            deinitialize();
            // TODO Auto-generated destructor stub
        }
        bool Net::initialized = false;
    } /* namespace net */
} /* namespace Duel6 */

