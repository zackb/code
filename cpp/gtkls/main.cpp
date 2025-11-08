#include <gtk-layer-shell/gtk-layer-shell.h>
#include <gtk/gtk.h>

static void on_activate(GApplication* app, gpointer user_data) {
    GtkWidget* window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Layer Shell Demo");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 40);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(box, 6);
    gtk_widget_set_margin_bottom(box, 6);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);

    GtkWidget* label = gtk_label_new("Hello from GTK layer-shell (Wayland / Hyprland)");
    gtk_box_append(GTK_BOX(box), label);

    gtk_window_set_child(GTK_WINDOW(window), box);

    gtk_layer_init_for_window(GTK_WINDOW(window));

    // put this window in the top layer
    // gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);

    // use overlay layer (above top layer)
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_OVERLAY);

    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);

    gtk_layer_set_exclusive_zone(GTK_WINDOW(window), 0);

    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, 0);

    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);

    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));

    gtk_window_present(GTK_WINDOW(window));

    // warn if not running under Wayland
    GdkDisplay* display = gdk_display_get_default();
#if defined(GDK_IS_WAYLAND_DISPLAY)
    if (!GDK_IS_WAYLAND_DISPLAY(display)) {
        g_warning("Not running under Wayland. This application targets Wayland layer-shell only.");
    }
#else
    // can't use the macro at compile time
    if (!g_strcmp0(gdk_display_get_name(display), "Wayland")) {
        /* nothing */
    }
#endif
}

int main(int argc, char** argv) {
    GtkApplication* app = gtk_application_new("com.example.LayerShellDemo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
