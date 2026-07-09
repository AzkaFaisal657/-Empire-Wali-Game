#pragma once
// ResourceManager.h
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H



class ResourceManager
{
    int wood = 0, stone = 0;
    int capacity = 0;
public:
    void setCapacity(int c) {
        capacity = c;
        if (wood > capacity) wood = capacity;
        if (stone > capacity) stone = capacity;
    }
    void addWood(int w) { wood += w; }
    void addStone(int s) { stone += s; }
    void removeWood(int w) { wood = std::max(0, wood - w); }
    void removeStone(int s) { stone = std::max(0, stone - s); }
    int getWood()  const { return wood; }
    int getStone() const { return stone; }



};




#endif // RESOURCE_MANAGER_H
