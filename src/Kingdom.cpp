#include <fstream>
#include "Kingdom.h"


// Implementation of Kingdom methods
Kingdom::Kingdom(int id, const std::string& name, const std::string& player)
    : kingdomId(id),
    name(name),
    playerName(player),
    isActive(false),
    lowerStats(buildingManager),
    middleStats(buildingManager),
    upperStats(buildingManager),
    mapX(0),
    mapY(0),
    currentFoodStorage(0),
    overallStrength(0),
    defensePower(0),
    attackPower(0)
{
    // Initialize population pointers
    pops[0] = &lowerStats;
    pops[1] = &middleStats;
    pops[2] = &upperStats;



    // Set up initial leader
    Leader defaultLeader = {
        "Leader of " + name,
        30,
        "human",
        {50, 50, 50}, // balanced popularity
        {0.33f, 0.33f, 0.34f}, // balanced resource distribution
        0.25f, // treasury ratio
        1.0f, // inflation factor
        1.0f // morale
    };
    leadership.addLeader(defaultLeader);
    leadership.election();


    setupInitialBuildings();             //this builds the initial default buildings, sets initial millitary strength booster, sets initial storage etc
   
}

void Kingdom::setMapPosition(int x, int y) { mapX = x; mapY = y; }

int Kingdom::getMapX() const { return mapX; }

int Kingdom::getMapY() const { return mapY; }

void Kingdom::setActive(bool active) { isActive = active; }

BuildingManager& Kingdom::getBuildingManager() { return buildingManager; }

Population* Kingdom::getPopulation(int index) {
    if (index >= 0 && index < 3) return pops[index];
    return nullptr;
}

TradeManager& Kingdom::getTradeManager() { return tradeManager; }

Leadership& Kingdom::getLeadership() { return leadership; }

Bank& Kingdom::getBank() { return bank; }

KharchaManager& Kingdom::getKharcha() { return kharcha; }

MilitaryManagement& Kingdom::getMilitary() { return military; }

int Kingdom::getFoodStorage() const { return currentFoodStorage; }

void Kingdom::setFoodStorage(int amount) { currentFoodStorage = amount; }

void Kingdom::addFood(int amount) { currentFoodStorage += amount; }

void Kingdom::removeFood(int amount) {
    currentFoodStorage -= amount;
    if (currentFoodStorage < 0) currentFoodStorage = 0;
}

void Kingdom::calculateStrength() {
    // Base strength from military units
    float militaryStrength = military.getTotalUnits() * 10.0f;

    // Add strength from buildings
    militaryStrength += buildingManager.getCount<Guardhouse>() * 50.0f;
    militaryStrength += buildingManager.getCount<Barracks>() * 75.0f;
    militaryStrength += buildingManager.getCount<Tavern>() * 30.0f;

    // Intelligence factor from spies
    float intelligenceFactor = 1.0f + (buildingManager.getTotalSpyCount() * 0.02f);

    // Guard strength
    float guardStrength = buildingManager.getTotalGuardCount() * 5.0f;

    // Calculate overall strength with leader's morale factor
    float moraleFactor = leadership.getCurrentLeader().morale;
    overallStrength = militaryStrength * moraleFactor * intelligenceFactor;

    // Defense is based more on guards and buildings
    defensePower = (guardStrength * 1.5f + militaryStrength * 0.5f) * moraleFactor;

    // Attack is based more on military units
    attackPower = (militaryStrength * 1.5f + guardStrength * 0.3f) * moraleFactor * intelligenceFactor;
}

float Kingdom::getOverallStrength() const { return overallStrength; }

float Kingdom::getDefensePower() const { return defensePower; }

float Kingdom::getAttackPower() const { return attackPower; }

void Kingdom::setupInitialBuildings()
{
#include <cstdlib> // For rand()
    // ----- Row 10 -----
    buildingManager.addBuilding<UpperDwelling>(10, 0, TILE_W, TILE_H, cellPos[10][0]);
    buildingManager.addBuilding<UpperDwelling>(10, 1, TILE_W, TILE_H, cellPos[10][1]);
    buildingManager.addBuilding<UpperDwelling>(10, 2, TILE_W, TILE_H, cellPos[10][2]);
    buildingManager.addBuilding<UpperDwelling>(10, 3, TILE_W, TILE_H, cellPos[10][3]);
    buildingManager.addBuilding<UpperDwelling>(10, 4, TILE_W, TILE_H, cellPos[10][4]);
    buildingManager.addBuilding<UpperDwelling>(10, 5, TILE_W, TILE_H, cellPos[10][5]);

    buildingManager.addBuilding<LowerDwelling>(10, 6, TILE_W, TILE_H, cellPos[10][6]);
    buildingManager.addBuilding<LowerDwelling>(10, 7, TILE_W, TILE_H, cellPos[10][7]);
    buildingManager.addBuilding<LowerDwelling>(10, 8, TILE_W, TILE_H, cellPos[10][8]);
    buildingManager.addBuilding<LowerDwelling>(10, 9, TILE_W, TILE_H, cellPos[10][9]);
    buildingManager.addBuilding<LowerDwelling>(10, 10, TILE_W, TILE_H, cellPos[10][10]);
    buildingManager.addBuilding<LowerDwelling>(10, 11, TILE_W, TILE_H, cellPos[10][11]);

    buildingManager.addBuilding<LowerDwelling>(11, 0, TILE_W, TILE_H, cellPos[11][0]);
    buildingManager.addBuilding<LowerDwelling>(11, 1, TILE_W, TILE_H, cellPos[11][1]);
    buildingManager.addBuilding<LowerDwelling>(11, 2, TILE_W, TILE_H, cellPos[11][2]);
    buildingManager.addBuilding<LowerDwelling>(11, 3, TILE_W, TILE_H, cellPos[11][3]);
    buildingManager.addBuilding<LowerDwelling>(11, 4, TILE_W, TILE_H, cellPos[11][4]);
    buildingManager.addBuilding<LowerDwelling>(11, 5, TILE_W, TILE_H, cellPos[11][5]);

    buildingManager.addBuilding<MiddleDwelling>(11, 6, TILE_W, TILE_H, cellPos[11][6]);
    buildingManager.addBuilding<MiddleDwelling>(11, 7, TILE_W, TILE_H, cellPos[11][7]);
    buildingManager.addBuilding<MiddleDwelling>(11, 8, TILE_W, TILE_H, cellPos[11][8]);
    buildingManager.addBuilding<MiddleDwelling>(11, 9, TILE_W, TILE_H, cellPos[11][9]);
    buildingManager.addBuilding<MiddleDwelling>(11, 10, TILE_W, TILE_H, cellPos[11][10]);
    buildingManager.addBuilding<MiddleDwelling>(11, 11, TILE_W, TILE_H, cellPos[11][11]);

    // ----- Guardhouses (moved up) -----
    buildingManager.addBuilding<Guardhouse>(1, 10, TILE_W, TILE_H, cellPos[1][10]);
    buildingManager.addBuilding<Guardhouse>(2, 11, TILE_W, TILE_H, cellPos[2][11]);
    buildingManager.addBuilding<Guardhouse>(1, 11, TILE_W, TILE_H, cellPos[1][11]);
    // ----baracks----
    buildingManager.addBuilding<Barracks>(2, 8, TILE_W, TILE_H, cellPos[2][8]);
    buildingManager.addBuilding<Barracks>(3, 9, TILE_W, TILE_H, cellPos[3][9]);

    buildingManager.addBuilding<Tavern>(0, 1, TILE_W, TILE_H, cellPos[0][1]);
    buildingManager.addBuilding<Tavern>(0, 2, TILE_W, TILE_H, cellPos[0][2]);

    // ----- 5 Woodcutters in column 1 (left side) -----
    buildingManager.addBuilding<Woodcutter>(3, 1, TILE_W, TILE_H, cellPos[3][1]);
    buildingManager.addBuilding<Woodcutter>(4, 1, TILE_W, TILE_H, cellPos[4][1]);
    buildingManager.addBuilding<Woodcutter>(5, 1, TILE_W, TILE_H, cellPos[5][1]);
    buildingManager.addBuilding<Woodcutter>(6, 1, TILE_W, TILE_H, cellPos[6][1]);
    buildingManager.addBuilding<Woodcutter>(7, 1, TILE_W, TILE_H, cellPos[7][1]);





    // ----- 5 StoneMansions in column 2 -----
    buildingManager.addBuilding<StoneMansion>(3, 2, TILE_W, TILE_H, cellPos[3][2]);
    buildingManager.addBuilding<StoneMansion>(4, 2, TILE_W, TILE_H, cellPos[4][2]);
    buildingManager.addBuilding<StoneMansion>(5, 2, TILE_W, TILE_H, cellPos[5][2]);
    buildingManager.addBuilding<StoneMansion>(6, 2, TILE_W, TILE_H, cellPos[6][2]);
    buildingManager.addBuilding<StoneMansion>(7, 2, TILE_W, TILE_H, cellPos[7][2]);




    // e.g. after wood/stone placements:
    buildingManager.addBuilding<Farm>(3, 3, TILE_W, TILE_H, cellPos[3][3]);
    buildingManager.addBuilding<Farm>(4, 3, TILE_W, TILE_H, cellPos[4][3]);
    buildingManager.addBuilding<Farm>(5, 3, TILE_W, TILE_H, cellPos[5][3]);
    buildingManager.addBuilding<Farm>(6, 3, TILE_W, TILE_H, cellPos[6][3]);
    buildingManager.addBuilding<Farm>(7, 3, TILE_W, TILE_H, cellPos[7][3]);



    buildingManager.addBuilding<Storehouse>(3, 11, TILE_W, TILE_H, cellPos[3][11]);
    buildingManager.addBuilding<Storehouse>(4, 11, TILE_W, TILE_H, cellPos[4][11]);
    buildingManager.addBuilding<Storehouse>(5, 11, TILE_W, TILE_H, cellPos[5][11]);




    // ----- Keep (at top left) -----
    buildingManager.addBuilding<Keep>(1, 1, TILE_W, TILE_H, cellPos[1][1]);



    military.recruit<SpearWoman>(10);
    military.recruit<ValkyrieRanger>(5);

    // Update storage capacity based on storehouses
    int totalCap = buildingManager.getTotalStorageCapacity();
    if (totalCap == 0) totalCap = 500; // Minimum capacity




    //yo this not bad ngl, its getting the total storage by counting the storehouses and then setting them for leaders AND populations 
    //im fine w it ig




    // Update resource capacities
    for (int i = 0; i < 3; ++i) {
        pops[i]->res.setCapacity(totalCap);
    }
    leadership.getResManager().setCapacity(totalCap);
    currentFoodStorage = totalCap;

    // Recalculate strength after adding buildings
    calculateStrength();
}

