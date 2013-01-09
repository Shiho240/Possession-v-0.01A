#include "screen_mainmenu.h"
#include "screen_setting.h"
#include "game.h"
#include "mKeyboard.h"

using namespace std;

int main()
{
    ///Application variables
    std::vector<Screen*> Screens;
    int screen = 0;

    ///Create the main SFML window
    sf::RenderWindow App(sf::VideoMode(1024, 768, 32), "SFML Game Template");
    //sf::RenderWindow App(sf::VideoMode(keyboard.getwidth(), keyboard.gethight(), 16), "SFML Game Template");
    App.UseVerticalSync(false);
    //App.SetFramerateLimit(300);

    ///Screen preparations
    Screen_MainMenu MainMenu(App);
    Screens.push_back(&MainMenu);
    Screen_Settings Settings(App);
    Screens.push_back(&Settings);
    Game Game(App);
    Screens.push_back(&Game);

    ///Main loop
    while (screen >= 0)
    {
        screen = Screens[screen]->Run();
    }

    return 0;
}
