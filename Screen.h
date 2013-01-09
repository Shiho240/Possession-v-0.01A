#ifndef SCREEN_H
#define SCREEN_H

//#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "resourcemanager.h"

#define mainmenu 0
#define game     2

#define settings 1
#define pause    3

class Screen
{
    public:
        //Screen();
        //virtual ~Screen();
        /**
        * @return positive int - go to corresponding screen
        * @return -1 - quit
        */
        //virtual int Run(sf::RenderWindow &App) = 0;
        virtual int Run() = 0;
    protected:
    private:
};

#endif // SCREEN_H