void Kingdom::printSummary() const {
    std::cout << "\n===== KINGDOM: " << name << " =====" << std::endl;
    std::cout << "Ruled by: " << playerName << std::endl;
    std::cout << "Leader: " << leadership.getCurrentLeader().name << std::endl;
    std::cout << "Map Position: (" << mapX << ", " << mapY << ")" << std::endl;
    std::cout << "Military Strength: " << overallStrength << std::endl;
    std::cout << "Defense Power: " << defensePower << std::endl;
    std::cout << "Attack Power: " << attackPower << std::endl;
    std::cout << "Population: " << buildingManager.getTotalPopulation() << std::endl;
    std::cout << "Food Storage: " << currentFoodStorage << std::endl;


    printLeaderResources();
}







void Kingdom::checkEvents(float dt , sf::RenderWindow& window) {
    // — Famine —
    if (famineActive) {
        famineTimer += dt;
        if (famineTimer >= famineDuration) {
            famineActive = false;
            std::cout << "Famine ended. Farms restored.\n";
        }
    }
    else if (currentFoodStorage < 100) {
        triggerFamine();
    }

    // — War — (example: Upper class too unhappy)
    if (!warActive && pops[2]->getSatisfactionLevel() < 0.20f) {
        triggerWar(window);
    }

    // — Riot — only if none already
    if (currentRiot == RiotClass::None) {
        for (int i = 0; i < 3; ++i) {
            if (pops[i]->getSatisfactionLevel() < RIOT_THRESHOLD) {
                triggerRiot(static_cast<RiotClass>(i), window);
                break;
            }
        }
    }
}

void Kingdom::triggerRiot(RiotClass rc , sf::RenderWindow& window) {
    currentRiot = rc;
    const char* name = (rc == RiotClass::Lower ? "Lower" :
        rc == RiotClass::Middle ? "Middle" : "Upper");
    std::cout << "\n!!! " << name << " Class Riot!!!\n"
        << "[E]lection  [C]oup  [G]ive resources\n";
    showRiotAlert(rc ,window);
}
void Kingdom::resolveRiot(char key) {
    RiotClass rc = currentRiot;
    const char* name = (rc == RiotClass::Lower ? "Lower" :
        rc == RiotClass::Middle ? "Middle" : "Upper");

    switch (key) {
    case 'E': case 'e':
        std::cout << "Election held for " << name
            << ". Satisfaction → 0.5\n";
        pops[int(rc)]->setSatisfactionLevel(0.5f);
        break;

    case 'C': case 'c':
        std::cout << "Coup! Changing leader.\n";
        leadership.coup();  // your method to rotate leader
        break;

    case 'G': case 'g': {
        int giftWood = 500, giftStone = 500;
        std::cout << "Gifted " << giftWood << " wood & "
            << giftStone << " stone to " << name << " class\n";
        pops[int(rc)]->res.addWood(giftWood);
        pops[int(rc)]->res.addStone(giftStone);
        break;
    }

    default:
        // any other key → riot persists
        return;
    }

    // once resolved, clear riot state
    currentRiot = RiotClass::None;
}






void Kingdom::triggerWar(sf::RenderWindow& window) {
    warActive = true;
    std::cout << "\n=== WAR DECLARED! All buildings catch fire! ===\n";
    buildingManager.triggerRandomFireOnAll();
    showWarAlert(window);
}

void Kingdom::triggerFamine()
{
    famineActive = true;
    famineTimer = 0.f;
    std::cout << "\n=== FAMINE! Farm output halved for "
        << famineDuration << "s ===\n";
}



void Kingdom::handleFoodStorage(float production, float consumption) {
    currentFoodStorage += int(production - consumption);
    int cap = buildingManager.getTotalStorageCapacity();
    if (currentFoodStorage > cap)
        currentFoodStorage = cap;

    if (currentFoodStorage < 0) {
        float totalUnits = float(military.getTotalUnits());
        float avgFoodPerU = totalUnits > 0 ? (consumption / totalUnits) : 0.f;
        int sustainUnits = avgFoodPerU > 0 ? int(production / avgFoodPerU) : 0;
        int extras = military.getTotalUnits() - sustainUnits;
        if (extras < 0) extras = military.getTotalUnits();

        for (int i = 0; i < extras; ++i) {
            Unit* u = military.removeLastUnit();
            delete u;
        }
        std::cout << "Food shortage! " << extras
            << " units have perished.\n";

        currentFoodStorage = 0;
    }
}




void Kingdom::executeBankOp() {



    int cls = -1, res = -1, amt = 0;
    if (pendingBankOp == BankOp::Deposit) {
        std::cout << "Deposit – class (0=low,1=mid,2=up): ";
        std::cin >> cls;
        std::cout << "Resource (0=wood,1=stone): ";
        std::cin >> res;
        std::cout << "Amount: ";
        std::cin >> amt;

        // Deduct from on-hand, then deposit
        if (res == 0) pops[cls]->res.removeWood(amt);
        else          pops[cls]->res.removeStone(amt);
        bank.deposit(cls, res, amt);
        std::cout << "Deposited " << amt << "\n";
    }
    else if (pendingBankOp == BankOp::Withdraw) {
        std::cout << "Withdraw – class (0=low,1=mid,2=up): ";
        std::cin >> cls;
        std::cout << "Resource (0=wood,1=stone): ";
        std::cin >> res;
        std::cout << "Amount: ";
        std::cin >> amt;

        int got = bank.withdraw(cls, res, amt);
        if (res == 0) pops[cls]->res.addWood(got);
        else          pops[cls]->res.addStone(got);
        std::cout << "Withdrew " << got << "\n";
    }

    // 1) Snapshot and clear the flag immediately
    BankOp op = pendingBankOp;
    pendingBankOp = BankOp::None;






}

