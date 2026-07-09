#pragma once
#ifndef KINGDOM_MANAGER_H
#define KINGDOM_MANAGER_H

#include <vector>
#include <memory>
#include <iostream>
#include "Kingdom.h"

// Manages all kingdoms in the multiplayer game
class KingdomManager {
private:
    std::vector<std::unique_ptr<Kingdom>> kingdoms;
    int activeKingdomIndex;
    int nextKingdomId;

public:
    KingdomManager() : activeKingdomIndex(-1), nextKingdomId(1) {}

    // Create a new kingdom
    Kingdom* createKingdom(const std::string& name, const std::string& playerName) {
        auto kingdom = std::make_unique<Kingdom>(nextKingdomId++, name, playerName);

        // Set initial map position based on kingdom count
        int baseX = kingdoms.size() * 10;
        int baseY = kingdoms.size() * 5;
        kingdom->setMapPosition(baseX, baseY);

        // Setup initial buildings
        kingdom->setupInitialBuildings();

        Kingdom* ptr = kingdom.get();
        kingdoms.push_back(std::move(kingdom));
        return ptr;
    }

    // Delete a kingdom (probably won't be needed but included for completeness)
    bool deleteKingdom(int kingdomId) {
        for (size_t i = 0; i < kingdoms.size(); i++) {
            if (kingdoms[i]->getId() == kingdomId) {
                if (static_cast<int>(i) == activeKingdomIndex) {
                    activeKingdomIndex = -1; // No active kingdom
                }
                kingdoms.erase(kingdoms.begin() + i);
                return true;
            }
        }
        return false;
    }

    // Get a kingdom by ID
    Kingdom* getKingdomById(int kingdomId) {
        for (auto& kingdom : kingdoms) {
            if (kingdom->getId() == kingdomId) {
                return kingdom.get();
            }
        }
        return nullptr;
    }

    // Get a kingdom by index
    Kingdom* getKingdomByIndex(int index) {
        if (index >= 0 && index < static_cast<int>(kingdoms.size())) {
            return kingdoms[index].get();
        }
        return nullptr;
    }

    // Get active kingdom
    Kingdom* getActiveKingdom() {
        if (activeKingdomIndex >= 0 && activeKingdomIndex < static_cast<int>(kingdoms.size())) {
            return kingdoms[activeKingdomIndex].get();
        }
        return nullptr;
    }

    // Set active kingdom by index
    bool setActiveKingdomByIndex(int index) {
        if (index >= 0 && index < static_cast<int>(kingdoms.size())) {
            // Set the previous active kingdom to inactive
            if (activeKingdomIndex >= 0 && activeKingdomIndex < static_cast<int>(kingdoms.size())) {
                kingdoms[activeKingdomIndex]->setActive(false);
            }

            // Set the new active kingdom
            activeKingdomIndex = index;
            kingdoms[activeKingdomIndex]->setActive(true);
            return true;
        }
        return false;
    }

    // Set active kingdom by ID
    bool setActiveKingdomById(int kingdomId) {
        for (size_t i = 0; i < kingdoms.size(); i++) {
            if (kingdoms[i]->getId() == kingdomId) {
                return setActiveKingdomByIndex(static_cast<int>(i));
            }
        }
        return false;
    }

    // Get all kingdoms
    const std::vector<std::unique_ptr<Kingdom>>& getAllKingdoms() const {
        return kingdoms;
    }

    // Get kingdom count
    size_t getKingdomCount() const {
        return kingdoms.size();
    }

    // Get active kingdom index
    int getActiveKingdomIndex() const {
        return activeKingdomIndex;
    }

    // Print all kingdoms
    void printAllKingdoms() const {
        std::cout << "\n===== ALL KINGDOMS =====" << std::endl;
        for (size_t i = 0; i < kingdoms.size(); i++) {
            std::cout << (i == static_cast<size_t>(activeKingdomIndex) ? "* " : "  ");
            std::cout << i + 1 << ". " << kingdoms[i]->getName()
                << " (Ruled by: " << kingdoms[i]->getPlayerName() << ")"
                << std::endl;
        }
        std::cout << "======================" << std::endl;
    }
};

#endif // KINGDOM_MANAGER_H
