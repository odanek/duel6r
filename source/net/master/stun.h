/*
 * stun.h
 *
 *  Created on: Sep 18, 2020
 *      Author: fanick
 */

#ifndef INCLUDE_STUN_H_
#define INCLUDE_STUN_H_


#include <sstream>
#include <vector>
#include <enet/enet.h>

namespace stun {

    template <typename T>
    T toBigEndian(T u){
        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source;
        source.u = u;

        T result = 0;

        for (size_t k = 0; k < sizeof(T); k++)
            result |= (source.u8[sizeof(T) - k - 1] << (k * 8));

        return result;
    }
    template <typename T>
    T fromBigEndian(T u){
        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source;
        source.u = u;

        T result = 0;

        for (size_t k = 0; k < sizeof(T); k++)
            result |= (source.u8[k] << (k * 8));

        return result;
    }


    #pragma pack(push, 1)
    enum type_t: enet_uint16 {
        BINDING_REQUEST = 0x0001,
        BINDING_RESPONSE = 0x0101
    };

    enum attribute_t: enet_uint16{
        MAPPED_ADDRESS = 0x0001,
        CHANGE_REQUEST = 0x0003,
        SOURCE_ADDRESS = 0x0004
    };

    enum flags_t: enet_uint32 {
        CHANGE_PORT = 0x02,
        CHANGE_ADDRESS = 0x04
    };

    struct changeAttribute {
        attribute_t name = CHANGE_REQUEST;
        enet_uint16 length = 0x04;
        enet_uint32 flags = 0;
    };
    struct addressAttribute {
        attribute_t name;
        enet_uint16 length = 0x08;
        enet_uint16 protocolFamily = 0x01; // IPv4
        enet_uint16 port;
        enet_uint32 address;
    };

    struct message {
        type_t type;
        enet_uint16 length;
        enet_uint8 tranId[16] = {1,2,3,4,5,6,7,8, 9,10,11,12,13,14,15,16};
        std::vector<addressAttribute> attributes;
        std::vector<changeAttribute> changeAttributes;
        addressAttribute mappedAddress;
        addressAttribute sourceAdrress;

        bool read(char * src, size_t len);
        size_t send(char ** dst);

        static constexpr size_t headersSize = sizeof(type) + sizeof(length) + sizeof(tranId);
    };
    #pragma pack(pop)
}

#endif /* INCLUDE_STUN_H_ */
