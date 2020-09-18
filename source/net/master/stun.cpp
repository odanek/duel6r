#include <cstring>
#include "stun.h"

namespace stun {
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
        size_t result = headersSize + attributesSize;
        char *dst = (char*) malloc(result);
        (*memblockPtr) = dst;
        memset(dst, 0, result);
        length = attributesSize;
        enet_uint16 typeN = toBigEndian((enet_uint16)type);
        enet_uint16 lengthN = toBigEndian(length);
        memcpy(dst, &typeN, 2);
        memcpy(dst + 2, &lengthN, 2);
        memcpy(dst + 4, &tranId, 16);

        addressAttribute *attributesPtr = (addressAttribute *) (dst + ((char*) &attributes - (char*) this));
        for (size_t i = 0; i < attributes.size(); i++) {
            addressAttribute & a = attributes[i];
            addressAttribute * adst = &(attributesPtr[i]);
            enet_uint16 nameN = toBigEndian((enet_uint16)a.name);
            enet_uint16 lengthN = toBigEndian(a.length);
            enet_uint16 protocolFamilyN = toBigEndian(a.protocolFamily);
            enet_uint16 portN = toBigEndian(a.port);
            enet_uint32 addressN = a.address;

            memcpy((char*)adst + 0, &nameN, 2);
            memcpy((char*)adst + 2, &lengthN, 2);
            memcpy((char*)adst + 4, &protocolFamilyN, 2);
            memcpy((char*)adst + 6, &portN, 2);
            memcpy((char*)adst + 8, &addressN, 4);
        }
        return result;
    }
}