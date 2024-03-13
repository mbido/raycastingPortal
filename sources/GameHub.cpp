#include "GameHub.hpp"

namespace gh {

    GameHub::GameHub()
    : GameManager("Portal 0.0", { "../sources/resources" })
    , menu(*this)
    , selectLevel(*this)
    , level1(*this, 1)
    {
        pushScene(menu);
    }

}