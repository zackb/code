default: debug

debug:
	cmake --preset debug
	cmake --build --preset debug
	ln -sf build/debug/compile_commands.json

release:
	cmake --preset release
	cmake --build --preset release
	ln -sf build/release/compile_commands.json

run: debug
	./build/debug/meconium

.PHONY: format
fmt:
	@echo "Formatting code with clang-format..."
	@find ./src ./include \( -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.c" -o -name "*.h" \) ! -name "json.hpp" -print0 | \
	xargs -0 -n 1 clang-format -i
	@echo "Done."

clean:
	rm -rf build

