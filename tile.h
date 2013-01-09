
class Tile : public Entity
{
    public:
        Tile(float tileWidth, float tileHeight, float x, float y, b2World &world)
        {
        float bodyx = ((x * (float)tileWidth)/10.0f);
        float bodyy = (-(y * (float)tileHeight)/10.0f);

        ///Almost got the Tiles the right size for instatiation in box2d but I have to add or subtact a bit for some reason.
        ///I'm going to call it good for now, I'll adjust more as the game progresses into real art.
        tileBodyDef.position.Set(bodyx - 0.22f, bodyy + 0.20f);

        ///set as box is half of the object size devided by the scaling factor
        ///using floats so that nothing gets truncated
        tileBox.SetAsBox(1.6f,1.6f);               //check this scale
        tileBody = world.CreateBody(&tileBodyDef);
        tileBody->CreateFixture(&tileBox, 0.0f);
        tileBody->SetUserData(this);
        };
        ~Tile(){delete this;};
    private:
        b2BodyDef tileBodyDef;
        b2PolygonShape tileBox;
        b2Body* tileBody;
        float bodyx;
        float bodyy;

}
