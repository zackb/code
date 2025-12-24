#include "window.hpp"
#include <gtkmm.h>

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("com.zackbartel.hyprman");

    return app->make_window_and_run<Hyprman>(argc, argv);
}
