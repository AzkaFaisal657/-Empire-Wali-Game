#pragma once
// Bank.h
#ifndef BANK_H
#define BANK_H
using namespace std;
class Bank {
    // paisa[resource][class]: 0=wood,1=stone × 0=lower,1=middle,2=upper
    int paisa[2][3];

public:
    Bank() {
        // you can change these initial balances as desired
        for (int r = 0; r < 2; ++r)
            for (int i = 0; i < 3; ++i)
                paisa[r][i] = 100;
    }

    // Deposit into the bank for class i, resource r
    void deposit(int classIdx, int resourceIdx, int amount) {
        paisa[resourceIdx][classIdx] += amount;
    }

    // Withdraw up to amount; returns actual withdrawn
 
      int withdraw(int classIdx, int resourceIdx, int amount) {
            int& bal = paisa[resourceIdx][classIdx];
            int w = (amount < bal) ? amount : bal;
            if (amount > bal) {
                // warn user
                std::cout << "Warning: requested " << amount
                    << " but only " << bal << " available. Withdrawing "
                    << w << " instead.\n";
            }
            bal -= w;
            return w;
        }
    

    // Check current balance
    int getBalance(int classIdx, int resourceIdx) const {
        return paisa[resourceIdx][classIdx];
    }
};

#endif 

