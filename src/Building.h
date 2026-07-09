#pragma once
#ifndef BUILDING_H
#define BUILDING_H
#include <iostream>
using namespace std;
#include <SFML/Graphics.hpp>
#include <string>
#include "Common.h"

class Building {
protected:
    bool initialized;
    int gridR, gridC;
    float tileW, tileH;
    sf::Vector2f worldPos;
    int level;
    BuildingState state;
    sf::Texture texture;
    sf::Image image;
    sf::Sprite sprite;
    sf::Texture fireTexture;
    sf::Sprite fireSprite;
    bool fireTextureLoaded = false;

public:
    Building()
        : gridR(-1), gridC(-1), initialized(false), level(1), state(BuildingState::NORMAL) {
    }
    virtual ~Building() = default;

    void init(int r, int c, float w, float h, const sf::Vector2f& pos) {
        gridR = r; gridC = c;
        tileW = w; tileH = h;
        worldPos = pos;
        level = 1;
        state = BuildingState::NORMAL;
        loadTexture();
    }

    virtual void loadTexture() {
        if (!texture.loadFromFile(getTexturePath())) {
            std::cout << "Failed to load texture: " << getTexturePath() << std::endl;
            return;
        }
        image = texture.copyToImage();
        sprite.setTexture(texture);
        sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y);
        float scale = tileW / float(texture.getSize().x);
        sprite.setScale(scale, scale);
        sprite.setPosition(worldPos.x, worldPos.y + tileH / 2.f);
        initialized = true;
    }

    virtual std::string getTexturePath() const {
        std::string base = "unknown";
        std::string suf = (state == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(level) + suf + ".png";
    }

    void setLevel(int lvl) {
        if (lvl >= 1 && lvl <= 3) { level = lvl; loadTexture(); }
    }
    /*void setState(BuildingState s) {
        state = s; loadTexture();
    }*/

    void loadFireTexture() {
        if (!fireTexture.loadFromFile("assets/images/fire.png")) {
            std::cout << "Failed to load fire texture!\n";
            return;
        }
        fireSprite.setTexture(fireTexture);
        // Center the fire sprite horizontally and place it at the building's base
        fireSprite.setOrigin(fireTexture.getSize().x / 2.f, fireTexture.getSize().y);
        // Scale fire to match building width
        float scale = tileW / float(fireTexture.getSize().x) * 0.8f; // 80% of building width
        fireSprite.setScale(scale, scale);
        fireSprite.setPosition(worldPos.x, worldPos.y + tileH / 2.f);
        fireTextureLoaded = true;
    }

    void draw(sf::RenderWindow& w) const {
        if (initialized) {
            w.draw(sprite);
            if (state == BuildingState::ON_FIRE && fireTextureLoaded) {
                w.draw(fireSprite);
            }
        }
    }

    void setState(BuildingState s) {
        state = s;
        if (state == BuildingState::ON_FIRE && !fireTextureLoaded) {
            loadFireTexture();
        }
        loadTexture();
    }


    //CONTAINS:    This is a pixel- perfect click detectoion,

    bool contains(const sf::Vector2f& p) const {
        if (!initialized) return false;         //if the building is not initialized, return false
        if (!sprite.getGlobalBounds().contains(p)) return false;          //if clicked outside buildings bounding box, return false ,, get global bounds gives u the rectangle that surrounds the sprite after moving scaling, rotating whatever
        sf::Vector2f loc = sprite.getInverseTransform().transformPoint(p);   //convert the click into local coordinates of the plain
        int x = int(loc.x + 0.5f), y = int(loc.y + 0.5f);
        if (x < 0 || y < 0 || x >= int(image.getSize().x) || y >= int(image.getSize().y)) return false;  //check if the pixels exists within the image bounds
        return image.getPixel(x, y).a > 0;      //only return true if pixel at x,y is not transparent (a>0)
    }

    int getGridR() const { return gridR; }  //gets u the row of the builing
    int getGridC() const { return gridC; }  //gets u the column of the building
    int getLevel() const { return level; }  // gets u the level DUHH
    BuildingState getState() const { return state; }
    virtual sf::Vector2i getSize() const { return { 1,1 }; }
};

//=============================================================================
// Derived classes (same as before)...
//=============================================================================

class Storehouse : public Building {
    int maxStorage;
public:
    Storehouse() : maxStorage(100) {}
    std::string getTexturePath() const override {
        std::string base = "store", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
    }
    sf::Vector2i getSize() const override { return { 1,2 }; }
    static constexpr int capTable[4] = { 0,2000, 1500, 2000 };

    int getMaxStorage() const {
        int lvl = getLevel();
        if (lvl < 1) lvl = 1;
        if (lvl > 3) lvl = 3;
        return capTable[lvl];
    }
};
class Keep : public Building {
    int playerLevel;
public:
    Keep() : playerLevel(1) {}
    std::string getTexturePath() const override {
        std::string base = "keep", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
    }



