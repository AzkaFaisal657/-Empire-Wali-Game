#pragma once


#ifndef BUILDING_MANAGER_H
#define BUILDING_MANAGER_H

#include "Building.h"
#include <cstddef>




class BuildingManager
{
private:
    void grow()
    {               //VECTOR EXPANSION LOGIC IN DMA SENSE
        size_t newCap = (capacity == 0 ? 4 : capacity * 2);
        Building** arr = new Building * [newCap];
        for (size_t i = 0; i < count; ++i) arr[i] = buildings[i];
        delete[] buildings;
        buildings = arr;
        capacity = newCap;
    }

    Building** buildings;
    size_t count;
    size_t capacity;
public:
    BuildingManager()
        : buildings(nullptr), count(0), capacity(0) {
    }

    ~BuildingManager() {
        clear();
        delete[] buildings;
    }

    template<typename T>    //??????????????????
    T* addBuilding(int r, int c, float w, float h, const sf::Vector2f& pos) {
        if (count >= capacity) grow();
        T* b = new T();             //create a new building object
        b->init(r, c, w, h, pos);       //call its init function
        buildings[count++] = b;         //store it
        return b;                           //return the building itself okay okay
    }


    // Generic counter: returns how many T* exist in the array  , made this cuz we're gonna need the individual count of each building later
    template<typename T>
    int getCount() const
    {
        int cnt = 0;
        for (size_t i = 0; i < count; ++i) {
            if (dynamic_cast<T*>(buildings[i]))
                ++cnt;
        }
        return cnt;
    }

    // REMOVE BUILDING FUNCTION - The core delete button
    bool removeBuildingAt(const sf::Vector2f& p) {
        // Check each building one by one
        for (size_t i = 0; i < count; ++i) {
            // If click hits this building
            if (buildings[i]->contains(p)) {
                delete buildings[i];
                // Slide all other buildings left to fill the empty spot
                for (size_t j = i; j < count - 1; ++j) {
                    buildings[j] = buildings[j + 1];
                }
                count--;// One less building now
                return true;// Removal successful
            }
        }
        return false;// No building found at position
    }

    Building* getBuildingAt(size_t index) const {
        if (index < count) return buildings[index];
        return nullptr;
    }


    void drawAllBuildings(sf::RenderWindow& win) {  //loops thru all the buildings and calls their draw function
        for (size_t i = 0; i < count; ++i)
            buildings[i]->draw(win);
    }

    Building* getBuildingAt(const sf::Vector2f& p) {    //loops thru all buildings, calls contains() on each buildign , if contains(worldPos) returns true, we return that building
        for (size_t i = 0; i < count; ++i)
            if (buildings[i]->contains(p)) return buildings[i];
        return nullptr;
    }

    void clear() {
        for (size_t i = 0; i < count; ++i) delete buildings[i];
        count = 0;
    }

    size_t getBuildingCount() const { return count; }

    void triggerRandomFireOnAll() {
        if (count == 0) return;

        int buildingsToBurn = 1 + (rand() % 3);
        for (int i = 0; i < buildingsToBurn; i++) {
            int index = rand() % count;
            if (buildings[index]->getState() != BuildingState::ON_FIRE) {
                buildings[index]->setState(BuildingState::ON_FIRE);
            }
        }
    }

    // Fire only on dwellings
    void triggerFireOnDwellings() {
        int dwellingCount = getCount<Dwelling>();
        if (dwellingCount == 0) return;

        int dwellingsToBurn = 1 + (rand() % 3);
        dwellingsToBurn = std::min(dwellingsToBurn, dwellingCount);

        for (int i = 0; i < dwellingsToBurn; i++) {
            // Find a random dwelling that's not on fire
            Building* dwelling = nullptr;
            do {
                int index = rand() % count;
                dwelling = dynamic_cast<Dwelling*>(buildings[index]);
            } while (!dwelling || dwelling->getState() == BuildingState::ON_FIRE);

            dwelling->setState(BuildingState::ON_FIRE);
        }
    }

