#! /bin/bash

echo "Hello! You most likely will want to run everything from the dev-studio." 
echo "That is how this is designed to be used, after all!"
echo ""
echo "Make sure you have SDL2 installed, or this code will not work!"
echo ""
echo "Ubuntu / Debian : sudo apt install libsdl2-dev"
echo "Fedora          : sudo dnf install SDL2-devel"
echo "Arch / Manjaro  : sudo pacman -S sdl2"
echo ""
echo "Another note, the Makefiles you find are largley depricated."
echo "Use them with extreme caution. The code is instead compiled with CMake."
echo "The commands are simple, assuming this script finishes,"
echo "you would only need to run 'cmake --build build'"
echo "The Devstudio handles all of that though! When in doubt, stay there."

echo "When you have those, hit any key to continue."
read -n1

echo "[1/4] Checking dependencies..."

command -v cmake >/dev/null || {
    echo "Error: cmake not installed"
    exit 1
}

command -v gcc >/dev/null || {
    echo "Error: gcc not installed"
    exit 1
}

command -v pkg-config >/dev/null || {
    echo "Error: pkg-config not installed"
    exit 1
}

pkg-config --exists sdl2 || {
    echo "Error: SDL2 development package not installed"
    exit 1
}


chmod +x ./scripts/*
cmake -B build
cmake --build build
./bin/devstudio
