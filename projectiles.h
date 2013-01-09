#ifndef PROJECTILES_H
#define PROJECTILES_H
#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "Enemy.h"
#include "SFML/Window.hpp"
#include "mKeyboard.h"
class projectile : public Entity
{
    public:
        projectile(sf::RenderWindow &App, b2World &world, float xPos, float yPos, int facing, int n_projectile_type);
        //~projectile();
        void draw();
        void update(int facing);
        void mylif(Enemy &e);
        void animate(float interval, int totalFrames, int row);
        void destroy();
        int getEntityType() {return 4;};
        void set_dead(){dead = true;};
        sf::Sprite m_Sprite;
        b2Body* projectileBody;
        bool dead;
        int projectile_type;
    private:
        sf::RenderWindow *pApp;
        ResourceManager GameResource;
        sf::Clock AClock;
        sf::Clock DeadClock;
        bool contacting;
        b2ContactEdge* edge;
        b2Vec2 Left;
        b2Vec2 Right;
        int frame;  //for animation
        int facing; //for animation
        b2Fixture* m_fixture;
        b2Body *m_body;
        b2BodyDef m_bodDef;
        b2PolygonShape m_dynamicBox;
        b2FixtureDef m_fixtureDef;
        float AClockTime;
};

#endif
