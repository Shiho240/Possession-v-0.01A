#ifndef TERRAINS_H
#define TERRAINS_H

#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "Level.h"

class Terrain : public Entity
{
public:
    Terrain(float hp, sf::Sprite Sprite, b2BodyDef bodDef, b2PolygonShape dynamicBox, b2FixtureDef fixtureDef, b2Body *body)
    {
        m_hp = hp;
        m_body = body;
        m_Sprite = Sprite;
        m_bodDef = bodDef;
        m_dynamicBox = dynamicBox;
        m_fixtureDef = fixtureDef;
        playerBody = body;
        playerBody->CreateFixture(&m_fixtureDef);
        playerBody->SetUserData(this);
        frame = 0;
        turnaround = 0;
        Font.LoadFromFile("resources/fonts/pirulen.ttf");
    }
    ~Terrain() {}
    void animate(float interval, int totalFrames, int row)
    {

    }
    ///**FOR COLLISION DETECTION***///
    int getEntityType()
    {
        return 3;
    }

    float get_hp(sf::RenderWindow &pApp)
    {
        sf::String HPDisplay("HP", Font, 10);
        HPDisplay.SetPosition(m_Sprite.GetPosition().x, m_Sprite.GetPosition().y);
        pApp.Draw(HPDisplay);
        cout << m_hp << endl;
    }

    float set_hp(float change)
    {
        cout << "CHANGING" << endl;
        m_hp += change;
    }
    float m_hp;
    sf::Clock AnimateClock; //for animation
    sf::Clock AIClock;
    int frame;  //for animation
    int facing; //for animation
    sf::Font Font;
    bool turnaround;
    b2Body *m_body;
    sf::Sprite m_Sprite;
    b2Body* playerBody;
    b2BodyDef m_bodDef;
    b2PolygonShape m_dynamicBox;
    b2FixtureDef m_fixtureDef;

    //get function for body
    //get function for velocities

private:

};



class Terrains : public Entity
{
public:
    Terrains(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, std::vector< Object > allTerrains);
    ~Terrains();
    void update();
    void draw();
protected:

private:



    Terrain * m_head;

    ResourceManager GameResource;
    float x;
    float y;
    sf::RenderWindow *pApp;
    sf::Clock *pClock;
    sf::Sprite Sprite;
    b2Body* playerBody;
    b2BodyDef playerDef;
    b2PolygonShape dynamicBox;
    b2FixtureDef fixtureDef;
    std::vector< Terrain > m_Sprites;


};

#endif


