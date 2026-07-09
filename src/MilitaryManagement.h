#pragma once
#ifndef MILITARY_MANAGEMENT_H
#define MILITARY_MANAGEMENT_H

#include "Unit.h"

#include <iostream>

class MilitaryManagement {
    Unit** units = nullptr;  // dynamic array of Unit pointers
    size_t count = 0;
    size_t capacity = 0;

    void grow() {
        size_t newCap = (capacity == 0 ? 4 : capacity * 2);
        Unit** tmp = new Unit * [newCap];
        for (size_t i = 0; i < count; ++i)
            tmp[i] = units[i];
        delete[] units;
        units = tmp;
        capacity = newCap;
    }

public:
    MilitaryManagement() = default;

    ~MilitaryManagement() {
        for (size_t i = 0; i < count; ++i)
            delete units[i];
        delete[] units;
    }

    // Recruit n new units of type T
    template<typename T>
    void recruit(size_t n) {
        for (size_t i = 0; i < n; ++i) {
            if (count >= capacity) grow();
            units[count++] = new T();
        }
    }

    // Total count of all units
    size_t getTotalUnits() const {
        return count;
    }

    // Total food consumption per hour by all units
    float getTotalFoodConsumptionPerHour() const {
        float sum = 0.f;
        for (size_t i = 0; i < count; ++i)
            sum += units[i]->getFoodPerHour();
        return sum;
    }

    // Remove the last unit, return pointer (so caller can delete and inspect)
    Unit* removeLastUnit() {
        if (count == 0) return nullptr;
        Unit* u = units[--count];
        return u;
    }


    // Print a summary by type, applying leader morale
    // morale: multiplier e.g. 0.9f = units fight at 90% strength
    void printSummary(float morale) const {
        size_t swCount = 0;
        float  swBase = 0.f;
        size_t vrCount = 0;
        float  vrBase = 0.f;

        for (size_t i = 0; i < count; ++i) {
            if (auto* sw = dynamic_cast<SpearWoman*>(units[i])) {
                ++swCount;
                swBase += sw->getBaseStrength();
            }
            else if (auto* vr = dynamic_cast<ValkyrieRanger*>(units[i])) {
                ++vrCount;
                vrBase += vr->getBaseStrength();
            }
        }

        float swFinal = swBase * morale;
        float vrFinal = vrBase * morale;
        float totalFood = getTotalFoodConsumptionPerHour();

        std::cout << "\n--- MILITARY SUMMARY ---\n";
        std::cout << "SpearWomen: " << swCount
            << " | Base STR=" << swBase
            << " | Final STR=" << swFinal << "\n";
        std::cout << "Rangers:    " << vrCount
            << " | Base STR=" << vrBase
            << " | Final STR=" << vrFinal << "\n";
        std::cout << "Total units:        " << getTotalUnits() << "\n";
        std::cout << "Total Food/hr need: " << totalFood << "\n\n";
    }
};

#endif // MILITARY_MANAGEMENT_H

