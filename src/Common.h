//made this for the awara enums and global variables we'll use for some jugaads 
#pragma once
#ifndef COMMON_H
#define COMMON_H






//int globalGOAT = 0;

using namespace std;
enum class BuildingState {
    NORMAL,
    ON_FIRE
};

enum class ResourceType { Wood, Stone };

struct Trade {
    int           fromClass;    // 0=Lower, 1=Middle, 2=Upper
    int           toClass;      // same indexing
    ResourceType  resource;     // Wood or Stone
    int           amount;       // how many units
    bool          isBad;        // true = bad trade, dents satisfaction
    const char* description;  // human-readable reason
};


//// helper to read a single integer with validation
//static int readInt(const std::string& prompt) {
//    int x;
//    while (true) {
//        std::cout << prompt;
//        if (std::cin >> x) {
//            // consume the rest of the line (newline)
//            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//            return x;
//        }
//        // bad input: clear and retry
//        std::cin.clear();
//        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//        std::cout << "  Invalid input, please enter a number.\n";
//    }
//}




#endif // COMMON_H
