#pragma once
#ifndef TRADING_SYSTEM_H
#define TRADING_SYSTEM_H

#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <iostream>
#include "KingdomManager.h"
#include "MapSystem.h"

// Resource types for trading
enum class TradeResourceType {
    WOOD,
    STONE,
    FOOD
};

// Trade offer between kingdoms
struct TradeOffer {
    int offerId;
    int offeringKingdomId;
    int targetKingdomId; // 0 means open to any kingdom
    TradeResourceType offerType;
    int offerAmount;
    TradeResourceType requestType;
    int requestAmount;
    std::time_t creationTime;
    bool isActive;

    TradeOffer(int id, int offering, int target,
        TradeResourceType ofType, int ofAmount,
        TradeResourceType reqType, int reqAmount)
        : offerId(id), offeringKingdomId(offering), targetKingdomId(target),
        offerType(ofType), offerAmount(ofAmount),
        requestType(reqType), requestAmount(reqAmount),
        isActive(true) {
        creationTime = std::time(nullptr);
    }

    std::string getOfferTypeString() const {
        switch (offerType) {
        case TradeResourceType::WOOD: return "Wood";
        case TradeResourceType::STONE: return "Stone";
        case TradeResourceType::FOOD: return "Food";
        default: return "Unknown";
        }
    }

    std::string getRequestTypeString() const {
        switch (requestType) {
        case TradeResourceType::WOOD: return "Wood";
        case TradeResourceType::STONE: return "Stone";
        case TradeResourceType::FOOD: return "Food";
        default: return "Unknown";
        }
    }
};

// Completed trade record
struct TradeRecord {
    int tradeId;
    int offerId; // Original offer ID
    int sellerKingdomId;
    int buyerKingdomId;
    TradeResourceType soldType;
    int soldAmount;
    TradeResourceType boughtType;
    int boughtAmount;
    std::time_t completionTime;
    bool wasIntercepted; // For smuggling checks

    TradeRecord(int id, int offer, int seller, int buyer,
        TradeResourceType soldT, int soldA,
        TradeResourceType boughtT, int boughtA)
        : tradeId(id), offerId(offer), sellerKingdomId(seller), buyerKingdomId(buyer),
        soldType(soldT), soldAmount(soldA),
        boughtType(boughtT), boughtAmount(boughtA),
        wasIntercepted(false) {
        completionTime = std::time(nullptr);
    }
};

// Trading system for resource exchange between kingdoms
class TradingSystem {
private:
    std::vector<TradeOffer> offers;
    std::vector<TradeRecord> tradeHistory;
    int nextOfferId;
    int nextTradeId;
    KingdomManager& kingdomManager;
    MapSystem& mapSystem;

    // Random number generator for smuggling checks
    std::mt19937 rng;

    // Helper to check and process if a kingdom has the required resources
    bool hasResources(Kingdom* kingdom, TradeResourceType type, int amount) const {
        if (!kingdom) return false;

        switch (type) {
        case TradeResourceType::WOOD:
            return kingdom->getLeadership().getResManager().getWood() >= amount;
        case TradeResourceType::STONE:
            return kingdom->getLeadership().getResManager().getStone() >= amount;
        case TradeResourceType::FOOD:
            return kingdom->getFoodStorage() >= amount;
        default:
            return false;
        }
    }

    // Helper to transfer resources between kingdoms
    void transferResources(Kingdom* from, Kingdom* to,
        TradeResourceType type, int amount) {
        if (!from || !to) return;

        switch (type) {
        case TradeResourceType::WOOD:
            from->getLeadership().getResManager().removeWood(amount);
            to->getLeadership().getResManager().addWood(amount);
            break;
        case TradeResourceType::STONE:
            from->getLeadership().getResManager().removeStone(amount);
            to->getLeadership().getResManager().addStone(amount);
            break;
        case TradeResourceType::FOOD:
            from->removeFood(amount);
            to->addFood(amount);
            break;
        }
    }

public:
    TradingSystem(KingdomManager& km, MapSystem& map)
        : kingdomManager(km), mapSystem(map), nextOfferId(1), nextTradeId(1),
        rng(std::random_device()()) {
    }

