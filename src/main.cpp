// main.cpp

#include <iostream>
/*#include "GameEngine.h"  */  // pulls in everything else you need
#include "MultiplayerMenu.h"


bool firstMenu();




int main() {

    if (!firstMenu())
        return 0;

    MultiplayerGame game;
    game.run();

    return 0;
}



























bool firstMenu()
{
    cout << "WELCOME TO EMPIRE WALI GAME!" << endl;
    cout << "PRESS [A] TO PLAY" << endl;
    cout << "PRESS [X] OR CLICK CLOSE WINDOW TO EXIT THE GAME (your loss)" << endl;
    // 1) Create SFML window for start screen immediately so we see errors
    sf::RenderWindow startWindow(sf::VideoMode(1024, 576), "Stronghold Multiplayer - Start");
    startWindow.setFramerateLimit(60);

    // 2) Paths to assets
    const std::string spritePath1 = "assets/images/start_screen.JPEG";
    const std::string spritePath2 = "assets/start_screen.png";
    const std::string fontPath1 = "assets/images/ARIAL.ttf";
    const std::string fontPath2 = "assets/images/arial.ttf";

    // 3) Load start screen texture
    sf::Texture startTexture;
    bool loaded = false;
    if (startTexture.loadFromFile(spritePath1)) {
        loaded = true;
    }
    else if (startTexture.loadFromFile(spritePath2)) {
        loaded = true;
    }
    else {
        std::cerr << "Failed to load start screen sprite. Tried: " << spritePath1 << " and " << spritePath2 << "\n";
    }
    sf::Sprite startSprite;
    if (loaded) {
        startSprite.setTexture(startTexture);
    }

    // 4) Load font
    sf::Font promptFont;
    bool fontLoaded = false;
    if (promptFont.loadFromFile(fontPath1)) {
        fontLoaded = true;
    }
    else if (promptFont.loadFromFile(fontPath2)) {
        fontLoaded = true;
    }
    else {
        std::cerr << "Failed to load font. Tried: " << fontPath1 << " and " << fontPath2 << "\n";
    }

    // 5) Prepare prompt text
    sf::Text promptText("Press A to Play", promptFont, 30);
    sf::FloatRect textBounds = promptText.getLocalBounds();
    promptText.setOrigin(textBounds.width / 2, textBounds.height / 2);
    promptText.setPosition(1024.f / 2.f, 700.f);

    // 6) Show start screen
    while (startWindow.isOpen()) {
        sf::Event event;
        while (startWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                startWindow.close();
                return 0;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {

                startWindow.close();
                return true;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X) {
                startWindow.close();
                return false;
            }
        }

        startWindow.clear(sf::Color::Black);
        if (loaded) startWindow.draw(startSprite);
        if (fontLoaded) startWindow.draw(promptText);
        startWindow.display();
    }
}