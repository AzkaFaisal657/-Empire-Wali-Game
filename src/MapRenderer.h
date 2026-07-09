#include <SFML/Graphics.hpp>
#include <iostream>
#include "MapSystem.h"

// Renders the 2x4 grid map using SFML
void showMapSFML(MapSystem& mapSystem) {
    // 1) Update logical map
    mapSystem.updateMap();

    // 2) Load background texture
    sf::Texture bgTex;
    if (!bgTex.loadFromFile("assets/images/map_background.png")) {
        std::cerr << "Failed to load background: assets/images/map_background.png\n";
        return;
    }
    sf::Sprite bgSprite(bgTex);

    // 3) Load cell sprites
    sf::Texture yourTex, enemyTex, allyTex, neutralTex;
    if (!yourTex.loadFromFile("assets/images/apna.png") ||
        !enemyTex.loadFromFile("assets/images/dushman.png") ||
        !allyTex.loadFromFile("assets/images/dost.png") ||
        !neutralTex.loadFromFile("assets/images/neutral.png")) {
        std::cerr << "Failed to load one of the cell sprites (apna/dushman/dost/neutral)\n";
        return;
    }
    sf::Sprite yourSpr(yourTex), enemySpr(enemyTex), allySpr(allyTex), neutralSpr(neutralTex);

    // 4) Create window sized to background
    unsigned int winW = bgTex.getSize().x;
    unsigned int winH = bgTex.getSize().y;
    sf::RenderWindow win(sf::VideoMode(winW, winH), "SFML Map View");
    win.setFramerateLimit(60);

    // 5) Determine grid dimensions from MapSystem
    int totalCells = mapSystem.getTotalCells();
    const int rows = 2;                  // fixed by design
    const int cols = totalCells / rows;  // should be 4

    float cellW = static_cast<float>(winW) / cols;
    float cellH = static_cast<float>(winH) / rows;

    // Main loop
    while (win.isOpen()) {
        sf::Event ev;
        while (win.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed ||
                (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape)) {
                win.close();
            }
        }

        win.clear();
        win.draw(bgSprite);

        // Draw each cell
        for (int idx = 0; idx < totalCells; ++idx) {
            int kingdomId = mapSystem.getKingdomAtCell(idx);
            if (kingdomId == 0) continue;  // empty cell

            // Determine which sprite to draw
            sf::Sprite* spriteToDraw = nullptr;
            int activeId = 0;
            if (mapSystem.getKingdomManager().getActiveKingdom()) {
                activeId = mapSystem.getKingdomManager().getActiveKingdom()->getId();
            }

            if (kingdomId == activeId) {
                spriteToDraw = &yourSpr;
            }
            else {
                RelationshipStatus rel =
                    mapSystem.getDiplomacy().getRelationship(activeId, kingdomId);
                if (rel == RelationshipStatus::WAR)          spriteToDraw = &enemySpr;
                else if (rel == RelationshipStatus::ALLIANCE ||
                    rel == RelationshipStatus::PEACE_TREATY) spriteToDraw = &allySpr;
                else                                          spriteToDraw = &neutralSpr;
            }

            // Compute cell position
            int col = idx % cols;
            int row = idx / cols;
            float left = col * cellW;
            float top = row * cellH;

            // Scale and center sprite in cell
            sf::FloatRect sBounds = spriteToDraw->getLocalBounds();
            float scaleX = (cellW * 0.5f) / sBounds.width;
            float scaleY = (cellH * 0.5f) / sBounds.height;
            spriteToDraw->setScale(scaleX, scaleY);
            spriteToDraw->setPosition(
                left + (cellW - sBounds.width * scaleX) / 2,
                top + (cellH - sBounds.height * scaleY) / 2
            );

            win.draw(*spriteToDraw);
        }

        win.display();
    }
}
