#pragma once
#ifndef MULTIPLAYER_MENU_H
#define MULTIPLAYER_MENU_H

#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include "KingdomManager.h"
#include "Diplomacy.h"
#include "Communication.h"
#include "MapSystem.h"
#include "WarSystem.h"
#include "TradingSystem.h"
#include "MapRenderer.h"    // your SFML showMapSFML()
// Forward declaration
class MultiplayerGame;

// Menu system for multiplayer game
class MultiplayerMenu {
private:
    MultiplayerGame& game;

    // Helper to clear console input buffer
    void clearInputBuffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Helper to get integer input with validation
    int getIntInput(const std::string& prompt, int min, int max) {
        int value;
        bool validInput = false;

        do {
            std::cout << prompt;
            std::cin >> value;

            if (std::cin.fail()) {
                std::cout << "Invalid input. Please enter a number.\n";
                clearInputBuffer();
            }
            else if (value < min || value > max) {
                std::cout << "Please enter a value between " << min << " and " << max << ".\n";
                clearInputBuffer();
            }
            else {
                validInput = true;
            }
        } while (!validInput);

        clearInputBuffer();
        return value;
    }

    // Helper to get string input with validation
    std::string getStringInput(const std::string& prompt, bool allowEmpty = false) {
        std::string value;
        bool validInput = false;

        do {
            std::cout << prompt;
            std::getline(std::cin, value);

            if (!allowEmpty && value.empty()) {
                std::cout << "Input cannot be empty. Please try again.\n";
            }
            else {
                validInput = true;
            }
        } while (!validInput);

        return value;
    }

public:
    MultiplayerMenu(MultiplayerGame& g) : game(g) {}

    // Display main menu
    void displayMainMenu();

    // Kingdom management menus
    void kingdomCreationMenu();
    void kingdomSelectionMenu();

    // Diplomacy menus
    void diplomacyMenu();
    void createTreatyMenu();
    void breakTreatyMenu();

    // Communication menus
    void communicationMenu();
    void sendMessageMenu();
    void viewMessagesMenu();

    // Map menus
    void mapMenu();
    void moveKingdomMenu();

    // War menus
    void warMenu();
    void declareWarMenu();
    void battleMenu();
    void surrenderMenu();

    // Trade menus
    void tradeMenu();
    void createTradeOfferMenu();
    void acceptTradeOfferMenu();
    void viewTradeHistoryMenu();
};

// Main game class that integrates all systems
class MultiplayerGame
{
private:
    KingdomManager kingdomManager;
    Diplomacy diplomacy;
    Communication communication;
    MapSystem mapSystem;
    WarSystem warSystem;
    TradingSystem tradingSystem;
    MultiplayerMenu menu;
    bool gameRunning;


    // SFML for castle view:
    sf::RenderWindow window;
    sf::Clock        clock;
    sf::Font         hudFont;
    sf::RectangleShape hud, ground;

public:
    MultiplayerGame()
        : diplomacy(kingdomManager),
        communication(kingdomManager),
        mapSystem(kingdomManager, diplomacy),
        warSystem(kingdomManager, diplomacy),
        tradingSystem(kingdomManager, mapSystem),
        menu(*this),
        gameRunning(true) 
    {



        hudFont.loadFromFile("assets/fonts/arial.ttf");
        //window({ 1024,768 }, "Stronghold Multiplayer")
        //window.setFramerateLimit(60);
        ground.setFillColor(sf::Color(50, 150, 50));
        hud.setFillColor(sf::Color(80, 80, 80));
        hud.setPosition(0, 768 - 100);
    }
    void enterCastleView();
    // Main game loop
    void run() {

        // Initialize game
        initializeGame();

        // Main game loop
        while (gameRunning) {
            menu.displayMainMenu();
        }

        std::cout << "Thank you for playing Stronghold Multiplayer!" << std::endl;
    }

