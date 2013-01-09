#include "screen.h"
#include <iostream>
#include <cstring>
#include <string>
#include <windows.h>
#include <fstream>
#include "mKeyboard.h"

class Screen_MainMenu : public Screen
{
    public:
        Screen_MainMenu(sf::RenderWindow &App);
        ~Screen_MainMenu();
        virtual int Run();
    protected:
    private:
        ResourceManager GameResource;
        sf::Image BackgroundImage;
        sf::Sprite BackgroundSprite;
        sf::Font Font;
        sf::RenderWindow *pApp;
};

Screen_MainMenu::Screen_MainMenu (sf::RenderWindow &App)
{
    pApp = &App;
}

Screen_MainMenu::~Screen_MainMenu ()
{
    //dtor
}

int Screen_MainMenu::Run ()
{
    mKeyboard keyboard;
    //sf::RenderWindow App(sf::VideoMode(keyboard.getwidth(), keyboard.gethight(), 32), "SFML Game Template");
    //pApp = &App;
    ///Check to see if config file exists if not create config file titled: settings.cfg
    fstream configFile;
    configFile.open("settings.cfg",fstream::in);
    if( !configFile.is_open() )
    {
        configFile.close();
        configFile.open("settings.cfg", fstream::out);
        configFile<<"Sound 100\nMusic 100\nResolution 1024x768\nKeybindings\nup W\ndown S\nleft A\nright D\nescape Escape\naction Num1\njump Space";
    }
    configFile.close();



    pApp->ShowMouseCursor(true);
    BackgroundSprite.SetImage(GameResource.getImage("background"));
    Font.LoadFromFile("resources/fonts/pirulen.ttf");

   sf::Music menumusic;
    menumusic.OpenFromFile("rickroll.ogg");
    menumusic.SetLoop(true);
    menumusic.SetVolume(keyboard.getMusic());
    menumusic.Play();

    sf::String TitleString("Possession v0.1", Font, 36);
    sf::String PlayString("Play", Font, 24);
    sf::String SettingsString("Settings", Font, 24);
    sf::String ExitString("Exit", Font, 24);

    sf::FloatRect TitleRect, JoinRect, SettingsRect, ExitRect;

    TitleRect = TitleString.GetRect();
    JoinRect = PlayString.GetRect();
    SettingsRect = SettingsString.GetRect();
    ExitRect = ExitString.GetRect();

    TitleString.SetPosition(pApp->GetWidth() / 2 - TitleRect.GetWidth() / 2, pApp->GetHeight() / 4 - TitleRect.GetHeight() / 2);
    PlayString.SetPosition(pApp->GetWidth() / 2 - JoinRect.GetWidth() / 2, pApp->GetHeight() / 4 - JoinRect.GetHeight() / 2 + 150);
    SettingsString.SetPosition(pApp->GetWidth() /2 - SettingsRect.GetWidth() / 2, pApp->GetHeight() / 4 - SettingsRect.GetHeight() / 2 + 200);
    ExitString.SetPosition(pApp->GetWidth() / 2 - ExitRect.GetWidth() / 2, pApp->GetHeight() / 4 - ExitRect.GetHeight() / 2 + 250);

    ///Menu loop
    while (pApp->IsOpened())
    {
        ///Process events
        sf::Event Event;
        while (pApp->GetEvent(Event))
        {
            unsigned int mouseX = pApp->GetInput().GetMouseX();
            unsigned int mouseY = pApp->GetInput().GetMouseY();

            ///Exit Hadeling
            //Window closed
            if (Event.Type == sf::Event::Closed)
            {
                return -1;
            }
            //Key pressed
            if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
            {
                return -1;
            }

            ///Mouse Listeners
            //mouse hover over "Join Game"
            else if ((mouseX > (pApp->GetWidth() / 2 - JoinRect.GetWidth() / 2)) && (mouseX < (pApp->GetWidth() / 2 + JoinRect.GetWidth() / 2))
                     && (mouseY > (pApp->GetHeight() / 4 - JoinRect.GetHeight() / 2 + 150)) && (mouseY < (pApp->GetHeight() / 4 + JoinRect.GetHeight() / 2 + 150)))
            {
                PlayString.SetColor(sf::Color::Blue);
                if ((Event.Type == sf::Event::MouseButtonReleased))
                {
                    return 2;
                }
            }
            //Mouse over "Settings"
            else if ((mouseX > (pApp->GetWidth() / 2 - SettingsRect.GetWidth() / 2)) && (mouseX < (pApp->GetWidth() / 2 + SettingsRect.GetWidth() / 2))
                     && (mouseY > (pApp->GetHeight() / 4 - SettingsRect.GetHeight() / 2 + 200)) && (mouseY < (pApp->GetHeight() / 4 + SettingsRect.GetHeight() / 2 + 200)))
            {
                SettingsString.SetColor(sf::Color::Blue);
                if ((Event.Type == sf::Event::MouseButtonReleased))
                {
                    return 1;
                }
            }
            //Mouse over "Exit"
            else if ((mouseX > (pApp->GetWidth() / 2 - ExitRect.GetWidth() / 2) ) && ( mouseX < (pApp->GetWidth() / 2 + ExitRect.GetWidth() / 2))
                     && (mouseY > (pApp->GetHeight() / 4 - ExitRect.GetHeight() / 2 + 250 )) && (mouseY < (pApp->GetHeight() / 4 + ExitRect.GetHeight() / 2 + 250)))
            {
                ExitString.SetColor(sf::Color::Blue);
                if ((Event.Type == sf::Event::MouseButtonReleased))
                {
                    return -1;
                }
            }
            else
            {
                PlayString.SetColor(sf::Color::White);
                SettingsString.SetColor(sf::Color::White);
                ExitString.SetColor(sf::Color::White);
            }
        }

        ///Clear Window
        pApp->Clear();
        ///Draw sprites and text
        pApp->Draw(BackgroundSprite);
        pApp->Draw(TitleString);
        pApp->Draw(PlayString);
        pApp->Draw(SettingsString);
        pApp->Draw(ExitString);
        ///Display Window
        pApp->Display();
    }

    ///Shouldn't reach this
    return (-1);
}

