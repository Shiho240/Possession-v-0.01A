#include "Enemy.h"

Enemies::Enemies(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, std::vector <std::vector< Object > > allObjects)
{
    //Enemy enemy(int hp, sf::Sprite Sprite, b2BodyDef bodDef, b2PolygonShape dynamicBox, b2FixtureDef fixtureDef, b2Body *body);
    Object object;
    ///These are the Nodes for the enemy types
    std::vector<Enemy> player;
    std::vector<Enemy> ranger;
    std::vector<Enemy> soldier;
    std::vector<Enemy> lancer;
    std::vector<Enemy> necromancer;
    std::vector<Enemy> mage;
    std::vector<Enemy> controller;
    std::vector<Enemy> boss;
    std::vector< sf::Key::Code > keybindings;

    //std::vector< std::vector < Enemy > >::iterator Iobject=allObjects.begin();
//    Object zobject;
    //std::vector< std::vector < Object> >::iterator Iobject=allObjects.begin();
    std::vector <std::vector< Enemy > > temp_Enemies;
for(int i = 1; i != allObjects.size(); i++)
    {
   for(std::vector < Object>::iterator it = allObjects[i].begin(); it != allObjects[i].end(); it++)
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

        playerDef.type = b2_dynamicBody;
        playerDef.position.Set((x/10.0f), -y/10.0f);

        //create body in world
        playerBody = world.CreateBody(&playerDef);

        ///set as box is half of the object size devided by the scaling factor
        dynamicBox.SetAsBox(1.0f, 1.0f);
        fixtureDef.shape = &dynamicBox;
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.1f;


        if(object.name == "Ranger")
        {
                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                ranger.push_back(*m_head);
        }
        else if(object.name == "Soldier")
        {

                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                soldier.push_back(*m_head);
        }
        else if(object.name == "Lancer")
        {
                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                lancer.push_back(*m_head);
        }
        else if(object.name == "Necromancer")
        {

                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                necromancer.push_back(*m_head);
        }
        else if(object.name == "Mage")
        {
                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                mage.push_back(*m_head);
        }
        else if(object.name == "Controller")
        {
                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                controller.push_back(*m_head);
        }
        else if(object.name == "Boss")
        {
                m_head = new Enemy(70, Sprite, playerDef, dynamicBox, fixtureDef, playerBody,i);
                boss.push_back(*m_head);
        }
        else
        {
               std::cout<<"Commander?"<<"\n";
                //i++;
        }
    }
    }

        temp_Enemies.push_back(player);
        temp_Enemies.push_back(ranger);
        temp_Enemies.push_back(soldier);
        temp_Enemies.push_back(lancer);
        temp_Enemies.push_back(necromancer);
        temp_Enemies.push_back(mage);
        temp_Enemies.push_back(controller);
        temp_Enemies.push_back(boss);

        m_Sprites = temp_Enemies;
        cout << "kjasgdfjagdjasgfja\n";
}

Enemies::~Enemies()
{
    //delete this;
    cout << "EnemieS destructor called" << endl;
}

void Enemies::update()
{
for(int i = 1; i != (m_Sprites.size() - 1); i++)
    {
   for(int j = 0; j<m_Sprites[i].size(); j++)
    {
        Enemy &temp = m_Sprites[i][j];
        x = temp.playerBody->GetPosition().x;
        y = -temp.playerBody->GetPosition().y;
        temp.m_Sprite.SetPosition(x*10.0f + 5.0f, y*10.0f + 3.0f);
        //SIMPLE AI
       Enemy *e = (Enemy*)temp.playerBody->GetUserData();
        if (e->m_hp <= 50)
        {
            //cout << "Erasing Enemy" << endl;
            e->dead = true;
            temp.dead = true;
            ///Destroy and recreate fixture of new size
            temp.m_Sprite.SetSubRect(sf::IntRect(50,150,75,175));
            e->playerBody->DestroyFixture(e->m_fixture);
            temp.create_dead_fixture();

        }
        if(temp.AIClock.GetElapsedTime() >= 2 && !temp.dead)
        {
            temp.AIClock.Reset();
            if(temp.turnaround)
                temp.turnaround = 0;
            else
                temp.turnaround = 1;
        }

        if(!temp.turnaround && !temp.dead)
        {
            //temp.playerBody->ApplyLinearImpulse(b2Vec2(-0.1f,0.0f),playerBody->GetWorldCenter());
            //Checking for crazy speeds
            temp.playerBody->SetLinearVelocity(b2Vec2(-0.5f,0.0f));
            if(temp.playerBody->GetLinearVelocity().x < -0.5f)
            {
                temp.playerBody->SetLinearVelocity(b2Vec2(-0.5f,0.0f));
            }
            if(temp.playerBody->GetLinearVelocity().y > 0.0f)
                temp.playerBody->SetLinearVelocity(b2Vec2(temp.playerBody->GetLinearVelocity().x,0.0f));
            temp.animate(.3,3,1);

        }
        else if(temp.turnaround && !temp.dead)
        {
            //temp.playerBody->ApplyLinearImpulse(b2Vec2(0.1f,0.0f),playerBody->GetWorldCenter());
            //Checking for crazy speeds
            temp.playerBody->SetLinearVelocity(b2Vec2(0.5f,0.0f));
            if(temp.playerBody->GetLinearVelocity().x > 0.5f)
            {
                temp.playerBody->SetLinearVelocity(b2Vec2(0.5f,0.0f));
            }
            if(temp.playerBody->GetLinearVelocity().y > 0.0f)
                temp.playerBody->SetLinearVelocity(b2Vec2(temp.playerBody->GetLinearVelocity().x,0.0f));
            temp.animate(.3,3,0);
        }

       if(e->possessed)
       {
        cout << "Enemy is possessed";
        temp.destroy();
        m_Sprites[i].erase(m_Sprites[i].begin()+j);
        j--;
        //e->playerBody->DestroyFixture(e->dead_fixture);
       }
        x = temp.playerBody->GetPosition().x;
        y = -temp.playerBody->GetPosition().y;

    }
    }
//ENEMY AI GOESSESSE HEREREERERE
}

void Enemies::draw()
{
for(int i = 1; i != m_Sprites.size(); i++)
    {
   for(std::vector < Enemy>::iterator it = m_Sprites[i].begin(); it != m_Sprites[i].end(); it++)
    {
        *m_head = *it;
        pApp->Draw(m_head->m_Sprite);
    }
    }
}

