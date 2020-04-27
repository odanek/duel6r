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

#include <SDL2/SDL_image.h>
#include <algorithm>
#include "Image.h"
#include "Exception.h"
#include "IoException.h"
#include "DataException.h"
#include "Format.h"
#include "File.h"

namespace Duel6 {

    Image::Image(Size width, Size height, void * d):
        dimensions { width, height, 1 },
        data((Color*)d, (Color*) d + 4*(width*height)){
    }

    Image::Image(Size width, Size height, Size depth)
        :dimensions { width, height, depth },
          data(width * height * depth) {
    }

    Image &Image::resize(Size width, Size height, Size depth) {
        dimensions[0] = width;
        dimensions[1] = height;
        dimensions[2] = depth;
        data.clear();
        data.resize(width * height * depth);
        return *this;
    }

    Image Image::flipY() const {
        auto width = dimensions[0];
        auto height = dimensions[1];
        auto depth = dimensions[2];

        Image result(width, height, depth);

        const Color *input;
        Color *output;

        for (Size z = 0; z < depth; z++) {
            for (Size y = 0; y < height; y++) {
                input = &data[z * width * height + (height - y - 1) * width];
                output = &result.data[z * width * height + y * width];
                memcpy(output, input, width * sizeof(Color));
            }
        }

        return result;
    }

    Image &Image::addSlice(const Image &slice) {
        if (dimensions[2] == 0) {
            dimensions[0] = slice.getWidth();
            dimensions[1] = slice.getHeight();
        }

        if (slice.getWidth() != dimensions[0] || slice.getHeight() != dimensions[1]) {
            D6_THROW(DataException, "Incompatible image slice size");
        }

        dimensions[2]++;

        Size sliceSize = dimensions[0] * dimensions[1];
        data.resize(sliceSize * dimensions[2]);

        memcpy(&data[sliceSize * (dimensions[2] - 1)], slice.data.data(), sliceSize * sizeof(Color));

        return *this;
    }

    void Image::save(const std::string &path) const {
        SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void *)data.data(), dimensions[0], dimensions[1], 32,
                4 * dimensions[0], 0xff, 0xff00, 0xff0000, 0);
        IMG_SavePNG(surface, path.c_str());
        SDL_FreeSurface(surface);
    }

    std::string Image::saveScreenshot() const {
        Int32 screenshotNumber = 0;
        std::string name;

        do {
            screenshotNumber++;
            name = Format("screenshot_{0}.png") << screenshotNumber;
        } while (File::exists(name));

        save(name);

        return name;
    }

    Image Image::load(const std::string &path) {
        SDL_Surface *surface;
        surface = IMG_Load(path.c_str());
        if (!surface) {
            D6_THROW(IoException, Format("Unable to load file {0}: {1}") << path << IMG_GetError());
        }

        Image image = fromSurface(surface);
        SDL_FreeSurface(surface);

        return image;
    }

    Image Image::loadStack(const std::string &path) {
        std::vector<std::string> textureFiles = File::listDirectory(path);
        std::sort(textureFiles.begin(), textureFiles.end());

        Image result;

        for (const auto &fileName : textureFiles) {
            Image slice = Image::load(path + fileName);
            result.addSlice(slice);
        }

        return result;
    }

    Image Image::fromSurface(SDL_Surface *surface) {
        if(surface->format->BytesPerPixel == 4 && surface->format->format == SDL_PIXELFORMAT_RGBA32){
            return Image(surface->w, surface->h, surface->pixels);
        }
        SDL_LockSurface(surface);

        auto width = surface->w;
        auto height = surface->h;
        auto bpp = surface->format->BytesPerPixel;

        Image image(width, height);
        auto input = (Uint8 *) surface->pixels;
        Color *output = &image.at(0);

        for (Int32 y = 0; y < height; y++) {
            for (Int32 x = 0; x < width; x++, output++, input += bpp) {
                Uint8 &red = output->red, &green = output->green, &blue = output->blue, &alpha = output->alpha;
                Uint32 pixel;
                switch (bpp) {
                    case 4:
                        pixel = *(Uint32*) input;
                        break;
                    case 1:
                        pixel = *input;
                        break;
                    case 2:
                        pixel = *(Uint16 *) input;
                        break;
                    case 3:
                        pixel = input[0] | input[1] << 8 | input[2] << 16; // Little endian
                        break;
                    default:
                        pixel = *(Uint32 *) input;
                        break;
                }

                SDL_GetRGBA(pixel, surface->format, &red, &green, &blue, &alpha);
                if(red == 0 && green == 0 && blue == 0){
                    alpha = 0;
                }
            }

            input += surface->pitch - (width * bpp);
        }

        SDL_UnlockSurface(surface);

        return image;
    }
}
