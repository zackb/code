default: 
	gcc -c wlr-layer-shell-unstable-v1-client-protocol.c -o layer_shell.o
	gcc -c xdg-shell-client-protocol.c -o xdg_shell.o
	g++ -std=c++17 -o wayland_popup \
    main.cpp \
    ../ext/imgui/*.cpp \
    ../ext/imgui/backends/imgui_impl_opengl3.cpp \
	layer_shell.o xdg_shell.o \
    -I../ext/imgui -I../ext/imgui/backends \
    -I/usr/include \
    -lwayland-client -lwayland-egl -lEGL -lGL -ldl -lm -pthread

