#include "projectile_manager.h"

projectile_manager::projectile_manager(sf::RenderWindow &App, b2World &world)
{
    pApp = &App;
    m_world = &world;
}
void projectile_manager::create_projectile(Player &p, int facing)
{
    m_projectiles.push_back(projectile(*pApp,*m_world,p.getx()*10+20.0f,p.gety()*10-15.0f, facing, p.get_body_type()));
    cout << p.get_body_type() << endl;
    count++;

}

void projectile_manager::draw()
{
for(std::vector <projectile>::iterator it = m_projectiles.begin(); it != m_projectiles.end(); it++)
    {

        if(it->projectile_type == 1)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Arrow"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,100,100,125));
        }
        else if(it->projectile_type == 2)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Sword"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,0,100,25));
        }
        //lancer
        else if(it->projectile_type == 3)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Sword"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,50,100,75));
        }
        //necromancer
        else if(it->projectile_type == 4)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Sword"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,175,100,200));
        }
        //mage
        else if(it->projectile_type == 5)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Sword"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,125,100,150));
        }
        //controller
        else if(it->projectile_type == 6)
        {
            it->m_Sprite.SetImage(GameResource.getImage("Sword"));
            it->m_Sprite.SetSubRect(sf::IntRect(75,150,100,175));
        }
        pApp->Draw(it->m_Sprite);
        it->projectileBody->SetUserData(this);
    }
}

void projectile_manager::update(int facing)
{
    for(int i = 0; i != (m_projectiles.size()); i++)
    {
        m_projectiles[i].update(facing);
    }

    for(std::vector <projectile>::iterator it = m_projectiles.begin(); it != m_projectiles.end(); it++)
    {
        if (it->dead == true)
        {
        it->destroy();
        it--;
        m_projectiles.erase(it +1);
        }
    }
}
void projectile_manager::destroy()
{
     for(std::vector <projectile>::iterator it = m_projectiles.begin(); it != m_projectiles.end(); it++)
    {
       it->destroy();
    }
}

