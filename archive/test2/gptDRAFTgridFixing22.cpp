// Diamond-Grid Game Engine with Persistent Textures & Building Class
// SFML 2.6.1, VS2022, single-file example

#include <SFML/Graphics.hpp>
#include <iostream>

//=============================================================================
// Building class: holds sprite, grid position, and pixel-perfect hit test
//=============================================================================
class Building {
public:
    Building() : gridR(-1), gridC(-1), initialized(false) {} //default constructor frfr

    void init(const sf::Texture& tex, const sf::Image& img,
        int r, int c, float tileW, float tileH, const sf::Vector2f& worldPos)
    {
        image = img;
        sprite.setTexture(tex);
        sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y);
        float scale = tileW / float(tex.getSize().x);
        sprite.setScale(scale, scale);
        sprite.setPosition(worldPos.x, worldPos.y + tileH / 2.f);
        gridR = r;
        gridC = c;
        initialized = true;
    }

    void draw(sf::RenderWindow& win) const {
        if (initialized)
            win.draw(sprite);
    }

    bool contains(const sf::Vector2f& worldPos) const {
        if (!initialized) return false;
        if (!sprite.getGlobalBounds().contains(worldPos)) return false;
        sf::Vector2f local = sprite.getInverseTransform().transformPoint(worldPos);
        int x = int(local.x + 0.5f);
        int y = int(local.y + 0.5f);
        if (x < 0 || y < 0 || x >= int(image.getSize().x) || y >= int(image.getSize().y))
            return false;
        return image.getPixel(x, y).a > 0;
    }

    int gridR, gridC;

private:
    bool initialized;
    sf::Sprite sprite;
    sf::Image image;
};

//=============================================================================
// GameEngine: manages window, grid, buildings, textures, and main loop
//=============================================================================
class GameEngine {
public:
    GameEngine();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void handleClick(int mx, int my);
    bool pointInConvex(const sf::ConvexShape&, const sf::Vector2f&);

    sf::RenderWindow window;
    sf::RectangleShape ground, hud;

    // Grid constants
    static const int GRID_R = 12;
    static const int GRID_C = 12;
    static constexpr float TILE_W = 64.f;
    static constexpr float TILE_H = 32.f;

    sf::ConvexShape cells[GRID_R][GRID_C];
    sf::Vector2f cellPos[GRID_R][GRID_C];

    // Building storage
    static const int MAX_BUILDINGS = 16;
    Building buildings[MAX_BUILDINGS];
    int buildingCount;

    // Persistent texture & image for buildings
    sf::Texture houseTexture;
    sf::Image   houseImage;

    int selectedR, selectedC;
};

//=============================================================================
// GameEngine implementation
//=============================================================================
GameEngine::GameEngine()
    : window({ 1024, 768 }, "Diamond Grid Engine"),
    ground({ 1024, 768 - 100 }), hud({ 1024, 100 }),
    buildingCount(0), selectedR(-1), selectedC(-1)
{
    window.setFramerateLimit(60);
    ground.setFillColor(sf::Color(50, 150, 50));
    hud.setFillColor(sf::Color(80, 80, 80));
    hud.setPosition(0, 768 - 100);

    // Compute grid positions and shapes
    float originX = 1024 / 2.f;
    float originY = 100.f;
    for (int r = 0; r < GRID_R; ++r) {
        for (int c = 0; c < GRID_C; ++c) {
            float x = (c - r) * (TILE_W * 0.5f) + originX;
            float y = (c + r) * (TILE_H * 0.5f) + originY;
            cellPos[r][c] = { x, y };
            sf::ConvexShape d;
            d.setPointCount(4);
            d.setPoint(0, { TILE_W / 2, 0 });
            d.setPoint(1, { TILE_W, TILE_H / 2 });
            d.setPoint(2, { TILE_W / 2, TILE_H });
            d.setPoint(3, { 0, TILE_H / 2 });
            d.setOrigin(TILE_W / 2, TILE_H / 2);
            d.setPosition(x, y);
            d.setFillColor(sf::Color(100, 200, 100));
            d.setOutlineColor(sf::Color::Black);
            d.setOutlineThickness(1.f);
            cells[r][c] = d;
        }
    }

    // Load persistent house texture & image
    if (!houseTexture.loadFromFile("assets/images/house1.png")) {
        std::cerr << "Failed to load house1.png" << std::endl;
    }
    houseImage = houseTexture.copyToImage();

    // Hardcoded building grid positions
    int br[4] = { 5, 4, 7, 2 };
    int bc[4] = { 6, 4, 8, 3 };
    for (int i = 0; i < 4 && buildingCount < MAX_BUILDINGS; ++i) {
        int r = br[i], c = bc[i];
        buildings[buildingCount].init(
            houseTexture, houseImage,
            r, c, TILE_W, TILE_H,
            cellPos[r][c]
        );
        ++buildingCount;
    }
}

bool GameEngine::pointInConvex(const sf::ConvexShape& shape, const sf::Vector2f& _p) {
    sf::Vector2f p = shape.getInverseTransform().transformPoint(_p);
    int n = shape.getPointCount();
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        auto pi = shape.getPoint(i);
        auto pj = shape.getPoint(j);
        if (((pi.y > p.y) != (pj.y > p.y)) &&
            (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x)) {
            inside = !inside;
        }
    }
    return inside;
}

void GameEngine::handleClick(int mx, int my) {
    sf::Vector2f mp = window.mapPixelToCoords({ mx, my });
    selectedR = selectedC = -1;
    for (int r = 0; r < GRID_R; ++r) {
        for (int c = 0; c < GRID_C; ++c) {
            if (cells[r][c].getGlobalBounds().contains(mp) &&
                pointInConvex(cells[r][c], mp)) {
                selectedR = r;
                selectedC = c;
                std::cout << "Clicked cell: (" << r << "," << c << ")\n";
            }
        }
    }
}

void GameEngine::processEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed)
            window.close();
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left)
            handleClick(e.mouseButton.x, e.mouseButton.y);
    }
}

void GameEngine::update() {
    // Future game logic goes here
}

void GameEngine::render() {
    window.clear();
    window.draw(ground);
    // Draw grid cells
    for (int r = 0; r < GRID_R; ++r)
        for (int c = 0; c < GRID_C; ++c)
            window.draw(cells[r][c]);
    // Draw buildings
    for (int i = 0; i < buildingCount; ++i)
        buildings[i].draw(window);
    // Highlight selected cell
    if (selectedR >= 0) {
        auto& d = cells[selectedR][selectedC];
        d.setFillColor(sf::Color(200, 100, 100));
        window.draw(d);
        d.setFillColor(sf::Color(100, 200, 100));
    }
    window.draw(hud);
    window.display();
}

void GameEngine::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

int main() {
    GameEngine game;
    game.run();
    return 0;
}
