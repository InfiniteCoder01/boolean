.PHONY: run clean

run: boolean
	./boolean

clean:
	rm -rf html boolean

boolean: src/*.c
	gcc src/*.c -lraylib -lm -o boolean

RAYLIB_WASM_PATH = "/home/infinitecoder/Downloads/raylib-5.5_webassembly"
html/index.html: src/*.c assets/**
	rm -rf html
	mkdir html

	emcc src/*.c -o html/index.html \
		-L$(RAYLIB_WASM_PATH)/lib -I$(RAYLIB_WASM_PATH)/include -lraylib \
		-s USE_GLFW=3 -s ASYNCIFY -s ASSERTIONS \
	    --shell-file shell.html --embed-file assets \
	    -DPLATFORM_WEB

serve: html/index.html
	python -m http.server -d html
