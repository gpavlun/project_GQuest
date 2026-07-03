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
echo "When you have those, hit any key to continue."
read -n1
chmod +x ./scripts/*
./scripts/build-boot.sh
./bin/devstudio
