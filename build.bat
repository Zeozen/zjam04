gcc -Wall -o zengine.exe^
 ./code/*.c^
 -I "C:\msys64\mingw64\include\SDL2"^
 -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image^
 -O2