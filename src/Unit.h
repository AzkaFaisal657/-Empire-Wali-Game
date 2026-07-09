#pragma once
#ifndef UNIT_H
#define UNIT_H

#include <string>

class Unit {
protected:
    float speed;           // movement speed
    float foodPerHour;     // food consumption per hour
    float baseStrength;    // base combat strength

public:
    Unit(float spd, float food, float str)
        : speed(spd), foodPerHour(food), baseStrength(str)
    {}

    virtual ~Unit() = default;

    float getSpeed() const { return speed; }
    float getFoodPerHour() const { return foodPerHour; }
    float getBaseStrength() const { return baseStrength; }

    // each unit type must name itself
    virtual const char* getTypeName() const = 0;
};


class SpearWoman : public Unit {
public:
    SpearWoman()
        : Unit(
            /*spd=*/1.2f,
            /*food=*/5.f,
            /*str=*/10.f    // base melee strength
        )
    {}

    const char* getTypeName() const override {
        return "SpearWoman";
    }

};


class ValkyrieRanger : public Unit {
public:
    ValkyrieRanger()
        : Unit(
            /*spd=*/1.8f,
            /*food=*/5.f,
            /*str=*/8.f    // base ranged strength
        )
    {}

    const char* getTypeName() const override {
        return "ValkyrieRanger";
    }
};


#endif // UNIT_H
