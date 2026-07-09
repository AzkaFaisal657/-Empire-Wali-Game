// KharchaManager.h
#ifndef KHARCHA_MANAGER_H
#define KHARCHA_MANAGER_H

class KharchaManager {
    // current spending for each class [0]=lower,1=middle,2=upper
    float currentKharcha[3];
    // ideal spending for each class
    float idealKharcha[3];
    // inflation factor (1.0 = no inflation)
    float inflation = 1.0f;

public:
    KharchaManager() {
        currentKharcha[0] =idealKharcha[0] = 150;  //these are the ideal paisa they should be giving the government fr
        currentKharcha[1] = idealKharcha[1] = 250;
        currentKharcha[2] = idealKharcha[2] = 400;

        



    }

    // record actual spending this turn (automate or input manually)
    void setCurrent(int classIdx, float amount) {
        currentKharcha[classIdx] = amount * inflation;
    }

    // optionally adjust ideal budgets
    void setIdeal(int classIdx, float amount) {
        idealKharcha[classIdx] = amount;
    }

    // adjust inflation factor
    void setInflation(float f) {
        inflation = (f > 0.f) ? f : 1.0f;
    }
    float getInflation() const { return inflation; }

    float getCurrent(int classIdx) const { return currentKharcha[classIdx]; }
    float getIdeal(int classIdx) const { return idealKharcha[classIdx]; }

    // Simple satisfaction delta:  
    //   -0.05 if under‐spending, +0.05 if over‐spending, 0 if exactly at ideal
    float computeSatDelta(int classIdx) const 
    {
        if (currentKharcha[classIdx] < idealKharcha[classIdx]) return -0.05f;
        else if (currentKharcha[classIdx] > idealKharcha[classIdx]) return +0.02f;
        else                                                         return 0.f;
    }
};

#endif // KHARCHA_MANAGER_H
#pragma once
