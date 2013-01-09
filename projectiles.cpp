#include "projectiles.h"

projectile::projectile(sf::RenderWindow &App, b2World &world, float xPos, float yPos, int facing, int n_projectile_type)
{
        //ranger
        projectile_type = n_projectile_type;
        float xOffset;
        if(facing == 0)
            xOffset = -4.5;
        else
            xOffset = 1;
        if(projectile_type == 1)
        {
            m_Sprite.SetImage(GameResource.getImage("Arrow"));
            m_Sprite.SetSubRect(sf::IntRect(75,100,100,125));
            m_Sprite.SetPosition(xPos, yPos);
            m_fixtureDef.density = 0.0f;
            AClockTime = 2;
        }
        //soldier
        else if(projectile_type == 2)
        {
            m_Sprite.SetImage(GameResource.getImage("Sword"));
            m_Sprite.SetSubRect(sf::IntRect(75,0,100,25));
            m_Sprite.SetPosition(xPos, yPos-50);
            AClockTime = 0.2;
            m_fixtureDef.density = 2.0f;
        }
        //lancer
        else if(projectile_type == 3)
        {
            m_Sprite.SetImage(GameResource.getImage("Sword"));
            m_Sprite.SetSubRect(sf::IntRect(75,50,100,75));
            m_Sprite.SetPosition(xPos, yPos-50);
            AClockTime = 0.5;
            m_fixtureDef.density = 2.0f;
        }
        //necromancer
        else if(projectile_type == 4)
        {
            m_Sprite.SetImage(GameResource.getImage("Sword"));
            m_Sprite.SetSubRect(sf::IntRect(75,175,100,200));
            m_Sprite.SetPosition(xPos, yPos-50);
            AClockTime = 1.5;
            m_fixtureDef.density = 2.0f;
        }
        //mage
        else if(projectile_type == 5)
        {
            m_Sprite.SetImage(GameResource.getImage("Sword"));
            m_Sprite.SetSubRect(sf::IntRect(75,125,100,150));
            m_Sprite.SetPosition(xPos, yPos-50);
            AClockTime = 2.0;
            m_fixtureDef.density = 2.0f;
        }
        //controller
        else if(projectile_type == 6)
        {
            m_Sprite.SetImage(GameResource.getImage("Sword"));
            m_Sprite.SetSubRect(sf::IntRect(75,150,100,175));
            m_Sprite.SetPosition(xPos, yPos-50);
            AClockTime = 1.5;
            m_fixtureDef.density = 2.0f;
        }
        pApp = &App;
        dead = false;

        if(facing == 0)
        m_Sprite.FlipX(true);

        m_bodDef.type = b2_dynamicBody;
        m_bodDef.position.Set(xPos/10.0f + xOffset, -yPos/10.0f);

        projectileBody = world.CreateBody(&m_bodDef);
        contacting = false;
        m_dynamicBox.SetAsBox(0.25f, 0.25f);
        m_fixtureDef.shape = &m_dynamicBox;
        m_fixtureDef.friction = 0.0f;
        m_fixture = projectileBody->CreateFixture(&m_fixtureDef);
        projectileBody->SetUserData(this);
        cout << "Sprite Made" << endl;
        b2Vec2 v(7.0f,0.0f);
        b2Vec2 b(-7.0f,0.0f);
        Left = b;
        Right = v;
    if(facing == 0)
            projectileBody->SetLinearVelocity( Left );

            //projectileBody->ApplyLinearImpulse(b2Vec2(-5.0f,7.0f),projectileBody->GetWorldCenter());
    else
            projectileBody->SetLinearVelocity( Right );
            //projectileBody->ApplyLinearImpulse(b2Vec2(5.0f,7.0f),projectileBody->GetWorldCenter());
}

/*projectile::~projectile()
{
  projectileBody->DestroyFixture(m_fixture);

}*/

void projectile::draw()
{
    //cout << "Drawing projectile"<< " at " << m_Sprite.GetPosition().x << " " << m_Sprite.GetPosition().y << endl;
    pApp->Draw(m_Sprite);
}

void projectile::update(int facing)
{
projectileBody->SetLinearVelocity( projectileBody->GetLinearVelocity() );
    int x;
    int y;
    x = projectileBody->GetPosition().x;
    y = -projectileBody->GetPosition().y;
    edge = projectileBody->GetContactList();
        m_Sprite.SetPosition(x*10.0f, y*10.0f+25.0f);

    if(edge && !contacting)
    {
        contacting = true;
        DeadClock.Reset();
    }
    if((AClock.GetElapsedTime() >= AClockTime && !dead) || (DeadClock.GetElapsedTime() >= 0.1 && contacting))
    {
        dead = true;
    }


}

void projectile::mylif(Enemy &en)
{
    cout << en.m_hp << "\n";
    en.set_hp(-10);
    cout << en.m_hp << "\n";


}



void projectile::destroy()
{
      projectileBody->DestroyFixture(m_fixture);
      projectileBody->GetWorld()->DestroyBody(projectileBody);
      projectileBody = NULL;
}
