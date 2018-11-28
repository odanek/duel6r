/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <algorithm>
#include "TextureManager.h"
#include "Util.h"
#include "File.h"

namespace Duel6 {
    TextureManager::TextureManager(const std::string &fileExtension)
            : nextKey(1), textureFileExtension(fileExtension) {}

    TextureManager::~TextureManager() {
        disposeAll();
    }
    const animation::Animation TextureManager::loadAnimation(const std::string &path) {
        return Util::loadAseImage(path);
    }

    const TextureList TextureManager::generateSprite(const animation::Animation& animation,
            const animation::Palette &substitutionTable,
            TextureFilter filtering,
            bool clamp) {
        TextureList list;

        for(uint16_t f = 0; f < animation.framesCount; f ++){
            Image frameImage(animation.width, animation.height);
            for(uint16_t p = 0; p < animation.width * animation.height; p++){
                frameImage.at(p).setAlpha(0);
            }
            Texture::Key textureKey = nextKey++;
            for(const auto & layer: animation.layers){

                //TODO: Filter for headband / hair etc.
                if(!layer.visible || layer.isGroupLayer) continue; //TODO group layers and visibility
                const animation::Cel & frame = layer.frames[f];
                const animation::Image & image = animation.images[frame.image];
                const float layerOpacity = layer.opacity / 255.0f;
                const auto x = frame.x;
                const auto y = frame.y;
                const auto width = image.width;
                const auto height = image.height;

                auto pixels = image.substitute(substitutionTable, layerOpacity * frame.opacity, animation.transparentIndex);
                for(uint16_t v = 0; v < height; v++) {
                    for(uint16_t u = 0; u < width; u++){

                        //TODO LAYER BLEND MODE
                        const auto & color = pixels[v * width + u];
                        if(color.r == 0 && color.g == 0 && color.b == 0 && color.a == 0){

                        } else {
                            frameImage.at( (y + v) * animation.width + u + x ).set(color.r, color.g, color.b, color.a);
                        }
                    }
                }
                // frameImage now contains pixels from all layers

            }
            Texture::Id textureId = globRenderer->createTexture(frameImage.getWidth(), frameImage.getHeight(), &frameImage.at(0), 1,
                                                          filtering, clamp);
            list.textures.push_back(Texture(textureKey, textureId));
        }
        return list;
    }

    const TextureList TextureManager::loadList(const std::string &path, TextureFilter filtering, bool clamp) {
        SubstitutionTable emptySubstitutionTable;
        return loadList(path, filtering, clamp, emptySubstitutionTable);
    }

    const TextureList TextureManager::loadList(const std::string &path, TextureFilter filtering, bool clamp,
                                           const SubstitutionTable &substitutionTable) {
        std::vector<std::string> textureFiles = File::listDirectory(path, textureFileExtension);
        std::sort(textureFiles.begin(), textureFiles.end());

        TextureList list;
        for (std::string &file : textureFiles) {
            Image image;
            Texture::Key textureKey = nextKey++;
            Util::loadTargaImage(path + file, image);
            substituteColors(image, substitutionTable);
            Texture::Id textureId = globRenderer->createTexture(image.getWidth(), image.getHeight(), &image.at(0), 1,
                                                          filtering, clamp);
            list.textures.push_back(Texture(textureKey, textureId));
        }

        return list;
    }

    const TextureDictionary TextureManager::loadDict(const std::string &path, TextureFilter filtering, bool clamp) {
        std::vector<std::string> textureFiles = File::listDirectory(path, textureFileExtension);

        TextureDictionary dict;
        for (std::string &file : textureFiles) {
            Image image;
            Texture::Key textureKey = nextKey++;
            Util::loadTargaImage(path + file, image);
            Texture::Id textureId = globRenderer->createTexture(image.getWidth(), image.getHeight(), &image.at(0), 1,
                                                                filtering, clamp);
            dict.textures[file] = Texture(textureKey, textureId);
        }

        return dict;
    }

    void TextureManager::dispose(Texture &texture) {
        auto key = texture.getKey();
        auto keyIterator = textureKeys.find(key);
        if (key != 0 && keyIterator != textureKeys.end()) {
            globRenderer->freeTexture(keyIterator->second);
            texture.id = 0;
        }
    }

    void TextureManager::dispose(TextureList &list) {
        for (Texture &texture : list.textures) {
            dispose(texture);
        }
        list.textures.clear();
    }

    void TextureManager::disposeAll() {
        for (auto &entry : textureKeys) {
            globRenderer->freeTexture(entry.second);
        }
        textureKeys.clear();
    }

    void TextureManager::substituteColors(Image &image, const SubstitutionTable &substitutionTable) {
        Size imgSize = image.getWidth() * image.getHeight();
        for (Size i = 0; i < imgSize; ++i) {
            Color &color = image.at(i);
            auto substituteColor = substitutionTable.find(color);
            if (substituteColor != substitutionTable.end()) {
                color = substituteColor->second;
            }
        }
    }
}
