#include <Box2D/Box2D.h>


class ContactListener: public b2ContactListener {
    virtual void BeginContact(Contact* contact) {
    }

    virtual void EndContact(Contact* contact) {
    }

    virtual void PreSolve(Contact* contact, const Manifold* oldManifold) {
    }

    virtual void PostSolve(Contact* contact, const ContactImpulse* impulse){
    }

    void BeginContact(b2Contact* contact) {
        BeginContact(static_cast<Contact*>(contact));
    }

    void EndContact(b2Contact* contact) {
        EndContact(static_cast<Contact*>(contact));
    }

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
        PreSolve(static_cast<Contact*>(contact), static_cast<const Manifold*>(oldManifold));
    }

    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
        PostSolve(static_cast<Contact*>(contact), static_cast<const ContactImpulse*>(impulse));
    }
