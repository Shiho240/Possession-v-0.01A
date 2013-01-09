#ifndef GAME_H
#define GAME_H

//#include "SpriteAdapter.h"
#include "screen.h"
#include <iostream>
#include "Box2D/Box2D.h"
#include "Level.h"
#include <sstream>
#include "Player.h"
#include "Enemy.h"
#include "projectiles.h"
//#include "Entity.h"


using namespace sf;

class Game : public Screen //, public Entity
{
    public:
        Game();
        Game(sf::RenderWindow &App);

        ~Game();
        void loadlevel(Level &level, int lvl,  b2World &world);
        //Level GetLevel() {return m_level;};
        //void setlevel(Level level) {m_level = level;};
        virtual int Run();
    protected:
    private:
        //Level m_level;
        ResourceManager GameResource;
        sf::Image BackgroundImage;
        sf::Sprite BackgroundSprite;
        sf::RenderWindow *pApp;
};


#endif // SCREEN_H
