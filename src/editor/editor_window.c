#include <SDL2/SDL.h>

#include <gio.h>
#include "editor.h"

int window_change(SDL_Window *window){
    int new_width;
    int new_height;
    int result = 0;
    SDL_GetWindowSize(window, &new_width, &new_height);

    window_w = new_width;
    window_h = new_height;
    return result;  
}

SDL_Window *init_editor_window(void){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window =
        SDL_CreateWindow(
            "Interface",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_w,
            window_h,
            SDL_WINDOW_RESIZABLE |
            SDL_WINDOW_SHOWN);
    return window;
}

SDL_Renderer *init_editor_renderer(SDL_Window *window){
    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );
    return renderer;  
}