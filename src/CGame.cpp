#include <functional>
#include <random>
#include <vector>
#include <algorithm>

#include "Common.h"
#include "CGame.h"

namespace odb {
    void CGame::init() {
        timeEllapsed = vx = vy = 0;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<> random(1, 220);

        x = random( gen );
        y = random( gen );

        mParticles.clear();

        for( int c = 0; c < 200; ++c ) {
            mParticles.emplace_back( random( gen ),
                                     random( gen ),
                                     random( gen ) / 30,
                                     random( gen ) / 30,
                                     127 + (random( gen )/2) );
        }
    }

    CGame::CGame() {
        gameState = EGameState::kGame;
        init();
    }


    void CGame::tick(long ms) {

        if (gameState == EGameState::kVictory) {
            return;
        }

        if ( mParticles.empty() ) {
            gameState = EGameState::kVictory;
            return;
        }

        x += vx;
        y += vy;
        timeEllapsed += ms;
        ++frame;

        if (timeEllapsed >= 15000) {
            gameState = EGameState::kGameOver;
            init();
        }


        std::vector<CParticle> toRemove;

        for ( auto& particle : mParticles ) {
            particle.update();

            if ( std::abs(particle.mX - x ) < 40 && std::abs(particle.mY - y ) < 40 ) {

            } else {
                toRemove.push_back(particle);
            }
        }

        mParticles.clear();


        for ( auto& particle : toRemove ) {
            mParticles.push_back(particle);
        }
    }

    CControlCallback CGame::getKeyPressedCallback() {
        return [&](ECommand command) {
            if (command == ECommand::kLeft) {
                vx = -16;
            }

            if (command == ECommand::kRight) {
                vx = 16;
            }
            if (command == ECommand::kUp) {
                vy = -16;
            }

            if (command == ECommand::kDown) {
                vy = 16;
            }
        };
    }

    CControlCallback CGame::getKeyReleasedCallback() {
        return [&](ECommand command) {


            switch (gameState) {
                case EGameState::kGame:
                    if (command == ECommand::kLeft) {
                        vx = 0;
                    }

                    if (command == ECommand::kRight) {
                        vx = 0;
                    }

                    if (command == ECommand::kUp) {
                        vy = 0;
                    }

                    if (command == ECommand::kDown) {
                        vy = 0;
                    }
                    return;


                case EGameState::kGameOver:
                    gameState = EGameState::kTitleScreen;


                case EGameState::kTitleScreen:
                    gameState = EGameState::kGame;
                    init();
                    return;
            }


        };
    }
}
