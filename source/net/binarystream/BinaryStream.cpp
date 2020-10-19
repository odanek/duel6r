#include "BinaryStream.h"

binarystream::binarystream(const unsigned char * buffer, size_t len) {
    write((const char *) buffer, (std::streamsize) len);
}

binarystream::binarystream(binarystream &&bs)
    : binarystream_base(std::move(bs)) {

}
