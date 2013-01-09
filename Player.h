#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "SFML/Window.hpp"
#include "mKeyboard.h"
#include "enemy.h"
#include "decay_timer.h"
//#include “cpGUI.h”

class Player : public Entity
{
    public:
        Player(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, float xPos, float yPos);
        ~Player();
        void update(sf::View &view);
        void draw();
        void animate(float interval, int totalFrames, int row);
        ///******Contact Overloaded*************///
        void startContact() { m_contacting = true; }
        void endContact() { m_contacting = false; }
        int getEntityType() {return 1;}
        void handleEnemyContact(Enemy &enemy);
        b2Body* playerBody;
        void change_sprite(sf::Sprite &sprite);
        bool pressing_possess();
        void go_ghost_form();
        void set_ghost_false();
        bool get_ghost_state();
        bool pressing_attack();
        float getx(){return x;};
        float gety(){return y;};
        void set_decay_timer(int n_time);
        bool getfacing(){return facing;};
        int get_body_type(){return body_type;};
        void set_body_type(int b){body_type == b;};
                int body_type;
    protected:
    private:
        decay_timer *d_timer;
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
        bool is_ghost;
        sf::RenderWindow *pApp;
        sf::Clock *pClock;
        sf::Sprite Sprite;
        sf::Clock AnimateClock; //for animation

        b2BodyDef playerDef;
        b2PolygonShape dynamicBox;
        b2FixtureDef fixtureDef;
        b2ContactEdge* edge;
};

#endif
