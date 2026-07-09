#pragma once
#ifndef WAR_SYSTEM_H
#define WAR_SYSTEM_H

#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <iostream>
#include "KingdomManager.h"
#include "Diplomacy.h"

// Battle result
struct BattleResult {
    int attackerId;
    int defenderId;
    bool attackerWon;
    int attackerLosses;
    int defenderLosses;
    std::string description;
    std::time_t timestamp;

    BattleResult(int attId, int defId, bool won, int attLoss, int defLoss, const std::string& desc)
        : attackerId(attId), defenderId(defId), attackerWon(won),
        attackerLosses(attLoss), defenderLosses(defLoss), description(desc) {
        timestamp = std::time(nullptr);
    }
};

// War status between kingdoms
struct WarStatus {
    int warId;
    int kingdom1Id;
    int kingdom2Id;
    std::time_t startTime;
    std::time_t endTime; // 0 means ongoing
    std::vector<BattleResult> battles;
    bool isActive;

    WarStatus(int id, int k1, int k2)
        : warId(id), kingdom1Id(k1), kingdom2Id(k2), isActive(true) {
        startTime = std::time(nullptr);
        endTime = 0;
    }
};

// War system for kingdom conflicts
class WarSystem {
private:
    std::vector<WarStatus> wars;
    int nextWarId;
    KingdomManager& kingdomManager;
    Diplomacy& diplomacy;

    // Random number generator
    std::mt19937 rng;

public:
    WarSystem(KingdomManager& km, Diplomacy& dip)
        : kingdomManager(km), diplomacy(dip), nextWarId(1), rng(std::random_device()()) {
    }

    // Declare war between kingdoms
    int declareWar(int attackerId, int defenderId, bool updateDiplomacy = true) {
        // Check if kingdoms exist
        Kingdom* attacker = kingdomManager.getKingdomById(attackerId);
        Kingdom* defender = kingdomManager.getKingdomById(defenderId);

        if (!attacker || !defender) {
            return -1;
        }

        // Check if they're already at war
        if (areKingdomsAtWar(attackerId, defenderId)) {
            return -1;
        }

        // Create new war
        wars.emplace_back(nextWarId, attackerId, defenderId);

        // Update diplomatic relationship if requested
        if (updateDiplomacy) {
            diplomacy.setRelationship(attackerId, defenderId, RelationshipStatus::WAR,
                attacker->getName() + " declared war on " + defender->getName());
        }

        std::cout << "\n" << attacker->getName() << " has declared war on " << defender->getName() << "!" << std::endl;

        return nextWarId++;
    }

    // End war between kingdoms
    bool endWar(int warId, bool attackerSurrender, const std::string& reason) {
        for (auto& war : wars) {
            if (war.warId == warId && war.isActive) {
                war.isActive = false;
                war.endTime = std::time(nullptr);

                Kingdom* kingdom1 = kingdomManager.getKingdomById(war.kingdom1Id);
                Kingdom* kingdom2 = kingdomManager.getKingdomById(war.kingdom2Id);

                if (kingdom1 && kingdom2) {
                    std::string winner = attackerSurrender ? kingdom2->getName() : kingdom1->getName();
                    std::string loser = attackerSurrender ? kingdom1->getName() : kingdom2->getName();

                    std::cout << "\nThe war between " << kingdom1->getName() << " and "
                        << kingdom2->getName() << " has ended." << std::endl;
                    std::cout << winner << " has defeated " << loser << "." << std::endl;
                    std::cout << "Reason: " << reason << std::endl;

                    // Update diplomatic relationship to peace treaty
                    diplomacy.setRelationship(war.kingdom1Id, war.kingdom2Id,
                        RelationshipStatus::PEACE_TREATY,
                        "War ended: " + reason);
                }

                return true;
            }
        }

        return false;
    }

    // Check if kingdoms are at war
    bool areKingdomsAtWar(int kingdom1Id, int kingdom2Id) const {
        for (const auto& war : wars) {
            if (war.isActive &&
                ((war.kingdom1Id == kingdom1Id && war.kingdom2Id == kingdom2Id) ||
                    (war.kingdom1Id == kingdom2Id && war.kingdom2Id == kingdom1Id))) {
                return true;
            }
        }
        return false;
    }

    // Get active war between kingdoms
    const WarStatus* getWarBetween(int kingdom1Id, int kingdom2Id) const {
        for (const auto& war : wars) {
            if (war.isActive &&
                ((war.kingdom1Id == kingdom1Id && war.kingdom2Id == kingdom2Id) ||
                    (war.kingdom1Id == kingdom2Id && war.kingdom2Id == kingdom1Id))) {
                return &war;
            }
        }
        return nullptr;
    }

    // Get war by ID
    const WarStatus* getWarById(int warId) const {
        for (const auto& war : wars) {
            if (war.warId == warId) {
                return &war;
            }
        }
        return nullptr;
    }

    // Get all active wars for a kingdom
    std::vector<const WarStatus*> getAllWarsForKingdom(int kingdomId) const {
        std::vector<const WarStatus*> result;
        for (const auto& war : wars) {
            if (war.isActive && (war.kingdom1Id == kingdomId || war.kingdom2Id == kingdomId)) {
                result.push_back(&war);
            }
        }
        return result;
    }

