#pragma once
#pragma once
#ifndef DIPLOMACY_H
#define DIPLOMACY_H

#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <algorithm>
#include "Kingdom.h"

// Relationship between kingdoms
enum class RelationshipStatus {
    WAR,
    NEUTRAL,
    ALLIANCE,
    PEACE_TREATY
};

// Treaty or agreement between kingdoms
struct Treaty {
    int treatyId;
    int kingdomId1;
    int kingdomId2;
    RelationshipStatus status;
    std::time_t creationTime;
    std::time_t expirationTime; // 0 means no expiration
    std::string terms;
    bool isActive;

    Treaty(int id, int k1, int k2, RelationshipStatus s, const std::string& t, int durationDays = 0)
        : treatyId(id), kingdomId1(k1), kingdomId2(k2), status(s), terms(t), isActive(true) {
        creationTime = std::time(nullptr);
        expirationTime = (durationDays > 0) ? creationTime + (durationDays * 24 * 60 * 60) : 0;
    }

    bool isExpired() const {
        if (expirationTime == 0) return false; // No expiration
        return std::time(nullptr) > expirationTime;
    }

    std::string getStatusString() const {
        switch (status) {
        case RelationshipStatus::WAR: return "WAR";
        case RelationshipStatus::NEUTRAL: return "NEUTRAL";
        case RelationshipStatus::ALLIANCE: return "ALLIANCE";
        case RelationshipStatus::PEACE_TREATY: return "PEACE TREATY";
        default: return "UNKNOWN";
        }
    }
};

// Historical relationship event
struct RelationshipEvent {
    int kingdomId1;
    int kingdomId2;
    RelationshipStatus statusBefore;
    RelationshipStatus statusAfter;
    std::string description;
    std::time_t timestamp;

    RelationshipEvent(int k1, int k2, RelationshipStatus before, RelationshipStatus after, const std::string& desc)
        : kingdomId1(k1), kingdomId2(k2), statusBefore(before), statusAfter(after), description(desc) {
        timestamp = std::time(nullptr);
    }
};

// Manages diplomatic relations between kingdoms
class Diplomacy {
private:
    // Track relationships between kingdoms
    std::map<std::pair<int, int>, RelationshipStatus> relationships;

    // Track treaties and agreements
    std::vector<Treaty> treaties;
    int nextTreatyId;

    // Track relationship history
    std::vector<RelationshipEvent> history;

    // Store kingdom manager reference
    KingdomManager& kingdomManager;

    // Get sorted pair of kingdom IDs for consistent mapping
    std::pair<int, int> getOrderedPair(int kingdom1, int kingdom2) const {
        return (kingdom1 < kingdom2) ? std::make_pair(kingdom1, kingdom2) : std::make_pair(kingdom2, kingdom1);
    }

public:
    Diplomacy(KingdomManager& km) : kingdomManager(km), nextTreatyId(1) {}

    // Get current relationship between kingdoms
    RelationshipStatus getRelationship(int kingdom1, int kingdom2) const {
        auto pair = getOrderedPair(kingdom1, kingdom2);
        auto it = relationships.find(pair);
        if (it != relationships.end()) {
            return it->second;
        }
        return RelationshipStatus::NEUTRAL; // Default relationship
    }

    // Set relationship between kingdoms
    void setRelationship(int kingdom1, int kingdom2, RelationshipStatus status, const std::string& reason) {
        auto pair = getOrderedPair(kingdom1, kingdom2);

        // Record previous status for history
        RelationshipStatus prevStatus = getRelationship(kingdom1, kingdom2);

        // Update relationship
        relationships[pair] = status;

        // Record in history
        history.emplace_back(kingdom1, kingdom2, prevStatus, status, reason);

        // If this is a war declaration, apply penalties for aggressor if breaking an alliance
        if (status == RelationshipStatus::WAR &&
            (prevStatus == RelationshipStatus::ALLIANCE || prevStatus == RelationshipStatus::PEACE_TREATY)) {

            // Get kingdoms
            Kingdom* k1 = kingdomManager.getKingdomById(kingdom1);
            Kingdom* k2 = kingdomManager.getKingdomById(kingdom2);

            if (k1 && k2) {
                // For now, we'll just log the betrayal
                std::cout << "BETRAYAL: " << k1->getName() << " has broken their agreement with "
                    << k2->getName() << "!" << std::endl;

                // In a more advanced implementation, we could apply penalties to morale,
                // population satisfaction, or resource production
            }
        }
    }

    // Create a new treaty
    int createTreaty(int kingdom1, int kingdom2, RelationshipStatus status, const std::string& terms, int durationDays = 0) {
        // Create treaty
        treaties.emplace_back(nextTreatyId, kingdom1, kingdom2, status, terms, durationDays);

        // Update relationship
        setRelationship(kingdom1, kingdom2, status, "Treaty created: " + terms);

        return nextTreatyId++;
    }

