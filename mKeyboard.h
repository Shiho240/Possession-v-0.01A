#ifndef MKEYBOARD_H
#define MKEYBOARD_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include "SFML/Window.hpp"
#include <cstdlib>

class mKeyboard
{
    public:
    mKeyboard();
    ~mKeyboard();
    ///don't think I will use these functions
    //sf::Key::Code getKeyboard(string inputkey);
    //sf::Key::Code getKeybindings(string keysearch);
    //void insert(sf::Key::Code newKeyBindings);
    int getwidth();
    int gethight();
    std::map<std::string, sf::Key::Code> m_keyboard;
    std::map<std::string, sf::Key::Code> m_keyBindings;
    //keybindings map used just for the purpose of storing the values of strings to make writing new setting.cfg files easier and removes the need for parsing the other maps from sf:key:code to strings
    std::map<std::string, std::string> m_keyBindingsUNF;
    void setBinding(std::string action, std::string bindKey);
    void setheight(int height);
    void setwidth (int width);
    void setSound(int soundVol);
    void setMusic(int musicVol);
    int getSound();
    int getMusic();
    void setResolution(string resolution);
    void writeToFile();
    private:

    std::string resolution;
    std::string resWidth;
    std::string resHight;
    float m_sound;
    float m_music;
};

inline void mKeyboard::setBinding(std::string action, std::string bindKey)
{
    m_keyBindingsUNF[action]=bindKey;
}

inline void mKeyboard::writeToFile()
{
    char oldfile[] ="temp.cfg";
    char newfile[] ="settings.cfg";
    cout<<"rewriting config!"<<endl;
    fstream configFile, tempFile;
    tempFile.open("temp.cfg", fstream::out);
    tempFile<<"Sound "<<m_sound<<'\n'<<"Music "<<m_music<<'\n'<<"Resolution "<<resWidth<<"x"<<resHight<<'\n'<<"Keybindings\nup "<<m_keyBindingsUNF["Up"]<<"\ndown "<<m_keyBindingsUNF["Down"]<<"\nleft "<<m_keyBindingsUNF["Left"]<<"\nright "<<m_keyBindingsUNF["Right"]<<"\nescape "<<m_keyBindingsUNF["Escape"]<<"\naction "<<m_keyBindingsUNF["Action"]<<"\njump "<<m_keyBindingsUNF["Space"];
    cout<<"wrote to temp config"<<endl;
    if(remove("settings.cfg")!=0)
    cout<<"OHSHI SOMEONE SET US UP THE BOMB AND IT FAILED"<<endl;
    cout<<"removed old settings.cfg"<<endl;
    tempFile.close();
    if(rename(oldfile,newfile)==0)
    cout<<"renamed temp to settings.cfg, all done!"<<endl;
  else
    perror( "Error renaming file" );
}

inline void mKeyboard::setResolution(string resolution)
{
    //streamlined wesleys resolution parser
  stringstream ss(resolution);
  getline(ss,resWidth,'x');
  cout<<resWidth<<endl;
  getline(ss,resHight,'\n');
  cout<<resHight<<endl;





        /*resWidth = "0000";
            resHight = "000";
            //std::cout << resolution << endl;
            bool first = true;
            int j = 0;
            for (int k = 0; k != resolution.size(); k++)
            {
                //std::cout << resolution[k] << "\n";
                if (resolution[k] == 'x' )
                {
                    //std::cout << "OH This is the problem lol\n";
                    first = false;
                    k++;
                    j = 0;
                }

                if (first == true)
                {
                    resWidth[j] = resolution[k];
                    //std::cout << "current Width " << j << " is " << resWidth[j] << "\n";
                    j++;
                }

                if (first == false)
                {
                    resHight[j] = resolution[k];
                    //std::cout << "current Hight " << j << " is " << resHight[j] << "\n";
                    j++;
                }
            }
            //std::cout << "Width is " << resWidth << endl;
            //std::cout << "Hight is " << resHight << endl;*/
}

inline void mKeyboard::setSound(int soundVol)
{
    m_sound=soundVol;
}

inline void mKeyboard::setMusic(int musicVol)
{
    m_music=musicVol;
}

inline int mKeyboard::getSound()
{
    return m_sound;
}

inline int mKeyboard::getMusic()
{
    return m_music;
}
///inline so accessable without a .cpp
inline int mKeyboard::getwidth()
{
        int width = atoi(resWidth.c_str());
        std::cout << "width is " << width << "\n";
        return width;
}

inline int mKeyboard::gethight()
{
        int hight = atoi(resHight.c_str());
        std::cout << "hight is " << hight << "\n";
        return hight;
}