    // Simulate a battle between kingdoms
    BattleResult simulateBattle(int attackerId, int defenderId) {
        // Get kingdoms
        Kingdom* attacker = kingdomManager.getKingdomById(attackerId);
        Kingdom* defender = kingdomManager.getKingdomById(defenderId);

        if (!attacker || !defender) {
            return BattleResult(attackerId, defenderId, false, 0, 0, "Invalid kingdoms");
        }

        // Recalculate kingdom strengths
        attacker->calculateStrength();
        defender->calculateStrength();

        // Get attack and defense values
        float attackPower = attacker->getAttackPower();
        float defensePower = defender->getDefensePower();

        // Add some randomness (±20%)
        std::uniform_real_distribution<float> dist(0.8f, 1.2f);
        attackPower *= dist(rng);
        defensePower *= dist(rng);

        // Determine winner
        bool attackerWins = attackPower > defensePower;

        // Calculate losses (more for the loser)
        float attackerLossRate = attackerWins ? 0.05f : 0.15f;
        float defenderLossRate = attackerWins ? 0.20f : 0.08f;

        // Apply losses to military units (simplified)
        int attackerLosses = static_cast<int>(attacker->getMilitary().getTotalUnits() * attackerLossRate);
        int defenderLosses = static_cast<int>(defender->getMilitary().getTotalUnits() * defenderLossRate);

        // Create description
        std::string description;
        if (attackerWins) {
            description = attacker->getName() + " launched a successful attack against " +
                defender->getName() + ". ";
        }
        else {
            description = attacker->getName() + " attack was repelled by " +
                defender->getName() + "'s defenses. ";
        }

        description += "Casualties: " + std::to_string(attackerLosses) + " (" +
            attacker->getName() + "), " + std::to_string(defenderLosses) +
            " (" + defender->getName() + ").";

        // Create battle result
        BattleResult result(attackerId, defenderId, attackerWins, attackerLosses, defenderLosses, description);

        // Find and update the appropriate war
        for (auto& war : wars) {
            if (war.isActive &&
                ((war.kingdom1Id == attackerId && war.kingdom2Id == defenderId) ||
                    (war.kingdom1Id == defenderId && war.kingdom2Id == attackerId))) {
                war.battles.push_back(result);
                break;
            }
        }

        return result;
    }

    // Display war status for a kingdom
    void displayWarStatusForKingdom(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        auto activeWars = getAllWarsForKingdom(kingdomId);

        std::cout << "\n===== WAR STATUS FOR " << kingdom->getName() << " =====" << std::endl;

        if (activeWars.empty()) {
            std::cout << "Not currently engaged in any wars." << std::endl;
        }
        else {
            std::cout << "Active Wars: " << activeWars.size() << std::endl;

            for (const auto* war : activeWars) {
                bool isAttacker = (war->kingdom1Id == kingdomId);
                int enemyId = isAttacker ? war->kingdom2Id : war->kingdom1Id;
                Kingdom* enemy = kingdomManager.getKingdomById(enemyId);

                if (!enemy) continue;

                std::cout << "\n- War with " << enemy->getName() << ":" << std::endl;

                // War duration
                std::time_t now = std::time(nullptr);
                int daysPassed = static_cast<int>((now - war->startTime) / (24 * 60 * 60));
                std::cout << "  Duration: " << (daysPassed > 0 ? std::to_string(daysPassed) + " days" : "Started today") << std::endl;

                // Battle summary
                int wins = 0, losses = 0;
                for (const auto& battle : war->battles) {
                    if ((battle.attackerId == kingdomId && battle.attackerWon) ||
                        (battle.defenderId == kingdomId && !battle.attackerWon)) {
                        wins++;
                    }
                    else {
                        losses++;
                    }
                }

                std::cout << "  Battles: " << war->battles.size() << " (Won: " << wins << ", Lost: " << losses << ")" << std::endl;

                // Recent battles
                if (!war->battles.empty()) {
                    std::cout << "  Recent Battle: " << war->battles.back().description << std::endl;
                }
            }
        }

        std::cout << "===================================" << std::endl;
    }

    // Display battle history for a war
    void displayBattleHistory(int warId) const {
        const WarStatus* war = getWarById(warId);
        if (!war) {
            std::cout << "War not found." << std::endl;
            return;
        }

        Kingdom* kingdom1 = kingdomManager.getKingdomById(war->kingdom1Id);
        Kingdom* kingdom2 = kingdomManager.getKingdomById(war->kingdom2Id);

        if (!kingdom1 || !kingdom2) {
            std::cout << "Invalid kingdoms in war record." << std::endl;
            return;
        }

        std::cout << "\n===== WAR HISTORY: " << kingdom1->getName() << " vs " << kingdom2->getName() << " =====" << std::endl;

        if (war->isActive) {
            std::cout << "Status: ACTIVE" << std::endl;
        }
        // Count victories
        int k1Victories = 0, k2Victories = 0;
        for (const auto& battle : war->battles) {
            if (battle.attackerId == war->kingdom1Id && battle.attackerWon) k1Victories++;
            else if (battle.attackerId == war->kingdom2Id && battle.attackerWon) k2Victories++;
            else if (battle.defenderId == war->kingdom1Id && !battle.attackerWon) k1Victories++;
            else if (battle.defenderId == war->kingdom2Id && !battle.attackerWon) k2Victories++;
        }

        std::cout << kingdom1->getName() << " Victories: " << k1Victories << std::endl;
        std::cout << kingdom2->getName() << " Victories: " << k2Victories << std::endl;

        std::cout << "\nBattle History:" << std::endl;

        std::cout << "===================================" << std::endl;
    }
};

#endif // WAR_SYSTEM_H