    // Initialize game with some starting kingdoms
    void initializeGame() {
        std::cout << "Welcome to Stronghold Multiplayer!" << std::endl;
        std::cout << "Initializing game..." << std::endl;

        // Update the map
        mapSystem.updateMap();
    }

    // Exit the game
    void exitGame() {
        gameRunning = false;
    }

    // Access to managers and systems
    KingdomManager& getKingdomManager() { return kingdomManager; }
    Diplomacy& getDiplomacy() { return diplomacy; }
    Communication& getCommunication() { return communication; }
    MapSystem& getMapSystem() { return mapSystem; }
    WarSystem& getWarSystem() { return warSystem; }
    TradingSystem& getTradingSystem() { return tradingSystem; }
};

// Implementation of MultiplayerMenu methods
void MultiplayerMenu::displayMainMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();

    std::cout << "\n===== STRONGHOLD MULTIPLAYER =====\n";

    if (activeKingdom) 
    {
        std::cout << "Active Kingdom: " << activeKingdom->getName()
            << " (Ruled by: " << activeKingdom->getPlayerName() << ")\n";
    }
    else {
        std::cout << "No kingdom selected\n";
    }

    std::cout << "\nMAIN MENU:\n";
    std::cout << "1. Kingdom Management\n";

    if (activeKingdom) {
        std::cout << "2. View Map\n";
        std::cout << "3. Diplomacy\n";
        std::cout << "4. Communication\n";
        std::cout << "5. War\n";
        std::cout << "6. Trade\n";
        std::cout << "7. Kingdom Summary\n";
    }

    std::cout << "0. Exit Game\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        game.exitGame();
        break;
    case 1:
        kingdomSelectionMenu();
        break;
    case 2:
        if (activeKingdom) 
           /* mapMenu();*/
           game.getMapSystem().updateMap();            // rebuild 2×4 data
        game.getMapSystem().displayMap();           // console output
        showMapSFML(game.getMapSystem());           // SFML window
        break;
        break;
    case 3:
        if (activeKingdom) diplomacyMenu();
        break;
    case 4:
        if (activeKingdom) communicationMenu();
        break;
    case 5:
        if (activeKingdom) warMenu();
        break;
    case 6:
        if (activeKingdom) tradeMenu();
        break;
    case 7:
        if (activeKingdom) activeKingdom->printSummary();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::kingdomSelectionMenu() {
    std::cout << "\n===== KINGDOM MANAGEMENT =====\n";

    // Display all kingdoms
    game.getKingdomManager().printAllKingdoms();

    std::cout << "\n1. Select Kingdom\n";
    std::cout << "2. Create New Kingdom\n";
    std::cout << "3. Enter Castle View\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1: {
        if (game.getKingdomManager().getKingdomCount() == 0) {
            std::cout << "No kingdoms available. Please create one first.\n";
            return;
        }

        int index = getIntInput("Enter kingdom number: ", 1, game.getKingdomManager().getKingdomCount());

        if (game.getKingdomManager().setActiveKingdomByIndex(index - 1)) {
            std::cout << "Kingdom selected.\n";
            Kingdom* active = game.getKingdomManager().getActiveKingdom();
            if (active) {
                active->printSummary();
            }
        }
        else {
            std::cout << "Failed to select kingdom.\n";
        }
        break;
    }
    case 2:
        kingdomCreationMenu();
        break;
    case 3:
        game.enterCastleView();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::kingdomCreationMenu() {
    std::cout << "\n===== CREATE NEW KINGDOM =====\n";

    std::string name = getStringInput("Enter kingdom name: ");
    std::string player = getStringInput("Enter ruler name: ");

    Kingdom* kingdom = game.getKingdomManager().createKingdom(name, player);
    if (kingdom) {
        std::cout << "Kingdom created successfully!\n";
        game.getKingdomManager().setActiveKingdomById(kingdom->getId());
        game.getMapSystem().updateMap();
        kingdom->printSummary();
    }
    else {
        std::cout << "Failed to create kingdom.\n";
    }
}

