#include "decay_timer.h"

decay_timer::decay_timer(sf::RenderWindow &App)
{
    pApp = &App;
    m_Border.SetImage(GameResource.getImage("BarBorder"));
    m_Border.SetSubRect(sf::IntRect(0,0,102,19));
    m_Sprite.SetImage(GameResource.getImage("LifeBar"));
    m_Sprite.SetSubRect(sf::IntRect(0,0,100,15));
    m_current_bar = 100;
}

float decay_timer::get_time()
{
    return m_Clock.GetElapsedTime();
}

void decay_timer::update(float x, float y)
{
    m_Sprite.SetPosition(x,y);
    m_Border.SetPosition(x,y-2);
    //Live Decay Timer
    if(m_Clock.GetElapsedTime() > 1 && m_current_bar > 0)
    {
        m_current_bar -= 1;
        m_Sprite.SetImage(GameResource.getImage("LifeBar"));
        m_Sprite.SetSubRect(sf::IntRect(0,0,m_current_bar,15));
        m_Clock.Reset();
    }
    //Ghost Decay Timer
    if(m_Clock.GetElapsedTime() > 1 && m_current_bar <= 0 && m_current_bar > -100)
    {
        m_current_bar -= 1;
        m_Sprite.SetImage(GameResource.getImage("GhostBar"));
        m_Sprite.SetSubRect(sf::IntRect(0,0,100 + m_current_bar,15));
        m_Clock.Reset();
    }
    if(m_current_bar <= -100)
    {
        cout << "Game Over" << endl;
    }
}

void decay_timer::draw()
{
    pApp->Draw(m_Sprite);
    pApp->Draw(m_Border);
}

void decay_timer::set_bar(int n_bar)
{
    m_current_bar = n_bar;
}

int decay_timer::get_bar()
{
    return m_current_bar;
}