void Kingdom::runBankMenu() {
    std::cout << "\n=== BANK MENU ===\n"
        << "1) Deposit\n"
        << "2) Withdraw\n"
        << "3) Show Balances\n"
        << "0) Cancel\n";

    int choice = readInt("Choice: ");

    switch (choice) {
    case 1: {
        int cls = readInt("Enter class (0=Lower,1=Middle,2=Upper): ");
        int type = readInt("Enter type  (0=Wood,1=Stone): ");
        int amt = readInt("Enter amount: ");

        if (type == 0) pops[cls]->res.removeWood(amt);
        else           pops[cls]->res.removeStone(amt);
        bank.deposit(cls, type, amt);
        std::cout << "Deposited " << amt << "\n";
        break;
    }
    case 2: {
        int cls = readInt("Enter class (0=Lower,1=Middle,2=Upper): ");
        int type = readInt("Enter type  (0=Wood,1=Stone): ");
        int amt = readInt("Enter amount: ");

        int got = bank.withdraw(cls, type, amt);
        if (type == 0) pops[cls]->res.addWood(got);
        else           pops[cls]->res.addStone(got);
        std::cout << "Withdrew " << got << "\n";
        break;
    }
    case 3:
        std::cout << "\n--- BANK BALANCES ---\n";
        for (int i = 0; i < 3; ++i) {
            const char* name = pops[i]->getName();
            int w = bank.getBalance(i, 0);
            int s = bank.getBalance(i, 1);
            std::cout << name << ": Wood=" << w << ", Stone=" << s << "\n";
        }
        break;
    default:
        std::cout << "Cancelling bank menu.\n";
        break;
    }

    // —— NEW —— block until Enter is pressed, flush the buffer
    std::cout << "\nPress [Enter] to return to the game...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}












void Kingdom::handleClick(int mx, int my, sf::RenderWindow& window)
{

    if (alertBoxActive) {
        return;
    }
    sf::Vector2f mp = window.mapPixelToCoords({ mx, my });

    if (pending != PendingBuild::None) {
        // Find which cell was clicked
        int targetR = -1, targetC = -1;
        bool found = false;

        for (int r = 0; r < GRID_R && !found; ++r) {
            for (int c = 0; c < GRID_C; ++c) {
                if (cells[r][c].getGlobalBounds().contains(mp) &&
                    pointInConvex(cells[r][c], mp)) {
                    targetR = r;
                    targetC = c;
                    found = true;
                    break;
                }
            }
        }

        // Validate placement
        if (targetR < 0) {
            std::cout << "Error: Please click inside the grid!\n";
        }
        else {
            // Check all cells the building would occupy
            bool canPlace = true;
            sf::Vector2i buildingSize = { 1,1 }; // Default size

            // Get the size of the pending building
            switch (pending) {
            case PendingBuild::Keep: buildingSize = { 3,3 }; break;
            case PendingBuild::Storehouse: buildingSize = { 1,2 }; break;
            case PendingBuild::Guardhouse:buildingSize = { 2,2 }; break;
            case PendingBuild::Tavern:buildingSize = { 1,2 }; break;
            default: buildingSize = { 1,1 }; break;
            }

            // Check each cell the building would occupy
            for (int r = targetR; r < targetR + buildingSize.y && canPlace; ++r) {
                for (int c = targetC; c < targetC + buildingSize.x && canPlace; ++c) {
                    if (r >= GRID_R || c >= GRID_C) {
                        std::cout << "Error: building would extend beyond grid at (" << r << "," << c << ")\n";
                        canPlace = false;
                    }
                    else if (buildingManager.hasBuildingAt(r, c)) {
                        std::cout << "Error: Cannot place building at  - Cell (" << r << "," << c << ") is occupied!\n";
                        canPlace = false;
                    }
                }
            }

            // Place the building if all checks pass
            if (canPlace) {
                bool placed = false;
                switch (pending) {
                case PendingBuild::Lower:
                    buildingManager.addBuilding<LowerDwelling>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Middle:
                    buildingManager.addBuilding<MiddleDwelling>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Upper:
                    buildingManager.addBuilding<UpperDwelling>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Woodcutter:
                    buildingManager.addBuilding<Woodcutter>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::StoneMansion:
                    buildingManager.addBuilding<StoneMansion>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Guardhouse:
                    buildingManager.addBuilding<Guardhouse>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Tavern:
                    buildingManager.addBuilding<Tavern>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Storehouse:
                    buildingManager.addBuilding<Storehouse>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                case PendingBuild::Keep:
                    buildingManager.addBuilding<Keep>(targetR, targetC, TILE_W, TILE_H, cellPos[targetR][targetC]);
                    placed = true;
                    break;
                default: break;
                }

                if (placed) {
                    std::cout << "Success: Placed building at (" << targetR << "," << targetC << ")\n";
                }
            }
        }

        pending = PendingBuild::None;
        return;
    }

    // Handle normal selection (non-building mode)
    selectedR = selectedC = -1;
    for (int r = 0; r < GRID_R; ++r) {
        for (int c = 0; c < GRID_C; ++c) {
            if (cells[r][c].getGlobalBounds().contains(mp) &&
                pointInConvex(cells[r][c], mp)) {
                selectedR = r;
                selectedC = c;
                return;
            }
        }
    }

    if (alertBoxActive) {
        if (handleAlertClick(mx, my,window)) {
            return; // Click was handled by the alert box
        }
    }


}



bool Kingdom::pointInConvex(const sf::ConvexShape& s, const sf::Vector2f& _p) {   //bhool gya kia krta ye
    sf::Vector2f p = s.getInverseTransform().transformPoint(_p);
    int n = s.getPointCount(); bool in = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        auto pi = s.getPoint(i), pj = s.getPoint(j);
        if (((pi.y > p.y) != (pj.y > p.y)) &&
            (p.x < (pj.x - pi.x) * (p.y - pi.y) / (pj.y - pi.y) + pi.x))
            in = !in;
    }
    return in;
}
void Kingdom::removeBuilding(int mx, int my, sf::RenderWindow& window) {
    sf::Vector2f mp = window.mapPixelToCoords({ mx, my }); // Convert mouse position to game coordinates
    // Try to remove and tell player what happened
    if (buildingManager.removeBuildingAt(mp)) {
        std::cout << "Building removed successfully!\n";// Poof! Gone!
    }
    else {
        std::cout << "No building found at this location.\n";
    }
}

void Kingdom::showBuildMenu() {
    std::cout << "\n=== BUILD MENU ===\n"
        << "1) Lower\n2) Middle\n3) Upper\n"
        << "4) Woodcutter\n5) Stone Mansion\n"
        << "6) Guardhouse\n7) Tavern\n"
        << "8) Storehouse\n9) Keep\n"
        << "0) Cancel\n"
        << "Choose type: ";
    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1: pending = PendingBuild::Lower;       break;
    case 2: pending = PendingBuild::Middle;      break;
    case 3: pending = PendingBuild::Upper;       break;
    case 4: pending = PendingBuild::Woodcutter;  break;
    case 5: pending = PendingBuild::StoneMansion; break;
    case 6: pending = PendingBuild::Guardhouse;  break;
    case 7: pending = PendingBuild::Tavern;      break;
    case 8: pending = PendingBuild::Storehouse;  break;
    case 9: pending = PendingBuild::Keep;        break;
    default: pending = PendingBuild::None;       break;
    }
    if (pending != PendingBuild::None)
        std::cout << "Click on the map to place your building.\n";
}




void Kingdom::runRecruitMenu() {
    // 1) Display header & show leader’s resources
    auto& bankRes = leadership.getResManager();
    std::cout << "\n=== RECRUIT MENU ===\n"
        << "Leader Wood:  " << bankRes.getWood() << "\n"
        << "Leader Stone: " << bankRes.getStone() << "\n\n"
        << "1) SpearWomen\n"
        << "2) ValkyrieRangers\n"
        << "0) Cancel\n";

    int choice = readInt("Choice: ");
    if (choice == 0) return;

    int count = readInt("How many to recruit? ");
    if (count <= 0) {
        std::cout << "Canceling: must recruit at least 1.\n";
        return;
    }

    // 2) Define per‐unit costs
    int woodCostPer = 0, stoneCostPer = 0;
    if (choice == 1) {
        woodCostPer = 200;   // you choose these
        stoneCostPer = 100;
    }
    else if (choice == 2) {
        woodCostPer = 150;
        stoneCostPer = 150;
    }
    else {
        std::cout << "Invalid unit type.\n";
        return;
    }

    // 3) Total cost
    int totalWoodCost = woodCostPer * count;
    int totalStoneCost = stoneCostPer * count;

    // 4) Check leader’s resources (but allow over‐drafting if you want; here we block)
    if (bankRes.getWood() < totalWoodCost || bankRes.getStone() < totalStoneCost) {
        std::cout << "Not enough leader resources: need "
            << totalWoodCost << " wood & "
            << totalStoneCost << " stone.\n";
        // If you want to allow partial or over‐draft, remove this return.
        return;
    }

    // 5) Deduct costs
    bankRes.removeWood(totalWoodCost);
    bankRes.removeStone(totalStoneCost);

    // 6) Recruit
    if (choice == 1) {
        military.recruit<SpearWoman>(count);
        std::cout << "Recruited " << count << " SpearWomen.\n";
    }
    else {
        military.recruit<ValkyrieRanger>(count);
        std::cout << "Recruited " << count << " ValkyrieRangers.\n";
    }
}