void MultiplayerMenu::diplomacyMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== DIPLOMACY =====\n";

    // Display diplomatic status
    game.getDiplomacy().printDiplomaticStatusForKingdom(activeKingdom->getId());

    std::cout << "\n1. Create Treaty/Alliance\n";
    std::cout << "2. Break Treaty/Alliance\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1:
        createTreatyMenu();
        break;
    case 2:
        breakTreatyMenu();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::createTreatyMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== CREATE TREATY =====\n";

    // List other kingdoms
    std::cout << "Available kingdoms:\n";
    bool foundKingdom = false;

    for (size_t i = 0; i < game.getKingdomManager().getKingdomCount(); i++) {
        Kingdom* other = game.getKingdomManager().getKingdomByIndex(i);
        if (!other || other->getId() == activeKingdom->getId()) continue;

        foundKingdom = true;
        std::cout << other->getId() << ". " << other->getName() << "\n";
    }

    if (!foundKingdom) {
        std::cout << "No other kingdoms available for treaties.\n";
        return;
    }

    int targetId = getIntInput("Enter kingdom ID for treaty: ", 1, 999);

    // Check if kingdom exists
    Kingdom* targetKingdom = game.getKingdomManager().getKingdomById(targetId);
    if (!targetKingdom || targetKingdom->getId() == activeKingdom->getId()) {
        std::cout << "Invalid kingdom selected.\n";
        return;
    }

    // Check if treaty already exists
    if (game.getDiplomacy().hasTreaty(activeKingdom->getId(), targetId)) {
        std::cout << "A treaty already exists with this kingdom.\n";
        return;
    }

    // Select treaty type
    std::cout << "\nTreaty types:\n";
    std::cout << "1. Alliance\n";
    std::cout << "2. Peace Treaty\n";
    std::cout << "0. Cancel\n";

    int treatyType = getIntInput("Select treaty type: ", 0, 2);
    if (treatyType == 0) return;

    RelationshipStatus status = (treatyType == 1) ?
        RelationshipStatus::ALLIANCE :
        RelationshipStatus::PEACE_TREATY;

    std::string terms = getStringInput("Enter treaty terms: ");
    int duration = getIntInput("Enter duration in days (0 for indefinite): ", 0, 9999);

    int treatyId = game.getDiplomacy().createTreaty(
        activeKingdom->getId(), targetId, status, terms, duration
    );

    if (treatyId > 0) {
        std::cout << "Treaty created successfully!\n";
    }
    else {
        std::cout << "Failed to create treaty.\n";
    }
}

void MultiplayerMenu::breakTreatyMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== BREAK TREATY =====\n";

    // Get all treaties for this kingdom
    auto treaties = game.getDiplomacy().getAllTreatiesForKingdom(activeKingdom->getId());

    if (treaties.empty()) {
        std::cout << "You have no active treaties.\n";
        return;
    }

    std::cout << "Your active treaties:\n";
    for (const auto* treaty : treaties) {
        int otherId = (treaty->kingdomId1 == activeKingdom->getId()) ?
            treaty->kingdomId2 : treaty->kingdomId1;

        Kingdom* other = game.getKingdomManager().getKingdomById(otherId);
        if (!other) continue;

        std::cout << "ID #" << treaty->treatyId << ": "
            << treaty->getStatusString() << " with "
            << other->getName() << " - " << treaty->terms << "\n";
    }

    int treatyId = getIntInput("Enter treaty ID to break (0 to cancel): ", 0, 9999);
    if (treatyId == 0) return;

    // Confirm with reason
    std::string reason = getStringInput("Enter reason for breaking treaty: ");

    if (game.getDiplomacy().breakTreaty(treatyId, activeKingdom->getId(), reason)) {
        std::cout << "Treaty broken successfully.\n";
    }
    else {
        std::cout << "Failed to break treaty. Check if the treaty ID is correct.\n";
    }
}

