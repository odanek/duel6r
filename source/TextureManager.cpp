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
#include "File.h"
#include "Video.h"

namespace Duel6 {
    TextureManager::TextureManager() {}

    Texture TextureManager::loadStack(const std::string &path, TextureFilter filtering, bool clamp) {
        SubstitutionTable emptySubstitutionTable;
        return loadStack(path, filtering, clamp, emptySubstitutionTable);
    }

    Texture TextureManager::loadStack(const std::string &path, TextureFilter filtering, bool clamp,
                                      const SubstitutionTable &substitutionTable) {
        Image image = Image::loadStack(path);
        substituteColors(image, substitutionTable);

        Texture texture = globRenderer->createTexture(image, filtering, clamp);
        return texture;
    }

    const TextureDictionary TextureManager::loadDict(const std::string &path, TextureFilter filtering, bool clamp) {
        std::vector<std::string> textureFiles = File::listDirectory(path);

        TextureDictionary dict;
        for (std::string &file : textureFiles) {
            Image image = Image::load(path + file);
            Texture texture = globRenderer->createTexture(image, filtering, clamp);
            dict.textures[file] = texture;
        }

        return dict;
    }

    void TextureManager::dispose(Texture texture) {
        globRenderer->freeTexture(texture);
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
