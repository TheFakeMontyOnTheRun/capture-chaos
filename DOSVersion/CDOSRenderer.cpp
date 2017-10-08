#include <array>
#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <conio.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/movedata.h>
#include <pc.h>
#include <sys/farptr.h>
#include <bios.h>
#include <sys/nearptr.h>

#include <cmath>
#include <algorithm>
#include <cstring>

#include "Common.h"
#include "CGame.h"
#include "CRenderer.h"

namespace odb {
    std::array<int, 320 * 200> mBuffer;

    int offset = 0;
    std::array<unsigned char, 320 * 200> buffer;
    int origin = 0;
    int lastOrigin = -1;
    unsigned char shade;
    long frame = 0;

    void fill(int x1, int y1, int w, int h, std::array<int, 4> colour) {
        int _x0 = x1 / 2;
        int _x1 = (x1 + w) / 2;
        int _y0 = (y1 * 200) / 480;
        int _y1 = ((y1 + h) * 200) / 480;

        int pixel = colour[1] & 0xFF;
        pixel += (colour[2] & 0xFF) << 8;
        pixel += (colour[3] & 0xFF) << 16;

        for (int y = _y0; y < _y1; ++y) {
            for (int x = _x0; x < _x1; ++x) {

                if (x < 0 || x >= 320 || y < 0 || y >= 200) {
                    continue;
                }

                mBuffer[(320 * y) + x] = pixel;
            }
        }
    };

    void drawSquare(int x1, int y1, int x2, int y2, std::array<int, 4> colour) {
        int _x0 = x1 / 2;
        int _x1 = x2 / 2;
        int _y0 = (y1 * 200) / 480;
        int _y1 = (y2 * 200) / 480;

        int pixel = colour[1] & 0xFF;
        pixel += (colour[2] & 0xFF) << 8;
        pixel += (colour[3] & 0xFF) << 16;

        for (int y = _y0; y < _y1; ++y) {
            for (int x = _x0; x < _x1; ++x) {

                if (x < 0 || x >= 320 || y < 0 || y >= 200) {
                    continue;
                }

                mBuffer[(320 * y) + x] = pixel;
            }
        }
    };

    unsigned char getPaletteEntry(int origin) {
        unsigned char shade = 0;
        shade += (((((origin & 0x0000FF)      ) << 2) >> 8)) << 6;
        shade += (((((origin & 0x00FF00) >> 8 ) << 3) >> 8)) << 3;
        shade += (((((origin & 0xFF0000) >> 16) << 3) >> 8)) << 0;
        return shade;
    }

    CRenderer::CRenderer(CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback) :
            mOnKeyPressedCallback(keyPressedCallback), mOnKeyReleasedCallback(keyReleasedCallback) {

        __dpmi_regs reg;

        reg.x.ax = 0x13;
        __dpmi_int(0x10, &reg);

        outp(0x03c8, 0);

        for (int r = 0; r < 4; ++r) {
            for (int g = 0; g < 8; ++g) {
                for (int b = 0; b < 8; ++b) {
                    outp(0x03c9, (r * (21)));
                    outp(0x03c9, (g * (8 )));
                    outp(0x03c9, (b * (8 )));
                }
            }
        }
    }

    void CRenderer::sleep(long ms) {
    }

    void CRenderer::handleSystemEvents() {
        mOnKeyReleasedCallback(ECommand::kLeft);
        mOnKeyReleasedCallback(ECommand::kRight);
        mOnKeyReleasedCallback(ECommand::kUp);
        mOnKeyReleasedCallback(ECommand::kDown);


        auto lastKey = bioskey(0x11);

        bdos(0xC, 0, 0);
        switch (lastKey) {
            case 18656:
                //up
                mOnKeyPressedCallback(ECommand::kUp);
                break;

            case 8051:
            case 20704:
                //down
                mOnKeyPressedCallback(ECommand::kDown);
                break;

            case 19424: //right arrow
            case 4209: //q
                //left
                mOnKeyPressedCallback(ECommand::kLeft);
                break;

            case 4709: //e
            case 19936: //right arrow
                //right
                mOnKeyPressedCallback(ECommand::kRight);
                break;
            case 283:
                textmode(C80);
                std::cout << "Thank you for playing Capture Chaos 486 - (c) 2017 - Daniel Monteiro" << std::endl;
                exit(0);
        }
    }

    void CRenderer::render(const CGame &game, long ms) {

        fill(0, 0, 640, 480, {0, 0, 0, 0});

        switch (game.gameState) {
            case CGame::EGameState::kTitleScreen:
                fill(0, 0, 640, 480, {0, 255, 0, 0});
                break;

            case CGame::EGameState::kGame:

                fill(480, game.timeEllapsed / (15000 / 480), 40, 480 - 40, {0, 128, 0, 0});

                for ( const auto& particle : game.mParticles ) {

                    fill( particle.mX * 2, particle.mY * 2, 40, 40, { 255, particle.mShade, particle.mShade, particle.mShade } );
                }


                fill(game.x * 2, game.y * 2, 40, 40, {0, 255, 0, 0});
                break;

            case CGame::EGameState::kVictory:
                fill(0, 0, 640, 480, {0, 255, 255, 0});
                break;

            case CGame::EGameState::kGameOver:
                fill(0, 0, 640, 480, {0, 255, 0, 0});
                break;
        }


        for (int offset = 0; offset < 320 * 200; ++offset) {
            origin = mBuffer[offset];

            if (origin != lastOrigin) {
                shade = getPaletteEntry(origin);
            }

            lastOrigin = origin;

            buffer[offset] = shade;
        }

        dosmemput(&buffer[0], 64000, 0xa0000);
    }
}
