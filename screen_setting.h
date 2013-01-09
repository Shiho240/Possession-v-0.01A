
//Some code taken from cpGUI example code given with library, copyright below
//
//cpGUI - Copyright (c) 2009 Jason Cupp
//
//This software is provided 'as-is', without any express or implied warranty.
//In no event will the authors be held liable for any damages arising from the
//use of this software.
//
//Permission is granted to anyone to use this software for any purpose, including
//commercial applications, and to alter it and redistribute it freely, subject to
//the following restrictions:
//
//
//1. The origin of this software must not be misrepresented; you must not claim
//that you wrote the original software. If you use this software in a product, an
//acknowledgment in the product documentation would be appreciated but is not required.
//
//
//2. Altered source versions must be plainly marked as such, and must not be
//misrepresented as being the original software.
//
//
//3. This notice may not be removed or altered from any source distribution.


#include "screen.h"
#include "config.h"
#include "cpGUI/cpGUI.h"
#include <sstream>
#include "mKeyboard.h"
class Screen_Settings : public Screen
{
    public:
        Screen_Settings(sf::RenderWindow &App);
        ~Screen_Settings();
        virtual int Run();
    protected:
    private:
        ResourceManager GameResource;
        sf::Image BackgroundImage;
        sf::Sprite BackgroundSprite;
        sf::Font Font;
        sf::RenderWindow *pApp;
};

Screen_Settings::Screen_Settings (sf::RenderWindow &App)
{
    pApp = &App;
}

Screen_Settings::~Screen_Settings ()
{
    //dtor
}

