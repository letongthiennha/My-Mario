/**
 * @file MontyMole.cpp
 * @author Prof. Dr. David Buzatto
 * @brief MontyMole class implementation.
 * 
 * @copyright Copyright (c) 2024
 */
#include "Baddie.h"
#include "CollisionProbe.h"
#include "Direction.h"
#include "GameWorld.h"
#include "MontyMole.h"
#include "raylib.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include "SpriteState.h"
#include <iostream>
#include <map>
#include <valarray>

MontyMole::MontyMole( Vector2 pos, Vector2 dim, Vector2 vel, Color color ) :
    Baddie( pos, dim, vel, color, 0.1, 2, 1, 200 ),
    maxJumpTime( 6 ),
    jumpTime( GetRandomValue( 2, maxJumpTime ) ),
    jumpTimeAcum( 0 ) {
}

MontyMole::~MontyMole() = default;

void MontyMole::update() {
    
    const float delta = GetFrameTime();

    if ( state == SPRITE_STATE_ACTIVE ) {

        frameAcum += delta;
        if ( frameAcum >= frameTime ) {
            frameAcum = 0;
            currentFrame++;
            currentFrame %= maxFrames;
        }

        if ( vel.x >= 0 ) {
            facingDirection = DIRECTION_RIGHT;
        } else {
            facingDirection = DIRECTION_LEFT;
        }

        jumpTimeAcum += delta;
        if ( jumpTimeAcum >= jumpTime ) {
            jumpTimeAcum = 0;
            vel.y = -200;
            jumpTime = GetRandomValue( 2, maxJumpTime );
        }

        pos.x = pos.x + vel.x * delta;
        pos.y = pos.y + vel.y * delta;

        vel.y += GameWorld::gravity;

    } else if ( state == SPRITE_STATE_DYING ) {

        dyingFrameAcum += delta;
        if ( dyingFrameAcum >= dyingFrameTime ) {
            dyingFrameAcum = 0;
            currentDyingFrame++;
            if ( currentDyingFrame == maxDyingFrames ) {
                state = SPRITE_STATE_TO_BE_REMOVED;
            }
        }

        pointsFrameAcum += delta;
        if ( pointsFrameAcum >= pointsFrameTime ) {
            pointsFrameAcum = pointsFrameTime;
        }

    }

    updateCollisionProbes();

}

void MontyMole::draw() {

    std::map<std::string, Texture2D> &textures = ResourceManager::getTextures();

    if ( state == SPRITE_STATE_ACTIVE ) {
        const char dir = facingDirection == DIRECTION_RIGHT ? 'R' : 'L';
        DrawTexturePro( textures[std::string( TextFormat( "montyMole%d%c", currentFrame, dir ) )],
                        Rectangle( 0, 0, dim.x, dim.y ),
                        Rectangle( pos.x + dim.x / 2, pos.y + dim.y / 2, dim.x, dim.y ),
                        Vector2( dim.x / 2, dim.y / 2 ), angle, WHITE );
    } else if ( state == SPRITE_STATE_DYING ) {
        DrawTexture( textures[std::string( TextFormat( "puft%d", currentDyingFrame ) )], pos.x, pos.y, WHITE );
        const std::string pointsStr = TextFormat( "guiPoints%d", earnedPoints );
        DrawTexture( textures[pointsStr],
                     pos.x + dim.x / 2 - textures[pointsStr].width / 2,
                     pos.y - textures[pointsStr].height - ( 50 * pointsFrameAcum / pointsFrameTime ),
                     WHITE );
    }

    if ( GameWorld::debug ) {
        cpN.draw();
        cpS.draw();
        cpE.draw();
        cpW.draw();
    }

}

void MontyMole::followTheLeader( Sprite* sprite ) {

    if ( pos.x < sprite->getX() - 60 ) {
        vel.x = std::abs( vel.x );
    } else if ( pos.x > sprite->getX() + 60 ) {
        vel.x = -std::abs( vel.x );
    }

}
