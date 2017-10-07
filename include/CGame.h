#ifndef CGAME_H
#define CGAME_H

namespace odb {

    class CParticle {
    public:
        CParticle( int aX, int aY, int aVX, int aVY, int aShade ):
                mX( aX ), mY( aY ), mVX( aVX ), mVY( aVY ), mShade( aShade )
        {}

        int mX = 0;
        int mY = 0;
        int mVX = 0;
        int mVY = 0;
        int mShade = 0;

        void update() {
            mX += mVX;
            mY += mVY;

            if ( mX <= 0 || mX >= 255 ) {
                mVX = mVX * -1;
            }

            if ( mY <= 0 || mY >= 255 ) {
                mVY = mVY * -1;
            }

        }
    };


    class CGame {
    public:

      enum class EGameState{ kTitleScreen, kGame, kVictory, kGameOver };

      EGameState gameState = EGameState::kTitleScreen;

      int x = 280;
      int y = 40;
      int vx = 0;
      int vy = 0;
      explicit CGame();
      long timeEllapsed = 0;
      int frame = 0;

        std::vector< CParticle > mParticles;

        void tick( long ms );
        CControlCallback getKeyPressedCallback();
        CControlCallback getKeyReleasedCallback();

        void init();
    };
}
#endif
