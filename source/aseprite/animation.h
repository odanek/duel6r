/*
 * Aseprite animation
 * Version 0.1
 * Copyright 2018 by Frantisek Veverka
 *
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <cstdint>
#include <cassert>

#include <string>
#include <iostream>
#include <utility>

#include <array>
#include <vector>
#include <map>

namespace animation{

enum class LoopType {
    FORWARD,
    REVERSE,
    PING_PONG
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;
};

class Palette {
public:
    std::array<Color, 256> colors;
};

class Image {
public:
    uint16_t width = 0;
    uint16_t height = 0;

    std::vector<uint8_t> pixels; // TOOD 32bit color images

    Image() = default;

    Image(uint16_t width, uint16_t height, std::vector<uint8_t> && pixels) :
        width(width),
        height(height),
        pixels(std::move(pixels)) {
    }

    Image(const Image & image) :
        width(image.width),
        height(image.height),
        pixels(image.pixels) {
    }

    Image(Image && image) :
        width(image.width),
        height(image.height),
        pixels(std::move(image.pixels)) {
    }

    Image & operator=(const animation::Image& image) {
        width = image.width;
        height = image.height;
        pixels = image.pixels;
        return *this;
    }

    Image & operator =(Image && image) {
        width = image.width;
        height = image.height;
        pixels = std::move(image.pixels);
        return *this;
    }

    std::vector<Color> substitute(const Palette& palette, uint8_t opacity = 255, uint8_t transparentIndex = 0) const {
        size_t size = pixels.size();
        std::vector<Color> result(size);

        for (size_t i = 0; i < size; i++) {
            result[i] = palette.colors[pixels[i]];
            result[i].a = result[i].a * (opacity / 255.0f);
            if (pixels[i] == transparentIndex) {
                result[i].r = 0;
                result[i].g = 0;
                result[i].b = 0;
                result[i].a = 0;
            }
        }
        return result;
    }
};

class Cel {
public:
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t opacity = 0;
    uint32_t image; //pointer to animation.images
};

class Frame {
public:
    uint16_t duration; //ms
};

class Layer {
public:
    class LayerView {
    public:
        bool visible = true;

        LayerView(const Layer & l) :
            visible(l.visible) {
        }
    };
    enum BLEND_MODE {
        Normal = 0,
        Multiply = 1,
        Screen = 2,
        Overlay = 3,
        Darken = 4,
        Lighten = 5,
        ColorDodge = 6,
        ColorBurn = 7,
        HardLight = 8,
        SoftLight = 9,
        Difference = 10,
        Exclusion = 11,
        Hue = 12,
        Saturation = 13,
        Color = 14,
        Luminosity = 15,
        Addition = 16,
        Subtract = 17,
        Divide = 18
    };

    BLEND_MODE blendMode = Normal;
    bool visible = true;
    bool isGroupLayer = false; // true -> no frames
    uint8_t opacity;
    std::string name;
    std::vector<Cel> frames;
    Layer(BLEND_MODE blendMode, bool visible, bool isGroupLayer,
          uint8_t opacity,
          const std::string & name,
          uint32_t framesCount) :
        blendMode(blendMode),
        visible(visible),
        isGroupLayer(isGroupLayer),
        opacity(opacity),
        name(name) {
        if (!isGroupLayer) {
            frames.resize(framesCount);
        }
    }

    Layer(const Layer & l) :
        blendMode(l.blendMode),
        visible(l.visible),
        isGroupLayer(l.isGroupLayer),
        opacity(l.opacity),
        name(l.name),
        frames(l.frames) {
    }

    Layer(Layer && l) :
        blendMode(l.blendMode),
        visible(l.visible),
        isGroupLayer(l.isGroupLayer),
        opacity(l.opacity),
        name(l.name),
        frames(std::move(l.frames)) {
    }

    Layer & operator =(const Layer & l) {
        blendMode = l.blendMode;
        visible = l.visible;
        isGroupLayer = l.isGroupLayer;
        opacity = l.opacity;
        name = l.name;
        frames = l.frames;
        return *this;
    }

    Layer & operator =(Layer && l) {
        blendMode = l.blendMode;
        visible = l.visible;
        isGroupLayer = l.isGroupLayer;
        opacity = l.opacity;
        name = l.name;
        frames = std::move(l.frames);
        return *this;
    }
};

class Loop {
public:
    uint16_t from;
    uint16_t to;
    LoopType loopType;
    std::string name;

    Loop(uint16_t from, uint16_t to, LoopType loopType, const std::string & name) :
        from(from),
        to(to),
        loopType(loopType),
        name(name) {
    }
};

class Animation {
public:

    class AnimationView {
    public:
        const Animation & animation;
        std::vector<Layer::LayerView> layerViews;
    public:
        AnimationView(const Animation & animation) :
            animation(animation),
            layerViews(animation.layers.begin(), animation.layers.end()) {
        }

        void setLayerVisibility(const std::string & name, bool visible) {
            for (uint32_t i = 0; i < layerViews.size(); i++) {
                if (animation.layers[i].name == name) {
                    layerViews[i].visible = visible;
                }
            }
        }

        void setVisibilityForAllLayers(bool visible) {
            for (auto & view : layerViews) {
                view.visible = visible;
            }
        }

        void hideAllLayers() {
            setVisibilityForAllLayers(false);
        }

        void showAllLayers() {
            setVisibilityForAllLayers(true);
        }

    };

    uint16_t width;
    uint16_t height;
    uint16_t framesCount;
    uint8_t transparentIndex;
    Palette palette;
    std::vector<std::vector<int32_t>> animations;
    std::vector<Frame> frames;
    std::vector<Layer> layers;
    std::vector<Image> images;
    std::vector<Loop> loops;
    std::map<std::string, uint32_t> animationLookup; //index to animations

    Animation() = default;
    Animation(const Animation &animation) = delete;
    Animation(Animation &&animation) = default;
    Animation& operator =(const Animation &animation) = delete;
    Animation& operator =(Animation &&animation) = default;

    void log() {
        std::cout << "Animation: frames:" << framesCount << " width: " << width << " height: " << height << "\n";
        std::cout << "  layers: \n";
        for (const auto & layer : layers) {
            std::cout << " name: " << layer.name << " " << (layer.isGroupLayer ? "[G]" : "") << "\n";
        }
    }

    bool hasAnimation(const std::string & animationName) {
        return animationLookup.count(animationName) > 0;
    }

    const std::vector<int32_t> & getAnimation(const std::string & animationName) const {
        const auto & it = animationLookup.find(animationName);
        if(it == animationLookup.end()){
            assert(animations.size() > 0);
            return animations[0];
        } else {
            return animations[it->second];
        }
    }

    AnimationView toView() const {
        return AnimationView(*this);
    }

    static animation::Animation loadAseImage(const std::string &path);
};

}
#endif
