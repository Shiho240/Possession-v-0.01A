#ifndef WEAPONS_H
#define WEAPONS_H

#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "SFML/Window.hpp"
#include "mKeyboard.h"
#include "Enemy.h"
#include "Player.h"
#include "Terrain.h"
//#include "MyContactListener.h"
//#include “cpGUI.h”


class Weapon : public Entity
{
    public:
        Weapon(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, float xPos, float yPos);
        ~Weapon();

        void draw();
        void animate(float interval, int totalFrames, int row);
        ///******Contact Overloaded*************///
        ///void startContact() { m_contacting = true; }
        //void endContact() { m_contacting = false; }
        int getEntityType() {return 4;}
        //void handleEnemyContact(Enemy &enemy);
    protected:
    private:
        mKeyboard keyboard;
        ResourceManager GameResource;
        float x;
        float y;
        float xScaled;  //scaling factor for box2d
        float yScaled;  //scaling factor for box2d
        int frame;  //for animation
        int facing; //for animation
        bool can_move; //for future use
        bool m_contacting;
        b2World *pWorld;
        sf::RenderWindow *pApp;
        sf::Clock *pClock;
        sf::Sprite Sprite;
        sf::Clock AnimateClock; //for animation
        b2Body* playerBody;
        b2BodyDef playerDef;
        b2PolygonShape dynamicBox;
        b2FixtureDef fixtureDef;
        b2ContactEdge* edge;
};

class Weapons : public Entity
{
        public:

            Weapons(sf::RenderWindow &App){pApp = &App; asdf = false;}
            ~Weapons(){}
            void update(Terrains &terrain, Enemies &enemies, Player &player, sf::View &view,  b2World &World);

        private:
                bool asdf;
                mKeyboard keyboard;
                std::vector<Weapon> m_Weapons;
                std::vector<sf::Sprite> m_Sprite;
                b2World *pWorld;
                sf::RenderWindow *pApp;
                sf::Clock *pClock;
                float x;
                float y;

};
#endif
