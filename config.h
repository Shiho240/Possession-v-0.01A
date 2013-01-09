#ifndef CONFIG_H
#define CONFIG_H
#include <fstream>
#include <vector>
#include "SFML/Audio.hpp"
#include "Screen.h"
#include <sstream>
class Config
{
    public:
        Config(sf::RenderWindow &App);
        ~Config();
        void readConfig();
        string getMusic();
        string getSound();

    private:
        sf::RenderWindow *pApp;
        int m_music;
        int m_sound;
        string resolution;
        //std::map <std::string, sf::Key::Code> m_keyboard;
        //std::vector<sf::Key::Code> m_keyBindings;
        //std::vector<sf::Key::Code>::iterator m_bindIt;

};



string Config::getMusic()
{
    std::stringstream ss;
    ss<<m_music;
    return ss.str();
};
string Config::getSound()
{
  std::stringstream ss;
    ss<<m_sound;
    return ss.str();
};

Config::Config(sf::RenderWindow &App)
{
pApp = &App;


};

Config::~Config()
{



};

void Config::readConfig()
{




};



#endif
