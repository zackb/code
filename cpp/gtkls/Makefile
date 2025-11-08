OUT=gtkls

default: debug

debug:
	cmake --preset debug
	cmake --build --preset debug
	ln -sf build/debug/compile_commands.json

release:
	cmake --preset release
	cmake --build --preset release
	ln -sf build/release/compile_commands.json

run: default
	./build/debug/$(OUT)

clean:
	rm -f $(OUT)
	rm -rf build

