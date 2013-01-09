#ifndef ENTITY_H
#define ENTITY_H

class Entity
{
    public:
        Entity() {health = 10;};
        ~Entity() {};
        int health;
        virtual void update() {};
        virtual void draw() {};
        virtual int getEntityType() {return 0;};
    protected:
    private:
};

#endif // ENTITY_H

