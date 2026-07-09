#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>

// Forward declarations
class Building;
class Resource;
class Population;

// Position structure for grid coordinates
struct Position {
    int x;
    int y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
};

// Grid cell class
class Cell {
private:
    Position pos;
    std::shared_ptr<Building> building;
    bool occupied;
    sf::RectangleShape shape;

public:
    Cell(int x = 0, int y = 0) : pos(x, y), occupied(false) {
        shape.setSize(sf::Vector2f(64, 64)); // 64x64 pixel cells
        shape.setPosition(x * 64.f, y * 64.f);
        shape.setFillColor(sf::Color(34, 139, 34)); // Forest green
        shape.setOutlineThickness(1.f);
        shape.setOutlineColor(sf::Color::Black);
    }

    bool isOccupied() const { return occupied; }
    
    void setBuilding(std::shared_ptr<Building> b) {
        building = b;
        occupied = (b != nullptr);
        if (b) {
            b->setPosition(pos);
        }
    }

    std::shared_ptr<Building> getBuilding() const { return building; }
    Position getPosition() const { return pos; }
    const sf::RectangleShape& getShape() const { return shape; }
};

// Base Building class
class Building {
protected:
    std::string name;
    int cost;
    Position position;
    sf::Sprite sprite;
    static sf::Texture buildingTextures[3]; // For different building types
    static bool texturesLoaded;

public:
    Building(const std::string& name, int cost) : name(name), cost(cost) {
        if (!texturesLoaded) {
            loadTextures();
        }
    }

    static void loadTextures() {
        if (!texturesLoaded) {
            // You'll need to provide these texture files
            buildingTextures[0].loadFromFile("assets/dwelling.png");
            buildingTextures[1].loadFromFile("assets/woodcutter.png");
            buildingTextures[2].loadFromFile("assets/barracks.png");
            texturesLoaded = true;
        }
    }

    virtual ~Building() = default;
    virtual void update() = 0;
    virtual void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    std::string getName() const { return name; }
    int getCost() const { return cost; }
    
    void setPosition(const Position& pos) {
        position = pos;
        sprite.setPosition(pos.x * 64.f, pos.y * 64.f);
    }
    
    Position getPosition() const { return position; }
};

bool Building::texturesLoaded = false;
sf::Texture Building::buildingTextures[3];

// Grid class
class Grid {
private:
    std::vector<std::vector<Cell>> cells;
    int width;
    int height;

public:
    Grid(int width = 10, int height = 10) : width(width), height(height) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Grid dimensions must be positive");
        }
        
        cells.resize(height);
        for (int y = 0; y < height; y++) {
            cells[y].resize(width);
            for (int x = 0; x < width; x++) {
                cells[y][x] = Cell(x, y);
            }
        }
    }

    bool placeBuilding(std::shared_ptr<Building> building, int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return false;
        }
        
        if (cells[y][x].isOccupied()) {
            return false;
        }
        
        cells[y][x].setBuilding(building);
        return true;
    }

    void draw(sf::RenderWindow& window) {
        // Draw grid cells
        for (const auto& row : cells) {
            for (const auto& cell : row) {
                window.draw(cell.getShape());
                if (cell.isOccupied()) {
                    cell.getBuilding()->draw(window);
                }
            }
        }
    }

    bool removeBuilding(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return false;
        }
        cells[y][x].setBuilding(nullptr);
        return true;
    }

    Cell& getCell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Cell coordinates out of bounds");
        }
        return cells[y][x];
    }

    const Cell& getCell(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Cell coordinates out of bounds");
        }
        return cells[y][x];
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

// Specific building types
class Dwelling : public Building {
private:
    int maxOccupants;
    int currentOccupants;

public:
    Dwelling(int maxOccupants = 4)
        : Building("Dwelling", 100), maxOccupants(maxOccupants), currentOccupants(0) {
        sprite.setTexture(buildingTextures[0]);
        sprite.setScale(0.5f, 0.5f); // Adjust scale to fit cell size
    }

    void update() override {
        if (currentOccupants < maxOccupants) {
            currentOccupants++;
        }
    }

    int getCurrentOccupants() const { return currentOccupants; }
};

class ResourceBuilding : public Building {
protected:
    int productionRate;
    std::string resourceType;

public:
    ResourceBuilding(const std::string& name, int cost, 
                    const std::string& resourceType, int productionRate)
        : Building(name, cost), resourceType(resourceType), productionRate(productionRate) {}

    void update() override {
        // Base resource production logic
    }

    int getProductionRate() const { return productionRate; }
};

class Woodcutter : public ResourceBuilding {
public:
    Woodcutter()
        : ResourceBuilding("Woodcutter", 150, "Wood", 5) {
        sprite.setTexture(buildingTextures[1]);
        sprite.setScale(0.5f, 0.5f);
    }

    void update() override {
        ResourceBuilding::update();
    }
};

// Game class to handle overall game state
class Game {
private:
    sf::RenderWindow window;
    Grid grid;
    sf::View gameView;
    bool isDragging;
    sf::Vector2i lastMousePos;

public:
    Game() : grid(15, 15), isDragging(false) {
        window.create(sf::VideoMode(1024, 768), "Stronghold Game");
        window.setFramerateLimit(60);
        
        // Initialize view for camera movement
        gameView = window.getDefaultView();
        window.setView(gameView);
    }

    void handleInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        isDragging = true;
                        lastMousePos = sf::Mouse::getPosition(window);
                    }
                    else if (event.mouseButton.button == sf::Mouse::Left) {
                        handleBuildingPlacement(event.mouseButton.x, event.mouseButton.y);
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        isDragging = false;
                    }
                    break;

                case sf::Event::MouseMoved:
                    if (isDragging) {
                        sf::Vector2i currentPos = sf::Mouse::getPosition(window);
                        sf::Vector2i delta = lastMousePos - currentPos;
                        gameView.move(delta.x * 1.f, delta.y * 1.f);
                        window.setView(gameView);
                        lastMousePos = currentPos;
                    }
                    break;

                case sf::Event::MouseWheelScrolled:
                    float zoom = (event.mouseWheelScroll.delta < 0) ? 1.1f : 0.9f;
                    gameView.zoom(zoom);
                    window.setView(gameView);
                    break;
            }
        }
    }

    void handleBuildingPlacement(int mouseX, int mouseY) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
        
        int gridX = static_cast<int>(worldPos.x / 64);
        int gridY = static_cast<int>(worldPos.y / 64);
        
        if (gridX >= 0 && gridX < grid.getWidth() && gridY >= 0 && gridY < grid.getHeight()) {
            // For demo purposes, place a Dwelling on left click
            auto dwelling = std::make_shared<Dwelling>();
            grid.placeBuilding(dwelling, gridX, gridY);
        }
    }

    void update() {
        // Update game logic here
    }

    void render() {
        window.clear(sf::Color(150, 200, 50)); // Light green background
        grid.draw(window);
        window.display();
    }

    void run() {
        while (window.isOpen()) {
            handleInput();
            update();
            render();
        }
    }
};

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 