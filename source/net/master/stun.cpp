#include <cstring>
#include "stun.h"

namespace stun {

    template<typename T>
    char* write(char *dst, T *src) {
        size_t size = sizeof(T);
        memcpy(dst, src, sizeof(T));
        return dst + size;
    }
    template<typename T>
    char* write(char *dst, T src, bool convertToBigEndian = true) {
        size_t size = sizeof(T);
        if(convertToBigEndian){
            src = toBigEndian(src);
        }
        memcpy(dst, &src, sizeof(T));
        return dst + size;
    }

bool message::read(char * src, size_t len) {
        if(len < headersSize || len > headersSize + 2 * sizeof (addressAttribute)){
            return false;
        }

        enet_uint16 typeN;
        enet_uint16 lengthN;
        enet_uint8 tranId[16];
        attributes.clear();
        attributes.reserve(4);

        size_t read = 0;
        memcpy(&typeN, src, 2);
        read += 2;
        memcpy(&lengthN, src + read, 2);
        read += 2;
        memcpy(&tranId, src + read, 16);
        read += 16;

        type = (type_t)fromBigEndian(typeN);
        length = fromBigEndian(lengthN);

        size_t remaining = len - read;
        size_t attr = read;
        while (remaining >= sizeof(addressAttribute)){
            read = 0;
            enet_uint16 nameN;
            enet_uint16 lengthN;
            enet_uint8 protocolFamilyN;
            enet_uint16 portN;
            enet_uint32 addressN;

            attribute_t name = (attribute_t)fromBigEndian(nameN);
            addressAttribute & tgt = (name == MAPPED_ADDRESS) ? mappedAddress : sourceAdrress;

            memcpy(&nameN, src + attr + read, 2);
            read += 2;
            memcpy(&lengthN, src + attr + read, 2);
            read += 2;
            memcpy(&protocolFamilyN, src + attr + read, 1);
            read += 1;
            memcpy(&portN, src + attr + read, 2);
            read += 2;
            memcpy(&addressN, src + attr + read, 4);
            read += 4;

            attr += read;
            remaining = remaining - read;

            tgt.name = name;
            tgt.length = fromBigEndian(lengthN);
            tgt.protocolFamily = fromBigEndian(protocolFamilyN);
            tgt.port = fromBigEndian(portN);
            tgt.address = fromBigEndian(addressN);

            attributes.push_back(tgt);
        }

        return true;
    }

    size_t message::send(char **memblockPtr) {
        size_t attributesSize = sizeof(addressAttribute) * attributes.size();
        size_t changeAttributesSize = sizeof(changeAttribute) * changeAttributes.size();
        size_t result = headersSize + attributesSize + changeAttributesSize;
        char *dst = (char*) malloc(result);
        (*memblockPtr) = dst;
        memset(dst, 0, result);

        length = attributesSize + changeAttributesSize;

        dst = write(dst, (enet_uint16)type);
        dst = write(dst, length);
        dst = write(dst, &tranId);

        for (const auto & a : attributes){
            dst = write(dst, (enet_uint16)a.name);
            dst = write(dst, a.length);
            dst = write(dst, a.protocolFamily);
            dst = write(dst, a.port);
            dst = write(dst, a.address, false);
        }
        for(const auto & a: changeAttributes){
            dst = write(dst, (enet_uint16)a.name);
            dst = write(dst, a.length);
            dst = write(dst, a.flags);
        }

        return result;
    }

}