void MultiplayerMenu::communicationMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    // Check for unread messages
    auto unread = game.getCommunication().getUnreadMessagesForKingdom(activeKingdom->getId());

    std::cout << "\n===== COMMUNICATION =====\n";
    if (!unread.empty()) {
        std::cout << "You have " << unread.size() << " unread message(s)!\n";
    }

    std::cout << "\n1. Send Message\n";
    std::cout << "2. View Inbox\n";
    std::cout << "3. View Outbox\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1:
        sendMessageMenu();
        break;
    case 2:
        game.getCommunication().displayInbox(activeKingdom->getId());
        viewMessagesMenu();
        break;
    case 3:
        game.getCommunication().displayOutbox(activeKingdom->getId());
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::sendMessageMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== SEND MESSAGE =====\n";

    // List other kingdoms
    std::cout << "Available recipients:\n";
    bool foundKingdom = false;

    for (size_t i = 0; i < game.getKingdomManager().getKingdomCount(); i++) {
        Kingdom* other = game.getKingdomManager().getKingdomByIndex(i);
        if (!other || other->getId() == activeKingdom->getId()) continue;

        foundKingdom = true;
        std::cout << other->getId() << ". " << other->getName() << "\n";
    }

    if (!foundKingdom) {
        std::cout << "No other kingdoms available to message.\n";
        return;
    }

    int recipientId = getIntInput("Enter recipient kingdom ID (0 to cancel): ", 0, 999);
    if (recipientId == 0) return;

    // Check if kingdom exists
    Kingdom* recipient = game.getKingdomManager().getKingdomById(recipientId);
    if (!recipient || recipient->getId() == activeKingdom->getId()) {
        std::cout << "Invalid recipient selected.\n";
        return;
    }

    std::string content = getStringInput("Enter your message:\n");

    if (game.getCommunication().sendMessage(activeKingdom->getId(), recipientId, content)) {
        std::cout << "Message sent successfully!\n";
    }
    else {
        std::cout << "Failed to send message.\n";
    }
}

void MultiplayerMenu::viewMessagesMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    int messageId = getIntInput("Enter message ID to view (0 to cancel): ", 0, 9999);
    if (messageId == 0) return;

    // Display the message
    game.getCommunication().displayMessage(messageId);

    // Mark as read
    game.getCommunication().markMessageAsRead(messageId);
}

void MultiplayerMenu::mapMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== MAP =====\n";

    // Display the map
    game.getMapSystem().displayMap();

    // Display neighbors
    game.getMapSystem().displayNeighbors(activeKingdom->getId());

    std::cout << "\n1. Move Kingdom\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1:
        moveKingdomMenu();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::moveKingdomMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== MOVE KINGDOM =====\n";
    std::cout << "Current position: (" << activeKingdom->getMapX() << ", " << activeKingdom->getMapY() << ")\n";

    int newX = getIntInput("Enter new X coordinate (0-49): ", 0, 49);
    int newY = getIntInput("Enter new Y coordinate (0-19): ", 0, 19);

    if (game.getMapSystem().moveKingdom(activeKingdom->getId(), newX, newY)) {
        std::cout << "Kingdom moved successfully!\n";
    }
    else {
        std::cout << "Failed to move kingdom. The destination may be occupied.\n";
    }
}

