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
	./build/debug/wandering

blend:
	blender --background --python blender_convert.py -- assets/mixamo/Zlorp.fbx assets/zlorp.glb assets/mixamo/Run.fbx assets/mixamo/Taunt.fbx assets/mixamo/Walking.fbx assets/mixamo/Jump.fbx


.PHONY: fmt
fmt:
	@echo "Formatting code with clang-format..."
	@find ./src \( -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.c" -o -name "*.h" \) ! -name "json.hpp" -print0 | \
	xargs -0 -n 1 clang-format -i
	@echo "Done."

clean:
	rm -rf build
