#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H
#include "projectiles.h"
#include "Player.h"
#include "Enemy.h"

class projectile_manager : Entity
{
    public:
        projectile_manager(sf::RenderWindow &App, b2World &world);
        void create_projectile(Player &p, int facing);
        void update(int facing);
        void draw();
        void destroy();
        virtual int getEntityType() {return 4;};
        int count;
    private:
        std::vector<projectile> m_projectiles;
        ResourceManager GameResource;
        sf::RenderWindow *pApp;
        b2World *m_world;
};

#endif
