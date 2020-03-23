#include "BinaryStream.h"

binarystream::binarystream(const unsigned char * buffer, size_t len) {
    write((const char *) buffer, len);
}

