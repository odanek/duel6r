#ifndef BINARYSTREAMS_BINARYSTREAM_H
#define BINARYSTREAMS_BINARYSTREAM_H
#include <sstream>
#include "BinaryStreamBase.h"

class binarystream: public binarystream_base<std::stringstream> {
public:
    using binarystream_base::binarystream_base;
    binarystream(const unsigned char * buffer, size_t len);
};

#endif
