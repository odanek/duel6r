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

#include <vector>
#include "Util.h"
#include "File.h"
#include "Video.h"
#include "File.h"
#include "IoException.h"

namespace Duel6 {
    namespace Util {
        static void readTgaColor(File &file, Color &color) {
            Uint8 colBytes[3];
            file.read(colBytes, 1, 3);
            color.set(colBytes[2], colBytes[1], colBytes[0],
                      (colBytes[0] == 0 && colBytes[1] == 0 && colBytes[2] == 0) ? 0 : 255);
        }

        static void writeTgaColor(File &file, const Color &color) {
            Uint8 colBytes[3] = {color.getBlue(), color.getGreen(), color.getRed()};
            file.write(colBytes, 1, 3);
        }

        void loadTargaImage(const std::string &path, Image &image) {
            File file(path, File::Mode::Binary, File::Access::Read);

            // Header
            Uint16 header[9];
            file.read(header, 2, 9);

            // Data
            image.resize(header[6], header[7]);
            Color color, *output = &image.at(0);
            Size remainingData = header[6] * header[7];
            Uint8 chunkLength, packet;

            while (remainingData > 0) {
                file.read(&packet, 1, 1);

                if (packet >= 0x80) {
                    // RLE chunk
                    chunkLength = packet - 0x7f;

                    readTgaColor(file, color);
                    for (Size i = 0; i < chunkLength; ++i) {
                        *output++ = color;
                    }
                } else {
                    // Raw chunk
                    chunkLength = packet + 1;

                    for (Size i = 0; i < chunkLength; ++i) {
                        readTgaColor(file, color);
                        *output++ = color;
                    }
                }

                remainingData -= chunkLength;
            }

            if (!(header[8] & 0x2000)) // Flip image vertically
            {
                for (Size y = 0; y < image.getHeight() / 2; y++) {
                    Color *row1 = &image.at(y * image.getWidth());
                    Color *row2 = &image.at((image.getHeight() - 1 - y) * image.getWidth());

                    for (Size x = 0; x < image.getWidth(); x++, row1++, row2++) {
                        Color tmp = *row1;
                        *row1 = *row2;
                        *row2 = tmp;
                    }
                }
            }
        }

        void saveTarga(const std::string &path, const Image &image) {
            File file(path, File::Mode::Binary, File::Access::Write);

            // Header
            Uint16 header[9] = {0, 10, 0, 0, 0, 0, (Uint16) image.getWidth(), (Uint16) image.getHeight(),
                                0x18};  // 0x2018
            file.write(header, 2, 9);

            // Data
            Size remainingData = image.getWidth() * image.getHeight();
            const Color *data = &image.at(0);
            Uint8 chunkLength;
            while (remainingData > 0) {
                if (remainingData > 1 && data[0] == data[1]) {
                    // RLE chunk
                    chunkLength = 2;
                    while (chunkLength < 128 && chunkLength < remainingData && data[0] == data[chunkLength]) {
                        ++chunkLength;
                    }

                    Uint8 packet = 0x80 + (chunkLength - 1);
                    file.write(&packet, 1, 1);
                    writeTgaColor(file, data[0]);
                } else {
                    // Raw chunk
                    chunkLength = 1;
                    while (chunkLength < 128 && chunkLength < remainingData &&
                           data[chunkLength - 1] != data[chunkLength]) {
                        ++chunkLength;
                    }

                    Uint8 packet = chunkLength - 1;
                    file.write(&packet, 1, 1);
                    for (Size i = 0; i < chunkLength; ++i) {
                        writeTgaColor(file, data[i]);
                    }
                }

                data += chunkLength;
                remainingData -= chunkLength;
            }
        }

        std::string saveScreenTga(const Video &video) {
            Int32 num = 0;
            std::string name;

            // Vyhledani cisla pod ktere ukladat
            while (num < 1000) {
                num++;
                name = Format("screenshot_{0}.tga") << num;
                if (!File::exists(name)) {
                    break;
                }
            }

            // Maximalne 1000 screenshotu
            if (num >= 1000) {
                D6_THROW(IoException, "Maximum number of 1000 screenshots reached");
            }

            Image image(video.getScreen().getClientWidth(), video.getScreen().getClientHeight());
            globRenderer->readScreenData(video.getScreen().getClientWidth(), video.getScreen().getClientHeight(),
                                         image);
            saveTarga(name, image);
            return name;
        }
    }
}