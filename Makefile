all: debug

debug:
	cmake --preset debug
	cmake --build --preset debug

release:
	cmake --preset release
	cmake --build --preset release

.PHONY: clean run release debug all
clean:
	rm -rf build bin/Linux/main

run:
	cd bin/Linux && ./main
