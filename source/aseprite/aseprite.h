/*
 * Aseprite binary loader
 * Version 0.1
 * Copyright 2018 by Frantisek Veverka
 *
 */
/**
 * Based on documentation of the aseprite binary format
 * https://github.com/aseprite/aseprite/blob/af32df1a2aa1b243effc5509f766276164d57c05/docs/ase-file-specs.md
 *  aseprite/docs/ase-file-specs.md
 */

#ifndef ASEPRITE_H
#define ASEPRITE_H
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <memory>
#include <variant>
#include "tinf/tinf.h"

namespace aseprite {

template <typename OUT>
bool operator & (std::ifstream & stream, OUT & out){
    stream.read((char *) &out, sizeof(OUT));
    return stream.good();
}

using BYTE = uint8_t;
using WORD = uint16_t;
using SHORT = int16_t;
using DWORD = uint32_t;
using LONG = int32_t;
using FIXED = int32_t; // convert 16.16 fixed to double: x / 65536

static_assert(sizeof(BYTE) == 1);
static_assert(sizeof(WORD) == 2);
static_assert(sizeof(SHORT) == 2);
static_assert(sizeof(DWORD) == 4);
static_assert(sizeof(LONG) == 4);
static_assert(sizeof(FIXED) == 4);

enum PIXELTYPE {
    RGBA, GRAYSCALE, INDEXED
};

union PIXEL_DATA {
    BYTE RGBA[4];
    BYTE GRAYSCALE[2];
    BYTE INDEXED;
};

struct PIXEL {
    PIXELTYPE TYPE;
    PIXEL_DATA DATA;
};

struct STRING {
    WORD length;
    std::vector<BYTE> data;

    STRING() = default;

    STRING(STRING && s);

    STRING & operator = (const STRING && s);

    bool read(std::ifstream & s);

    std::string toString() const;
};

struct ASE_HEADER {
public:
    DWORD fileSize; // file size
    WORD magicNumber; //    (0xA5E0)
    WORD frames;
    WORD width; //pixels
    WORD height; //pixels
    WORD bitDepth; //8 / 16/ 32
    DWORD flags; // 1 - layer opacity has valid value

    WORD speed; // in milliseconds. deprecated use frame duration
    DWORD stuff_0; // zero
    DWORD stuff_1; // zero
    BYTE transparentIndex; // palette index of transparent color in 8 bit sprites

    BYTE stuff_2[3]; //unused
    WORD colorsCount; // 0 = 256

    BYTE pixelWidth; //if zero aspect is 1:1
    BYTE pixelHeight; //if zero aspect is 1:1
    BYTE reserved[92]; // future

public:

    bool read(std::ifstream & s);

    void toString();
};

enum CHUNK_TYPE {
    PALETTE_OLD_0x0004 = 0x0004,
    PALETTE_OLD_0x0011 = 0x0011,
          LAYER_0x2004 = 0x2004,
            CEL_0x2005 = 0x2005,
      CEL_EXTRA_0x2006 = 0x2006,
           MASK_0x2016 = 0x2016, // 0x2016 DEPRECATED
           PATH_0x2017 = 0x2017, // 0x2017 never used
     FRAME_TAGS_0x2018 = 0x2018,
        PALETTE_0x2019 = 0x2019,
      USER_DATA_0x2020 = 0x2020, //TODO
          SLICE_0x2022 = 0x2022,
};

struct Color {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a = 255;
};

struct PALETTE_OLD_CHUNK {
    std::array<Color, UINT8_MAX + 1> colors;

    PALETTE_OLD_CHUNK() = default;

    PALETTE_OLD_CHUNK(PALETTE_OLD_CHUNK && p);

    PALETTE_OLD_CHUNK(std::ifstream & s);

    PALETTE_OLD_CHUNK & operator = (const PALETTE_OLD_CHUNK && palatte);

    bool read(std::ifstream & s);
};

struct PALETTE_CHUNK {
    std::array<Color, UINT8_MAX + 1> colors;

    PALETTE_CHUNK(PALETTE_CHUNK && palette);

    PALETTE_CHUNK(std::ifstream & s);

    PALETTE_CHUNK & operator = (const PALETTE_CHUNK && palette);

    bool read (std::ifstream & s);
};

struct LAYER_CHUNK {
    WORD flags; // 1 = Visible
                // 2 = Editable
                // 4 = Lock movement
                // 8 = Background
                // 16 = Prefer linked cels
                // 32 = The layer group should be displayed collapsed
                // 64 = The layer is a reference layer
    WORD layerType; //0 - normal, 1 - group layer
    WORD layerChildLevel; // see NOTE.1

    WORD width; // default layer width in pixels . ignored
    WORD height; //ignored

