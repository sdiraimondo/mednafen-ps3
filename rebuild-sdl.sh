#!/bin/sh

export PLATFORM=sdl
cd system-sdl
make clean
make -j2
cd ../nestopia
make clean
make -j2
cd ../gambatte
make clean
make -j2
cd ../mednafen
make clean
make -j2
cd ..