void Kingdom::printFoodStats() const {
    int production = buildingManager.getFoodProductionPerHour();
    int consumption = military.getTotalFoodConsumptionPerHour();
    int capacity = buildingManager.getTotalStorageCapacity();
    int storage = currentFoodStorage;

    std::cout << "\n--- FOOD METRICS ---\n"
        << "Production:  " << production << " food/hr\n"
        << "Consumption: " << consumption << " food/hr\n"
        << "Storage:     " << storage << " / " << capacity << "\n\n";
}
void Kingdom::printLeaderResources() const
{
    // Access the leadership ResourceManager
    const ResourceManager& rm = leadership.getResManager();

    // Optional: also show shared food storage
    int foodStor = currentFoodStorage;
    int foodCap = buildingManager.getTotalStorageCapacity();

    std::cout << "\n--- LEADERSHIP RESOURCES ---\n"
        << "Wood:  " << rm.getWood() << "\n"
        << "Stone: " << rm.getStone() << "\n"
        << "Food Storage: " << foodStor
        << " / " << foodCap << "\n\n";
}




void Kingdom::printLeadershipStats() const {
    if (leadership.getCurrentLeader().name.empty()) {
        std::cout << "No leader elected yet.\n";
        return;
    }
    const Leader& L = leadership.getCurrentLeader();
    std::cout << "\n--- CURRENT LEADER ---\n"
        << "Name:        " << L.name << "\n"
        << "Age:         " << L.age << "\n"
        << "Race:        " << L.race << "\n"
        << "Treasury Cut:  " << (L.treasuryRatio * 100.f) << "%\n"
        << "Popularity:  [L:" << L.popularity[0]
        << " M:" << L.popularity[1]
        << " U:" << L.popularity[2] << "]\n"
        << "Ratios:      [L:" << (L.ratio[0] * 100.f) << "% "
        << "M:" << (L.ratio[1] * 100.f) << "% "
        << "U:" << (L.ratio[2] * 100.f) << "%]\n\n";
}







void Kingdom::printBankStats() const {
    std::cout << "\n--- BANK BALANCES ---\n";
    const char* names[3] = { "Lower Class", "Middle Class", "Upper Class" };
    for (int i = 0; i < 3; ++i) {
        int w = bank.getBalance(i, 0);   // resourceIdx=0→wood
        int s = bank.getBalance(i, 1);   // resourceIdx=1→stone
        std::cout << names[i]
            << " — Wood: " << w
            << ", Stone: " << s << "\n";
    }
    std::cout << "\n";
}



void Kingdom::printResourceStats() const {               // i made these variable shere because in the future i plan to use actual global variables that will store stuff 24/7
    // buildingManager already sums production for you:
    int woodPerHour = buildingManager.getWoodProductionPerHour();
    int stonePerHour = buildingManager.getStoneProductionPerHour();
    int food = buildingManager.getFoodProductionPerHour();

    std::cout << "\n--- RESOURCE PRODUCTION (per hour) ---\n"
        << "Wood:  " << woodPerHour << "\n"
        << "Stone: " << stonePerHour << "\n"
        << "Food:  " << food << "\n\n";
}








void Kingdom::printSimplePopStats() const {   // i made these variable shere because in the future i plan to use actual global variables that will store stuff 24/7
    int lower = buildingManager.getLowerClassPopulation();
    int middle = buildingManager.getMiddleClassPopulation();
    int upper = buildingManager.getUpperClassPopulation();
    int total = buildingManager.getTotalPopulation();

    std::cout << "\n--- POPULATION STATS ---\n"
        << "Lower class:  " << lower << "\n"
        << "Middle class: " << middle << "\n"
        << "Upper class:  " << upper << "\n"
        << "Total pop:    " << total << "\n\n";
}






void Kingdom::printPopulationStats() const {
    // counts & totals
    int lowerCount = lowerStats.getCount();
    int middleCount = middleStats.getCount();
    int upperCount = upperStats.getCount();
    int lowerPop = lowerStats.getPopulation();
    int middlePop = middleStats.getPopulation();
    int upperPop = upperStats.getPopulation();
    int totalPop = lowerPop + middlePop + upperPop;

    // satisfaction
    float lowSat = lowerStats.getSatisfactionLevel();
    float midSat = middleStats.getSatisfactionLevel();
    float upSat = upperStats.getSatisfactionLevel();

    // resources
    int lowWood = lowerStats.res.getWood();
    int lowStone = lowerStats.res.getStone();
    int midWood = middleStats.res.getWood();
    int midStone = middleStats.res.getStone();
    int upWood = upperStats.res.getWood();
    int upStone = upperStats.res.getStone();

    std::cout << "\n=== POPULATION & CLASS STATS ===\n";

    std::cout << "-- Lower Class --\n"
        << " Dwellings:   " << lowerCount
        << ", Population: " << lowerPop
        << ", Satisfaction: " << (lowSat * 100.f) << "%\n"
        << " Wood:        " << lowWood
        << ", Stone:      " << lowStone << "\n\n";

    std::cout << "-- Middle Class --\n"
        << " Dwellings:   " << middleCount
        << ", Population: " << middlePop
        << ", Satisfaction: " << (midSat * 100.f) << "%\n"
        << " Wood:        " << midWood
        << ", Stone:      " << midStone << "\n\n";

    std::cout << "-- Upper Class --\n"
        << " Dwellings:   " << upperCount
        << ", Population: " << upperPop
        << ", Satisfaction: " << (upSat * 100.f) << "%\n"
        << " Wood:        " << upWood
        << ", Stone:      " << upStone << "\n\n";

    std::cout << "Total population: " << totalPop << "\n\n";
}


void Kingdom::printMilitaryStats() const {
    // how many buildings of each type
    int ghCount = buildingManager.getCount<Guardhouse>();
    int tvCount = buildingManager.getCount<Tavern>();

    // how many units inside them
    int totGuards = buildingManager.getTotalGuardCount();
    int totSpies = buildingManager.getTotalSpyCount();

    std::cout << "\n--- MILITARY STATS ---\n"
        << "Guardhouses:    " << ghCount << "\n"
        << "Total Guards:   " << totGuards << "\n"
        << "Taverns:        " << tvCount << "\n"
        << "Total Spies:    " << totSpies << "\n\n";
}










void Kingdom::initSFML(const sf::Font& font, sf::RenderWindow& window) {
    // 1) Store the font
    hudFont = font;

    // 2) Create your ground and HUD bars
    ground.setSize({ 1024.f, 768.f - 100.f });
    ground.setFillColor(sf::Color(50, 150, 50));
    ground.setPosition(0.f, 0.f);

    hud.setSize({ 1024.f, 100.f });
    hud.setFillColor(sf::Color(80, 80, 80));
    hud.setPosition(0.f, 768.f - 100.f);

    // 3) Build the isometric diamond grid
    float originX = 1024.f / 2.f;
    float originY = 100.f;
    for (int r = 0; r < GRID_R; ++r) {
        for (int c = 0; c < GRID_C; ++c) {
            float x = (c - r) * (tileW * .5f) + originX;
            float y = (c + r) * (tileH * .5f) + originY;
            cellPos[r][c] = { x, y };

            sf::ConvexShape d;
            d.setPointCount(4);
            d.setPoint(0, { tileW / 2,    0 });
            d.setPoint(1, { tileW,      tileH / 2 });
            d.setPoint(2, { tileW / 2,    tileH });
            d.setPoint(3, { 0,          tileH / 2 });
            d.setOrigin(tileW / 2, tileH / 2);
            d.setPosition(x, y);


            // Set semi-transparent fill color - alpha of 120 (out of 255)
            sf::Color cellColor = sf::Color(100, 200, 100, 60);
            d.setFillColor(cellColor);

            // Semi-transparent black outline
            d.setOutlineColor(sf::Color(0, 0, 0, 90));
            d.setOutlineThickness(1.0f);

            cells[r][c] = d;
        }
    }






    // 4) Initialize HUD text
    for (int i = 0; i < 10; ++i) {
        hudText[i].setFont(hudFont);
        hudText[i].setCharacterSize(16);
        hudText[i].setFillColor(sf::Color::White);
        hudText[i].setPosition(10.f, 768.f - 90.f + i * 18.f);
        hudText[i].setString("");
    }

    // 5) **Re‑place all your buildings now that cellPos is correct**
    //    This must come *after* you build the grid.
    setupInitialBuildings();

    initResourceBar(window);
    initBackground(window);
}


