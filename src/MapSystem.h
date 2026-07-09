#pragma once
#ifndef MAP_SYSTEM_H
#define MAP_SYSTEM_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "KingdomManager.h"
#include "Diplomacy.h"

// Simple text-based map system for kingdoms
class MapSystem {
private:
    static const int MAP_WIDTH = 50;
    static const int MAP_HEIGHT = 20;

   

    KingdomManager& kingdomManager;
    Diplomacy& diplomacy;
    std::vector<int>   cellKingdomIds;  // size = TOTAL_CELLS

public:
    // grid dimensions
    static const int COLS = 4;
    static const int ROWS = 2;
    static const int TOTAL_CELLS = COLS * ROWS;



    KingdomManager& getKingdomManager() { return kingdomManager; }
    Diplomacy& getDiplomacy() { return diplomacy; }


    MapSystem(KingdomManager& km, Diplomacy& dp)
        : kingdomManager(km), diplomacy(dp)
    {
        // initialize 8 slots to empty (0)
        for (int i = 0; i < TOTAL_CELLS; ++i)
            cellKingdomIds.push_back(0);
    }
    // SFML needs these:
    int  getTotalCells()      const { return TOTAL_CELLS; }
    int getKingdomAtCell(int idx) const {
        if (idx < 0 || idx >= TOTAL_CELLS) return 0;
        return cellKingdomIds[idx];
    }

    int getCellIndexForKingdom(int kingdomId) const {
        for (int i = 0; i < TOTAL_CELLS; ++i)
            if (cellKingdomIds[i] == kingdomId)
                return i;
        return -1;
    }



    // Update map with current kingdom positions
    void updateMap() {
        // Clear the map first
 // clear
        for (int i = 0; i < TOTAL_CELLS; ++i)
            cellKingdomIds[i] = 0;

        // assign up to 8 kingdoms in order
        int count = kingdomManager.getKingdomCount();
        if (count > TOTAL_CELLS) count = TOTAL_CELLS;

        for (int i = 0; i < count; ++i) {
            Kingdom* k = kingdomManager.getKingdomByIndex(i);
            if (k)
                cellKingdomIds[i] = k->getId();
        }
    }

    // Move a kingdom on the map
    bool moveKingdom(int kingdomId, int newX, int newY)
    {
        // check bounds
        if (newX < 0 || newX >= COLS || newY < 0 || newY >= ROWS)
            return false;

        int newIdx = newY * COLS + newX;
        // occupied?
        if (cellKingdomIds[newIdx] != 0)
            return false;

        int oldIdx = getCellIndexForKingdom(kingdomId);
        if (oldIdx < 0)
            return false;

        // relocate
        cellKingdomIds[oldIdx] = 0;
        cellKingdomIds[newIdx] = kingdomId;
        return true;;
    }

    // Get the kingdom ID at a specific position
    int getKingdomIdAt(int x, int y) const
    {
        if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) {

            if (x < 0 || x >= COLS || y < 0 || y >= ROWS)
                return 0;
            return cellKingdomIds[y * COLS + x];
        }
    }
    // Calculate distance between two kingdoms
    double calculateDistance(int kingdom1Id, int kingdom2Id) const 
    {

        int idx1 = getCellIndexForKingdom(kingdom1Id);
        int idx2 = getCellIndexForKingdom(kingdom2Id);
        if (idx1 < 0 || idx2 < 0)
            return -1.0;

        int x1 = idx1 % COLS;
        int y1 = idx1 / COLS;
        int x2 = idx2 % COLS;
        int y2 = idx2 / COLS;

        int dx = x2 - x1;
        int dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }

    // Check if two kingdoms are neighbors (share territory borders)
    bool areNeighbors(int kingdom1Id, int kingdom2Id) const 
    {
        double d = calculateDistance(kingdom1Id, kingdom2Id);
        return d > 0 && d <= 1.0;
    }

    // Display the world map
    void displayMap() const
    {
        std::cout << "\n=== WORLD MAP (2x4) ===\n";
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                int idx = r * COLS + c;
                int kid = cellKingdomIds[idx];
                char sym = '.';

                if (kid != 0) {
                    Kingdom* K = kingdomManager.getKingdomById(kid);
                    Kingdom* A = kingdomManager.getActiveKingdom();
                    if (A && K && A->getId() == kid) {
                        sym = '@';
                    }
                    else {
                        RelationshipStatus rel = diplomacy.getRelationship(
                            A ? A->getId() : 0,
                            kid
                        );
                        switch (rel) {
                        case RelationshipStatus::WAR:          sym = 'X'; break;
                        case RelationshipStatus::ALLIANCE:     sym = 'A'; break;
                        case RelationshipStatus::PEACE_TREATY: sym = 'P'; break;
                        default:                               sym = 'N'; break;
                        }
                    }
                }
                std::cout << '[' << sym << ']';
            }
            std::cout << '\n';
        }
        std::cout << "======================\n";

        // Legend
        std::cout << "Legend: @ Your Kingdom | K Other Kingdom | A Ally | P Peace Treaty | X Enemy | * Territory | . Empty" << std::endl;
        std::cout << "======================" << std::endl;
    }

    // Display neighboring kingdoms
    void displayNeighbors(int kingdomId) const
    {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        std::cout << "\n===== NEIGHBORS OF " << kingdom->getName() << " =====" << std::endl;

        bool hasNeighbors = false;
        for (size_t i = 0; i < kingdomManager.getKingdomCount(); i++) {
            Kingdom* other = kingdomManager.getKingdomByIndex(i);
            if (!other || other->getId() == kingdomId) continue;

            if (areNeighbors(kingdomId, other->getId())) {
                double distance = calculateDistance(kingdomId, other->getId());
                RelationshipStatus rel = diplomacy.getRelationship(kingdomId, other->getId());

                std::cout << "- " << other->getName() << " (Distance: " << std::fixed << std::setprecision(1) << distance << ") - ";

                switch (rel) {
                case RelationshipStatus::WAR:
                    std::cout << "AT WAR";
                    break;
                case RelationshipStatus::ALLIANCE:
                    std::cout << "ALLIANCE";
                    break;
                case RelationshipStatus::PEACE_TREATY:
                    std::cout << "PEACE TREATY";
                    break;
                default:
                    std::cout << "NEUTRAL";
                }

                std::cout << std::endl;
                hasNeighbors = true;
            }
        }

        if (!hasNeighbors) {
            std::cout << "No neighboring kingdoms found." << std::endl;
        }

        std::cout << "======================" << std::endl;
    }
};




#endif // MAP_SYSTEM_H
