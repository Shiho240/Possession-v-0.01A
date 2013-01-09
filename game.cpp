#include "game.h"
#include "mKeyboard.h"
#include "MyContactListener.h"
#include <SFML/Audio.hpp>
#include "resourcemanager.h"
#include "Terrain.h"
#include "Weapons.h"
#include "Possession_manager.h"
#include "projectile_manager.h"
#include "decay_timer.h"
Game::Game (sf::RenderWindow &App)
{
    pApp = &App;
}

Game::~Game ()
{
    //dtor
}

void Game::loadlevel(Level &level, int lvl,  b2World &world)
{
    string filename;
    std::stringstream ss;
    ss.str("");
	ss<<lvl;
    filename = "level" + ss.str() + ".tmx";
    level.LoadFromFile(filename, world);
}

int Game::Run ()
{
    Event Event2;
    ///instatiate Keyboard Bindings
    mKeyboard keyboard;
    ResourceManager resources;
    pApp->Close();
    pApp->Create(sf::VideoMode(keyboard.getwidth(),keyboard.gethight(),32), "SFML Game Template");
    pApp->SetFramerateLimit(300);
    int frameNumber = 0;
    int currentFPS = 0; //show this on screen
    int lastFPSFrame = 0;
    sf::Clock FPSClock;
    sf::Sprite Cursor;
    Cursor.SetImage(resources.getImage("Cursor"));



///Box2D Setup
    // Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);
	// Do we want to let bodies sleep?
	bool doSleep = true;
	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity, doSleep);
    //variables for step
    float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;



  int lvl = 2;
  bool level_cleared = false;
  View View(FloatRect(0,0,300,300));
  float view_max = 700;
  float view_min = 0;
  b2Vec2 vel;
  bool once = false;


    ///Setup and Initializations

    ///Music instatiation/player
    Music Music1;
    Music1.OpenFromFile("Reflectia.ogg");
    Music1.SetLoop(true);
    Music1.SetVolume(keyboard.getMusic());
    Music1.Play();

    ///Menu loop
    ///This is basically a while true, While the screen is being displayed.
    while (pApp->IsOpened())
    {
            //Cursor.SetPosition(sf::Event::MouseMoveEvent::X, sf::Event:MouseMoveEvent::Y);
///Box2D Setup
    // Define the gravity vector.
	b2Vec2 gravity(0.0f, -10.0f);
	// Do we want to let bodies sleep?
	bool doSleep = true;
	// Construct a world object, which will hold and simulate the rigid bodies.
	b2World world(gravity, doSleep);
    //variables for step
    float32 timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 6;
	int32 positionIterations = 2;
 //at global scope
  MyContactListener myContactListenerInstance;

  //in FooTest constructor
  world.SetContactListener(&myContactListenerInstance);

///

    ///Setup and Init per level

    Level level;
    loadlevel(level, lvl, world);
    level_cleared = false;


    Enemies *enemies;
    Player *player;
    sf::Clock *pClock;
    Terrains *terrain;
    Weapons *projectile1;
      ///REMOVE THIS///

    Object object;
    vector<vector<Object> > entityObjects;
    entityObjects = level.getAllObjects();
    float posY = entityObjects[0][0].rect.Top;
    float posX = entityObjects[0][0].rect.Left;
    enemies = new Enemies(*pApp, *pClock, world, entityObjects);
    player = new Player(*pApp, *pClock, world, posX, posY);
    terrain = new Terrains(*pApp, *pClock, world, entityObjects[8]);
    pApp->ShowMouseCursor(false);

  //projectile *p = new projectile(*pApp, world, player->getx(), player->gety());
   projectile_manager p_manager(*pApp, world);
///This is where the game actually starts to be ran.

        while (level_cleared != true)     //From here until the end of while is where we are going to start to write the actual game code.
        {
            world.ClearForces();
            world.Step(timeStep, velocityIterations, positionIterations);
            Event Event;                     //Instantiates the event dedtection.
            float Offset = 200.f * pApp->GetFrameTime(); //set offset

            ///Exit Handling
            while (pApp->GetEvent(Event))    //probably have to do an app.getevent(event)
            {
                // Close window : exit
                if (Event.Type == Event::Closed)    //nice no crash if closed.
                {
                    pApp->Close();
                    return -1;
                }
                // Escape key : exit
                if (pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Escape"]))
                {
                    pApp->Close();
                    return -1;
                }
    float mousex = Event.MouseMove.X;
    float mousey = Event.MouseMove.Y;
    Cursor.SetPosition(mousex, mousey);

            }

        ///This is my temporary level Changer from level to level this will eventually hold conditions like the boss being beeten or contact with a certain object.
        if ((pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Down"])) && lvl == 2)
        {

        lvl--;
        level_cleared = true;
        sf::Sleep(.3);
        }
        else if ((pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Down"])) && lvl == 1)
        {

        lvl++;
        level_cleared = true;
        sf::Sleep(.3);
        }

///This is a test to see the FPS and why movement is different across Maps
frameNumber++;
if (FPSClock.GetElapsedTime() > 1 ) {
    currentFPS = frameNumber - lastFPSFrame;
    lastFPSFrame = frameNumber;
    //std::cout << "Current FPS is " <<currentFPS << endl;
    FPSClock.Reset();
}

        ///This is all of the updates for the player and the enemies/ the draws for the screen
        ///Handling the View Update relative to the Player Update. Still needs logic but It is getting there.
        player->update(View);
        if(player->pressing_attack() && once)
        {
                once = false;
                cout << "Player :" << player->getx() << " " << player->gety() << endl;
               p_manager.create_projectile(*player, player->getfacing());
        }
        else if(!(player->pressing_attack()) && !once)
        {
            once = true;
        }
        terrain->update();
        enemies->update();
        pApp->Clear();
        pApp->SetView(View);
        level.SetDrawingBounds(View.GetRect());
        level.Draw(*pApp);
        p_manager.update(player->getfacing());
        p_manager.draw();
               // p->update();
                //p->draw();
       terrain->draw();
        player->draw();
        enemies->draw();
        pApp->Draw(Cursor);

        pApp->Display();

       /// sf::Sleep(2); ///Used for testing priarily
        }
    }

    ///Shouldn't reach this
    return (-1);

}

