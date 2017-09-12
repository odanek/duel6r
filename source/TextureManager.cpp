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
            : nextId(0), textureFileExtension(fileExtension) {}

    TextureManager::~TextureManager() {
        disposeAll();
    }

    const TextureList TextureManager::load(const std::string &path, TextureFilter filtering, bool clamp) {
        SubstitutionTable emptySubstitutionTable;
        return load(path, filtering, clamp, emptySubstitutionTable);
    }

    const TextureList TextureManager::load(const std::string &path, TextureFilter filtering, bool clamp,
                                           const SubstitutionTable &substitutionTable) {
        std::vector<std::string> textureFiles;
        File::listDirectory(path, textureFileExtension, textureFiles);
        std::sort(textureFiles.begin(), textureFiles.end());

        Int32 listId = nextId++;
        textureMap.insert(std::make_pair(listId, std::make_unique<TextureArray>()));
        TextureArray &list = *textureMap.at(listId);

        for (std::string &file : textureFiles) {
            Image image;
            Util::loadTargaImage(path + file, image);
            substituteColors(image, substitutionTable);
            Texture texture = globRenderer->createTexture(image.getWidth(), image.getHeight(), &image.at(0), 1,
                                                          filtering, clamp);
            list.push_back(texture);
        }

        return TextureList(listId, list);
    }

    void TextureManager::dispose(TextureList &list) {
        dispose(list.getKey());
        list.release();
    }

    void TextureManager::dispose(const Int32 key) {
        auto entry = textureMap.find(key);
        if (entry != textureMap.end()) {
            releaseTextureIds(*(entry->second));
            textureMap.erase(key);
        }
    }

    void TextureManager::disposeAll() {
        for (auto &entry : textureMap) {
            releaseTextureIds(*(entry.second));
        }
        textureMap.clear();
    }

    void TextureManager::releaseTextureIds(const TextureArray &list) {
        for (const Texture &texture : list) {
            globRenderer->freeTexture(texture);
        }
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
