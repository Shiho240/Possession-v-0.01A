#include "Weapons.h"

Weapon::Weapon(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, float xPos, float yPos)
{
    pApp = &App;
    x = xPos;
    y = yPos;
    xScaled = x/10.0f - 5.0f;
    yScaled = -y/10.0f + 1.5f;
    //frame = 0;
    //facing = 1; //direction player is facing, 1 is right, 0 is left
    //can_move = true;
    cout << "asdfsdfaadsfasdf\n";
    Sprite.SetImage(GameResource.getImage("Arrow"));
    Sprite.SetSubRect(sf::IntRect(0,0,25,25));
    Sprite.SetPosition(x, y);
    //Sprite.SetCenter(Sprite.GetSize() / 2.f);
    playerDef.type = b2_dynamicBody;

    playerDef.position.Set(xScaled -3, yScaled -4);  //scaling factor for box2d
    cout << "asdfadfsdfadfsafdasf\n";

    //create body in world
    playerBody = world.CreateBody(&playerDef);
    cout << "asdfadfsdfadfsafdasf\n";


    dynamicBox.SetAsBox(1.25f, 1.25f);
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.5f;
    fixtureDef.friction = .05f;
    playerBody->CreateFixture(&fixtureDef);
    playerBody->SetUserData(this);
    cout << "the object is there\n";
}

Weapon::~Weapon()
{
    delete this;
}

void Weapon::draw()
{
    pApp->Draw(Sprite);
}

void Weapon::animate(float interval, int totalFrames, int row)
{
    if (AnimateClock.GetElapsedTime() >= interval)
    {
        //loop through frames
        if (frame == totalFrames)
            frame = 0;
        //set subrect
        Sprite.SetSubRect(sf::IntRect(frame * Sprite.GetSize().x, row * Sprite.GetSize().y + row, (frame * Sprite.GetSize().x) + Sprite.GetSize().x, (row * Sprite.GetSize().y + Sprite.GetSize().y + row))); //x1, y1, x2, y2
        //move to next frame
        frame++;
        //reset clock
        AnimateClock.Reset();
    }

}

///These are the functions for the PLURAL WEAPONS CLASSES
void Weapons::update(Terrains &terrain, Enemies &enemies, Player &player, sf::View &view, b2World &World)
{
        player.update(view);
        terrain.update();
        enemies.update();





        if(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Space"]) && (asdf == false))
        {
            cout<<"Imma make wesley a gel banana\n";
            asdf = true;
            cout << "true\n";
            // instantiate here
            ///Update position of Sprite
            Weapon * projectile;
            x = player.playerBody->GetPosition().x;//*pClock->GetElapsedTime();     //check scaling factors
            y = -player.playerBody->GetPosition().y;//*pClock->GetElapsedTime();
            float xScaled = x*10.0f + 5.0f;
            float yScaled = y*10.0f + 5.0f;
            cout << xScaled << " lkajdsfljal " << yScaled << " adsfadfasf \n";
            projectile = new Weapon(*pApp, *pClock, World, xScaled, yScaled);
        }
         if(!pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Space"]))
         {
             asdf = false;
         }
        ///here we can do the things we need to instatiate the projectiles ect ect.
        //if space bar is pressed we should have the projectiles instatiated and start their movement.
        ///Use is clicked for the spacebar. so we need don't infinate loop instantiations.
        //is pressed will do bad things.
        //so if it is clicked we need to do

}

