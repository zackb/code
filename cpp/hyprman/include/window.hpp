#pragma once

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class Hyprman : public Gtk::Window {

public:
    Hyprman();
    Hyprman(const Hyprman&) = delete;
    Hyprman& operator=(const Hyprman&) = delete;
    ~Hyprman() override;

protected:
    // signal handlers
    void on_button_clicked();

    // member widgets
    Gtk::Button button;
};
