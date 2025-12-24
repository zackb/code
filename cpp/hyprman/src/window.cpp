#include "window.hpp"

Hyprman::Hyprman() : button("Click Me") {
    set_title("hyprman");
    set_default_size(200, 100);
    set_margin_top(10);
    set_margin_bottom(10);
    set_margin_start(10);
    set_margin_end(10);

    button.set_size_request(50, 50);
    button.signal_clicked().connect(sigc::mem_fun(*this, &Hyprman::on_button_clicked));

    set_child(button);
    button.show();
}

void Hyprman::on_button_clicked() { button.set_label("Clicked!"); }

Hyprman::~Hyprman() {}
