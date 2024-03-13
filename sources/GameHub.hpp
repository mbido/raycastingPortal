#include <gf/GameManager.h>

#include "MenuScene.hpp"
#include "SelectLevelScene.hpp"
#include "LevelScene.hpp"

namespace gh {

    struct GameHub : gf::GameManager {
        GameHub();

        
        // Scenes

        MenuScene menu;
        SelectLevelScene selectLevel;
        LevelScene level1;

    };
}