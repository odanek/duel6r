/*
 * Publisher.cpp
 *
 *  Created on: Oct 7, 2020
 *      Author: fanick
 */
#include <iostream>
#include <thread>
#include <curl/curl.h>
#include <curl/easy.h>
#include <chrono>

#include "Publisher.h"

namespace Duel6 {
    using namespace std::literals::chrono_literals;

    void Publisher::publish(const std::string &jsonData) {

        std::lock_guard<std::mutex> guard(dataMutex);
        data = jsonData;
        dataNeeded = false;
    }

    Publisher::Publisher(const std::string &url, uint16_t port)
        : url(url), port(port) {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    void Publisher::start() {
        running = true;
        thread = std::thread(&Publisher::run, this);
    }

    void Publisher::run() {

        std::string dataToSend;
        CURLcode result;
        CURL *curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Publisher::run Cannot initialize libcurl.\n";
            dataNeeded = false;
            return;
        }
        struct curl_slist *hs = NULL;
        hs = curl_slist_append(hs, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_PORT, port);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
        CURLcode lastResult;
        while (true) {
            {
                std::lock_guard<std::mutex> guard(dataMutex);
                dataToSend = data;
                dataNeeded = true;
            }
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataToSend.c_str());
            result = curl_easy_perform(curl);

            /* Check for errors */
            if (result != CURLE_OK) {
                if(result != lastResult){
                    /* if errors have occured, tell us wath's wrong with 'result'*/
                    fprintf(stderr, "Publisher::run(): url %s port %u \n", url.c_str(), port);
                    fprintf(stderr, "Publisher::run(): curl_easy_perform() failed: %s\n", curl_easy_strerror(result));

                }
                std::lock_guard<std::mutex> guard(dataMutex);
                dataNeeded = false;
            }
            lastResult = result;
            {
                std::lock_guard<std::mutex> guard(runningMutex);
                if (!running) {
                    break;
                }
            }
            std::this_thread::sleep_for(100ms);
        }
        curl_slist_free_all(hs);
        curl_easy_cleanup(curl);
    }

    void Publisher::stop() {
        {
            std::lock_guard<std::mutex> guard(runningMutex);
            if(!running){
                return;
            }
            running = false;
        }
        shutdownCV.notify_all();
        thread.join();
    }

    Publisher::~Publisher() {
        curl_global_cleanup();
        stop();
    }

    bool Publisher::wantsNewData(){
        if(!running){
            return false;
        }
        std::lock_guard<std::mutex> guard(dataMutex);
        return dataNeeded;
    }

} /* namespace Duel6 */
