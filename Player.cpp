#include "Player.h"

Player::Player(sf::RenderWindow &App, sf::Clock &Clock, b2World &world, float xPos, float yPos)
{

    pApp = &App;
    x = xPos;
    y = yPos;
    xScaled = x/10.0f - 5.0f;
    yScaled = -y/10.0f + 1.5f;
    frame = 0;
    facing = 1; //direction player is facing, 1 is right, 0 is left
    can_move = true;
    is_ghost = true;
    Sprite.SetImage(GameResource.getImage("player"));
    Sprite.SetSubRect(sf::IntRect(0,0,25,25));
    Sprite.SetPosition(x, y);
    d_timer = new decay_timer(*pApp);
    d_timer->set_bar(0);
    //Sprite.SetCenter(Sprite.GetSize() / 2.f);
    playerDef.type = b2_dynamicBody;
    playerDef.position.Set(xScaled, yScaled);  //scaling factor for box2d
    body_type = 0;
    //create body in world
    playerBody = world.CreateBody(&playerDef);


    dynamicBox.SetAsBox(1.25f, 1.25f);
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 0.5f;
    fixtureDef.friction = .05f;
    playerBody->CreateFixture(&fixtureDef);
    playerBody->SetUserData(this);
}

Player::~Player()
{
    delete this;
}

void Player::update(sf::View &view)
{
    //If the timer runs out go ghost form
    if(d_timer->get_bar() <= 0 && !is_ghost)
    {
        go_ghost_form();
    }
    b2Vec2 vel = playerBody->GetLinearVelocity();
    edge = playerBody->GetContactList();
    if(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Left"]) && vel.x > -4)
    {
        playerBody->ApplyLinearImpulse(b2Vec2(-0.1f,0.0f),playerBody->GetWorldCenter());
        animate(.2,3,1);
        facing = 0;
    }
    if(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Right"]) && vel.x < 4)
    {

        playerBody->ApplyLinearImpulse(b2Vec2(0.1f,0.0f),playerBody->GetWorldCenter());
        animate(.2,3,0);
        facing = 1;
    }

    if(((pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Up"]))) && edge && vel.y < 10)
    {
        playerBody->ApplyLinearImpulse(b2Vec2(0.0f,5.0f),playerBody->GetWorldCenter());
    }
    //attack animation
    if(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Space"]))
    {
        if(facing == 0)
        {
            animate(.2,3,3);
            //frame = 0;
        }
        else
        {
            animate(.2,3,2);
            //frame = 0;
        }
    }
    //if no keys being pressed return to idle frame
    if(!(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Left"]) || pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Right"]) || pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Space"])))
    {
        if(facing == 0)
        {
            animate(.3,3,1);
            frame = 2;
        }
        else
        {
            animate(.3,3,0);
            frame = 2;
        }
    }

    //Release body and go back into ghost form
    if((pApp->GetInput().IsKeyDown(sf::Key::R)) && !is_ghost)
    {
        go_ghost_form();
    }

    ///Update position of Sprite
    x = playerBody->GetPosition().x;//*pClock->GetElapsedTime();     //check scaling factors
    y = -playerBody->GetPosition().y;//*pClock->GetElapsedTime();
    xScaled = x*10.0f + 5.0f;
    yScaled = y*10.0f + 5.0f;
    Sprite.SetPosition(xScaled, yScaled);

    ///Map is small on my map so I'm not updating the y position.
    ///Also going to have some if statements for the edge of map conditionals ect ect. Can probably extern those.
    sf::Vector2f  Center = view.GetCenter();
    float viewy = Center.y;
    float viewx = Center.x;
    view.SetCenter(x*10.0f, y*10.0f);
    d_timer->update(xScaled-150,yScaled-150);
}

void Player::draw()
{
    pApp->Draw(Sprite);
    d_timer->draw();
}

void Player::animate(float interval, int totalFrames, int row)
{
    /*
        TO-DO: dynamically adjust animation speed by distance covered, etc.

        Requirements before use:
        Private Members:
            sf::Clock AnimateClock;
            int frame - initialized to 0, # of frames in animation
        Assumes that the tiles  for the sprite's tileset are in one row/line
    */

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

void Player::handleEnemyContact(Enemy &enemy)
{
    enemy.set_hp(-10);
    float enemyhp = enemy.get_hp(*pApp);

}

void Player::change_sprite(sf::Sprite &sprite)
{
    Sprite.SetImage(*sprite.GetImage());
}

bool Player::pressing_possess()
{
    if(pApp->GetInput().IsKeyDown(sf::Key::E))
    {
        return true;
    }
    else
        return false;
}

void Player::go_ghost_form()
{
    Sprite.SetImage(GameResource.getImage("player"));
    is_ghost = true;
    d_timer->set_bar(0);
}

void Player::set_ghost_false()
{
    is_ghost = false;
    d_timer->set_bar(100);
}

bool Player::get_ghost_state()
{
    return is_ghost;
}

bool Player::pressing_attack()
{
    if(pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Space"]))
        return true;
    else
        return false;
}

void Player::set_decay_timer(int n_time)
{
    d_timer->set_bar(n_time);
}