void MultiplayerMenu::warMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== WAR =====\n";

    // Display war status
    game.getWarSystem().displayWarStatusForKingdom(activeKingdom->getId());

    std::cout << "\n1. Declare War\n";
    std::cout << "2. Conduct Battle\n";
    std::cout << "3. Surrender\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1:
        declareWarMenu();
        break;
    case 2:
        battleMenu();
        break;
    case 3:
        surrenderMenu();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::declareWarMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== DECLARE WAR =====\n";

    // List other kingdoms
    std::cout << "Available kingdoms:\n";
    bool foundKingdom = false;

    for (size_t i = 0; i < game.getKingdomManager().getKingdomCount(); i++) {
        Kingdom* other = game.getKingdomManager().getKingdomByIndex(i);
        if (!other || other->getId() == activeKingdom->getId()) continue;

        // Skip kingdoms we're already at war with
        if (game.getWarSystem().areKingdomsAtWar(activeKingdom->getId(), other->getId())) continue;

        foundKingdom = true;
        std::cout << other->getId() << ". " << other->getName();

        // Show relationship
        RelationshipStatus rel = game.getDiplomacy().getRelationship(activeKingdom->getId(), other->getId());
        switch (rel) {
        case RelationshipStatus::ALLIANCE:
            std::cout << " (ALLY)";
            break;
        case RelationshipStatus::PEACE_TREATY:
            std::cout << " (PEACE TREATY)";
            break;
        default:
            break;
        }

        std::cout << "\n";
    }

    if (!foundKingdom) {
        std::cout << "No kingdoms available to declare war on.\n";
        return;
    }

    int targetId = getIntInput("Enter kingdom ID to attack (0 to cancel): ", 0, 999);
    if (targetId == 0) return;

    // Check if kingdom exists
    Kingdom* targetKingdom = game.getKingdomManager().getKingdomById(targetId);
    if (!targetKingdom || targetKingdom->getId() == activeKingdom->getId()) {
        std::cout << "Invalid kingdom selected.\n";
        return;
    }

    // Check if already at war
    if (game.getWarSystem().areKingdomsAtWar(activeKingdom->getId(), targetId)) {
        std::cout << "You are already at war with this kingdom.\n";
        return;
    }

    // Confirm declaration
    std::cout << "Are you sure you want to declare war on " << targetKingdom->getName() << "? (y/n): ";
    char confirm;
    std::cin >> confirm;
    clearInputBuffer();

    if (std::tolower(confirm) != 'y') {
        std::cout << "War declaration cancelled.\n";
        return;
    }

    int warId = game.getWarSystem().declareWar(activeKingdom->getId(), targetId);

    if (warId > 0) {
        std::cout << "War declared successfully!\n";
    }
    else {
        std::cout << "Failed to declare war.\n";
    }
}

void MultiplayerMenu::battleMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== CONDUCT BATTLE =====\n";

    // Get all wars
    auto wars = game.getWarSystem().getAllWarsForKingdom(activeKingdom->getId());

    if (wars.empty()) {
        std::cout << "You are not currently at war with any kingdom.\n";
        return;
    }

    std::cout << "Your active wars:\n";
    for (const auto* war : wars) {
        int enemyId = (war->kingdom1Id == activeKingdom->getId()) ? war->kingdom2Id : war->kingdom1Id;
        Kingdom* enemy = game.getKingdomManager().getKingdomById(enemyId);

        if (!enemy) continue;

        std::cout << "War #" << war->warId << ": vs " << enemy->getName() << "\n";
    }

    int warId = getIntInput("Enter war ID for battle (0 to cancel): ", 0, 9999);
    if (warId == 0) return;

    // Validate war exists
    const WarStatus* war = game.getWarSystem().getWarById(warId);
    if (!war || !war->isActive) {
        std::cout << "Invalid war selected.\n";
        return;
    }

    // Determine enemy
    int enemyId = (war->kingdom1Id == activeKingdom->getId()) ? war->kingdom2Id : war->kingdom1Id;
    Kingdom* enemy = game.getKingdomManager().getKingdomById(enemyId);

    if (!enemy) {
        std::cout << "Enemy kingdom not found.\n";
        return;
    }

    // Confirm battle
    std::cout << "Are you sure you want to attack " << enemy->getName() << "? (y/n): ";
    char confirm;
    std::cin >> confirm;
    clearInputBuffer();

    if (std::tolower(confirm) != 'y') {
        std::cout << "Battle cancelled.\n";
        return;
    }

    // Conduct battle
    BattleResult result = game.getWarSystem().simulateBattle(activeKingdom->getId(), enemyId);

    std::cout << "\nBATTLE RESULT:\n";
    std::cout << result.description << "\n";
}