    sf::Vector2i getSize() const override { return { 3,3 }; }
    void setPlayerLevel(int p) { playerLevel = p; } int getPlayerLevel()const { return playerLevel; }
};
class Dwelling : public Building
{
protected: int population;
public:

    static constexpr int popTable[4] = { 0,  50, 100, 150 }; // this is the fixed value for each of the levels , can adjust from here easy and i think implementing more levels would NOT be too hard either

    /// Returns the population for this dwelling, adjusted for fire
    int getTotalPopulation() const
    {
        int lvl = getLevel();
        // clamp to [1–3]
        if (lvl < 1) lvl = 1;
        if (lvl > 3) lvl = 3;
        int basePop = popTable[lvl];
        // if on fire, they only have half population
        if (getState() == BuildingState::ON_FIRE)
            return basePop / 2;
        return basePop;
    }


    // In Dwelling (header)
public:
    // returns population for this dwelling (you control how it scales)



};
class LowerDwelling : public Dwelling
{
public: std::string getTexturePath() const override
{
    std::string base = "lower", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
}
};
class MiddleDwelling : public Dwelling
{
public: std::string getTexturePath() const override
{
    std::string base = "middle", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
}
};
class UpperDwelling : public Dwelling
{
public: std::string getTexturePath() const override
{
    std::string base = "upper", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
}
};
class ResourceBuilding : public Building

{
protected: int resourceProduction;
public:


    static constexpr int prodTable[4] = { 0, 100, 200, 300 };
    /// Returns production per hour for this building, halved if on fire
   virtual int getProductionPerHour() const {
        int lvl = getLevel();
        // clamp to valid range
        if (lvl < 1) lvl = 1;
        if (lvl > 3) lvl = 3;
        int baseProd = prodTable[lvl];
        // if burning, only half production
        if (getState() == BuildingState::ON_FIRE)
            return baseProd / 2;
        return baseProd;
    }


    virtual std::string getResourceType() const = 0;
};
class StoneMansion : public ResourceBuilding
{
public: std::string getTexturePath() const override
{
    std::string base = "stone", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
} std::string getResourceType() const override { return "stone"; }
};
class Woodcutter : public ResourceBuilding
{
public: std::string getTexturePath() const override
{
    std::string base = "wood", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
} std::string getResourceType() const override { return "wood"; }
};




//=============================================================================
// Farm (produces Food) — a 2×2 resource building
//=============================================================================
class Farm : public ResourceBuilding {
public:
    // Textures follow your existing pattern: "farm"+level+"[fire].png"
    std::string getTexturePath() const override {
        std::string base = "farm";
        std::string suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
    }
    std::string getResourceType() const override { return "food"; }

    // make it a 2×2 on the grid
    sf::Vector2i getSize() const override { return { 2, 2 }; }



    // 5 farms level1 → 100 units @ avg 2 food/hr → need 200 food/hr total
// so each level1 farm = 40 food/hr, level2 = 80, level3 = 120
    int getProductionPerHour() const override {
        int lvl = getLevel();
        if (lvl < 1) lvl = 1; if (lvl > 3) lvl = 3;
        static constexpr int foodTable[4] = { 0,200,400,600 };
        int p = foodTable[lvl];
        if (getState() == BuildingState::ON_FIRE) p /= 2;
        return p;
    }



};






class MilitaryBuilding : public Building
{
public: sf::Vector2i getSize()const override
{
    return{ 2,2 };
}
};
class Guardhouse : public MilitaryBuilding
{
public:


    // fixed guards per level: [0]=unused, lvl1=10, lvl2=20, lvl3=30
    static constexpr int guardTable[4] = { 0, 10, 20, 30 };

    Guardhouse() = default;


    int getGuardCount() const { //returns guard count of one guardhouse, reduce to half if on fire (ez shit)
        int lvl = getLevel();
        if (lvl < 1) lvl = 1;
        if (lvl > 3) lvl = 3;

        int base = guardTable[lvl];
        if (getState() == BuildingState::ON_FIRE)
            return base / 2;
        return base;
    }



    std::string getTexturePath() const override
    {
        std::string base = "guard", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
    }
};
class Tavern : public MilitaryBuilding
{
public:

    // fixed spies per level: [0]=unused, lvl1=10, lvl2=20, lvl3=25
    static constexpr int spyTable[4] = { 0, 10, 20, 25 };

    Tavern() = default;


    // returns # of spies based on level, half if on fire
    int getSpyCount() const
    {
        int lvl = getLevel();
        if (lvl < 1) lvl = 1;
        if (lvl > 3) lvl = 3;

        int base = spyTable[lvl];
        if (getState() == BuildingState::ON_FIRE)
            return base / 2;
        return base;
    }


    std::string getTexturePath() const override

    {
        std::string base = "tavern", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
        return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
    }
};
class Barracks : public MilitaryBuilding
{
public: std::string getTexturePath() const override
{
    std::string base = "barracks", suf = (getState() == BuildingState::ON_FIRE ? "fire" : "");
    return "assets/images/" + base + std::to_string(getLevel()) + suf + ".png";
}
};




#endif 