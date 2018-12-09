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

    const Texture TextureManager::loadStack(const std::string &path, TextureFilter filtering, bool clamp) {
        SubstitutionTable emptySubstitutionTable;
        return loadStack(path, filtering, clamp, emptySubstitutionTable);
    }

    const Texture TextureManager::loadStack(const std::string &path, TextureFilter filtering, bool clamp,
                                           const SubstitutionTable &substitutionTable) {
        std::vector<std::string> textureFiles = File::listDirectory(path, textureFileExtension);
        std::sort(textureFiles.begin(), textureFiles.end());

        auto size = Util::loadTargaSize(path + textureFiles[0]);
        Image image(size.first, size.second, textureFiles.size());

        Size offset = 0;
        for (const auto &file : textureFiles) {
            Util::loadTargaData(path + file, &image.at(offset));
            offset += image.getWidth() * image.getHeight();
        }

        substituteColors(image, substitutionTable);

        Texture::Key textureKey = nextKey++;
        Texture::Id textureId = globRenderer->createTexture(image.getWidth(), image.getHeight(), image.getDepth(), &image.at(0), 1,
                                                          filtering, clamp);
        textureKeys[textureKey] = textureId;

        return Texture(textureKey, textureId);
    }

    const TextureDictionary TextureManager::loadDict(const std::string &path, TextureFilter filtering, bool clamp) {
        std::vector<std::string> textureFiles = File::listDirectory(path, textureFileExtension);

        TextureDictionary dict;
        for (std::string &file : textureFiles) {
            Image image;
            Texture::Key textureKey = nextKey++;
            Util::loadTargaImage(path + file, image);
            Texture::Id textureId = globRenderer->createTexture(image.getWidth(), image.getHeight(), 1, &image.at(0), 1,
                                                                filtering, clamp);
            textureKeys[textureKey] = textureId;
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

    void TextureManager::disposeAll() {
        for (auto &entry : textureKeys) {
            globRenderer->freeTexture(entry.second);
        }
        textureKeys.clear();
    }

    void TextureManager::substituteColors(Image &image, const SubstitutionTable &substitutionTable) {
        Size imgSize = image.getWidth() * image.getHeight() * image.getDepth();
        for (Size i = 0; i < imgSize; ++i) {
            Color &color = image.at(i);
            auto substituteColor = substitutionTable.find(color);
            if (substituteColor != substitutionTable.end()) {
                color = substituteColor->second;
            }
        }
    }
}
