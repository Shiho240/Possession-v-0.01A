#include "Terrain.h"

Terrains::Terrains(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, std::vector< Object > allTerrains)
{
    //Terrain enemy(int hp, sf::Sprite Sprite, b2BodyDef bodDef, b2PolygonShape dynamicBox, b2FixtureDef fixtureDef, b2Body *body);
    Object object;
    ///These are the Nodes for the enemy types
    std::vector<Terrain> terrain;
    std::vector< sf::Key::Code > keybindings;

    //std::vector< std::vector < Terrain > >::iterator Iobject=allTerrains.begin();
//    Object zobject;
    //std::vector< std::vector < Object> >::iterator Iobject=allTerrains.begin();
for (std::vector < Object>::iterator it = allTerrains.begin(); it != allTerrains.end(); it++)
{
        object = *it;
        //cout << object.name << "\n";
        float posY = object.rect.Top;
        float posX = object.rect.Left;
        x = posX;
        y = posY;
       // cout << "enemy pos: " << x << " " << y << endl;
        pApp = &App;

        Sprite.SetImage(GameResource.getImage(object.name));
        Sprite.SetSubRect(sf::IntRect(0,25,25,50));
        Sprite.SetPosition(x, y);
        //Sprite.SetCenter(Sprite.GetSize() / 2.f);

        playerDef.type = b2_staticBody;
        playerDef.position.Set((x/10.0f), -y/10.0f);

        //create body in world
        playerBody = world.CreateBody(&playerDef);

        ///set as box is half of the object size devided by the scaling factor
        dynamicBox.SetAsBox(1.0f, 1.0f);
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.1f;


        if(object.name != "Ranger" && object.name != "Soldier" && object.name != "Lancer" && object.name != "Necromancer" && object.name != "Mage" && object.name != "Controller" && object.name != "Boss")
        {
                m_head = new Terrain(10, Sprite, playerDef, dynamicBox, fixtureDef, playerBody);
                terrain.push_back(*m_head);
        }
        else
        {
               std::cout<<"Commander?"<<"\n";

        }

        m_Sprites = terrain;
}
}

Terrains::~Terrains()
{
    //delete this;
}

void Terrains::update()
{
for(int i = 0; i != m_Sprites.size(); i++)
    {
        Terrain &temp = m_Sprites[i];
        x = temp.playerBody->GetPosition().x;
        y = -temp.playerBody->GetPosition().y;
        temp.m_Sprite.SetPosition(x*10.0f + 5.0f, y*10.0f + 3.0f);
        //SIMPLE AI
        if (temp.m_hp <= 0)
            cout << "I am Dead" << endl;
        if(temp.AIClock.GetElapsedTime() >= 2)
        {
            temp.AIClock.Reset();
            if(temp.turnaround)
                temp.turnaround = 0;
            else
                temp.turnaround = 1;
        }

        if(!temp.turnaround)
        {
            temp.playerBody->ApplyLinearImpulse(b2Vec2(-0.1f,0.0f),playerBody->GetWorldCenter());
            temp.animate(.3,3,1);

        }
        else if(temp.turnaround)
        {
            temp.playerBody->ApplyLinearImpulse(b2Vec2(0.1f,0.0f),playerBody->GetWorldCenter());
            temp.animate(.3,3,0);
        }
        x = temp.playerBody->GetPosition().x;
        y = -temp.playerBody->GetPosition().y;
    }
//ENEMY AI GOESSESSE HEREREERERE
}

void Terrains::draw()
{
for(std::vector < Terrain >::iterator it = m_Sprites.begin(); it != m_Sprites.end(); it++)
    {
        *m_head = *it;
        pApp->Draw(m_head->m_Sprite);
    }
}


