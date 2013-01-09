#ifndef ENEMIES_H
#define ENEMIES_H

#include "entity.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "Level.h"

class Enemy : public Entity
{
public:
    Enemy(float hp, sf::Sprite Sprite, b2BodyDef bodDef, b2PolygonShape dynamicBox, b2FixtureDef fixtureDef, b2Body *body, int i)
    {
        m_hp = hp;
        m_body = body;
        m_Sprite = Sprite;
        m_bodDef = bodDef;
        m_dynamicBox = dynamicBox;
        m_fixtureDef = fixtureDef;
        playerBody = body;
        m_fixture = playerBody->CreateFixture(&m_fixtureDef);
        playerBody->SetUserData(this);
        frame = 0;
        turnaround = 0;
        dead = false;
        possessed = false;
        player_touching = false;
        Font.LoadFromFile("resources/fonts/pirulen.ttf");
        body_type = i;
    }
    ~Enemy()
    {
        cout << "Enemy Desctructor" << endl;
    }
    void animate(float interval, int totalFrames, int row)
    {
        if (AnimateClock.GetElapsedTime() >= interval)
        {
            //loop through frames
            if (frame == totalFrames)
                frame = 0;
            //set subrect
            m_Sprite.SetSubRect(sf::IntRect(frame * m_Sprite.GetSize().x, row * m_Sprite.GetSize().y + row, (frame * m_Sprite.GetSize().x) + m_Sprite.GetSize().x, (row * m_Sprite.GetSize().y + m_Sprite.GetSize().y + row))); //x1, y1, x2, y2
            //move to next frame
            frame++;
            //reset clock
            AnimateClock.Reset();
        }
    }
    ///**FOR COLLISION DETECTION***///
    int getEntityType()
    {
        return 2;
    }

    float get_hp(sf::RenderWindow &pApp)
    {
        sf::String HPDisplay("HP", Font, 10);
        HPDisplay.SetPosition(m_Sprite.GetPosition().x, m_Sprite.GetPosition().y);
        pApp.Draw(HPDisplay);
        cout << m_hp << endl;
        return m_hp;
    }

    float set_hp(float change)
    {
       // cout << "CHANGING" << endl;
        m_hp += change;
    }
    void create_dead_fixture()
    {
        b2PolygonShape n_box;
        n_box.SetAsBox(1.0f, 0.10f);
        m_fixtureDef.shape = &n_box;
        dead_fixture = playerBody->CreateFixture(&m_fixtureDef);
    }
    void destroy()
    {
      playerBody->DestroyFixture(dead_fixture);
      playerBody->GetWorld()->DestroyBody(playerBody);
      playerBody = NULL;
    }
    float getx(){return m_Sprite.GetPosition().x;};
    float gety(){return m_Sprite.GetPosition().y;};
    float m_hp;
    sf::Clock AnimateClock; //for animation
    sf::Clock AIClock;
    int frame;  //for animation
    int facing; //for animation
    bool dead;
    bool turnaround;
    bool player_touching;
    b2Fixture* m_fixture;
    b2Fixture* dead_fixture;
    sf::Font Font;
    b2Body *m_body;
    sf::Sprite m_Sprite;
    b2Body* playerBody;
    b2BodyDef m_bodDef;
    b2PolygonShape m_dynamicBox;
    b2FixtureDef m_fixtureDef;
    bool possessed;
    int body_type;

    //get function for body
    //get function for velocities

private:

};



class Enemies : public Entity
{
public:
    Enemies(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, std::vector <std::vector< Object > > allObjects);
    ~Enemies();
    void update();
    void draw();
protected:

private:



    Enemy * m_head;

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
    std::vector <std::vector< Object > > m_Enemies;
    std::vector <std::vector< Enemy > > m_Sprites;


};

#endif

