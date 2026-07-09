#pragma once


#ifndef POPULATION_H
#define POPULATION_H

#include "ResourceManager.h"
#include "BuildingManager.h"




// ───────── Population Base + Subclasses ─────────
class Population {
protected:
    BuildingManager& mgr;   // reference to your building manager
public:
    ResourceManager res;     // their own resource bucket

    Population(BuildingManager& bm) : mgr(bm) {}
    virtual ~Population() = default;

    virtual int   getCount()            const = 0; // number of dwellings
    virtual int   getPopulation()       const = 0; // total people
    virtual float getSatisfactionLevel()const = 0;
    virtual void  setSatisfactionLevel(float) = 0;

    virtual const char* getName()       const = 0;
};

class LowerClassPopulation : public Population {
    float satisfaction = 0.75f;
public:
    LowerClassPopulation(BuildingManager& bm) : Population(bm) {}

    int getCount() const override {
        return mgr.getCount<LowerDwelling>();
    }
    int getPopulation() const override {
        return mgr.getLowerClassPopulation();
    }
    float getSatisfactionLevel() const override {
        return satisfaction;
    }
    void setSatisfactionLevel(float s) override {
        satisfaction = s;
    }
    const char* getName() const override {
        return "Lower Class";
    }
};

class MiddleClassPopulation : public Population {
    float satisfaction = 0.85f;
public:
    MiddleClassPopulation(BuildingManager& bm) : Population(bm) {}

    int getCount() const override {
        return mgr.getCount<MiddleDwelling>();
    }
    int getPopulation() const override {
        return mgr.getMiddleClassPopulation();
    }
    float getSatisfactionLevel() const override {
        return satisfaction;
    }
    void setSatisfactionLevel(float s) override {
        satisfaction = s;
    }
    const char* getName() const override {
        return "Middle Class";
    }
};

class UpperClassPopulation : public Population {
    float satisfaction = 0.95f;
public:
    UpperClassPopulation(BuildingManager& bm) : Population(bm) {}

    int getCount() const override {
        return mgr.getCount<UpperDwelling>();
    }
    int getPopulation() const override {
        return mgr.getUpperClassPopulation();
    }
    float getSatisfactionLevel() const override {
        return satisfaction;
    }
    void setSatisfactionLevel(float s) override {
        satisfaction = s;
    }
    const char* getName() const override {
        return "Upper Class";
    }
};





#endif // POPULATION_H