/*
 * Net.h
 *
 *  Created on: Mar 9, 2020
 *      Author: fanick
 */

#ifndef SOURCE_NET_NET_H_
#define SOURCE_NET_NET_H_

#include "Type.h"

namespace Duel6 {
    namespace net {

        class Net {
        private:
            static bool initialized;
        public:
            Net();
            bool initialize();
            virtual ~Net();
        private:
            void deinitialize();
        };

    } /* namespace net */
} /* namespace Duel6 */

#endif /* SOURCE_NET_NET_H_ */
