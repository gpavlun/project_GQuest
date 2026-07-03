#! /bin/bash

OPTION=${1:-0}

if [ "$OPTION" -eq 0 ]; then
    make game VERBOSE=0 2>&1
else
    x-terminal-emulator -e bash -c 'make game VERBOSE=1; echo; read -n1 -rsp "press any key to close"' &
fi



