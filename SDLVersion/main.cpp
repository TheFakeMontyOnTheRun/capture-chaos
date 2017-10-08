#include <functional>
#include <memory>
#include <vector>
#include <iostream>

#include "Common.h"
#include "CGame.h"
#include "CRenderer.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

#endif



#ifdef __EMSCRIPTEN__
void enterFullScreenMode() {
    EmscriptenFullscreenStrategy s;
    memset(&s, 0, sizeof(s));
    s.scaleMode = EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT;
    s.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE;
    s.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
    emscripten_enter_soft_fullscreen(0, &s);
}
#endif

std::shared_ptr<odb::CRenderer> renderer;
odb::CGame game;

void gameLoopTick() {
  renderer->render( game, 33 );
    renderer->handleSystemEvents();
    game.tick( 33 );
}


int main() {
    renderer = std::make_shared<odb::CRenderer>( game.getKeyPressedCallback(), game.getKeyReleasedCallback() );

#ifdef __EMSCRIPTEN__
    enterFullScreenMode();
  emscripten_set_main_loop( gameLoopTick, 30, 1 );
#else
    while ( true ) {
        gameLoopTick();
        renderer->sleep( 33 );
    }
#endif
    return 0;
}