    // Break a treaty (either side can break it)
    bool breakTreaty(int treatyId, int breakingKingdomId, const std::string& reason) {
        for (auto& treaty : treaties) {
            if (treaty.treatyId == treatyId && treaty.isActive) {
                // Make sure the breaking kingdom is part of the treaty
                if (treaty.kingdomId1 != breakingKingdomId && treaty.kingdomId2 != breakingKingdomId) {
                    return false; // This kingdom is not part of the treaty
                }

                // Mark treaty as inactive
                treaty.isActive = false;

                // Find the other kingdom in the treaty
                int otherKingdomId = (treaty.kingdomId1 == breakingKingdomId) ? treaty.kingdomId2 : treaty.kingdomId1;

                // Update relationship to neutral (or could set to war)
                setRelationship(breakingKingdomId, otherKingdomId, RelationshipStatus::NEUTRAL,
                    "Treaty broken by " + kingdomManager.getKingdomById(breakingKingdomId)->getName() + ": " + reason);

                return true;
            }
        }
        return false; // Treaty not found or already inactive
    }

    // Check if a treaty exists between kingdoms
    bool hasTreaty(int kingdom1, int kingdom2) const {
        for (const auto& treaty : treaties) {
            if (!treaty.isActive) continue;

            if ((treaty.kingdomId1 == kingdom1 && treaty.kingdomId2 == kingdom2) ||
                (treaty.kingdomId1 == kingdom2 && treaty.kingdomId2 == kingdom1)) {
                return !treaty.isExpired();
            }
        }
        return false;
    }

    // Get treaty between kingdoms
    const Treaty* getTreatyBetween(int kingdom1, int kingdom2) const {
        for (const auto& treaty : treaties) {
            if (!treaty.isActive) continue;

            if ((treaty.kingdomId1 == kingdom1 && treaty.kingdomId2 == kingdom2) ||
                (treaty.kingdomId1 == kingdom2 && treaty.kingdomId2 == kingdom1)) {
                if (!treaty.isExpired()) {
                    return &treaty;
                }
            }
        }
        return nullptr;
    }

    // Get treaty by ID
    const Treaty* getTreatyById(int treatyId) const {
        for (const auto& treaty : treaties) {
            if (treaty.treatyId == treatyId) {
                return &treaty;
            }
        }
        return nullptr;
    }

    // Get relationship history between kingdoms
    std::vector<RelationshipEvent> getHistory(int kingdom1, int kingdom2) const {
        std::vector<RelationshipEvent> result;
        for (const auto& event : history) {
            if ((event.kingdomId1 == kingdom1 && event.kingdomId2 == kingdom2) ||
                (event.kingdomId1 == kingdom2 && event.kingdomId2 == kingdom1)) {
                result.push_back(event);
            }
        }
        return result;
    }

    // Get all treaties for a kingdom
    std::vector<const Treaty*> getAllTreatiesForKingdom(int kingdomId) const {
        std::vector<const Treaty*> result;
        for (const auto& treaty : treaties) {
            if ((treaty.kingdomId1 == kingdomId || treaty.kingdomId2 == kingdomId) &&
                treaty.isActive && !treaty.isExpired()) {
                result.push_back(&treaty);
            }
        }
        return result;
    }

    // Process expired treaties
    void processExpiredTreaties() {
        for (auto& treaty : treaties) {
            if (treaty.isActive && treaty.isExpired()) {
                treaty.isActive = false;

                // Update relationship to neutral
                setRelationship(treaty.kingdomId1, treaty.kingdomId2, RelationshipStatus::NEUTRAL,
                    "Treaty expired naturally");
            }
        }
    }

    // Print diplomatic status for a kingdom
    void printDiplomaticStatusForKingdom(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        std::cout << "\n===== DIPLOMATIC STATUS FOR " << kingdom->getName() << " =====" << std::endl;

        bool hasRelations = false;
        for (size_t i = 0; i < kingdomManager.getKingdomCount(); i++) {
            Kingdom* otherKingdom = kingdomManager.getKingdomByIndex(i);
            if (!otherKingdom || otherKingdom->getId() == kingdomId) continue;

            RelationshipStatus status = getRelationship(kingdomId, otherKingdom->getId());
            const Treaty* treaty = getTreatyBetween(kingdomId, otherKingdom->getId());

            std::cout << "- " << otherKingdom->getName() << ": ";

            switch (status) {
            case RelationshipStatus::WAR:
                std::cout << "AT WAR";
                break;
            case RelationshipStatus::NEUTRAL:
                std::cout << "NEUTRAL";
                break;
            case RelationshipStatus::ALLIANCE:
                std::cout << "ALLIANCE";
                break;
            case RelationshipStatus::PEACE_TREATY:
                std::cout << "PEACE TREATY";
                break;
            }

            if (treaty) {
                std::cout << " (Treaty: " << treaty->terms << ")";

                if (treaty->expirationTime > 0) {
                    std::time_t now = std::time(nullptr);
                    if (treaty->expirationTime > now) {
                        int daysLeft = static_cast<int>((treaty->expirationTime - now) / (24 * 60 * 60));
                        std::cout << " - " << daysLeft << " days remaining";
                    }
                }
            }

            std::cout << std::endl;
            hasRelations = true;
        }

        if (!hasRelations) {
            std::cout << "No diplomatic relations established." << std::endl;
        }

        std::cout << "===================================" << std::endl;
    }
};

#endif // DIPLOMACY_H
