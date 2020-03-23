#ifndef BINARYSTREAMS_FILEBINARYSTREAM_H
#define BINARYSTREAMS_FILEBINARYSTREAM_H

#include <fstream>
#include "BinaryStreamBase.h"


class fbinarystream : public binarystream_base<std::fstream> {
public:
    using binarystream_base::binarystream_base;
};
#endif