void Kingdom::processSFML(const sf::Event& e, sf::RenderWindow& window)
{

    // 1) Console menus
    if (bankMenuActive) {
        runBankMenu();
        bankMenuActive = false;
        return;
    }
    if (recruitMenuActive) {
        runRecruitMenu();
        recruitMenuActive = false;
        return;
    }


    else if (e.type == sf::Event::MouseButtonPressed) {
        if (e.mouseButton.button == sf::Mouse::Left) {
            handleClick(e.mouseButton.x, e.mouseButton.y, window);
        }
        else if (e.mouseButton.button == sf::Mouse::Right) {
            removeBuilding(e.mouseButton.x, e.mouseButton.y, window);
        }

    }
    else if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {



            // GLOBAL TESTING AHHHHHHH
        //case sf::Keyboard::X:
        //    if (globalGOAT)
        //        globalGOAT = 0;
        //    else
        //        globalGOAT = 1;
        //    break;




        case sf::Keyboard::R:
            printResourceStats();
            break;
        case sf::Keyboard::P:
            printSimplePopStats();;
            break;
        case sf::Keyboard::M:
            printMilitaryStats();
            break;
        case sf::Keyboard::C:
            printPopulationStats();
            break;
        case sf::Keyboard::D:
            std::cout << "Right-click on a building to remove it.\n";
            break;
        case sf::Keyboard::B:     // okay so this shows u the build menu and changes this check to true, then depending on the check we enable our set of cases that take inputs from 0-9 or more to build a specific type of building fr
            buildMenuActive = true;
            showBuildMenu();    // prints “1) Lower … 0) Cancel”
            break;

        case sf::Keyboard::J:
            buildingManager.triggerRandomFireOnAll();
            cout << "Random fire on ALL buildings!\n";
            break;
        case sf::Keyboard::K:
            buildingManager.triggerFireOnDwellings();
            cout << "Fire spreads through dwellings!\n";
            break;
        case sf::Keyboard::L:
            buildingManager.triggerFireOnNonDwellings();
            cout << "Fire spreads through non-dwellings!\n";
            break;
        case sf::Keyboard::E: // Extinguish all fires
            buildingManager.extinguishAllFires();
            std::cout << "All fires extinguished!\n";
            break;
        case sf::Keyboard::O:  // O for Elect
            leadership.election();
            std::cout << "Election! New leader is: "
                << leadership.getCurrentLeader().name << "\n";
            break;

        case sf::Keyboard::U:  // U for cOUp
            leadership.coup();
            std::cout << "Coup! New leader is: "
                << leadership.getCurrentLeader().name << "\n";
            break;
            // e.g. alongside your other cases:
        case sf::Keyboard::V:
            printLeadershipStats();
            break;

        case sf::Keyboard::Y:  // Y = open Bank Menu
            bankMenuActive = true;
            break;
        case sf::Keyboard::T:
            military.printSummary(leadership.getCurrentLeader().morale);
            break;
        case sf::Keyboard::I:
            recruitMenuActive = true;
            break;
        case sf::Keyboard::Q:
            printLeaderResources();
            break;
        case sf::Keyboard::G:
            printFoodStats();
            break;
            //case sf::Keyboard::A:  // deposit
            //    pendingBankOp = BankOp::Deposit;
            //    break;

            //case sf::Keyboard::W:  // withdraw
            //    pendingBankOp = BankOp::Withdraw;
            //    break;

        case sf::Keyboard::N:   // N for baNk
            printBankStats();
            break;

        case sf::Keyboard::S:
                saveKingdom("kingdom_save.txt");
            break;

        case sf::Keyboard::W:
            
            loadKingdom("kingdom_save.txt");
            break;

        default:
            break;
            if (buildMenuActive)
            {
                switch (e.key.code)
                {

                    // Handle build menu number keys
                case sf::Keyboard::Num1: pending = PendingBuild::Lower; break;
                case sf::Keyboard::Num2: pending = PendingBuild::Middle; break;
                case sf::Keyboard::Num3: pending = PendingBuild::Upper; break;
                case sf::Keyboard::Num4: pending = PendingBuild::Woodcutter; break;
                case sf::Keyboard::Num5: pending = PendingBuild::StoneMansion; break;
                case sf::Keyboard::Num6: pending = PendingBuild::Guardhouse; break;
                case sf::Keyboard::Num7: pending = PendingBuild::Tavern; break;
                case sf::Keyboard::Num8: pending = PendingBuild::Storehouse; break;
                case sf::Keyboard::Num9: pending = PendingBuild::Keep; break;
                case sf::Keyboard::Num0: pending = PendingBuild::None; break;
                default:
                    break;
                }
                if (pending != PendingBuild::None)
                {
                    std::cout << "Click on the map to place your building.\n";
                }
                buildMenuActive = false;  // leave build‐menu mode whether they picked or cancelled
            }
        }
    }
}

void Kingdom::updateSFML(float dt, sf::RenderWindow& window)
{


    updateResourceBar(window);



    // 1) Pull raw production totals
    int totalWood = buildingManager.getWoodProductionPerHour();
    int totalStone = buildingManager.getStoneProductionPerHour();

    // 2) Leadership takes its treasury cut
    Leader& L = leadership.getCurrentLeader();
    float tShare = L.treasuryRatio;            // e.g. 0.10f = 10%
    int   woodForLead = int(totalWood * tShare);
    int   stoneForLead = int(totalStone * tShare);

    // deposit into leadership’s ResourceManager
    leadership.getResManager().addWood(woodForLead);
    leadership.getResManager().addStone(stoneForLead);





    // 3) Remainder goes to the three classes, but clamp at total storage capacity
    int remWood = totalWood - woodForLead;
    int remStone = totalStone - stoneForLead;

    float rL = L.ratio[0], rM = L.ratio[1], rU = L.ratio[2];

    // compute raw additions
    int addW_L = int(remWood * rL);
    int addW_M = int(remWood * rM);
    int addW_U = int(remWood * rU);

    int addS_L = int(remStone * rL);
    int addS_M = int(remStone * rM);
    int addS_U = int(remStone * rU);

    // global capacity from all storehouses
    int capacity = buildingManager.getTotalStorageCapacity();

    // clamp and apply for each class:

    // Lower
    {
        int currW = lowerStats.res.getWood();
        if (currW + addW_L > capacity) addW_L = capacity - currW;
        if (addW_L < 0) addW_L = 0;
        lowerStats.res.addWood(addW_L);

        int currS = lowerStats.res.getStone();
        if (currS + addS_L > capacity) addS_L = capacity - currS;
        if (addS_L < 0) addS_L = 0;
        lowerStats.res.addStone(addS_L);
    }

    // Middle
    {
        int currW = middleStats.res.getWood();
        if (currW + addW_M > capacity) addW_M = capacity - currW;
        if (addW_M < 0) addW_M = 0;
        middleStats.res.addWood(addW_M);

        int currS = middleStats.res.getStone();
        if (currS + addS_M > capacity) addS_M = capacity - currS;
        if (addS_M < 0) addS_M = 0;
        middleStats.res.addStone(addS_M);
    }

    // Upper
    {
        int currW = upperStats.res.getWood();
        if (currW + addW_U > capacity) addW_U = capacity - currW;
        if (addW_U < 0) addW_U = 0;
        upperStats.res.addWood(addW_U);

        int currS = upperStats.res.getStone();
        if (currS + addS_U > capacity) addS_U = capacity - currS;
        if (addS_U < 0) addS_U = 0;
        upperStats.res.addStone(addS_U);
    }







    // ——————————
    // A) Apply leader’s inflation to KharchaManager
    kharcha.setInflation(leadership.getCurrentLeader().inflationFactor);





    //  Kharcha satisfaction adjustments
    for (int i = 0; i < 3; ++i) {
        // 1) Compute the raw delta from spending vs. ideal
        float delta = kharcha.computeSatDelta(i);

        // 2) Apply it to the old satisfaction
        float newSat = pops[i]->getSatisfactionLevel() + delta;

        // 3) Manually clamp between 0 and 1
        if (newSat < 0.f)        newSat = 0.f;
        else if (newSat > 1.f)   newSat = 1.f;

        // 4) Store the updated satisfaction
        pops[i]->setSatisfactionLevel(newSat);
    }




    int production = buildingManager.getFoodProductionPerHour();
    int consumption = military.getTotalFoodConsumptionPerHour();
    handleFoodStorage(production, consumption);






    //  AI WITHDRAW/DEPSOSIT LOGIC FOR EACH CLASS , WILL BE TURNED ON LATER ABHI MANUAL

    //// Example: if a pop’s on-hand resources > X, deposit half automatically
    //for (int i = 0; i < 3; ++i) {
    //    int wood = pops[i]->res.getWood();
    //    int stone = pops[i]->res.getStone();
    //    if (wood > 50) {
    //        int w = wood / 2;
    //        pops[i]->res.removeWood(w);
    //        bank.deposit(i, 0, w);
    //    }
    //    if (stone > 50) {
    //        int s = stone / 2;
    //        pops[i]->res.removeStone(s);
    //        bank.deposit(i, 1, s);
    //    }
    //}









    //// 5) Simplified satisfaction update: average(old, popularity, ratio) depdnding on leaderrrrr
    //for (int i = 0; i < 3; ++i) {
    //    float oldSat = pops[i]->getSatisfactionLevel();
    //    float popNorm = L.popularity[i] / 100.f;

    //    if (popNorm<0.5)
    //    pops[i]->setSatisfactionLevel(oldSat-(popNorm/10));

    //    else 
    //        pops[i]->setSatisfactionLevel(oldSat + (popNorm / 10));
    //};








    // 2) One random trade per second:
    tradeManager.update(dt, pops);
    //tradeManager.update(dt, pops, globalGOAT);
    //tradeManager.update(dt, pops, globalGOAT);


      // 3) (Optional) other per‐tick logic…

          // 7) Check for auto‐events (riots, war, famine)
    checkEvents(dt  , window);



}


