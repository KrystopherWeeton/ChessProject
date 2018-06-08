/*#ifndef GameStorage_H
#define GameStorage_H

#include <map>
#include "RAFile.h"
#include "Move.h"

struct GameReturn {
    std::string name;
    std::vector<Move> game();
};

class GameStorage {
private:
    //The name of the control file we use for all of the games
    //static std::string CONTROL_NAME = "overview.txt";
    
    //A map with all of our games
    std::map<std::string, std::vector<Move>> games;
    
    //Saves one of the games
    void saveGame();
public:
    //Adds a game to our list
    void addGame(std::string, std::vector<Move>);
    
    //When this dies, save all the games by default
    ~GameStorage() {
        saveGames();
    }
    
    //Saves all of our games to text files
    void saveGames();
    
    //Loads a game from a path provided from the user
    GameReturn loadGame();
    
    //Manages storage
    void manageStorage();
    
};

#endif
*/
