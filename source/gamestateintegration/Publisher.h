/*
 * Publisher.h
 *
 *  Created on: Oct 7, 2020
 *      Author: fanick
 */

#ifndef GAMESTATEINTEGRATION_PUBLISHER_H_
#define GAMESTATEINTEGRATION_PUBLISHER_H_

#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
#include <cstdint>
#include <curl/curl.h>

namespace Duel6 {

    class Publisher {
    private:
        std::string url;
        uint16_t port;
        std::string data = "{}";
        bool running = false;
        bool dataNeeded = false;
        std::thread thread;

        std::mutex dataMutex;
        std::mutex runningMutex;
        std::mutex shutdownMutex;
        std::condition_variable shutdownCV;
    public:

        Publisher(const std::string & url, uint16_t port);

        void publish(const std::string & jsonData);

        void start();

        void run();

        void stop();

        bool wantsNewData();

        virtual ~Publisher();
    };

} /* namespace Duel6 */

#endif /* SOURCE_NET_GAMESTATEINTEGRATION_PUBLISHER_H_ */
