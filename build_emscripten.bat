emcc -O2 ./code/game.c ./code/main.c ./code/render.c ./code/update.c ./code/zgrid.c ./code/zmath.c ./code/zsdl.c ./code/ini.c -s ALLOW_MEMORY_GROWTH=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="[""png""]" -s USE_SDL_MIXER=2 -s LINKABLE=1 -s EXPORT_ALL=1 --shell-file shell.html -o index.html --preload-file ./assets