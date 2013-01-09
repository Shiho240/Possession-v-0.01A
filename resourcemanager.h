#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;

class ResourceManager
{
    public:
        ResourceManager();
        ~ResourceManager();
        sf::Image& getImage(string fileName);
    protected:
    private:
        sf::Image BackgroundImage;
        sf::Image PlayerImage;
        sf::Image RangerImage;
        sf::Image SoldierImage;
        sf::Image LancerImage;
        sf::Image NecromancerImage;
        sf::Image MageImage;
        sf::Image ControllerImage;
        sf::Image CursorImage;
        sf::Image TerrainImage;
        sf::Image ArrowImage;
        sf::Image LifeBarImage;
        sf::Image GhostBarImage;
        sf::Image BarBorderImage;
        sf::Image SwordImage;
        map< string, sf::Image > Images;
};

#endif //RESOURCEMANAGER_H
