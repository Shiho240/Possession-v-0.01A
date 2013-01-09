#include "Possession_manager.h"

Possession_manager::Possession_manager()
{
    cout << "Possession Manager constructor" << endl;
}

void Possession_manager::possess(Player &p, Enemy &e)
{
    cout << "Possess called" << endl;
    if(e.player_touching && e.dead && p.get_ghost_state())
    {

        p.body_type = e.body_type;
        cout << p.body_type << endl;
        p.change_sprite(e.m_Sprite);
        p.set_ghost_false();
        e.possessed = true;
        cout << "Change Sprite Called" << endl;

    }
}
