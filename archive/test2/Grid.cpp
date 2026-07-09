#include "Stronghold.h"
#include <stdexcept>

// Cell implementation
Cell::Cell(int x, int y) : pos(x, y), occupied(false) {}

void Cell::setBuilding(std::shared_ptr<Building> b) {
    building = b;
    occupied = (b != nullptr);
    if (b) {
        b->setPosition(pos);
    }
}

// Grid implementation
Grid::Grid(int width, int height) : width(width), height(height) {
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

bool Grid::placeBuilding(std::shared_ptr<Building> building, int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    if (cells[y][x].isOccupied()) {
        return false;
    }
    
    cells[y][x].setBuilding(building);
    return true;
}

bool Grid::removeBuilding(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    
    cells[y][x].setBuilding(nullptr);
    return true;
}

Cell& Grid::getCell(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Cell coordinates out of bounds");
    }
    return cells[y][x];
}

const Cell& Grid::getCell(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Cell coordinates out of bounds");
    }
    return cells[y][x];
} 