    // Create a new trade offer
    int createTradeOffer(int kingdomId, int targetKingdomId,
        TradeResourceType offerType, int offerAmount,
        TradeResourceType requestType, int requestAmount) {
        // Validate kingdoms
        Kingdom* offering = kingdomManager.getKingdomById(kingdomId);
        Kingdom* target = (targetKingdomId > 0) ? kingdomManager.getKingdomById(targetKingdomId) : nullptr;

        if (!offering || (targetKingdomId > 0 && !target)) {
            return -1;
        }

        // Check if offering kingdom has the resources
        if (!hasResources(offering, offerType, offerAmount)) {
            return -1;
        }

        // Create the offer
        offers.emplace_back(nextOfferId, kingdomId, targetKingdomId,
            offerType, offerAmount, requestType, requestAmount);

        return nextOfferId++;
    }

    // Accept a trade offer
    bool acceptTradeOffer(int offerId, int acceptingKingdomId, bool isSmuggling = false) {
        // Find the offer
        TradeOffer* offer = nullptr;
        for (auto& o : offers) {
            if (o.offerId == offerId && o.isActive) {
                offer = &o;
                break;
            }
        }

        if (!offer) return false;

        // Check if this kingdom can accept the offer
        if (offer->targetKingdomId != 0 && offer->targetKingdomId != acceptingKingdomId) {
            return false;
        }

        // Get the kingdoms
        Kingdom* seller = kingdomManager.getKingdomById(offer->offeringKingdomId);
        Kingdom* buyer = kingdomManager.getKingdomById(acceptingKingdomId);

        if (!seller || !buyer) return false;

        // Check if kingdoms have the resources
        if (!hasResources(seller, offer->offerType, offer->offerAmount) ||
            !hasResources(buyer, offer->requestType, offer->requestAmount)) {
            return false;
        }

        // Check smuggling risk
        bool intercepted = false;
        if (isSmuggling) {
            // Check if kingdoms are neighbors
            bool areNeighbors = mapSystem.areNeighbors(seller->getId(), buyer->getId());

            // If not neighbors, higher chance of interception
            float interceptionChance = areNeighbors ? 0.1f : 0.4f;

            // Roll for interception
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            intercepted = (dist(rng) < interceptionChance);

            if (intercepted) {
                std::cout << "\nSMUGGLING INTERCEPTED: Trade between " << seller->getName()
                    << " and " << buyer->getName() << " was caught by authorities!" << std::endl;

                // Penalties could be implemented here
                // For now, just make both kingdoms lose the resources
                hasResources(seller, offer->offerType, offer->offerAmount / 2);
                hasResources(buyer, offer->requestType, offer->requestAmount / 2);

                // Record the failed trade
                TradeRecord record(nextTradeId++, offer->offerId,
                    seller->getId(), buyer->getId(),
                    offer->offerType, 0, // No resources transferred
                    offer->requestType, 0);
                record.wasIntercepted = true;
                tradeHistory.push_back(record);

                // Mark offer as inactive
                offer->isActive = false;

                return false;
            }
        }

        // Execute the trade
        transferResources(seller, buyer, offer->offerType, offer->offerAmount);
        transferResources(buyer, seller, offer->requestType, offer->requestAmount);

        // Record the trade
        TradeRecord record(nextTradeId++, offer->offerId,
            seller->getId(), buyer->getId(),
            offer->offerType, offer->offerAmount,
            offer->requestType, offer->requestAmount);

        record.wasIntercepted = intercepted;
        tradeHistory.push_back(record);

        // Mark offer as inactive
        offer->isActive = false;

        // Announce the trade
        std::cout << "\nTRADE COMPLETED: " << seller->getName()
            << " traded " << offer->offerAmount << " " << offer->getOfferTypeString()
            << " to " << buyer->getName()
            << " for " << offer->requestAmount << " " << offer->getRequestTypeString()
            << (isSmuggling ? " (Black Market Trade)" : "") << std::endl;

        return true;
    }

    // Cancel a trade offer (only by the offering kingdom)
    bool cancelTradeOffer(int offerId, int kingdomId) {
        for (auto& offer : offers) {
            if (offer.offerId == offerId && offer.isActive && offer.offeringKingdomId == kingdomId) {
                offer.isActive = false;
                return true;
            }
        }
        return false;
    }

    // Get active offers for a kingdom (both offering and potential target)
    std::vector<const TradeOffer*> getActiveOffersForKingdom(int kingdomId) const {
        std::vector<const TradeOffer*> result;
        for (const auto& offer : offers) {
            if (!offer.isActive) continue;

            if (offer.offeringKingdomId == kingdomId ||
                offer.targetKingdomId == kingdomId ||
                offer.targetKingdomId == 0) {
                result.push_back(&offer);
            }
        }
        return result;
    }

