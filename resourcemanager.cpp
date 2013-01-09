#include "resourcemanager.h"

ResourceManager::ResourceManager()
{
    //Load the images used in the game
    BackgroundImage.LoadFromFile("resources/images/background.png");
    PlayerImage.LoadFromFile("resources/images/spiritwalk1.png");
    RangerImage.LoadFromFile("resources/images/rangerwalk2.png");
    SoldierImage.LoadFromFile("resources/images/fighterwalk2.png");
    CursorImage.LoadFromFile("cursor.png");
    TerrainImage.LoadFromFile("target.png");
    ArrowImage.LoadFromFile("resources/images/weaponsandprojectiles.png");
    LifeBarImage.LoadFromFile("resources/images/lifebar.png");
    GhostBarImage.LoadFromFile("resources/images/ghostbar.png");
    BarBorderImage.LoadFromFile("resources/images/barborder.png");
    SwordImage.LoadFromFile("resources/images/weaponsandprojectiles.png");
    MageImage.LoadFromFile("resources/images/magewalk1.png");
    LancerImage.LoadFromFile("resources/images/lancerwalk1.png");
    NecromancerImage.LoadFromFile("resources/images/necromancerwalk1.png");
    ControllerImage.LoadFromFile("resources/images/controllerwalk1.png");
    LancerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    PlayerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    RangerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    SoldierImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    LancerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    MageImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    NecromancerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    ControllerImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    CursorImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    TerrainImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    ArrowImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    SwordImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    LifeBarImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    GhostBarImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    BarBorderImage.CreateMaskFromColor(sf::Color(255,255,255),0);
    //Insert images into map
    Images["background"] = BackgroundImage;
    Images["player"] = PlayerImage;
    Images["Ranger"] = RangerImage;
    Images["Soldier"] = SoldierImage;
    Images["Mage"] = MageImage;
    Images["Lancer"] = LancerImage;
    Images["Necromancer"] = NecromancerImage;
    Images["Controller"] = ControllerImage;
    Images["Cursor"] = CursorImage;
    Images["Terrain"] = TerrainImage;
    Images["Arrow"] = ArrowImage;
    Images["Sword"] = SwordImage;
    Images["LifeBar"] = LifeBarImage;
    Images["GhostBar"] = GhostBarImage;
    Images["BarBorder"] = BarBorderImage;
}

ResourceManager::~ResourceManager()
{
    Images.clear();
}

sf::Image& ResourceManager::getImage(string fileName)
{
    return Images[fileName];
}