bool Kingdom::saveKingdom(const std::string& filename) {
    // Open file in append mode
    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for saving: " << filename << std::endl;
        return false;
    }

    // Add a clear separator between kingdoms
    file << "\n==================================\n";
    file << "# Kingdom Data Entry\n";
    file << "==================================\n";

    // Save kingdom basic information with a unique identifier
    file << "KingdomID: " << kingdomId << "\n";
    file << "Name: " << name << "\n";
    file << "PlayerName: " << playerName << "\n";
    file << "MapPosition: " << mapX << " " << mapY << "\n";
    file << "FoodStorage: " << currentFoodStorage << "\n";

    // Save leader resources
    file << "# Leader Resources\n";
    file << "LeaderWood: " << leadership.getResManager().getWood() << "\n";
    file << "LeaderStone: " << leadership.getResManager().getStone() << "\n";

    // Save building counts
    file << "# Buildings\n";
    file << "LowerDwellings: " << buildingManager.getCount<LowerDwelling>() << "\n";
    file << "MiddleDwellings: " << buildingManager.getCount<MiddleDwelling>() << "\n";
    file << "UpperDwellings: " << buildingManager.getCount<UpperDwelling>() << "\n";
    file << "Woodcutters: " << buildingManager.getCount<Woodcutter>() << "\n";
    file << "StoneMansions: " << buildingManager.getCount<StoneMansion>() << "\n";
    file << "Guardhouses: " << buildingManager.getCount<Guardhouse>() << "\n";
    file << "Taverns: " << buildingManager.getCount<Tavern>() << "\n";
    file << "Storehouses: " << buildingManager.getCount<Storehouse>() << "\n";
    file << "Keeps: " << buildingManager.getCount<Keep>() << "\n";

    // Save population resources
    file << "# Population Resources\n";
    file << "LowerClassWood: " << lowerStats.res.getWood() << "\n";
    file << "LowerClassStone: " << lowerStats.res.getStone() << "\n";
    file << "MiddleClassWood: " << middleStats.res.getWood() << "\n";
    file << "MiddleClassStone: " << middleStats.res.getStone() << "\n";
    file << "UpperClassWood: " << upperStats.res.getWood() << "\n";
    file << "UpperClassStone: " << upperStats.res.getStone() << "\n";

    // Save military units
    file << "# Military\n";
    file << "TotalUnits: " << military.getTotalUnits() << "\n";


    std::cout << "Kingdom " << name << " (ID: " << kingdomId << ") appended to: " << filename << std::endl;
    return true;
}


bool Kingdom::loadKingdom(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file for loading: " << filename << std::endl;
        return false;
    }

    std::string line;
    int kingdomCount = 0;
    bool inKingdomData = false;

    std::cout << "\n====== KINGDOM SAVE DATA ======\n";

    // Read and display file contents
    while (std::getline(file, line)) {
        // Check for kingdom section separator
        if (line.find("==================================") != std::string::npos) {
            if (inKingdomData) {
                // End of a kingdom section
                std::cout << "---------------------------\n";
                inKingdomData = false;
            }
            else {
                // Start of a kingdom section
                inKingdomData = true;
                kingdomCount++;
                std::cout << "\nKingdom #" << kingdomCount << ":\n";
                std::cout << "---------------------------\n";
            }
            continue;
        }

        // Skip empty lines
        if (!line.empty()) {
            std::cout << line << std::endl;
        }
    }

    std::cout << "\n===========================\n";
    std::cout << "Total Kingdoms Found: " << kingdomCount << std::endl;
    std::cout << "Kingdom data displayed from: " << filename << std::endl;
    return true;
}



void Kingdom::renderSFML(sf::RenderWindow& window) {
    // Draw background (only if texture loaded successfully)
    if (backgroundTexture.getSize().x > 0) {
        window.draw(backgroundSprite);
    }
    else {
        // Draw original ground as fallback
        window.draw(ground);
    }

    // 2) Draw the grid cells
    for (int r = 0; r < GRID_R; ++r) {
        for (int c = 0; c < GRID_C; ++c) {
            window.draw(cells[r][c]);
        }
    }

    // 3) Highlight selected cell
    if (selectedR >= 0 && selectedC >= 0) {
        sf::ConvexShape hl = cells[selectedR][selectedC];
        hl.setFillColor(sf::Color(200, 100, 100, 128));
        window.draw(hl);
    }

    // 4) Draw all building sprites
    buildingManager.drawAllBuildings(window);




    drawAlertBox(window);




    // 5) Draw HUD bar
    window.draw(hud);

    // 6) Draw any HUD text lines
    for (int i = 0; i < 10; ++i) {
        window.draw(hudText[i]);
    }


    window.draw(resourceBar);
    for (auto& icon : resourceIcons) {
        window.draw(icon);
    }
    for (auto& text : resourceTexts) {
        window.draw(text);
    }



}







