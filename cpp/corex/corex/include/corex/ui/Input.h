#pragma once

namespace ui {

    enum class Key { Up, Down, Left, Right, Enter, Space, Escape, Unknown };

    bool pollEvent();
    bool keyPressed(Key key);
    bool quitRequested();

} // namespace ui