    // Fire only on non-dwellings
    void triggerFireOnNonDwellings() {
        int nonDwellingCount = count - getCount<Dwelling>();
        if (nonDwellingCount == 0) return;

        int buildingsToBurn = 1 + (rand() % 3);
        buildingsToBurn = std::min(buildingsToBurn, nonDwellingCount);

        for (int i = 0; i < buildingsToBurn; i++) {
            // Find a random non-dwelling that's not on fire
            Building* b = nullptr;
            do {
                int index = rand() % count;
                b = buildings[index];
            } while (dynamic_cast<Dwelling*>(b) || b->getState() == BuildingState::ON_FIRE);

            b->setState(BuildingState::ON_FIRE);
        }
    }
    // Function to extinguish all fires
    void extinguishAllFires() {
        for (size_t i = 0; i < count; ++i) {
            if (buildings[i]->getState() == BuildingState::ON_FIRE) {
                buildings[i]->setState(BuildingState::NORMAL);
            }
        }
    }


    /// Resources
    int getWoodProductionPerHour() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* rb = dynamic_cast<ResourceBuilding*>(buildings[i]);
            if (rb && rb->getResourceType() == "wood")
                sum += rb->getProductionPerHour();
        }
        return sum;
    }


    /// Food production per hour, summed across all Farms
    int getFoodProductionPerHour() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* fb = dynamic_cast<Farm*>(buildings[i]);
            if (fb)
                sum += fb->getProductionPerHour();
        }
        return sum;
    }

    // Total storage capacity from all Storehouses
// Returns the sum of getMaxStorage() for every Storehouse on the map:
    int getTotalStorageCapacity() const {
        int cap = 0;
        for (size_t i = 0; i < count; ++i) {
            if (auto* sh = dynamic_cast<Storehouse*>(buildings[i])) {
                cap += sh->getMaxStorage();
            }
        }
        return cap;
    }

    int getStoneProductionPerHour() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* rb = dynamic_cast<ResourceBuilding*>(buildings[i]);
            if (rb && rb->getResourceType() == "stone")
                sum += rb->getProductionPerHour();
        }
        return sum;
    }

    /// Population
    int getLowerClassPopulation() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* ld = dynamic_cast<LowerDwelling*>(buildings[i]);
            if (ld) sum += ld->getTotalPopulation();
        }
        return sum;
    }

    int getMiddleClassPopulation() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* md = dynamic_cast<MiddleDwelling*>(buildings[i]);
            if (md) sum += md->getTotalPopulation();
        }
        return sum;
    }

    int getUpperClassPopulation() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* ud = dynamic_cast<UpperDwelling*>(buildings[i]);
            if (ud) sum += ud->getTotalPopulation();
        }
        return sum;
    }

    // total population (all classes)
    int getTotalPopulation() const {
        return getLowerClassPopulation()
            + getMiddleClassPopulation()
            + getUpperClassPopulation();
    }



    int getTotalSpyCount() const { //iterates through all of your taverns (max 3 btw) and then calls getSpyCount on each of them and returns the total sum
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* tavern = dynamic_cast<Tavern*>(buildings[i]);
            if (tavern) {
                sum += tavern->getSpyCount();
            }
        }
        return sum;
    }


    int getTotalGuardCount() const {
        int sum = 0;
        for (size_t i = 0; i < count; ++i) {
            auto* guardhouse = dynamic_cast<Guardhouse*>(buildings[i]);
            if (guardhouse) {
                sum += guardhouse->getGuardCount();
            }
        }
        return sum;
    }
    //iterates thru ALL building and checks if kisi building ka R and C are same as the R C where u want another building,
                                         // not too optimized i think, something like  making a 2D boolean array wouldven been better???
                                               // didnt use the same logic as the click thing

    bool hasBuildingAt(int r, int c) const {
        for (size_t i = 0; i < count; ++i) {
            Building* b = buildings[i];
            sf::Vector2i size = b->getSize();

            // Check all cells this building occupies
            for (int dr = 0; dr < size.y; ++dr) {
                for (int dc = 0; dc < size.x; ++dc) {
                    if (b->getGridR() + dr == r && b->getGridC() + dc == c) {
                        return true;
                    }
                }
            }
        }
        return false;
    }


};



#endif // BUILDING_MANAGER_H