inline mKeyboard::mKeyboard()
{
        ///Create the Map of all Keys
        m_keyboard["A"] = sf::Key::A;
        m_keyboard["B"] = sf::Key::B;
        m_keyboard["C"] = sf::Key::C;
        m_keyboard["D"] = sf::Key::D;
        m_keyboard["E"] = sf::Key::E;
        m_keyboard["F"] = sf::Key::F;
        m_keyboard["G"] = sf::Key::G;
        m_keyboard["H"] = sf::Key::H;
        m_keyboard["I"] = sf::Key::I;
        m_keyboard["J"] = sf::Key::J;
        m_keyboard["K"] = sf::Key::K;
        m_keyboard["L"] = sf::Key::L;
        m_keyboard["M"] = sf::Key::M;
        m_keyboard["N"] = sf::Key::N;
        m_keyboard["O"] = sf::Key::O;
        m_keyboard["P"] = sf::Key::P;
        m_keyboard["Q"] = sf::Key::Q;
        m_keyboard["R"] = sf::Key::R;
        m_keyboard["S"] = sf::Key::S;
        m_keyboard["T"] = sf::Key::T;
        m_keyboard["U"] = sf::Key::U;
        m_keyboard["V"] = sf::Key::V;
        m_keyboard["W"] = sf::Key::W;
        m_keyboard["X"] = sf::Key::X;
        m_keyboard["Y"] = sf::Key::Y;
        m_keyboard["Z"] = sf::Key::Z;
        m_keyboard["1"] = sf::Key::Num1;
        m_keyboard["2"] = sf::Key::Num2;
        m_keyboard["3"] = sf::Key::Num3;
        m_keyboard["4"] = sf::Key::Num4;
        m_keyboard["5"] = sf::Key::Num5;
        m_keyboard["6"] = sf::Key::Num6;
        m_keyboard["7"] = sf::Key::Num7;
        m_keyboard["8"] = sf::Key::Num8;
        m_keyboard["9"] = sf::Key::Num9;
        m_keyboard["0"] = sf::Key::Num0;
        m_keyboard["Escape"] = sf::Key::Escape;
        m_keyboard["Up"] = sf::Key::Up;
        m_keyboard["Left"] = sf::Key::Left;
        m_keyboard["Right"] = sf::Key::Right;
        m_keyboard["Down"] = sf::Key::Down;
        m_keyboard["Space"] = sf::Key::Space;
        m_keyboard["Possess"] = sf::Key::E;

        ///Begin parse of the config file
        // bool flag = false;
        std::fstream configFile;
        configFile.open("settings.cfg",std::fstream::in);
        std::string dataBuffer;
        while(configFile.peek()!= std::ifstream::traits_type::eof())
        {
        configFile>>dataBuffer;

        if (dataBuffer == "Music")
        {
            configFile>>dataBuffer;
            m_music = atoi(dataBuffer.c_str());
        }

        else if (dataBuffer == "Sound")
        {
            configFile>>dataBuffer;
            m_sound = atoi(dataBuffer.c_str());
        }

        else if (dataBuffer == "Resolution")
        {
            resWidth = "0000";
            resHight = "000";
            configFile>>dataBuffer;
            resolution = dataBuffer;
            //std::cout << resolution << endl;
            bool first = true;
            int j = 0;
            for (int k = 0; k != resolution.size(); k++)
            {
                //std::cout << resolution[k] << "\n";
                if (resolution[k] == 'x' )
                {
                    //std::cout << "OH This is the problem lol\n";
                    first = false;
                    k++;
                    j = 0;
                }

                if (first == true)
                {
                    resWidth[j] = resolution[k];
                    //std::cout << "current Width " << j << " is " << resWidth[j] << "\n";
                    j++;
                }

                if (first == false)
                {
                    resHight[j] = resolution[k];
                    //std::cout << "current Hight " << j << " is " << resHight[j] << "\n";
                    j++;
                }
            }
            //std::cout << "Width is " << resWidth << endl;
            //std::cout << "Hight is " << resHight << endl;
        }
        ///Read the Map for the Key Bindings and put them into a map
        else if (dataBuffer == "Keybindings")
        {
            ///We can Add more Keys here if need be.
            configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Up"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Up"] = dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Down"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Down"]=dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Left"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Left"] = dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Right"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Right"] = dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Escape"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Escape"] = dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Num1"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Num1"] = dataBuffer;
                        configFile>>dataBuffer;
            configFile>>dataBuffer;
            m_keyBindings["Space"] = m_keyboard[dataBuffer];
            m_keyBindingsUNF["Space"] = dataBuffer;
        }
        else
        {

        }
    }
    configFile.close();
}
///inline so accessable without a .cpp
inline mKeyboard::~mKeyboard()
{
  m_keyboard.clear();
  m_keyBindings.clear();
}
#endif

