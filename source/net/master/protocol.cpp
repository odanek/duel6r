#include <sstream>
#include "protocol.h"
std::string hostToIPaddress(address_t a, port_t p){
    std::ostringstream is;
    hostAddress hostAddress;
    hostAddress.address = a;

    is << (uint16_t) hostAddress.a[0];
    is << ".";
    is << (uint16_t) hostAddress.a[1];
    is << ".";
    is << (uint16_t) hostAddress.a[2];
    is << ".";
    is << (uint16_t) hostAddress.a[3];
    is << ":";
    is << p;
    return is.str();
}