void Kingdom::showRiotAlert(RiotClass riotClass, sf::RenderWindow& window) {
    alertBoxActive = true;
    riotAlertActive = true;
    warAlertActive = false;
    riotAlertClass = riotClass;

    // Set up alert box
    alertBox.setSize(sf::Vector2f(400, 250));
    alertBox.setFillColor(sf::Color(200, 100, 100, 240)); // Red alert color
    alertBox.setOutlineColor(sf::Color::Black);
    alertBox.setOutlineThickness(3);
    alertBox.setPosition((window.getSize().x - alertBox.getSize().x) / 2,
        (window.getSize().y - alertBox.getSize().y) / 2);

    // Set up title
    alertTitle.setFont(resourceFont);
    alertTitle.setCharacterSize(24);
    alertTitle.setFillColor(sf::Color::White);
    alertTitle.setString("RIOT ALERT!");
    alertTitle.setPosition(
        alertBox.getPosition().x + (alertBox.getSize().x - alertTitle.getLocalBounds().width) / 2,
        alertBox.getPosition().y + 15
    );

    // Set up message
    std::string classType;
    switch (riotClass) {
    case RiotClass::Lower: classType = "Lower"; break;
    case RiotClass::Middle: classType = "Middle"; break;
    case RiotClass::Upper: classType = "Upper"; break;
    default: classType = "Unknown"; break;
    }

    alertMessage.setFont(resourceFont);
    alertMessage.setCharacterSize(18);
    alertMessage.setFillColor(sf::Color::White);
    alertMessage.setString("The " + classType + " Class has started a riot!\nChoose your response:");
    alertMessage.setPosition(
        alertBox.getPosition().x + 20,
        alertBox.getPosition().y + 50
    );

    // Set up close button at top right
    closeButton.setSize(sf::Vector2f(30, 30));
    closeButton.setFillColor(sf::Color(150, 50, 50));
    closeButton.setOutlineColor(sf::Color::Black);
    closeButton.setOutlineThickness(2);
    closeButton.setPosition(
        alertBox.getPosition().x + alertBox.getSize().x - 40,
        alertBox.getPosition().y + 10
    );

    closeButtonText.setFont(resourceFont);
    closeButtonText.setCharacterSize(20);
    closeButtonText.setFillColor(sf::Color::White);
    closeButtonText.setString("X");
    closeButtonText.setPosition(
        closeButton.getPosition().x + (closeButton.getSize().x - closeButtonText.getLocalBounds().width) / 2 - 2,
        closeButton.getPosition().y + (closeButton.getSize().y - closeButtonText.getLocalBounds().height) / 2 - 10
    );

    // Create three option buttons
    riotOptionButtons.clear();
    riotOptionTexts.clear();

    // Button 1: Negotiate
    sf::RectangleShape button1(sf::Vector2f(360, 40));
    button1.setFillColor(sf::Color(70, 130, 180));
    button1.setOutlineColor(sf::Color::Black);
    button1.setOutlineThickness(2);
    button1.setPosition(
        alertBox.getPosition().x + 20,
        alertBox.getPosition().y + 100
    );
    riotOptionButtons.push_back(button1);

    sf::Text text1;
    text1.setFont(resourceFont);
    text1.setCharacterSize(16);
    text1.setFillColor(sf::Color::White);
    text1.setString("Negotiate (Costs gold but ends riot peacefully)");
    text1.setPosition(
        button1.getPosition().x + 10,
        button1.getPosition().y + (button1.getSize().y - text1.getLocalBounds().height) / 2 - 5
    );
    riotOptionTexts.push_back(text1);

    // Button 2: Force
    sf::RectangleShape button2(sf::Vector2f(360, 40));
    button2.setFillColor(sf::Color(178, 34, 34));
    button2.setOutlineColor(sf::Color::Black);
    button2.setOutlineThickness(2);
    button2.setPosition(
        alertBox.getPosition().x + 20,
        alertBox.getPosition().y + 150
    );
    riotOptionButtons.push_back(button2);

    sf::Text text2;
    text2.setFont(resourceFont);
    text2.setCharacterSize(16);
    text2.setFillColor(sf::Color::White);
    text2.setString("Use Force (Military solution, lowers satisfaction)");
    text2.setPosition(
        button2.getPosition().x + 10,
        button2.getPosition().y + (button2.getSize().y - text2.getLocalBounds().height) / 2 - 5
    );
    riotOptionTexts.push_back(text2);

    // Button 3: Ignore
    sf::RectangleShape button3(sf::Vector2f(360, 40));
    button3.setFillColor(sf::Color(105, 105, 105));
    button3.setOutlineColor(sf::Color::Black);
    button3.setOutlineThickness(2);
    button3.setPosition(
        alertBox.getPosition().x + 20,
        alertBox.getPosition().y + 200
    );
    riotOptionButtons.push_back(button3);

    sf::Text text3;
    text3.setFont(resourceFont);
    text3.setCharacterSize(16);
    text3.setFillColor(sf::Color::White);
    text3.setString("Ignore (Riot continues, may spread to other classes)");
    text3.setPosition(
        button3.getPosition().x + 10,
        button3.getPosition().y + (button3.getSize().y - text3.getLocalBounds().height) / 2 - 5
    );
    riotOptionTexts.push_back(text3);
}








void Kingdom::showWarAlert(sf::RenderWindow& window) {
    alertBoxActive = true;
    riotAlertActive = false;
    warAlertActive = true;

    // Set up alert box
    alertBox.setSize(sf::Vector2f(400, 200));
    alertBox.setFillColor(sf::Color(100, 0, 0, 240)); // Dark red alert color
    alertBox.setOutlineColor(sf::Color::Black);
    alertBox.setOutlineThickness(3);
    alertBox.setPosition((window.getSize().x - alertBox.getSize().x) / 2,
        (window.getSize().y - alertBox.getSize().y) / 2);

    // Set up title
    alertTitle.setFont(resourceFont);
    alertTitle.setCharacterSize(24);
    alertTitle.setFillColor(sf::Color::White);
    alertTitle.setString("WAR ALERT!");
    alertTitle.setPosition(
        alertBox.getPosition().x + (alertBox.getSize().x - alertTitle.getLocalBounds().width) / 2,
        alertBox.getPosition().y + 15
    );

    // Set up message
    alertMessage.setFont(resourceFont);
    alertMessage.setCharacterSize(18);
    alertMessage.setFillColor(sf::Color::White);
    alertMessage.setString("Your kingdom is under attack!\n\nEnemy forces are approaching.\nPrepare your defenses immediately!");
    alertMessage.setPosition(
        alertBox.getPosition().x + 20,
        alertBox.getPosition().y + 50
    );

    // Set up close button at top right
    closeButton.setSize(sf::Vector2f(30, 30));
    closeButton.setFillColor(sf::Color(150, 50, 50));
    closeButton.setOutlineColor(sf::Color::Black);
    closeButton.setOutlineThickness(2);
    closeButton.setPosition(
        alertBox.getPosition().x + alertBox.getSize().x - 40,
        alertBox.getPosition().y + 10
    );

    closeButtonText.setFont(resourceFont);
    closeButtonText.setCharacterSize(20);
    closeButtonText.setFillColor(sf::Color::White);
    closeButtonText.setString("X");
    closeButtonText.setPosition(
        closeButton.getPosition().x + (closeButton.getSize().x - closeButtonText.getLocalBounds().width) / 2 - 2,
        closeButton.getPosition().y + (closeButton.getSize().y - closeButtonText.getLocalBounds().height) / 2 - 10
    );

    // Set up OK button at bottom center
    sf::RectangleShape okButton(sf::Vector2f(100, 40));
    okButton.setFillColor(sf::Color(70, 130, 180));
    okButton.setOutlineColor(sf::Color::Black);
    okButton.setOutlineThickness(2);
    okButton.setPosition(
        alertBox.getPosition().x + (alertBox.getSize().x - okButton.getSize().x) / 2,
        alertBox.getPosition().y + alertBox.getSize().y - 50
    );
    riotOptionButtons.clear();
    riotOptionButtons.push_back(okButton);

    sf::Text okText;
    okText.setFont(resourceFont);
    okText.setCharacterSize(18);
    okText.setFillColor(sf::Color::White);
    okText.setString("OK");
    okText.setPosition(
        okButton.getPosition().x + (okButton.getSize().x - okText.getLocalBounds().width) / 2,
        okButton.getPosition().y + (okButton.getSize().y - okText.getLocalBounds().height) / 2 - 5
    );
    riotOptionTexts.clear();
    riotOptionTexts.push_back(okText);
}


void Kingdom::drawAlertBox(sf::RenderWindow& window) {
    if (!alertBoxActive) return;

    window.draw(alertBox);
    window.draw(alertTitle);
    window.draw(alertMessage);
    window.draw(closeButton);
    window.draw(closeButtonText);

    for (size_t i = 0; i < riotOptionButtons.size(); i++) {
        window.draw(riotOptionButtons[i]);
        window.draw(riotOptionTexts[i]);
    }
}

bool Kingdom::handleAlertClick(int mx, int my ,sf::RenderWindow& window) {
    if (!alertBoxActive) return false;

    sf::Vector2f mousePos = window.mapPixelToCoords({ mx, my });

    // Check if close button was clicked
    if (closeButton.getGlobalBounds().contains(mousePos)) {
        alertBoxActive = false;
        return true;
    }

    // Check option buttons for riot alert
    if (riotAlertActive) {
        for (size_t i = 0; i < riotOptionButtons.size(); i++) {
            if (riotOptionButtons[i].getGlobalBounds().contains(mousePos)) {
                // Handle the riot option selected
                resolveRiot('1' + i); // '1' for first option, '2' for second, etc.
                alertBoxActive = false;
                return true;
            }
        }
    }

    // Check OK button for war alert
    if (warAlertActive && !riotOptionButtons.empty()) {
        if (riotOptionButtons[0].getGlobalBounds().contains(mousePos)) {
            alertBoxActive = false;
            return true;
        }
    }

    return false;
}













