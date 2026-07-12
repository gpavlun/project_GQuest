Project layout:
../.
├── bin
│   ├── devstudio.grg
│   ├── editor.grg
│   ├── game.grg
│   └── launcher-wrapper
├── build
│   ├── devstudio
│   ├── editor
│   ├── game
│   ├── logging
│   └── shared
├── documentation
│   ├── arch_gen.sh
│   ├── architecture.md
│   └── README.md
├── first-time-startup.sh
├── launcher.conf
├── libs
│   ├── gcurses
│   │   ├── build
│   │   ├── demo
│   │   ├── demo.c
│   │   ├── include
│   │   │   └── gcurses.h
│   │   ├── lib
│   │   ├── makefile
│   │   └── src
│   │       ├── gcurses.c
│   │       └── objective.c
│   ├── logging
│   │   ├── build
│   │   ├── include
│   │   │   └── logging.h
│   │   ├── lib
│   │   ├── makefile
│   │   └── src
│   │       └── logging.c
│   └── objects
│       ├── build
│       ├── include
│       │   └── objects.h
│       ├── lib
│       ├── makefile
│       ├── README.md
│       └── src
│           └── oop.c
├── makefile
├── run.sh
├── scripts
│   ├── build-boot.sh
│   ├── build-editor.sh
│   ├── build-game.sh
│   ├── run-editor.sh
│   └── run-game.sh
├── src
│   ├── devstudio
│   │   ├── bootmenu.c
│   │   ├── confparser.c
│   │   ├── dev.h
│   │   └── launcher-wrapper.c
│   ├── editor
│   │   ├── chunk_operations.c
│   │   ├── editor.c
│   │   ├── editor.h
│   │   ├── editor_menu.c
│   │   └── editor_window.c
│   ├── game
│   │   ├── game.c
│   │   └── game.h
│   └── shared
│       ├── cli_input.c
│       └── shared.h
├── tmp
└── world_data
    ├── _0_0_.ck
    ├── _0_-1_.ck
    ├── _0_1_.ck
    ├── _-1_0_.ck
    ├── _1_0_.ck
    ├── _-1_-1_.ck
    ├── _-1_1_.ck
    ├── _1_-1_.ck
    └── _1_1_.ck

33 directories, 51 files

Lines of code by file:
  281 ../src/editor/editor.c
   89 ../src/editor/editor_menu.c
  162 ../src/editor/chunk_operations.c
   88 ../src/editor/editor.h
   40 ../src/editor/editor_window.c
  476 ../src/game/game.c
   12 ../src/game/game.h
   56 ../src/devstudio/confparser.c
   37 ../src/devstudio/launcher-wrapper.c
  340 ../src/devstudio/bootmenu.c
   18 ../src/devstudio/dev.h
   35 ../src/shared/cli_input.c
   83 ../src/shared/shared.h
  117 ../libs/objects/src/oop.c
   62 ../libs/objects/include/objects.h
  111 ../libs/logging/src/logging.c
   26 ../libs/logging/include/logging.h
  175 ../libs/gcurses/demo.c
    0 ../libs/gcurses/src/objective.c
  562 ../libs/gcurses/src/gcurses.c
  257 ../libs/gcurses/include/gcurses.h
 3027 total
