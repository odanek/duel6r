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

#ifndef DUEL6_FONTCACHE_H
#define DUEL6_FONTCACHE_H

#include <string>
#include <unordered_map>
#include <list>
#include "Type.h"
#include "renderer/RendererTypes.h"
#include "renderer/Renderer.h"

namespace Duel6 {
    class FontCache {
    private:
        struct Entry {
            std::string text;
            Texture texture;
            Uint32 width;
            Uint32 height;
        };
        typedef std::list <Entry> EntryList;
        typedef std::unordered_map <std::string, EntryList::iterator> EntryMap;

    private:
        mutable EntryList entryList;
        mutable EntryMap entryMap;
        Renderer &renderer;
        Size maxEntries;

    public:
        explicit FontCache(Renderer &renderer, Size maxEntries)
                : renderer(renderer), maxEntries(maxEntries) {}

        ~FontCache() {
            empty();
        }

        bool has(const std::string &text) const {
            return entryMap.find(text) != entryMap.end();
        }

        Texture get(const std::string &text) const {
            auto iter = entryMap.find(text);
            Entry entry = *iter->second;
            entryList.erase(iter->second);
            entryList.push_front(entry);
            iter->second = entryList.begin();
            return entry.texture;
        }
        Texture get(const std::string &text, Float32 &width, Float32 &height) const {
            auto iter = entryMap.find(text);
            Entry entry = *iter->second;
            entryList.erase(iter->second);
            entryList.push_front(entry);
            iter->second = entryList.begin();
            width = entry.width;
            height = entry.height;
            return entry.texture;
        }
        void add(const std::string &text, Texture texture) const {
            entryList.push_front(Entry{text, texture});
            entryMap.insert(std::make_pair(text, entryList.begin()));
            if (entryList.size() > maxEntries) {
                Entry lastEntry = entryList.back();
                entryList.pop_back();
                entryMap.erase(lastEntry.text);
                renderer.freeTexture(lastEntry.texture); //TADY TO OBCAS VYLITNE pri intenzivni zatezi
            }
        }
        void add(const std::string &text, Texture texture, Float32 width, Float32 height) const {
            entryList.push_front(Entry{text, texture, (Uint32)width, (Uint32)height});
            entryMap.insert(std::make_pair(text, entryList.begin()));
            if (entryList.size() > maxEntries) {
                Entry lastEntry = entryList.back();
                entryList.pop_back();
                entryMap.erase(lastEntry.text);
                renderer.freeTexture(lastEntry.texture);
            }
        }
        void empty() {
            for (const Entry &entry : entryList) {
                renderer.freeTexture(entry.texture);
            }
            entryList.clear();
            entryMap.clear();
        }
    };
}

#endif