void MultiplayerMenu::surrenderMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== SURRENDER =====\n";

    // Get all wars
    auto wars = game.getWarSystem().getAllWarsForKingdom(activeKingdom->getId());

    if (wars.empty()) {
        std::cout << "You are not currently at war with any kingdom.\n";
        return;
    }

    std::cout << "Your active wars:\n";
    for (const auto* war : wars) {
        int enemyId = (war->kingdom1Id == activeKingdom->getId()) ? war->kingdom2Id : war->kingdom1Id;
        Kingdom* enemy = game.getKingdomManager().getKingdomById(enemyId);

        if (!enemy) continue;

        std::cout << "War #" << war->warId << ": vs " << enemy->getName() << "\n";
    }

    int warId = getIntInput("Enter war ID to surrender (0 to cancel): ", 0, 9999);
    if (warId == 0) return;

    // Validate war exists
    const WarStatus* war = game.getWarSystem().getWarById(warId);
    if (!war || !war->isActive) {
        std::cout << "Invalid war selected.\n";
        return;
    }

    // Determine enemy
    int enemyId = (war->kingdom1Id == activeKingdom->getId()) ? war->kingdom2Id : war->kingdom1Id;
    Kingdom* enemy = game.getKingdomManager().getKingdomById(enemyId);

    if (!enemy) {
        std::cout << "Enemy kingdom not found.\n";
        return;
    }

    // Confirm surrender
    std::cout << "Are you sure you want to surrender to " << enemy->getName() << "? (y/n): ";
    char confirm;
    std::cin >> confirm;
    clearInputBuffer();

    if (std::tolower(confirm) != 'y') {
        std::cout << "Surrender cancelled.\n";
        return;
    }

    // Execute surrender
    bool isAttacker = (war->kingdom1Id == activeKingdom->getId());
    std::string reason = getStringInput("Enter reason for surrender: ");

    if (game.getWarSystem().endWar(warId, isAttacker, reason)) {
        std::cout << "You have surrendered to " << enemy->getName() << ".\n";
    }
    else {
        std::cout << "Failed to process surrender.\n";
    }
}

void MultiplayerMenu::tradeMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== TRADE =====\n";

    // Display active trade offers
    game.getTradingSystem().displayActiveOffers(activeKingdom->getId());

    std::cout << "\n1. Create Trade Offer\n";
    std::cout << "2. Accept Trade Offer\n";
    std::cout << "3. View Trade History\n";
    std::cout << "0. Back\n";
    std::cout << "Enter choice: ";

    int choice;
    std::cin >> choice;
    clearInputBuffer();

    switch (choice) {
    case 0:
        return;
    case 1:
        createTradeOfferMenu();
        break;
    case 2:
        acceptTradeOfferMenu();
        break;
    case 3:
        viewTradeHistoryMenu();
        break;
    default:
        std::cout << "Invalid choice. Please try again.\n";
    }
}

