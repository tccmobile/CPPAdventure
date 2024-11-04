#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <algorithm>
using namespace std;

class Location {
public:
    std::string name;
    std::string description;
    std::vector<std::shared_ptr<Location> > connections;
    std::vector<std::string> items;
    bool isEndLocation;

    Location(const std::string& n, const std::string& desc, bool isEnd = false)
        : name(n), description(desc), isEndLocation(isEnd) {}

    void addConnection(std::shared_ptr<Location> location) {
        connections.push_back(location);
    }

    void addItem(const std::string& item) {
        items.push_back(item);
    }
};

class Player {
public:
    std::vector<std::string> inventory;
    
    void addItem(const std::string& item) {
        inventory.push_back(item);
        std::cout << "You picked up: " << item << std::endl;
    }

    bool hasItem(const std::string& item) const {
        return find(inventory.begin(), inventory.end(), item) != inventory.end();
    }

    void showInventory() const {
        std::cout << "\nInventory:" << std::endl;
        if (inventory.empty()) {
            std::cout << "Empty" << std::endl;
            return;
        }
        for (const auto& item : inventory) {
            std::cout << "- " << item << std::endl;
        }
    }
};

class Game {
private:
    std::shared_ptr<Location> currentLocation;
    Player player;

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void displayLocation() {
        clearScreen();
        std::cout << "\n=== " << currentLocation->name << " ===" << std::endl;
        std::cout << currentLocation->description << std::endl;
        
        if (!currentLocation->items.empty()) {
            std::cout << "\nYou see:" << std::endl;
            for (const auto& item : currentLocation->items) {
                std::cout << "- " << item << std::endl;
            }
        }

        std::cout << "\nPossible exits:" << std::endl;
        for (size_t i = 0; i < currentLocation->connections.size(); ++i) {
            std::cout << i + 1 << ". Go to " << currentLocation->connections[i]->name << std::endl;
        }
    }

    void handleItems() {
        if (!currentLocation->items.empty()) {
            std::cout << "\nWould you like to pick up any items? (y/n): ";
            char choice;
            std::cin >> choice;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (choice == 'y' || choice == 'Y') {
                for (auto it = currentLocation->items.begin(); it != currentLocation->items.end();) {
                    player.addItem(*it);
                    it = currentLocation->items.erase(it);
                }
            }
        }
    }

    // Recursive function to explore locations
    void exploreLocation() {
        if (currentLocation->isEndLocation) {
            displayLocation();
            std::cout << "\nCongratulations! You've reached the end of your adventure!" << std::endl;
            return;
        }

        while (true) {
            displayLocation();
            handleItems();
            
            std::cout << "\nWhat would you like to do?" << std::endl;
            std::cout << "1-" << currentLocation->connections.size() << ". Move to a new location" << std::endl;
            std::cout << "i. Check inventory" << std::endl;
            std::cout << "q. Quit game" << std::endl;
            
            std::string choice;
            std::cout << "\nEnter your choice: ";
            std::getline(std::cin, choice);

            if (choice == "q" || choice == "Q") {
                std::cout << "Thanks for playing!" << std::endl;
                return;
            }
            
            if (choice == "i" || choice == "I") {
                player.showInventory();
                std::cout << "\nPress Enter to continue...";
                std::cin.get();
                continue;
            }

            try {
                std::vector<std::shared_ptr<Location>>::size_type index = std::stoi(choice) - 1;
                if (index >= 0 && index < static_cast<int>(currentLocation->connections.size())) {
                    currentLocation = currentLocation->connections[index];
                    exploreLocation(); // Recursive call
                    return;
                }
            } catch (...) {
                // Invalid input, will show menu again
            }
            
            std::cout << "Invalid choice. Press Enter to continue...";
            std::cin.get();
        }
    }

public:
    Game(std::shared_ptr<Location> startLocation) : currentLocation(startLocation) {}

    void start() {
        std::cout << "Welcome to the Adventure Game!" << std::endl;
        std::cout << "Press Enter to begin...";
        std::cin.get();
        exploreLocation();
    }
};

int main() {
    // Create locations
    auto cave = std::make_shared<Location>("Cave", 
        "You're in a dimly lit cave. Water drips from the ceiling.");
    auto forest = std::make_shared<Location>("Forest", 
        "You're in a dense forest. Sunlight filters through the leaves.");
    auto ruins = std::make_shared<Location>("Ancient Ruins", 
        "You stand before crumbling stone walls covered in mysterious symbols.");
    auto temple = std::make_shared<Location>("Temple", 
        "You've reached a magnificent temple atop a mountain.", true);

    // Add connections
    cave->addConnection(forest);
    forest->addConnection(cave);
    forest->addConnection(ruins);
    ruins->addConnection(forest);
    ruins->addConnection(temple);

    // Add items
    cave->addItem("Torch");
    forest->addItem("Magic Stone");
    ruins->addItem("Ancient Key");

    // Create and start game
    Game game(cave);
    game.start();

    return 0;
}
