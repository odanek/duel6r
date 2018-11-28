/*
 * Aseprite to animation transformation
 * Version 0.1
 * Copyright 2018 by Frantisek Veverka
 *
 */

#ifndef ASEPRITE_TO_ANIMATION_H_
#define ASEPRITE_TO_ANIMATION_H_
#include "aseprite.h"
#include "animation.h"

animation::LoopType from(uint16_t type){
	switch(type){
		case 0: return animation::LoopType::FORWARD;
		case 1: return animation::LoopType::REVERSE;
		case 2: return animation::LoopType::PING_PONG;
		default: return animation::LoopType::FORWARD;
	}
}
std::vector<uint8_t> from(const std::vector<aseprite::PIXEL_DATA> & in){
	std::vector<uint8_t> result(in.size());
	for(size_t i = 0; i < in.size(); i++){
	    result[i] = in[i].INDEXED;
	}
	return result;
}
animation::Animation fromASEPRITE(const aseprite::ASEPRITE & ase){
	animation::Animation animation;
	animation.width = ase.header.width;
	animation.height = ase.header.height;
	animation.framesCount = ase.header.frames;
	animation.transparentIndex = ase.header.transparentIndex;
	animation.frames.reserve(animation.framesCount);

	for(const auto & chunk : ase.frames[0].chunks){
		if(chunk.type == 0x2019) {
			const auto & palette_chunk = std::get<aseprite::PALETTE_CHUNK>(chunk.data);
			for(size_t i = 0; i < palette_chunk.colors.size(); i++){
				animation.palette.colors[i].r = palette_chunk.colors[i].r;
				animation.palette.colors[i].g = palette_chunk.colors[i].g;
				animation.palette.colors[i].b = palette_chunk.colors[i].b;
				animation.palette.colors[i].a = palette_chunk.colors[i].a;

			}

		}
		if(chunk.type == 0x2018) {
			const auto & tag_chunk = std::get<aseprite::TAG_CHUNK>(chunk.data);
			animation.loops.reserve(tag_chunk.tags.size());
			for(const auto & tag : tag_chunk.tags) {
				animation.loops.emplace_back(
					tag.from,
					tag.to,
					from(tag.direction),
					tag.name.toString()
				);
			}
		}
		if(chunk.type == 0x2004) {
			const auto & layer = std::get<aseprite::LAYER_CHUNK>(chunk.data);
			animation.layers.push_back(animation::Layer(
					layer.flags & 0x1,
					layer.layerType == 1,
					layer.opacity,
					layer.name.toString(),
					animation.framesCount));
		}
	}
	for(size_t f = 0; f < animation.framesCount; f++){
		animation.frames[f].duration = ase.frames[f].duration;
		for(const auto & chunk : ase.frames[f].chunks){
			if(chunk.type == 0x2005) {
				const auto & cel_chunk = std::get<aseprite::CEL_CHUNK>(chunk.data);
				auto & layer = animation.layers[cel_chunk.layerIndex]; //TODO bounds check
				auto & cel = layer.frames[f];
				if(cel_chunk.type == 1) { // linked cel
					auto & linkedCel = animation.layers[cel_chunk.layerIndex].frames[cel_chunk.frameLink];
					cel.image = linkedCel.image;
					cel.opacity = linkedCel.opacity;
					cel.x = linkedCel.x;
					cel.y = linkedCel.y;

				} else {
					cel.x = cel_chunk.x;
					cel.y = cel_chunk.y;
					cel.opacity = cel_chunk.opacity;
                    animation.images.emplace_back();
                    auto & image = animation.images.back();
                    cel.image = animation.images.size() - 1;
                    image.width = cel_chunk.width;
                    image.height = cel_chunk.height;
                    image.pixels = from(cel_chunk.pixels);
				}
			}
		}
	}
    for(const auto & loop : animation.loops) {
        const float SPRITE_SPEED_COEFFICIENT = 1.0f/10.0f;
        std::vector<int16_t> animationLoop;

        if(loop.loopType == animation::LoopType::FORWARD){
            uint16_t loopLength = loop.to - loop.from + 1;
            animationLoop.reserve(loopLength * 2 + 2); // format is frame,duration,...,frame,duraion,-1,0

            for(uint16_t frame = loop.from; frame <= /*(!)*/ loop.to; frame++) {
                animationLoop.push_back(frame);
                animationLoop.push_back(animation.frames[frame].duration * SPRITE_SPEED_COEFFICIENT);
            }

        } else if(loop.loopType == animation::LoopType::PING_PONG) {
            uint16_t loopLength = loop.to - loop.from + 1;
            animationLoop.reserve(loopLength * 4); // format is frame,duration,...,frame,duraion,-1,0

            for(uint16_t frame = loop.from; frame < /*(!)*/ loop.to; frame++) {
                animationLoop.push_back(frame);
                animationLoop.push_back(animation.frames[frame].duration * SPRITE_SPEED_COEFFICIENT);
            }
            for(uint16_t frame = loop.to; frame >= loop.from; frame--) {
                animationLoop.push_back(frame);
                animationLoop.push_back(animation.frames[frame].duration * SPRITE_SPEED_COEFFICIENT);
            }

        } else if(loop.loopType == animation::LoopType::REVERSE) {
            uint16_t loopLength = loop.to - loop.from + 1;
            animationLoop.reserve(loopLength * 2 + 2); // format is frame,duration,...,frame,duraion,-1,0

            for(uint16_t frame = loop.to; frame >= loop.from; frame--) {
                animationLoop.push_back(frame);
                animationLoop.push_back(animation.frames[frame].duration * SPRITE_SPEED_COEFFICIENT);
            }

        }
        animationLoop.push_back(-1);
        animationLoop.push_back(0);
        animation.animations.push_back(animationLoop);
        animation.animationLookup[loop.name] = animation.animations.size() - 1;
    }
	return animation;
}


#endif /* ASEPRITE_TO_ANIMATION_H_ */