void MultiplayerMenu::createTradeOfferMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== CREATE TRADE OFFER =====\n";

    // Display kingdom resources
    std::cout << "Your Resources:\n";
    std::cout << "Wood: " << activeKingdom->getLeadership().getResManager().getWood() << "\n";
    std::cout << "Stone: " << activeKingdom->getLeadership().getResManager().getStone() << "\n";
    std::cout << "Food: " << activeKingdom->getFoodStorage() << "\n";

    // Select target kingdom
    std::cout << "\nTarget Kingdom:\n";
    std::cout << "0. Any Kingdom\n";

    for (size_t i = 0; i < game.getKingdomManager().getKingdomCount(); i++) //prints the kingdoms
    {  
        Kingdom* other = game.getKingdomManager().getKingdomByIndex(i);
        if (!other || other->getId() == activeKingdom->getId()) continue;

        std::cout << other->getId() << ". " << other->getName() << "\n";
    }

    int targetId = getIntInput("Enter target kingdom ID (0 for any): ", 0, 999);

    if (targetId != 0) {
        // Validate target kingdom
        Kingdom* target = game.getKingdomManager().getKingdomById(targetId);
        if (!target || target->getId() == activeKingdom->getId()) {
            std::cout << "Invalid target kingdom.\n";
            return;
        }
    }

    // Select resource to offer
    std::cout << "\nResource to Offer:\n";
    std::cout << "1. Wood\n";
    std::cout << "2. Stone\n";
    std::cout << "3. Food\n";

    int offerType = getIntInput("Select resource to offer: ", 1, 3);
    TradeResourceType offerResType;

    switch (offerType) {
    case 1: offerResType = TradeResourceType::WOOD; break;
    case 2: offerResType = TradeResourceType::STONE; break;
    case 3: offerResType = TradeResourceType::FOOD; break;
    default: return;
    }

    // Enter amount to offer
    int maxOffer = 0;
    switch (offerResType) {
    case TradeResourceType::WOOD:
        maxOffer = activeKingdom->getLeadership().getResManager().getWood();
        break;
    case TradeResourceType::STONE:
        maxOffer = activeKingdom->getLeadership().getResManager().getStone();
        break;
    case TradeResourceType::FOOD:
        maxOffer = activeKingdom->getFoodStorage();
        break;
    }

    if (maxOffer <= 0) {
        std::cout << "You don't have any of this resource to offer.\n";
        return;
    }

    int offerAmount = getIntInput("Enter amount to offer (1-" + std::to_string(maxOffer) + "): ", 1, maxOffer);

    // Select resource to request
    std::cout << "\nResource to Request:\n";
    std::cout << "1. Wood\n";
    std::cout << "2. Stone\n";
    std::cout << "3. Food\n";

    int requestType = getIntInput("Select resource to request: ", 1, 3);
    TradeResourceType requestResType;

    switch (requestType) {
    case 1: requestResType = TradeResourceType::WOOD; break;
    case 2: requestResType = TradeResourceType::STONE; break;
    case 3: requestResType = TradeResourceType::FOOD; break;
    default: return;
    }

    // Enter amount to request
    int requestAmount = getIntInput("Enter amount to request: ", 1, 9999);

    // Create the offer
    int offerId = game.getTradingSystem().createTradeOffer(
        activeKingdom->getId(), targetId,
        offerResType, offerAmount,
        requestResType, requestAmount
    );

    if (offerId > 0) {
        std::cout << "Trade offer created successfully!\n";
    }
    else {
        std::cout << "Failed to create trade offer.\n";
    }
}

void MultiplayerMenu::acceptTradeOfferMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== ACCEPT TRADE OFFER =====\n";

    // Display only offers this kingdom can accept
    auto allOffers = game.getTradingSystem().getActiveOffersForKingdom(activeKingdom->getId());
    bool hasOffers = false;

    std::cout << "Available Offers:\n";
    for (const auto* offer : allOffers) {
        // Skip offers from this kingdom
        if (offer->offeringKingdomId == activeKingdom->getId()) continue;

        // Only show offers for this kingdom or open offers
        if (offer->targetKingdomId != 0 && offer->targetKingdomId != activeKingdom->getId()) continue;

        Kingdom* offering = game.getKingdomManager().getKingdomById(offer->offeringKingdomId);
        if (!offering) continue;

        std::cout << "ID #" << offer->offerId << ": "
            << offering->getName() << " offers "
            << offer->offerAmount << " " << offer->getOfferTypeString()
            << " for " << offer->requestAmount << " " << offer->getRequestTypeString()
            << std::endl;

        hasOffers = true;
    }

    if (!hasOffers) {
        std::cout << "No offers available to accept.\n";
        return;
    }

    int offerId = getIntInput("Enter offer ID to accept (0 to cancel): ", 0, 9999);
    if (offerId == 0) return;

    // Check if offer exists
    bool offerFound = false;
    for (const auto* offer : allOffers) {
        if (offer->offerId == offerId &&
            offer->offeringKingdomId != activeKingdom->getId() &&
            (offer->targetKingdomId == 0 || offer->targetKingdomId == activeKingdom->getId())) {
            offerFound = true;
            break;
        }
    }

    if (!offerFound) {
        std::cout << "Invalid offer ID.\n";
        return;
    }

    // Ask about smuggling
    std::cout << "Do you want to conduct this as a black market trade? (y/n): ";
    char smuggle;
    std::cin >> smuggle;
    clearInputBuffer();

    bool isSmuggling = (std::tolower(smuggle) == 'y');

    if (isSmuggling) {
        std::cout << "WARNING: Black market trades risk interception and penalties.\n";
        std::cout << "Proceed with smuggling? (y/n): ";
        char confirm;
        std::cin >> confirm;
        clearInputBuffer();

        if (std::tolower(confirm) != 'y') {
            isSmuggling = false;
        }
    }

    // Accept the offer
    if (game.getTradingSystem().acceptTradeOffer(offerId, activeKingdom->getId(), isSmuggling)) {
        std::cout << "Trade completed successfully!\n";
    }
    else {
        if (isSmuggling) {
            std::cout << "Trade failed! Your smuggling attempt may have been intercepted.\n";
        }
        else {
            std::cout << "Trade failed. You may not have enough resources.\n";
        }
    }
}

