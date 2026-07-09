#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

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

public:
    Cell(int x = 0, int y = 0);
    bool isOccupied() const { return occupied; }
    void setBuilding(std::shared_ptr<Building> b);
    std::shared_ptr<Building> getBuilding() const { return building; }
    Position getPosition() const { return pos; }
};

// Grid class - manages the game map
class Grid {
private:
    std::vector<std::vector<Cell>> cells;
    int width;
    int height;

public:
    Grid(int width = 10, int height = 10);
    bool placeBuilding(std::shared_ptr<Building> building, int x, int y);
    bool removeBuilding(int x, int y);
    Cell& getCell(int x, int y);
    const Cell& getCell(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

// Base Building class
class Building {
protected:
    std::string name;
    int cost;
    Position position;

public:
    Building(const std::string& name, int cost);
    virtual ~Building() = default;
    virtual void update() = 0;
    std::string getName() const { return name; }
    int getCost() const { return cost; }
    void setPosition(const Position& pos) { position = pos; }
    Position getPosition() const { return position; }
};

// Specific building types
class Dwelling : public Building {
private:
    int maxOccupants;
    int currentOccupants;

public:
    Dwelling(int maxOccupants = 4);
    void update() override;
    int getCurrentOccupants() const { return currentOccupants; }
};

class ResourceBuilding : public Building {
protected:
    int productionRate;
    std::string resourceType;

public:
    ResourceBuilding(const std::string& name, int cost, const std::string& resourceType, int productionRate);
    void update() override;
    int getProductionRate() const { return productionRate; }
};

class Woodcutter : public ResourceBuilding {
public:
    Woodcutter();
    void update() override;
}; 