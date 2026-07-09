#include "Stronghold.h"

// Base Building implementation
Building::Building(const std::string& name, int cost) : name(name), cost(cost) {}

// Dwelling implementation
Dwelling::Dwelling(int maxOccupants)
    : Building("Dwelling", 100), maxOccupants(maxOccupants), currentOccupants(0) {}

void Dwelling::update() {
    // TODO: Implement population growth/decline logic
    if (currentOccupants < maxOccupants) {
        currentOccupants++;
    }
}

// ResourceBuilding implementation
ResourceBuilding::ResourceBuilding(const std::string& name, int cost, 
                                 const std::string& resourceType, int productionRate)
    : Building(name, cost), resourceType(resourceType), productionRate(productionRate) {}

void ResourceBuilding::update() {
    // Base resource production logic
    // Actual resource generation will be handled by the resource management system
}

// Woodcutter implementation
Woodcutter::Woodcutter()
    : ResourceBuilding("Woodcutter", 150, "Wood", 5) {}

void Woodcutter::update() {
    ResourceBuilding::update();
    // Additional woodcutter-specific logic can be added here
} 