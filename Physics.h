#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>

/*!
 * A wrapper for the physics engine
 */
namespace PhysicsEngine {

float const PI = b2_pi;

enum BodyType { STATIC_BODY = 0, KINEMATIC_BODY, DYNAMIC_BODY };

inline b2BodyType getBodyType(BodyType bodyType) {
    switch(bodyType) {
        case STATIC_BODY:
            return b2_staticBody;
        case KINEMATIC_BODY:
            return b2_kinematicBody;
        case DYNAMIC_BODY:
            return b2_dynamicBody;
        default:
            return b2_staticBody;
    }
}

struct Vec2: public b2Vec2 {
    inline Vec2() {
    }

    inline Vec2(Vec2 const & vec2) {
        this->x = vec2.x;
        this->y = vec2.y;
    }

    inline Vec2(float32 x, float32 y): b2Vec2(x, y) {
    }

    inline Vec2 operator -(Vec2 const & vec2) const { Vec2 v(x-vec2.x, y-vec2.y); return v; }
    inline Vec2 operator +(Vec2 const & vec2) const { Vec2 v(x+vec2.x, y+vec2.y); return v; }
    inline Vec2 operator *(float scalar) const { Vec2 v(scalar*this->x, scalar*this->y); return v; }

    inline Vec2 & operator =(Vec2 const & vec2) {
        if (&vec2 == this) { return *this; }
        this->x = vec2.x;
        this->y = vec2.y;
        return *this;
    }
};

class PhysicalBodyDef: public b2BodyDef {
};

class PhysicalFixtureDef: public b2FixtureDef {
};

class PolygonShape : public b2PolygonShape {
public:
    inline void SetAsEdge(const Vec2 &v1, const Vec2 &v2) {
        b2PolygonShape::SetAsEdge(b2Vec2(v1.x, v1.y), b2Vec2(v2.x, v2.y));
    }
};

class CircleShape : public b2CircleShape {
public:
};

class Filter: public b2Filter {
public:
    inline Filter() {
        maskBits = 0;
        categoryBits = 0;
        groupIndex = 0;
    }
};

class Manifold: public b2Manifold {
};

class WorldManifold: public b2WorldManifold {
};

struct ContactEdge: public b2ContactEdge {
};

class PhysicalFixture;
class PhysicalBody: public b2Body {
public:
    inline Vec2 const & GetPosition() const {
        return static_cast<Vec2 const &>(b2Body::GetPosition());
    }
    inline Vec2 GetLinearVelocity() const {
        b2Vec2 v = b2Body::GetLinearVelocity();
        return Vec2(v.x, v.y);
    }
    inline ContactEdge const * GetContactList () const {
        return static_cast<ContactEdge const *>(b2Body::GetContactList());
    }
    inline PhysicalFixture * GetFixtureList ();

    inline Vec2 const & GetWorldCenter() const {
        return static_cast<Vec2 const &>(b2Body::GetWorldCenter());
    }
    inline Vec2 const & GetLocalCenter() const {
        return static_cast<Vec2 const &>(b2Body::GetLocalCenter());
    }
};

class PhysicalFixture: public b2Fixture {
public:
    inline PhysicalFixture * GetNext () {
        return static_cast<PhysicalFixture*>(b2Fixture::GetNext());
    }

    inline PhysicalBody const * GetBody() const {
        return static_cast<PhysicalBody const *>(b2Fixture::GetBody());
    }

    inline void SetFilterData(const Filter& filter) {
        b2Fixture::SetFilterData(static_cast<b2Filter const &>(filter));
    }

    inline Filter const & GetFilterData() const {
        return static_cast<Filter const &>(b2Fixture::GetFilterData());
    }
};

PhysicalFixture * PhysicalBody::GetFixtureList () {
        return static_cast<PhysicalFixture *>(b2Body::GetFixtureList());
}

class Joint: public b2Joint {
};

class JointDef: public b2JointDef {
};

class RevoluteJointDef: public b2RevoluteJointDef {
};

class RevoluteJoint: public b2RevoluteJoint {
};

class DistanceJointDef: public b2DistanceJointDef {
};

class DistanceJoint: public b2DistanceJoint {
};

class WeldJointDef: public b2WeldJointDef {
};

class WeldJoint: public b2WeldJoint {
};

class MouseJointDef: public b2MouseJointDef {
};

class MouseJoint: public b2MouseJoint {
};

class Contact: public b2Contact {
public:
    inline Manifold * 	GetManifold () {
        return static_cast<Manifold*>(b2Contact::GetManifold());
    }

    inline PhysicalFixture const * GetFixtureA () const {
        return static_cast<PhysicalFixture const *>(b2Contact::GetFixtureA());
    }

    inline PhysicalFixture const * GetFixtureB () const {
        return static_cast<PhysicalFixture const *>(b2Contact::GetFixtureB());
    }
};

class ContactImpulse: public b2ContactImpulse {
};

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
};

class PhysicalWorld: public b2World {
public :
    inline PhysicalWorld(Vec2 const & vec2, bool doSleep):b2World(vec2, doSleep) {
    }

    inline ~PhysicalWorld() {
    }

    inline PhysicalBody * CreateBody(PhysicalBodyDef const * def) {
        b2BodyDef bodyDef = *def;
        PhysicalBody * body = static_cast<PhysicalBody *>(b2World::CreateBody(&bodyDef));
        body->SetUserData(NULL);
        return body;
    }

    inline RevoluteJoint * CreateJoint(RevoluteJointDef const *def) {
        b2RevoluteJointDef jointDef = *def;
        return static_cast<RevoluteJoint*>(b2World::CreateJoint(&jointDef));
    }

    inline DistanceJoint * CreateJoint(DistanceJointDef const *def) {
        b2DistanceJointDef jointDef = *def;
        return static_cast<DistanceJoint*>(b2World::CreateJoint(&jointDef));
    }

    inline WeldJoint * CreateJoint(WeldJointDef const *def) {
        b2WeldJointDef jointDef = *def;
        return static_cast<WeldJoint*>(b2World::CreateJoint(&jointDef));
    }

    inline MouseJoint * CreateJoint(MouseJointDef const *def) {
        b2MouseJointDef jointDef = *def;
        return static_cast<MouseJoint*>(b2World::CreateJoint(&jointDef));
    }
};

inline float vec2Dot(const b2Vec2& a, const b2Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

inline float vec2Cross(const b2Vec2& a, const b2Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

};
#endif
