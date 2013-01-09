#include "Entity.h"
#include "projectiles.h"
#include "resourcemanager.h"
#include <math.h>
#include <Box2D/Box2D.h>
#include "SFML/Window.hpp"
#include "mKeyboard.h"
#include "Possession_manager.h"

//Possesion manager
Possession_manager possession_manager;

class MyContactListener: public b2ContactListener
{
    void BeginContact (b2Contact* contact)
    {
        //Get User Data
        void* bodyAUserData = contact->GetFixtureA()->GetBody()->GetUserData();
        void* bodyBUserData = contact->GetFixtureB()->GetBody()->GetUserData();
        b2Body *bodyA = contact->GetFixtureA()->GetBody();
        b2Body *bodyB = contact->GetFixtureB()->GetBody();
      //If its contact between two objects that have user data
      if ( bodyAUserData && bodyBUserData )
        {
        cout << "===========================================\n";
        cout << "Contact" << endl;
        //Find out what the two objects are
        cout << "Casting to entity" << endl;
        Entity *e1 = (Entity*)bodyB->GetUserData();
        Entity *e2 = (Entity*)bodyA->GetUserData();
        cout << "Getting entity type" << endl;
        int t1 = e1->getEntityType();
        int t2 = e2->getEntityType();
        cout << "Got entity type, now crashing" << endl;
        cout << "t1: " << t1 << endl;
        cout << "t2: " << t2 << endl;
        //Set objects appropriately
        if(t1 == 1)
        {
            cout << "player on ";
        }
        else if (t1 == 2)
            cout << "enemy on ";
        if(t2 == 1)
            cout << "player" << endl;
        else if (t2 == 2)
            cout << "enemy" << endl;
        cout << t1 << " on " << t2 << endl;
        //if player and enemy contact cast as player and enemy and handle
        if(t1 == 1 && t2 == 2)
        {
            Player *p = (Player*)bodyB->GetUserData();
            Enemy *e = (Enemy*)bodyA->GetUserData();
            p->handleEnemyContact(*e);
            e->player_touching = true;
            //Only call possess if possess key is pressed
            if(p->pressing_possess())
            {
                possession_manager.possess(*p,*e);
            }
        }
        if (t1 == 2 && t2 == 1) //"There is no particular ordering of the A and B fixtures" - from iforce tutorial
        {
            Player *p = (Player*)bodyA->GetUserData();
            Enemy *e = (Enemy*)bodyB->GetUserData();
            p->handleEnemyContact(*e);
        }
        if (t1 == 4 && t2 == 2)
        {
            projectile *f = (projectile*)bodyB->GetUserData();
            Enemy *e = (Enemy*)bodyA->GetUserData();
            f->mylif(*e);
            f->set_dead();
        }
        if (t1 == 2 && t2 == 4)
        {
            projectile *f = (projectile*)bodyA->GetUserData();
            Enemy *e = (Enemy*)bodyB->GetUserData();
            f->mylif(*e);
            f->set_dead();
        }

        }
    }

};
