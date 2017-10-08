#include <functional>
#include <vector>

#include "Common.h"

#include "CGame.h"
#include "CRenderer.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <iostream>

namespace odb {

    SDL_Surface *video;

    CRenderer::CRenderer( CControlCallback keyPressedCallback, CControlCallback keyReleasedCallback ):
    mOnKeyPressedCallback( keyPressedCallback ), mOnKeyReleasedCallback( keyReleasedCallback )
    {
        SDL_Init( SDL_INIT_EVERYTHING );
        video = SDL_SetVideoMode( 640, 480, 0, 0 );
    }

    void CRenderer::sleep( long ms ) {
        SDL_Delay(33);
    }

    void CRenderer::handleSystemEvents() {
        SDL_Event event;

        while ( SDL_PollEvent( &event ) ) {

            if( event.type == SDL_QUIT ) {
#ifndef __EMSCRIPTEN__
                exit(0);
#endif
            }

            if ( event.type == SDL_KEYUP ) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_q:
#ifndef __EMSCRIPTEN__
                        exit(0);
#endif
                    case SDLK_LEFT:
                        mOnKeyReleasedCallback( ECommand::kLeft );
                        break;

                    case SDLK_RIGHT:
                        mOnKeyReleasedCallback( ECommand::kRight );
                        break;

                    case SDLK_UP:
                        mOnKeyReleasedCallback( ECommand::kUp );
                        break;

                    case SDLK_DOWN:
                        mOnKeyReleasedCallback( ECommand::kDown );
                        break;
		default:
		  break;
		}
            }

            if ( event.type == SDL_KEYDOWN ) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_LEFT:
                        mOnKeyPressedCallback( ECommand::kLeft );
                        break;
                    case SDLK_RIGHT:
                        mOnKeyPressedCallback( ECommand::kRight );
                        break;

                    case SDLK_UP:
                        mOnKeyPressedCallback( ECommand::kUp );
                        break;

                    case SDLK_DOWN:
                        mOnKeyPressedCallback( ECommand::kDown );
                        break;
		default:
		  break;
                }
            }

        }
}

  void CRenderer::render( const CGame& game, long ms ) {

    SDL_Rect rect;
    rect = { 0, 0, 640, 480 };
    int px = game.x / 40;
    int py = game.y / 40;
    
    SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 0, 0 ) );
    
    switch ( game.gameState ) {
    case CGame::EGameState::kTitleScreen:
      rect = { 320, 240, 640, 480 };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 255, 0 ) );
      break;
      
    case CGame::EGameState::kGame:
      
      rect = SDL_Rect{ 0, 0, 40, 40 };
      rect.x = game.x * 2;
      rect.y = game.y * 2;
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 0, 0 ) );


      rect = SDL_Rect{ 480, 0, 40, 40 };
      rect.y = game.timeEllapsed / ( 15000 / 480 );
      rect.h = 480 - rect.y;
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 128, 0, 0 ) );
      
      rect.h = 40;

        for ( const auto& particle : game.mParticles ) {

            rect.x = particle.mX * 2;
	        rect.y = particle.mY * 2;
            rect.w = 40;
            rect.h = 40;

            SDL_FillRect( video, &rect, SDL_MapRGB( video->format, particle.mShade, particle.mShade, particle.mShade ) );
        }
      
      break;
      
    case CGame::EGameState::kVictory:
        rect = SDL_Rect{ 0,0, 640, 480 };
        SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 0, 255, 0 ) );
            break;
      
    case CGame::EGameState::kGameOver:
      rect = SDL_Rect{ 0,0, 640, 480 };
      SDL_FillRect( video, &rect, SDL_MapRGB( video->format, 255, 0, 0 ) );
      break;
      
      
    }
    
    
    SDL_Flip(video);
  }
}
