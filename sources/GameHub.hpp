#ifndef GAMEHUB_HPP
#define GAMEHUB_HPP

#include <gf/GameManager.h>

#include "MenuScene.hpp"
#include "SelectLevelScene.hpp"
#include "LevelScene.hpp"
#include "LevelEndScene.hpp"

namespace gh {

    struct GameHub : gf::GameManager {
        GameHub();

        
        // Scenes

        MenuScene menu;
        SelectLevelScene selectLevel;
        LevelScene level1;
        LevelEndScene level1End;

    };
}

#endif // GAMEHUB_HPP