#include "Stronghold.h"
#include <iostream>

void printGrid(const Grid& grid) {
    for (int y = 0; y < grid.getHeight(); y++) {
        for (int x = 0; x < grid.getWidth(); x++) {
            const Cell& cell = grid.getCell(x, y);
            if (cell.isOccupied()) {
                std::cout << "[" << cell.getBuilding()->getName()[0] << "]";
            } else {
                std::cout << "[ ]";
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    try {
        // Create a 10x10 grid
        Grid grid(10, 10);

        // Create some buildings
        auto dwelling = std::make_shared<Dwelling>();
        auto woodcutter = std::make_shared<Woodcutter>();

        // Place buildings on the grid
        if (grid.placeBuilding(dwelling, 3, 4)) {
            std::cout << "Placed dwelling at (3,4)" << std::endl;
        }

        if (grid.placeBuilding(woodcutter, 3, 5)) {
            std::cout << "Placed woodcutter at (3,5)" << std::endl;
        }

        // Print the grid
        std::cout << "\nGrid state:\n";
        printGrid(grid);

        // Simulate a few updates
        std::cout << "\nSimulating updates...\n";
        for (int i = 0; i < 3; i++) {
            dwelling->update();
            woodcutter->update();
            std::cout << "Update " << i + 1 << ": Dwelling occupants: " 
                     << dwelling->getCurrentOccupants() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 