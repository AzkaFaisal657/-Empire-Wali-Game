#pragma once
// TradeManager.h
#ifndef TRADE_MANAGER_H
#define TRADE_MANAGER_H

#include "Common.h"
#include <cstdlib>   // for rand/srand
#include <ctime>     // for time

class Population;  // forward







class TradeManager
{
    static constexpr int NumTrades = 6;
    Trade trades[NumTrades] = {
        // from, to, resource, amount, isBad,                  description
        {2, 0, ResourceType::Wood,  120, true,  "Overcharged Lower Class"},
        {2, 1, ResourceType::Stone,  80, false, "Generous to Middle Class"},
        {1, 0, ResourceType::Stone, 100, false, "Fair deal with Lower Class"},
        {0, 2, ResourceType::Wood,   50, true,  "Underpaid Upper Class"},
        {0, 1, ResourceType::Stone,  70, true,  "Shortchanged Middle Class"},
        {1, 2, ResourceType::Wood,  140, false, "Gift to Upper Class"}
    };

    float timer = 0.f;

public:
    TradeManager() {
        std::srand((unsigned)std::time(nullptr));  // seed once
    }

    /// Call every frame, dt in seconds. Fires one trade per second.
    template<typename PopArray>
    void update(float dt, PopArray& pops ) {
        timer += dt;
        if (timer < 1.f) return;
        timer -= 1.f;

        // pick a random trade
        int idx = std::rand() % NumTrades;
        const Trade& t = trades[idx];

        //if (globalGOAT)
        //{        // log it
        //    std::cout << "["
        //        << (t.isBad ? "Bad" : "Good")
        //        << "] "
        //        << pops[t.fromClass]->getName() << " → "
        //        << pops[t.toClass]->getName() << ": "
        //        << t.amount << (t.resource == ResourceType::Wood ? " wood" : " stone")
        //        << "  (" << t.description << ")\n";

        //}
        // apply resource transfer
        if (t.resource == ResourceType::Wood) {
            pops[t.fromClass]->res.removeWood(t.amount);
            pops[t.toClass]->res.addWood(t.amount);
        }
        else {
            pops[t.fromClass]->res.removeStone(t.amount);
            pops[t.toClass]->res.addStone(t.amount);
        }

        // on bad trade, dent satisfaction
        if (t.isBad) {
            float sat = pops[t.toClass]->getSatisfactionLevel();
            pops[t.toClass]->setSatisfactionLevel(std::max(0.f, sat - 0.05f));
        }
    }
};




#endif // TRADE_MANAGER_H

