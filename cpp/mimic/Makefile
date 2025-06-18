default: debug

debug:
	cmake --preset debug
	cmake --build --preset debug
	ln -sf build/debug/compile_commands.json

release:
	cmake --preset release
	cmake --build --preset release
	ln -sf build/release/compile_commands.json

install: release
	cmake --install build/release

run: debug
	./build/debug/mimic

clean:
	rm -rf build