void Kingdom::initResourceBar(sf::RenderWindow& window) {
    if (resourceBarInitialized) return;

    // Create the main bar
    resourceBar.setSize(sf::Vector2f(window.getSize().x, 40));
    resourceBar.setPosition(0, 0);
    resourceBar.setFillColor(sf::Color(210, 180, 140, 230)); // Tan/beige color with slight transparency
    resourceBar.setOutlineColor(sf::Color(139, 69, 19)); // Brown outline
    resourceBar.setOutlineThickness(2);

    // Load font
    if (!resourceFont.loadFromFile("ARIAL.ttf")) {
        std::cout << "Warning: Unable to load font for resource bar. Using default.\n";
        // Continue without the font - we'll use default SFML font
    }

    // Create placeholders for resource icons (we'll use colored rectangles if no images are available)
    // For wood (brown)
    sf::RectangleShape woodRect(sf::Vector2f(25, 25));
    woodRect.setFillColor(sf::Color(139, 69, 19)); // Brown

    // For stone (gray)
    sf::RectangleShape stoneRect(sf::Vector2f(25, 25));
    stoneRect.setFillColor(sf::Color(128, 128, 128)); // Gray

    // For population (yellow/gold)
    sf::RectangleShape popRect(sf::Vector2f(25, 25));
    popRect.setFillColor(sf::Color(255, 215, 0)); // Gold

    // For military (red)
    sf::RectangleShape militaryRect(sf::Vector2f(25, 25));
    militaryRect.setFillColor(sf::Color(178, 34, 34)); // Firebrick red

    sf::RectangleShape foodRect(sf::Vector2f(25, 25));
    foodRect.setFillColor(sf::Color(50, 205, 50));

    // Create the resource icons
    sf::RenderTexture rt;
    rt.create(25, 25);

    // Wood icon
    if (!woodTexture.loadFromFile("assets/images/wood_icon.png")) {
        rt.clear(sf::Color::Transparent);
        rt.draw(woodRect);
        rt.display();
        woodTexture = rt.getTexture();
    }

    // Stone icon
    if (!stoneTexture.loadFromFile("assets/images/stone_icon.png")) {
        rt.clear(sf::Color::Transparent);
        rt.draw(stoneRect);
        rt.display();
        stoneTexture = rt.getTexture();
    }

    // Population icon
    if (!populationTexture.loadFromFile("assets/images/population_icon.png")) {
        rt.clear(sf::Color::Transparent);
        rt.draw(popRect);
        rt.display();
        populationTexture = rt.getTexture();
    }

    // Military icon
    if (!militaryTexture.loadFromFile("assets/images/military_icon.png")) {
        rt.clear(sf::Color::Transparent);
        rt.draw(militaryRect);
        rt.display();
        militaryTexture = rt.getTexture();
    }

    // food icon
    if (!foodTexture.loadFromFile("assets/images/food_icon.png")) {
        rt.clear(sf::Color::Transparent);
        rt.draw(foodRect);
        rt.display();
        foodTexture = rt.getTexture();
    }

    // Create sprites for each resource and add to the vector
    sf::Sprite woodSprite(woodTexture);

    // Set the proper scale to fit within a 25x25 area
    float woodScaleX = 25.0f / woodTexture.getSize().x;
    float woodScaleY = 25.0f / woodTexture.getSize().y;
    woodSprite.setScale(woodScaleX, woodScaleY);

    woodSprite.setPosition(20, 7);
    resourceIcons.push_back(woodSprite);

    sf::Sprite stoneSprite(stoneTexture);
    float stoneScaleX = 25.0f / stoneTexture.getSize().x;
    float stoneScaleY = 25.0f / stoneTexture.getSize().y;
    stoneSprite.setScale(stoneScaleX, stoneScaleY);
    stoneSprite.setPosition(170, 7);
    resourceIcons.push_back(stoneSprite);

    sf::Sprite popSprite(populationTexture);
    float popScaleX = 25.0f / populationTexture.getSize().x;
    float popScaleY = 25.0f / populationTexture.getSize().y;
    popSprite.setScale(popScaleX, popScaleY);
    popSprite.setPosition(320, 7);
    resourceIcons.push_back(popSprite);

    sf::Sprite militarySprite(militaryTexture);
    float militaryScaleX = 25.0f / militaryTexture.getSize().x;
    float militaryScaleY = 25.0f / militaryTexture.getSize().y;
    militarySprite.setScale(militaryScaleX, militaryScaleY);
    militarySprite.setPosition(470, 7);
    resourceIcons.push_back(militarySprite);


    sf::Sprite foodSprite(foodTexture);
    float foodScaleX = 25.0f / foodTexture.getSize().x;
    float foodScaleY = 25.0f / foodTexture.getSize().y;
    foodSprite.setScale(foodScaleX, foodScaleY);
    foodSprite.setPosition(620, 7);
    resourceIcons.push_back(foodSprite);

    // Create text fields for the resource values
    sf::Text woodText;
    woodText.setFont(resourceFont);
    woodText.setCharacterSize(18);
    woodText.setFillColor(sf::Color::Black);
    woodText.setPosition(55, 7);
    resourceTexts.push_back(woodText);

    sf::Text stoneText;
    stoneText.setFont(resourceFont);
    stoneText.setCharacterSize(18);
    stoneText.setFillColor(sf::Color::Black);
    stoneText.setPosition(205, 7);
    resourceTexts.push_back(stoneText);

    sf::Text popText;
    popText.setFont(resourceFont);
    popText.setCharacterSize(18);
    popText.setFillColor(sf::Color::Black);
    popText.setPosition(355, 7);
    resourceTexts.push_back(popText);

    sf::Text militaryText;
    militaryText.setFont(resourceFont);
    militaryText.setCharacterSize(18);
    militaryText.setFillColor(sf::Color::Black);
    militaryText.setPosition(505, 7);
    resourceTexts.push_back(militaryText);

    foodProductionText.setFont(resourceFont);
    foodProductionText.setCharacterSize(17);
    foodProductionText.setFillColor(sf::Color(0, 100, 0));
    foodProductionText.setPosition(655, 7);
    resourceTexts.push_back(foodProductionText);
    // Create food consumption text (red)
    foodConsumptionText.setFont(resourceFont);
    foodConsumptionText.setCharacterSize(17);
    foodConsumptionText.setFillColor(sf::Color(128, 0, 0));
    foodConsumptionText.setPosition(655, 22); // Position below production
    resourceTexts.push_back(foodConsumptionText);

    resourceBarInitialized = true;
}
void Kingdom::updateResourceBar(sf::RenderWindow& window) {
    if (!resourceBarInitialized) initResourceBar(window);

    // Update wood value
    int woodProduction = buildingManager.getWoodProductionPerHour();
    resourceTexts[0].setString(std::to_string(woodProduction));

    // Update stone value
    int stoneProduction = buildingManager.getStoneProductionPerHour();
    resourceTexts[1].setString(std::to_string(stoneProduction));

    // Update population value
    int totalPopulation = buildingManager.getTotalPopulation();
    resourceTexts[2].setString(std::to_string(totalPopulation));

    // Update military value (guards + spies)
    int militaryStrength = buildingManager.getTotalGuardCount() + buildingManager.getTotalSpyCount();
    resourceTexts[3].setString(std::to_string(militaryStrength));

    int foodProduction = buildingManager.getFoodProductionPerHour();
    resourceTexts[4].setString("+" + std::to_string(foodProduction));

    int foodConsumption = military.getTotalFoodConsumptionPerHour();//sirf military wali strong women consuming food
    resourceTexts[5].setString("-" + std::to_string(foodConsumption));


}


void Kingdom::initBackground(sf::RenderWindow& window) {
    // Skip if already initialized
    if (backgroundInitialized) return;

    // Load the background texture
    if (!backgroundTexture.loadFromFile("assets/images/bk1.png")) {
        std::cout << "Warning: Unable to load background image. Using default color.\n";
        // We'll continue without the background, the existing ground color will show
    }
    else {
        // Setup the background sprite
        backgroundSprite.setTexture(backgroundTexture);

        // Scale the background to fit the window if needed
        float scaleX = static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(window.getSize().y - 100) / backgroundTexture.getSize().y; // Account for HUD
        backgroundSprite.setScale(scaleX, scaleY);

        // Position at the top of the gameplay area (below resource bar)
        backgroundSprite.setPosition(0, 40); // 40 is the height of the resource bar
    }

    backgroundInitialized = true;
}