inline int Screen_Settings::Run ()
{
    mKeyboard keyboard;
    cp::cpGuiContainer myGUI;
    ///Setup and Initializations
    Config settingsConfig(*pApp);
    settingsConfig.readConfig();
    pApp->ShowMouseCursor(true);
    BackgroundSprite.SetImage(GameResource.getImage("background"));
    Font.LoadFromFile("resources/fonts/pirulen.ttf");

//settings init
    int getmusicVol=keyboard.getMusic();
    int getsoundVol=keyboard.getSound();
    bool noChanges=true;
//get config file volume settings
    string setSoundVol, setMusicVol;
    stringstream ss,ss2;
    ss<<getmusicVol;
    setMusicVol=ss.str();
    ss2<<getsoundVol;
    setSoundVol=ss2.str();

sf::Music settingmusic;
    settingmusic.OpenFromFile("Trololo.ogg");
    settingmusic.SetLoop(true);
    settingmusic.SetVolume(keyboard.getMusic());
    settingmusic.Play();

	cp::cpTextInputBox setMusicVolBox(pApp, &myGUI, setMusicVol, 600, 330, 50, 25);
	setMusicVolBox.SetBackgroundColor(sf::Color::Black);
    setMusicVolBox.SetLabelColor(sf::Color::White);

    cp::cpTextInputBox setSoundVolBox(pApp, &myGUI, setSoundVol, 600, 385, 50, 25);
	setSoundVolBox.SetBackgroundColor(sf::Color::Black);
    setSoundVolBox.SetLabelColor(sf::Color::White);

    cp::cpDropDownBox resBox(pApp, &myGUI, "Select Your Resolution", 630, 432, 200, 20);

    cp::cpDropDownBox bindBox(pApp, &myGUI, "Select Action to Bind", 630, 482, 200, 20);

    cp::cpDropDownBox keyBox(pApp, &myGUI,"Select Key to Bind",830,482,200,20);

	// Set the maximum selection box depth before a scrollbar is used.
	// 100 is default
	resBox.SetMaxDepth(200);
    bindBox.SetMaxDepth(200);
    keyBox.SetMaxDepth(200);
	std::string reschoices[] = {"1024x768", "1280x800", "1280x1024", "1366x768", "1680x1050", "1920x1080", "1920x1200"};
	for(int t=0; t < 7; t++)
		resBox.AddChoice(reschoices[t]);
    std::string bindchoices[]= {"Up", "Down", "Left", "Right", "Escape", "Num1", "Space"};
    for(int t=0; t < 7; t++)
        bindBox.AddChoice(bindchoices[t]);
    std::string keychoices[]= {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Z", "X", "C", "V", "B", "N", "M", "Num1", "Space"};
    for(int t=0; t < 28; t++)
        keyBox.AddChoice(keychoices[t]);
//sfml menu stuff
    sf::String TitleString("Settings", Font, 36);
    sf::String MusicString("Music", Font, 24);
    sf::String SoundString("Sound", Font, 24);
     sf::String ResolutionString("Resolution",Font,24);
     sf::String ApplyString("Apply Settings",Font, 24);
     sf::String BindingString("Keybindings", Font, 24);
    sf::String ReturnString("Back", Font, 24);


    sf::FloatRect TitleRect, MusicRect, SoundRect, ReturnRect, ResolutionRect, ApplyRect, BindingRect;

    TitleRect = TitleString.GetRect();
    MusicRect = MusicString.GetRect();
    SoundRect = SoundString.GetRect();
    ReturnRect = ReturnString.GetRect();
    ResolutionRect = ResolutionString.GetRect();
    ApplyRect=ApplyString.GetRect();
    BindingRect=BindingString.GetRect();

    TitleString.SetPosition(pApp->GetWidth() / 2 - TitleRect.GetWidth() / 2, pApp->GetHeight() / 4 - TitleRect.GetHeight() / 2);
    MusicString.SetPosition(pApp->GetWidth() / 2 - MusicRect.GetWidth() / 2, pApp->GetHeight() / 4 - MusicRect.GetHeight() / 2 + 150);
    SoundString.SetPosition(pApp->GetWidth() /2 - SoundRect.GetWidth() / 2, pApp->GetHeight() / 4 - SoundRect.GetHeight() / 2 + 200);
    ResolutionString.SetPosition(pApp->GetWidth() / 2 - ResolutionRect.GetWidth() / 2, pApp->GetHeight() / 4 - ResolutionRect.GetHeight() / 2 + 250);
    BindingString.SetPosition(pApp->GetWidth() / 2 - BindingRect.GetWidth() / 2, pApp->GetHeight() / 4 - BindingRect.GetHeight() / 2 + 300);
    ApplyString.SetPosition(pApp->GetWidth() / 2 - ApplyRect.GetWidth() / 2, pApp->GetHeight() / 4 - ApplyRect.GetHeight() / 2 + 350);
    ReturnString.SetPosition(pApp->GetWidth() / 2 - ReturnRect.GetWidth() / 2, pApp->GetHeight() / 4 - ReturnRect.GetHeight() / 2 + 400);

std::string musicVol, soundVol;
int selection=0;
int keyselect = 0;
int bindingselection =0;
const sf::Input& input = pApp->GetInput();
    ///Menu loop
    while (pApp->IsOpened())
    {
        ///Process events
        sf::Event Event;
        while (pApp->GetEvent(Event))
        {
            unsigned int mouseX = pApp->GetInput().GetMouseX();
            unsigned int mouseY = pApp->GetInput().GetMouseY();
            setMusicVolBox.ProcessTextInput(&Event);
            setSoundVolBox.ProcessTextInput(&Event);
            //Window closed
            if (Event.Type == sf::Event::Closed)
            {
                return -1;
            }
            //Key pressed
            if (pApp->GetInput().IsKeyDown(keyboard.m_keyBindings["Escape"]))
            {
                return -1;
            }
            if ((mouseX > (pApp->GetWidth() / 2 - ReturnRect.GetWidth() / 2) ) && ( mouseX < (pApp->GetWidth() / 2 + ReturnRect.GetWidth() / 2))
                     && (mouseY > (pApp->GetHeight() / 4 - ReturnRect.GetHeight() / 2 + 400 )) && (mouseY < (pApp->GetHeight() / 4 + ReturnRect.GetHeight() / 2 + 400)))
            {
                ReturnString.SetColor(sf::Color::Blue);
                if ((Event.Type == sf::Event::MouseButtonReleased))
                {
                    return 0;
                }
            }
          else if ((mouseX > (pApp->GetWidth() / 2 - ApplyRect.GetWidth() / 2) ) && ( mouseX < (pApp->GetWidth() / 2 + ApplyRect.GetWidth() / 2))
                     && (mouseY > (pApp->GetHeight() / 4 - ApplyRect.GetHeight() / 2 + 350 )) && (mouseY < (pApp->GetHeight() / 4 + ApplyRect.GetHeight() / 2 + 350)))
            {
                ApplyString.SetColor(sf::Color::Blue);
                if ((Event.Type == sf::Event::MouseButtonReleased))
                {
                    keyboard.writeToFile();
                }
            }
            else
            {
                MusicString.SetColor(sf::Color::White);
                SoundString.SetColor(sf::Color::White);
                ReturnString.SetColor(sf::Color::White);
                ApplyString.SetColor(sf::Color::White);
                BindingString.SetColor(sf::Color::White);
            }
        //check text input box input

		if(setMusicVolBox.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
		{
			musicVol = setMusicVolBox.GetLabelText();
			noChanges=false;
        }
        if(setSoundVolBox.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
        {
           soundVol = setSoundVolBox.GetLabelText();
           noChanges=false;
        }

        if(resBox.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
			{
			    selection = resBox.GetSelection();
			    noChanges = false;
			}
        if(bindBox.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
            {
                bindingselection = bindBox.GetSelection();
                noChanges = false;
            }
        if(keyBox.CheckState(&input) == cp::CP_ST_MOUSE_LBUTTON_RELEASED)
            {
                keyselect = keyBox.GetSelection();
                noChanges = false;
            }
        }//end while loop
        if(noChanges==false)
        {

        selection = resBox.GetSelection();
        bindingselection = bindBox.GetSelection();
        keyselect = keyBox.GetSelection();
        keyboard.setSound(atoi(soundVol.c_str()));
        keyboard.setMusic(atoi(musicVol.c_str()));
        keyboard.setResolution(reschoices[selection]);
        keyboard.setBinding(bindchoices[bindingselection],keychoices[keyselect]);
        //cout<<"binding "<<bindchoices[bindingselection]<< " to key "<<keychoices[keyselect]<<endl;
        }
        ///Clear Window
        pApp->Clear();
        ///Draw sprites and text
        pApp->Draw(BackgroundSprite);
        pApp->Draw(TitleString);
        pApp->Draw(MusicString);
        pApp->Draw(SoundString);
        pApp->Draw(ReturnString);
        pApp->Draw(ResolutionString);
        pApp->Draw(ApplyString);
        pApp->Draw(BindingString);
        setMusicVolBox.Draw();
        setSoundVolBox.Draw();
        resBox.Draw();
        bindBox.Draw();
        keyBox.Draw();
        ///Display Window
        pApp->Display();
    }

    ///Shouldn't reach this
    return (-1);
}

