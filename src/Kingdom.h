#pragma once

#ifndef KINGDOM_H
#define KINGDOM_H

#include <string>
#include <vector>
#include "BuildingManager.h"
#include "Population.h"
#include "TradeManager.h"
#include "ResourceManager.h"
#include "Leadership.h"
#include "Bank.h"
#include "KharchaManager.h"
#include "MilitaryManagement.h"
#include <limits>
//#include "Common.h"
//extern int globalGOAT;



// helper to read a single integer with validation
static int readInt(const std::string& prompt) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) {
            // consume the rest of the line (newline)
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        // bad input: clear and retry
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input, please enter a number.\n";
    }
}






// Forward declarations
class Diplomacy;
class Communication;

// Represents a player's kingdom
class Kingdom {
private:
    std::string name;
    std::string playerName;
    int kingdomId;
    bool isActive; // Is this kingdom currently being played?

    // Core game components
    BuildingManager buildingManager;
    LowerClassPopulation lowerStats;
    MiddleClassPopulation middleStats;
    UpperClassPopulation upperStats;
    Population* pops[3];
    TradeManager tradeManager;
    Leadership leadership;
    Bank bank;
    KharchaManager kharcha;
    MilitaryManagement military;

    // Map coordinates
    int mapX;
    int mapY;

    // Resource storage
    int currentFoodStorage;

    // Kingdom stats
    float overallStrength;
    float defensePower;
    float attackPower;


    //missing partss:::
    enum class RiotClass { Lower = 0, Middle = 1, Upper = 2, None = 3 };
    RiotClass currentRiot = RiotClass::None;
    bool warActive = false;
    bool famineActive = false;
    float famineTimer = 0.f;
    float famineDuration = 30.f;

    void checkEvents(float dt, sf::RenderWindow& window);
    void handleFoodStorage(float production, float consumption);
    void triggerRiot(RiotClass rc , sf::RenderWindow& window);
    void resolveRiot(char key);
    void triggerWar(sf::RenderWindow& window);
    void triggerFamine();

    enum class BankOp { None, Deposit, Withdraw };
    BankOp pendingBankOp = BankOp::None;
    bool bankMenuActive = false;

    void executeBankOp();
    void runBankMenu();

    enum class PendingBuild {
        None,
        Lower,
        Middle,
        Upper,
        Woodcutter,
        StoneMansion,
        Guardhouse,
        Tavern,
        Storehouse,
        Keep
    };
    PendingBuild pending = PendingBuild::None;
    bool buildMenuActive = false;

    void handleClick(int mx, int my, sf::RenderWindow& window);
    bool pointInConvex(const sf::ConvexShape&, const sf::Vector2f&);
    void removeBuilding(int mx, int my, sf::RenderWindow& window);
    void showBuildMenu();


    bool recruitMenuActive = false;
    void runRecruitMenu();




    sf::Clock fireEventClock;
    float fireEventInterval = 30.0f;




    //ui shit
    void printResourceStats() const;
    void printSimplePopStats() const;
    void printPopulationStats() const;
    void printMilitaryStats() const;
    void printLeadershipStats() const;
    void printBankStats() const;
    void printFoodStats() const;
    void printLeaderResources() const;

    //file handling functions
        // Save kingdom data to file
    bool saveKingdom(const std::string& filename);

    // Load kingdom data from file
    bool loadKingdom(const std::string& filename);




    // SFML‐specific members for grid & HUD
// in Kingdom.h, under private:
    sf::RectangleShape ground, hud;
    static const int GRID_R = 12, GRID_C = 12;
    float tileW = 64.f, tileH = 32.f;
    sf::ConvexShape cells[GRID_R][GRID_C];
    sf::Vector2f    cellPos[GRID_R][GRID_C];
    sf::Font        hudFont;
    sf::Text        hudText[10];
    int selectedR, selectedC;

    static constexpr float TILE_W = 64.f, TILE_H = 32.f;





    // Alert box properties
    bool alertBoxActive = false;
    bool warAlertActive = false;
    bool riotAlertActive = false;
    RiotClass riotAlertClass = RiotClass::None;
    sf::RectangleShape alertBox;
    sf::Text alertTitle;
    sf::Text alertMessage;
    sf::RectangleShape closeButton;
    sf::Text closeButtonText;
    std::vector<sf::RectangleShape> riotOptionButtons;
    std::vector<sf::Text> riotOptionTexts;
    bool backgroundInitialized = false;




    sf::RectangleShape resourceBar;
    sf::Font resourceFont;
    std::vector<sf::Text> resourceTexts;
    std::vector<sf::Sprite> resourceIcons;
    sf::Texture woodTexture, stoneTexture, populationTexture, militaryTexture;
    bool resourceBarInitialized = false;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture foodTexture;
    sf::Text foodProductionText;        //added some textures 
    sf::Text foodConsumptionText;




    static constexpr float RIOT_THRESHOLD = 0.30f; // <30% sat → riot







public:



    //for alert boxes 
    void showRiotAlert(RiotClass riotClass, sf::RenderWindow& window);
    void showWarAlert(sf::RenderWindow& window);
    void drawAlertBox(sf::RenderWindow& window);
    bool handleAlertClick(int mx, int my, sf::RenderWindow& window);

    void initResourceBar(sf::RenderWindow& window);           //draws and loads  shi
    void updateResourceBar(sf::RenderWindow& window);   // respective getters for displaying info
    void initBackground(sf::RenderWindow& window);


    Kingdom(int id, const std::string& name, const std::string& player);
    //int globalGOAT = 0;
    // Getters
    int getId() const { return kingdomId; }
    std::string getName() const { return name; }
    std::string getPlayerName() const { return playerName; }
    bool getIsActive() const { return isActive; }

    // Map position
    void setMapPosition(int x, int y);
    int getMapX() const;
    int getMapY() const;

    // Activate/deactivate kingdom
    void setActive(bool active);

    // Access core components
    BuildingManager& getBuildingManager();
    Population* getPopulation(int index);
    TradeManager& getTradeManager();
    Leadership& getLeadership();
    Bank& getBank();
    KharchaManager& getKharcha();
    MilitaryManagement& getMilitary();

    // Food management
    int getFoodStorage() const;
    void setFoodStorage(int amount);
    void addFood(int amount);
    void removeFood(int amount);

    // Military strength calculations
    void calculateStrength();
    float getOverallStrength() const;
    float getDefensePower() const;
    float getAttackPower() const;

    // Setup initial buildings (simplified version of GameEngine::setupInitialBuildings)
    void setupInitialBuildings();

    // Print kingdom summary
    void printSummary() const;





    /// Initialize any SFML resources (fonts, build the grid, HUD text…)
    void initSFML(const sf::Font& font, sf::RenderWindow& window);

    /// Handle a single SFML event in “castle view”
    void processSFML(const sf::Event& e, sf::RenderWindow& window);

    /// Per‐frame logic (fires, production, trade, resource distribution…)
    void updateSFML(float dt, sf::RenderWindow& window);

    /// Draw everything (grid, buildings, HUD, menus) into the window  , i added ground and hug 
    void renderSFML(sf::RenderWindow& window);






};












#endif // KINGDOM_H