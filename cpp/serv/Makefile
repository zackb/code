OUT=serv

default: debug

debug:
	cmake --preset debug
	cmake --build --preset debug
	ln -sf build/debug/compile_commands.json

release:
	cmake --preset release
	cmake --build --preset release
	ln -sf build/release/compile_commands.json

basic:
	g++ -std=c++20 main.cpp -o $(OUT)
	./$(OUT)

run: default
	./build/debug/$(OUT)

clean:
	rm $(OUT)