    WORD blendMode; // Normal         = 0
                    // Multiply       = 1
                    // Screen         = 2
                    // Overlay        = 3
                    // Darken         = 4
                    // Lighten        = 5
                    // Color Dodge    = 6
                    // Color Burn     = 7
                    // Hard Light     = 8
                    // Soft Light     = 9
                    // Difference     = 10
                    // Exclusion      = 11
                    // Hue            = 12
                    // Saturation     = 13
                    // Color          = 14
                    // Luminosity     = 15
                    // Addition       = 16
                    // Subtract       = 17
                    // Divide         = 18
    BYTE opacity;
    BYTE unused[3];
    STRING name;

    LAYER_CHUNK(LAYER_CHUNK && layer);

    LAYER_CHUNK(std::ifstream & s);

    LAYER_CHUNK & operator = (const LAYER_CHUNK && layer);

    bool read(std::ifstream & s);
};

struct TAG {
    WORD from;
    WORD to;
    BYTE direction; // 0 - forward, 1 - reverse, 2 - ping-pong
    STRING name; //name of the animation loop (e.g. walk, jump, etc.)

    TAG() = default;

    TAG(TAG && t);

    TAG & operator = (const TAG && t);

    bool read(std::ifstream & s);
};

struct TAG_CHUNK {
    std::vector<TAG> tags;
    TAG_CHUNK(std::ifstream & s);

    TAG_CHUNK(TAG_CHUNK && tag);

    TAG_CHUNK & operator = (TAG_CHUNK && tag);

    bool read (std::ifstream & s);
};

struct SLICE_KEY {
    struct TYPE_NINE_PATCHES { // 9-patches slice - a 3x3 grid
        LONG centerX;
        LONG centerY;
        DWORD centerWidth;
        DWORD centerHeight;
    };

    struct TYPE_PIVOT {
        LONG pivotX;
        LONG pivotY;
    };

    union t_data {
        TYPE_NINE_PATCHES ninePatches;
        TYPE_PIVOT pivot;
    };

    DWORD frame;
    LONG x;
    LONG y;
    DWORD width;
    DWORD height;

    t_data data;
};

struct SLICE_CHUNK {
    std::vector<SLICE_KEY> sliceKeys;
    DWORD count;
    DWORD flags;
    STRING name;

    SLICE_CHUNK(std::ifstream & s);

    bool read(std::ifstream & s);
};

struct CEL_CHUNK {
    WORD layerIndex; // see NOTE.2
    SHORT x;
    SHORT y;
    BYTE opacity;
    WORD type; // 0 - raw cel, 1 - linked cel, 2 - compressed
    std::vector<PIXEL_DATA> pixels;

    WORD width = 0; //type == 0,2
    WORD height = 0; // type == 0,2
    WORD frameLink; // type == 1

    CEL_CHUNK & operator =(const CEL_CHUNK && cel);

    CEL_CHUNK(CEL_CHUNK && cel);

    CEL_CHUNK(std::ifstream & s, PIXELTYPE pixelFormat, DWORD dataSize);

    bool read (std::ifstream & s, PIXELTYPE pixelFormat, DWORD dataSize);

    bool readRawPixels(std::ifstream & s, PIXELTYPE pixelFormat);

    bool readCompressedPixels(std::ifstream & s, PIXELTYPE pixelFormat, DWORD sourceLen);
};

struct CHUNK {
    // all variants must have move constructor, move assignment operator
    // first variant must have default constructor
    using chunk_t = std::variant<
        PALETTE_OLD_CHUNK,
        LAYER_CHUNK ,
        PALETTE_CHUNK,
        CEL_CHUNK,
        TAG_CHUNK,
        SLICE_CHUNK>;

    chunk_t data;
    WORD type;

    CHUNK(chunk_t && data, WORD type);

    CHUNK(CHUNK && c);
};

struct FRAME {
    DWORD size; // bytes
    WORD magicNumber; //0xF1FA
    WORD chunks_old; // if 0xFFFF use chunks field (the new one below};
    WORD duration; // in milliseconds
    BYTE reserved[2]; // 0
    DWORD chunkCount; // if zero, use chunks_old
    std::vector<CHUNK> chunks;

    bool read(std::ifstream & s, PIXELTYPE pixelFormat);
};

struct ASEPRITE {
    ASE_HEADER header;
    std::vector<FRAME> frames;

    ASEPRITE(std::string filename);
private:
    static bool tinf_initialized;
};


/*
 Notes
NOTE.1

The child level is used to show the relationship of this layer with the last one read, for example:

Layer name and hierarchy      Child Level
-----------------------------------------------
- Background                  0
  `- Layer1                   1
- Foreground                  0
  |- My set1                  1
  |  `- Layer2                2
  `- Layer3                   1

NOTE.2

The layer index is a number to identify any layer in the sprite, for example:

Layer name and hierarchy      Layer index
-----------------------------------------------
- Background                  0
  `- Layer1                   1
- Foreground                  2
  |- My set1                  3
  |  `- Layer2                4
  `- Layer3                   5

 */


}

#endif
