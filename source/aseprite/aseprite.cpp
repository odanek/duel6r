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

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <memory>
#include <variant>
#include "tinf/tinf.h"
#include "aseprite.h"

namespace aseprite {

bool operator &(std::ifstream & s, STRING & string) {
	return string.read(s);
}

bool operator & (std::ifstream & s, aseprite::ASE_HEADER & header){
	return header.read(s);
}

STRING::STRING(STRING && s): length(s.length), data(std::move(s.data)){

}

STRING & STRING::operator = (const STRING && s){
	data = std::move(s.data);
	return *this;
}

bool STRING::read(std::ifstream & s) {
	bool result = s & length;
	if (!result) {
		return result;
	}
	data.resize(length);
	for (size_t i = 0; i < length && result; i++) {
		result = result && s & data[i];
	}
	return result;
}

std::string STRING::toString() const {
	return std::string(reinterpret_cast<const char *>(&data[0]), data.size());
}

bool ASE_HEADER::read(std::ifstream & s){ //order must match order of member variables
	return s & fileSize
		&& s & magicNumber
		&& s & frames
		&& s & width
		&& s & height
		&& s & bitDepth
		&& s & flags
		&& s & speed
		&& s & stuff_0
		&& s & stuff_1
		&& s & transparentIndex
		&& s & stuff_2
		&& s & colorsCount
		&& s & pixelWidth
		&& s & pixelHeight
		&& s & reserved;
}

void ASE_HEADER::toString(){
	std::cout << "HEADER\n"
	   "fileSize           :"      << fileSize                << "\n"
	   "magicNumber        :"      << std::hex << magicNumber << std::dec  << "\n"
	   "frames             :"      << frames                  << "\n"
	   "width              :"      << width                   << "\n"
	   "height             :"      << height                  << "\n"
	   "bitDepth           :"      << bitDepth                << "\n"
	   "flags              :"      << flags                   << "\n"
	   "speed              :"      << speed                   << "\n"
	   "stuff_0            :"      << stuff_0                 << "\n"
	   "stuff_1            :"      << stuff_1                 << "\n"
	   "transparentIndex   :"      << (int)transparentIndex   << "\n"
	   "stuff_2            :(len)" << sizeof(stuff_2)         << "\n"
	   "colorsCount        :"      << colorsCount             << "\n"
	   "pixelWidth         :"      << (int)pixelWidth         << "\n"
	   "pixelHeight        :"      << (int)pixelHeight        << "\n"
	   "reserved           :(len)" << sizeof(reserved)        << "\n";
}

PALETTE_OLD_CHUNK::PALETTE_OLD_CHUNK(PALETTE_OLD_CHUNK && p) :
		colors(std::move(p.colors)) {

}

PALETTE_OLD_CHUNK::PALETTE_OLD_CHUNK(std::ifstream & s){
	read(s);
}

PALETTE_OLD_CHUNK & PALETTE_OLD_CHUNK::operator = (const PALETTE_OLD_CHUNK && palatte) {
	colors = std::move(palatte.colors);
	return *this;
}


bool PALETTE_OLD_CHUNK::read(std::ifstream & s){
	WORD packets;
	bool result = s & packets;
	WORD lastIndex = 0;

	if (result) {
		for (WORD i = 0; i < packets; i++) {
			BYTE skip;
			BYTE colorsCount = 0;
			result = result
					&& s & skip
					&& s & colorsCount;
			WORD colorNum = colorsCount == 0 ? 256 : colorsCount;
			if (!result) return result;
			lastIndex += skip;
			for (WORD c = 0; c < colorNum; c++) {
				if (lastIndex > 255) return false;
				Color & color = colors[lastIndex++];
				result = result
						&& s & color.r
						&& s & color.g
						&& s & color.b;

			}
		}
	}
	return result;
}


PALETTE_CHUNK::PALETTE_CHUNK(PALETTE_CHUNK && palette):colors(std::move(palette.colors)){

}
PALETTE_CHUNK::PALETTE_CHUNK(std::ifstream & s){
	read(s);
}

PALETTE_CHUNK & PALETTE_CHUNK::operator = (const PALETTE_CHUNK && palette) {
	colors = std::move(palette.colors);
	return *this;
}

bool PALETTE_CHUNK::read (std::ifstream & s){
	DWORD newSize; // total number of entries
	DWORD first;
	DWORD last;
	BYTE unused[8];

	bool result = s & newSize
			&& s & first
			&& s & last
			&& s & unused;
	if(!result) return result;
	for (DWORD i = first; i <= last && result; i++) {
		if(i > 255) return false; // we don't support such fancy graphics
		Color & c = colors[i];
		WORD flags;
		STRING foobar;
		result = result
				&& s & flags
				&& s & c.r
				&& s & c.g
				&& s & c.b
				&& s & c.a;
		if(result && flags & 0x1){
			result = result && s & foobar; // throw away the color name
		}
	}
	return result;
}




LAYER_CHUNK::LAYER_CHUNK(LAYER_CHUNK && layer) noexcept {
	flags = layer.flags;
	layerType = layer.layerType;
	layerChildLevel = layer.layerChildLevel;
	width = layer.width;
	height = layer.height;
	blendMode = layer.blendMode;
	opacity = layer.opacity;
	name = std::move(layer.name);
}
LAYER_CHUNK::LAYER_CHUNK(std::ifstream & s){
	read(s);
}
LAYER_CHUNK & LAYER_CHUNK::operator = (const LAYER_CHUNK && layer) {
	flags = layer.flags;
	layerType = layer.layerType;
	layerChildLevel = layer.layerChildLevel;
	width = layer.width;
	height = layer.height;
	blendMode = layer.blendMode;
	opacity = layer.opacity;
	name = std::move(layer.name);
	return *this;
}

bool LAYER_CHUNK::read(std::ifstream & s){
	return s & flags
			&& s & layerType
			&& s & layerChildLevel
			&& s & width
			&& s & height
			&& s & blendMode
			&& s & opacity
			&& s & unused
			&& s & name;
}


TAG::TAG(TAG && t):from(t.from),
		to(t.to),
		direction(t.direction),
		name(std::move(t.name)){

}
TAG & TAG::operator = (const TAG && t){
	from = t.from;
	to = t.to;
	direction = t.direction;
	name = std::move(t.name);
	return *this;
}

bool TAG::read(std::ifstream & s) {
	BYTE unused[8];
	BYTE color[3]; // unused, color of the tag
	BYTE extra; //ignored, 0
	return s & from
			&& s & to
			&& s & direction
			&& s & unused
			&& s & color
			&& s & extra
			&& s & name;
}

TAG_CHUNK::TAG_CHUNK(std::ifstream & s){
	read(s);
}

TAG_CHUNK::TAG_CHUNK(TAG_CHUNK && tag):tags(std::move(tag.tags)){}
TAG_CHUNK & TAG_CHUNK::operator = (TAG_CHUNK && tag){
	tags = std::move(tag.tags);
	return *this;
}
bool TAG_CHUNK::read (std::ifstream & s) {
	WORD count;
	BYTE future[8]; // unused
	bool result = s & count
			&& s & future;
	tags.reserve(count);
	for (WORD i = 0; i < count && result; ++ i){
		TAG t;
		result = result && t.read(s);
		if(result) tags.push_back(std::move(t));
	}

	return result;
}


SLICE_CHUNK::SLICE_CHUNK(std::ifstream & s){
	read(s);
}
bool SLICE_CHUNK::read(std::ifstream & s){
	DWORD reserved;
	bool result = s & count
			&& s & flags
			&& s & reserved
			&& s & name;
	if(!result) return result;
	sliceKeys.resize(count);
	for(DWORD i = 0; i < count && result; i++){
		auto & k = sliceKeys[i];
		result = result
				&& s & k.frame
				&& s & k.x
				&& s & k.y
				&& s & k.width
				&& s & k.height;
		switch(flags){
		case 1:{
			auto & n = k.data.ninePatches;
			result = result
					&& s & n.centerX
					&& s & n.centerY
					&& s & n.centerWidth
					&& s & n.centerHeight;
			break;
		}
		case 2:{
			auto & pivot = k.data.pivot;
			result = result
					&& s & pivot.pivotX
					&& s & pivot.pivotY;
			break;}
		case 0:{
			break;
		}
		}
	}
	return result;
}

CEL_CHUNK & CEL_CHUNK::operator = (const CEL_CHUNK && cel){
	layerIndex = cel.layerIndex;
	x = cel.x;
	y = cel.y;
	opacity = cel.opacity;
	type = cel.type;
	pixels = std::move(cel.pixels);
	width = cel.width;
	height = cel.height;
	frameLink = cel.frameLink;

	return *this;
}

CEL_CHUNK::CEL_CHUNK(CEL_CHUNK && cel){
	layerIndex = cel.layerIndex;
	x = cel.x;
	y = cel.y;
	opacity = cel.opacity;
	type = cel.type;
	pixels = std::move(cel.pixels);
	width = cel.width;
	height = cel.height;
	frameLink = cel.frameLink;
}

CEL_CHUNK::CEL_CHUNK(std::ifstream & s, PIXELTYPE pixelFormat, DWORD dataSize){
	read(s, pixelFormat, dataSize);
}

CEL_CHUNK::~CEL_CHUNK(){
	pixels.clear(); //TODO remove
}
// chunkSize - to tell size of compressed data
bool CEL_CHUNK::read (std::ifstream & s, PIXELTYPE pixelFormat, DWORD dataSize){
	BYTE reserved[7];
	bool result = s & layerIndex
			&& s & x
			&& s & y
			&& s & opacity
			&& s & type
			&& s & reserved;
	if(!result) return result;
	switch (type){
	case 0:{
		result = readRawPixels(s, pixelFormat);
		break;
	}
	case 1:{
		result = s & frameLink;
		break;
	}
	case 2: {
		result = readCompressedPixels(s, pixelFormat, dataSize - 16 /*size of already read data*/);
		break;
	}
	default: result = false;
	}

	return result;
}

bool  CEL_CHUNK::readRawPixels(std::ifstream & s, PIXELTYPE pixelFormat){
	bool result = s & width && s & height;
	if(!result) return result;
	DWORD dim = width * height;
	pixels.resize(dim);

	switch(pixelFormat){
		case INDEXED:{
			for(DWORD i = 0; (i < dim) && result; i++) {
				result = result && s & pixels[i].INDEXED;
			}
			break;
		}
		case GRAYSCALE:{
			for(DWORD i = 0; (i < dim) && result; i++) {
				result = result && s & pixels[i].GRAYSCALE;
			}
			break;
		}
		case RGBA:{
			for(DWORD i = 0; i < dim && result; i++) {
				result = result && s & pixels[i].RGBA;
			}
			break;
		}
		default:
			result = false;
	}

	return result;
}
bool  CEL_CHUNK::readCompressedPixels(std::ifstream & s, PIXELTYPE pixelFormat, DWORD sourceLen){
	bool result = s & width && s & height;
	if(!result) return result;
	DWORD dim = width * height;
	pixels.resize(dim);
	sourceLen -= 4; /* width, height */
	std::vector<BYTE> source(sourceLen);
	s.read((char*)source.data() /*zlib header*/, sourceLen);

	result = s.good();
	if(!result){
		return result;
	}


	DWORD expectedLen = dim;
	switch (pixelFormat) {
		case INDEXED: {
			break;
		}
		case GRAYSCALE: {
			expectedLen *= 2;
			break;
		}
		case RGBA: {
			expectedLen *= 4;
			break;
		}
		default:
			result = false;
	}
	std::vector<BYTE> uncompressed(expectedLen);
	DWORD destLen;
	auto outcome = tinf_uncompress(uncompressed.data(), &destLen, source.data() + 2, sourceLen -2 - 4/*zlib header, crc*/);
	result = result && TINF_OK == outcome;

	result = result && destLen == expectedLen;

	if(!result) return result;
	switch (pixelFormat) {
			   case INDEXED: {
				   for(DWORD i = 0 ; i < dim; i++) {
					   pixels[i].INDEXED = uncompressed[i];
				   }
				   break;
			   }
			   case GRAYSCALE: {
				   for(DWORD i = 0 ; i < dim; i++) {
					   pixels[i].GRAYSCALE[0] = uncompressed[2 * i];
					   pixels[i].GRAYSCALE[1] = uncompressed[2 * i + 1];
				   }
				   break;
			   }
			   case RGBA: {
				   for(DWORD i = 0 ; i < dim; i++) {
					   pixels[i].RGBA[0] = uncompressed[4 * i];
					   pixels[i].RGBA[1] = uncompressed[4 * i + 1];
					   pixels[i].RGBA[2] = uncompressed[4 * i + 2];
					   pixels[i].RGBA[3] = uncompressed[4 * i + 3];
				   }
				   break;
			   }
	}
	return result;
}


CHUNK::CHUNK(chunk_t && data, WORD type):data(std::move(data)), type(type){

}

CHUNK::CHUNK(CHUNK && c) {
	std::cout << "CHUNK(CHUNK && ) called :( \n";
	data = std::move(c.data);
	type = c.type;
	c.type = 0;
}

CHUNK::~CHUNK() {
	type = 0; //TODO remove
}

bool FRAME::read(std::ifstream & s, PIXELTYPE pixelFormat) {
	bool result = s & size
			&& s & magicNumber
			&& s & chunks_old
			&& s & duration
			&& s & reserved
			&& s & chunkCount;

	if(result) {
		std::cout <<std::hex<< "DEBUG Frame: magicNumber: " << magicNumber << " chunks_old: " << chunks_old << std::dec << "\n";
		chunks.reserve(chunkCount);
		for(size_t c = 0; c < chunkCount && result; c++){
			DWORD size;
			WORD type;
			auto p = s.tellg();
			result = result && s & size && s & type;
			auto p2 = s.tellg();
			std::cout <<std::hex<< "0x" << p2 << ":DEBUG Chunk: size: " << size << " type: " << type << std::dec << "\n";
			switch (type) {
				case PALETTE_OLD_0x0004:{
				case PALETTE_OLD_0x0011:
					chunks.emplace_back(PALETTE_OLD_CHUNK(s), type);
					break;
				}
				case LAYER_0x2004:{;
					chunks.emplace_back(LAYER_CHUNK(s), type);
					break;
				}
				case CEL_0x2005:{
					chunks.emplace_back(CEL_CHUNK(s, pixelFormat, size - 6), type);
					break;
				}
				case FRAME_TAGS_0x2018:{
					chunks.emplace_back(TAG_CHUNK(s), type);
					break;
				}
				case PALETTE_0x2019:{
					chunks.emplace_back(PALETTE_CHUNK(s), type);
					break;
				}
				case SLICE_0x2022:{
					chunks.emplace_back(SLICE_CHUNK(s), type);
					break;
				}
				default:
					std::cout << "^ not parsed\n";
					s.seekg(size + p); // skip data
			}
			result = result && s.good();
		}
	}
	return result;
}


ASEPRITE::ASEPRITE(std::string filename) :
		file(filename, std::ios::in | std::ios::binary) {
	if (!file.good()) {
		std::cout << "File " << filename << " not good\n";
		file.close();
		return;
	}
	if(!ASEPRITE::tinf_initialized){
	    tinf_init();
	    ASEPRITE::tinf_initialized = true;
	}
	if (readAseHeader()) {
		header.toString();
		PIXELTYPE pixelFormat = header.bitDepth == 8 ? INDEXED : header.bitDepth == 16 ? GRAYSCALE : RGBA;
		frames.resize(header.frames);
		for (size_t f = 0; f < header.frames && file.good(); f++) {
			std::cout << " FRAME " << f << "\n";
			frames[f].read(file, pixelFormat);
		}
	}

	file.close();
}

bool ASEPRITE::readAseHeader() {
	return file & header;
}
bool ASEPRITE::tinf_initialized = false;
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

