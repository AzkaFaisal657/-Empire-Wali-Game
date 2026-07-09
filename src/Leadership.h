#pragma once
// Leadership.h
#ifndef LEADERSHIP_H
#define LEADERSHIP_H

#include <string>
#include <cstdlib>        // for rand()
#include "ResourceManager.h"
#include "Population.h"   // for index meaning

// Maximum leaders you plan to register
static const int MaxLeaders = 5;

struct Leader {
    std::string name;
    int         age;
    std::string race;
    // popularity among classes: [0]=lower, [1]=middle, [2]=upper
    float         popularity[3];
    // resource‐split ratios summing to 1.0: same indexing
    float       ratio[3];

  // fraction of production that goes to leadership
 // e.g. 0.10 = 10% to leadership, rest to classes
        float       treasuryRatio;
        float       inflationFactor;    // new: how much prices rise under this leader (1.0 = no inflation)
        float       morale;

};

class Leadership {
    ResourceManager res;           // pool of resources under leadership control
    Leader          leaders[MaxLeaders];
    int             leaderCount = 0;
    int             currentLeader = -1;   // index into leaders[]

public:
    Leadership() = default;

    // register a new candidate; call before using election()
    bool addLeader(const Leader& l) {
        if (leaderCount >= MaxLeaders) return false;
        leaders[leaderCount++] = l;
        return true;
    }

    // immediately pick a random new leader from ALL candidates
    void election() {
        if (leaderCount == 0) return;
        currentLeader = std::rand() % leaderCount;
    }

    // pick a random new leader DIFFERENT from the current one
    void coup() {
        if (leaderCount <= 1) return;
        int choice;
        do {
            choice = std::rand() % leaderCount;
        } while (choice == currentLeader);
        currentLeader = choice;
    }

    // get the active leader; caller must ensure currentLeader>=0
     // Non-const version (for code that mutates the leader):
    Leader& getCurrentLeader() {
        return leaders[currentLeader];
    }

    // Const overload (for const contexts like printing):
    const Leader& getCurrentLeader() const {
        return leaders[currentLeader];
    }

    // clear and deposit into leadership
    ResourceManager& getResManager() { return res; }

    // new const overload
    const ResourceManager& getResManager() const { return res; }
    //                                                 ^^^^^
};

#endif // LEADERSHIP_H
