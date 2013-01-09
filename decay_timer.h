#ifndef DECAY_TIMER_H
#define DECAY_TIMER_H
#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "SFML/Window.hpp"

class decay_timer
{
    public:
            decay_timer(sf::RenderWindow &App);
            float get_time();
            void update(float x, float y);
            void draw();
            void set_bar(int n_bar);
            int get_bar();
    private:
        ResourceManager GameResource;
        sf::RenderWindow *pApp;
        sf::Sprite m_Sprite;
        sf::Sprite m_Border;
        sf::Clock m_Clock;
        int m_current_bar;

};

#endif