    // Get trade history for a kingdom
    std::vector<const TradeRecord*> getTradeHistoryForKingdom(int kingdomId) const {
        std::vector<const TradeRecord*> result;
        for (const auto& trade : tradeHistory) {
            if (trade.sellerKingdomId == kingdomId || trade.buyerKingdomId == kingdomId) {
                result.push_back(&trade);
            }
        }
        return result;
    }

    // Display active trade offers for a kingdom
    void displayActiveOffers(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        auto activeOffers = getActiveOffersForKingdom(kingdomId);

        std::cout << "\n===== ACTIVE TRADE OFFERS =====" << std::endl;

        if (activeOffers.empty()) {
            std::cout << "No active trade offers available." << std::endl;
        }
        else {
            std::cout << "Offers you can respond to:" << std::endl;
            bool hasOffers = false;

            for (const auto* offer : activeOffers) {
                // Skip offers from this kingdom
                if (offer->offeringKingdomId == kingdomId) continue;

                // Only show offers for this kingdom or open offers
                if (offer->targetKingdomId != 0 && offer->targetKingdomId != kingdomId) continue;

                Kingdom* offering = kingdomManager.getKingdomById(offer->offeringKingdomId);
                if (!offering) continue;

                std::cout << "ID #" << offer->offerId << ": "
                    << offering->getName() << " offers "
                    << offer->offerAmount << " " << offer->getOfferTypeString()
                    << " for " << offer->requestAmount << " " << offer->getRequestTypeString()
                    << std::endl;

                hasOffers = true;
            }

            if (!hasOffers) {
                std::cout << "No offers available to accept." << std::endl;
            }

            std::cout << "\nYour active offers:" << std::endl;
            hasOffers = false;

            for (const auto* offer : activeOffers) {
                // Only show offers from this kingdom
                if (offer->offeringKingdomId != kingdomId) continue;

                std::string targetName = "Any Kingdom";
                if (offer->targetKingdomId > 0) {
                    Kingdom* target = kingdomManager.getKingdomById(offer->targetKingdomId);
                    if (target) targetName = target->getName();
                }

                std::cout << "ID #" << offer->offerId << ": "
                    << "You offer " << offer->offerAmount << " " << offer->getOfferTypeString()
                    << " for " << offer->requestAmount << " " << offer->getRequestTypeString()
                    << " (To: " << targetName << ")"
                    << std::endl;

                hasOffers = true;
            }

            if (!hasOffers) {
                std::cout << "You have no active offers." << std::endl;
            }
        }

        std::cout << "============================" << std::endl;
    }

    // Display trade history for a kingdom
    void displayTradeHistory(int kingdomId) const {
        Kingdom* kingdom = kingdomManager.getKingdomById(kingdomId);
        if (!kingdom) return;

        auto history = getTradeHistoryForKingdom(kingdomId);

        std::cout << "\n===== TRADE HISTORY FOR " << kingdom->getName() << " =====" << std::endl;

        if (history.empty()) {
            std::cout << "No trade history found." << std::endl;
        }
        else {
            for (const auto* trade : history) {
                Kingdom* other = kingdomManager.getKingdomById(
                    (trade->sellerKingdomId == kingdomId) ? trade->buyerKingdomId : trade->sellerKingdomId
                );

                if (!other) continue;

                if (trade->wasIntercepted) {
                    std::cout << "INTERCEPTED: Attempted trade with " << other->getName() << std::endl;
                    continue;
                }

                if (trade->sellerKingdomId == kingdomId) {
                    std::cout << "SOLD: " << trade->soldAmount << " "
                        << (trade->soldType == TradeResourceType::WOOD ? "Wood" :
                            (trade->soldType == TradeResourceType::STONE ? "Stone" : "Food"))
                        << " to " << other->getName()
                        << " for " << trade->boughtAmount << " "
                        << (trade->boughtType == TradeResourceType::WOOD ? "Wood" :
                            (trade->boughtType == TradeResourceType::STONE ? "Stone" : "Food"))
                        << std::endl;
                }
                else {
                    std::cout << "BOUGHT: " << trade->soldAmount << " "
                        << (trade->soldType == TradeResourceType::WOOD ? "Wood" :
                            (trade->soldType == TradeResourceType::STONE ? "Stone" : "Food"))
                        << " from " << other->getName()
                        << " for " << trade->boughtAmount << " "
                        << (trade->boughtType == TradeResourceType::WOOD ? "Wood" :
                            (trade->boughtType == TradeResourceType::STONE ? "Stone" : "Food"))
                        << std::endl;
                }
            }
        }

        std::cout << "=========================" << std::endl;
    }
};

#endif // TRADING_SYSTEM_H