void MultiplayerMenu::viewTradeHistoryMenu() {
    Kingdom* activeKingdom = game.getKingdomManager().getActiveKingdom();
    if (!activeKingdom) return;

    std::cout << "\n===== TRADE HISTORY =====\n";

    // Display trade history
    game.getTradingSystem().displayTradeHistory(activeKingdom->getId());
}






void MultiplayerGame::enterCastleView() {


    Kingdom* K = kingdomManager.getActiveKingdom();
    if (!K) { std::cout << "No kingdom selected.\n"; return; }








    cout << "\nPress" << endl;
    cout << "[R] - print resource rates" << endl;
    cout << "[P] - print population stats" << endl;
    cout << "[C] - Class detailed stats (pop, sat, res)\n";
    cout << "[M] - print millitary stats" << endl;
    cout << "[B] - open the build menu/ Add building " << endl;
    cout << "[D] - Display removal instructions\n";
    cout << "[J] - Trigger fire event (test for all the buildings)\n";
    cout << "[K] - Trigger fire event (test for only the dwellings)\n";
    cout << "[L] - Trigger fire event (test for all the other buildings , apart from dwellings)\n";
    cout << "[E] - Extinguish all fires\n";
    cout << "[O] - Hold election (random leader)\n";
    cout << "[U] - Trigger coup (random other leader)\n";
    cout << "[V] - View current leader's status \n";
    cout << "[A] - Deposit resources into the bank\n";
    cout << "[W] - Withdraw resources from the bank\n";
    cout << "[N] - Print bank balances for each class\n";
    cout << "[Y] - Print bank menu ahh\n";
    cout << "[T] – Print military summary(counts, strength ± morale, food / hr)\n";
    cout << "[I] - OPEN RECRUIT MENU AHHH\n";
    cout << "[G] – Show food production, consumption& storage\n";
    cout << "[Q] – Show leadership (wood & stone) and shared food storage\n";
    cout << "[X] - ENABLE / DISABLE TRADE LOGS\n";
    cout << "[S] - Save Kingdom\n";
    cout << "[W] - Load Kingdom \n";





    // 2) Create & configure the SFML window
    sf::RenderWindow window(
        sf::VideoMode(1024, 768),
        "Castle View — " + K->getName()
    );
    window.setFramerateLimit(60);

    




    K->initSFML(hudFont, window);

    while (window.isOpen()) {

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) { window.close(); return; }
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                return;  // back to console menu

            K->processSFML(e, window);
        }
        float dt = clock.restart().asSeconds();
        K->updateSFML(dt,window);

        window.clear();
        K->renderSFML(window);
        window.display();
    }
}



#endif // MULTIPLAYER_MENU_H
