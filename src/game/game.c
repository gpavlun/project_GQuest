#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>

#include <gio.h>
#include <logging.h>

#include "game.h"

uint8_t RUNNING;
pthread_mutex_t origin_lock;

extern logging_t logging;

struct debug_t{
    uint8_t keydown:1;
    uint8_t keyup:1;
    uint8_t win_dim:1;
    uint8_t coords: 1;
    uint8_t cell_borders: 1;
    uint8_t chunk_borders: 1;
}debug;


world_t world;

int window_w = 800;
int window_h = 550;

int window_change(SDL_Window *window){
    int new_width;
    int new_height;
    int result = 0;
    SDL_GetWindowSize(window, &new_width, &new_height);
    if(window_w!=new_width||window_h!=new_height){
        if(debug.win_dim) logging.info("Window changed size!");
        result = 1;
    }
    window_w = new_width;
    window_h = new_height;
    return result;  
}


SDL_Window *init_window(void){
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
    window_change(window);
    return window;
}

SDL_Renderer *init_renderer(SDL_Window *window){
    SDL_Renderer* renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );
    return renderer;  
}

uint64_t MOVEMENT_COOLDOWN = 10;
uint16_t MOVEMENT_INCREMENT = 3;

uint16_t POS_X_INCREMENT = 5;
uint16_t NEG_X_INCREMENT = 5;
uint16_t POS_Y_INCREMENT = 10;
uint16_t NEG_Y_INCREMENT = 10;

typedef struct player_position{
    uint64_t x_pos;
    uint64_t y_pos;
    uint64_t z_pos;
}player_t;

player_t player;


typedef struct inputs{
    uint8_t w:1;
    uint8_t a:1;
    uint8_t s:1;
    uint8_t d:1;
}inputs_t;

void multipress(inputs_t key){
    int tempx = player.x_pos;
    int tempy = player.y_pos;


    static uint64_t last_press;
    uint8_t reset = 0;
    if( key.w && ((SDL_GetTicks64() - last_press) >= MOVEMENT_COOLDOWN) ){
        tempy -= MOVEMENT_INCREMENT;
        reset = 1;
    }
    if( key.s && ((SDL_GetTicks64() - last_press) >= MOVEMENT_COOLDOWN) ){
        tempy += MOVEMENT_INCREMENT;
        reset = 1;
    }
    if( key.a && ((SDL_GetTicks64() - last_press) >= MOVEMENT_COOLDOWN) ){
        tempx -= MOVEMENT_INCREMENT;
        reset = 1;
    }
    if( key.d && ((SDL_GetTicks64() - last_press) >= MOVEMENT_COOLDOWN) ){
        tempx += MOVEMENT_INCREMENT;
        reset = 1;
    }
    if(reset){
        last_press = SDL_GetTicks64();
        if(debug.coords){ 
            logging.data("player x: ",tempx);
            logging.data("player y: ",tempy);
        }
    }
    
    player.x_pos = tempx;
    player.y_pos = tempy;
}


void *event_handler(){
    SDL_Event event;
    const uint8_t *keyscan = SDL_GetKeyboardState(NULL);
    inputs_t inputs;
    while(RUNNING){

        while(SDL_PollEvent(&event)){
        
            switch(event.type){
                case(SDL_QUIT):{
                
                    RUNNING = 0;
                
                $}case(SDL_KEYUP):{

                    if(debug.keyup) logging.data("Keyup",event.key.keysym.sym);

                    switch(event.key.keysym.sym){
                        case(SDLK_ESCAPE):{ // window ctrls
                            RUNNING = 0;
                        $}case(SDLK_o):{ // dev
                            debug.chunk_borders = !debug.chunk_borders;
                            if(debug.chunk_borders) logging.info("Chunk borders enabled");
                            else logging.info("Chunk borders disabled");
                        $}case(SDLK_p):{
                            debug.cell_borders = !debug.cell_borders;
                            if(debug.cell_borders) logging.info("Cell borders enabled");
                            else logging.info("Cell borders disabled");
                        $}default:$
                    }

                $}case(SDL_KEYDOWN):{
                
                    if(debug.keydown) logging.data("Keydown",event.key.keysym.sym);
                
                $}default:$
            }
        }


        inputs.w = keyscan[SDL_SCANCODE_W];
        inputs.a = keyscan[SDL_SCANCODE_A];
        inputs.s = keyscan[SDL_SCANCODE_S];
        inputs.d = keyscan[SDL_SCANCODE_D];
        multipress(inputs);

        usleep(event_rate);       
    }

    pthread_exit(0);
}









void assign_terrain(cell_t *cell, uint16_t type){
    cell->terrain.type = type;
    switch(type){
        case(grass_tt):{
            cell->terrain.tags.Liquid = 0;
            cell->terrain.tags.Movement = 1;
            cell->terrain.tags.Vision = 1;
        $}case(stone_tt):{
            cell->terrain.tags.Liquid = 0;
            cell->terrain.tags.Movement = 0;
            cell->terrain.tags.Vision = 1;
        $}case(water_tt):{
            cell->terrain.tags.Liquid = 1;
            cell->terrain.tags.Movement = 1;
            cell->terrain.tags.Vision = 1;
        $}
    }
}

void assign_biome(chunk_t *chunk, uint16_t biome){
    chunk->biome = biome;
    int i, j;
    for(i=0; i<CHUNK_SIZE; i++){
        for(j=0; j<CHUNK_SIZE; j++){
            assign_terrain(&(chunk->cell_array[i][j]), grass_tt);
        }
    }
}


int origin_chunk_x, origin_chunk_y;
void load_world(void){

    pthread_mutex_lock(&origin_lock);

    char path[64];
    FILE *chunk_file;

    int temp_chunk_x = origin_chunk_x - RENDER_DISTANCE/2 - (!(RENDER_DISTANCE%2));
    int temp_chunk_y = origin_chunk_y - RENDER_DISTANCE/2 - (!(RENDER_DISTANCE%2));

    int chunk_x = temp_chunk_x;
    int chunk_y = temp_chunk_y;
    int world_x,world_y;

    for(world_y=0;world_y<RENDER_DISTANCE;world_y++){
        chunk_y = temp_chunk_y + world_y;
        for(world_x=0;world_x<RENDER_DISTANCE;world_x++){
            chunk_x = temp_chunk_x + world_x;

            sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER, chunk_x, chunk_y);

            chunk_file = fopen(path, "rb");

            if(chunk_file){
                world.world_array[world_y][world_x].chunk = malloc(sizeof(chunk_t));
                world.world_array[world_y][world_x].x_pos = chunk_x;
                world.world_array[world_y][world_x].y_pos = chunk_y;
                if(fread(world.world_array[world_y][world_x].chunk, sizeof(chunk_t), 1, chunk_file)!=1) 
                    logging.error(500,"Couldn't read data!");
                world.world_array[world_y][world_x].defined = 1;
                fclose(chunk_file);
            }else{
                world.world_array[world_y][world_x].defined = 0;
            }
        }        
    }

    pthread_mutex_unlock(&origin_lock);
}

void init_world(){
    int world_x,world_y;
    for(world_y=0;world_y<RENDER_DISTANCE;world_y++) for(world_x=0;world_x<RENDER_DISTANCE;world_x++){
        world.world_array[world_y][world_x].chunk = malloc(sizeof(chunk_t));
    }
}



typedef struct camera_data{
    int x;
    int y;
}camera_t;
camera_t camera;

int start_game(void) {

    pthread_mutex_init(&origin_lock, NULL);

    RUNNING = 1;
    logging.info("Begin program");

    SDL_Window *window = init_window();
    if(window)  
            logging.info("Window initialized");
    else    logging.error(NO_RETRUN, "Window failed");
       
    SDL_Renderer* renderer = init_renderer(window);
    if(renderer)
            logging.info("Renderer initialized");
    else    logging.error(NO_RETRUN, "Renderer failed");

    pthread_t event_thread;
    if(pthread_create(&event_thread, NULL, event_handler, NULL))
            logging.error(NO_RETRUN, "Event thread failed");        
    else    logging.info("Event thread initialized");




    //init player

    player.x_pos = 0;
    player.y_pos = 0;
    camera.x = 0;
    camera.y = 0;
    
    
    chunk_t chunk;
    assign_biome(&chunk, 1);

    init_world();
    load_world();


    //draw terrain()
    SDL_Rect cell_box;
    cell_box.w = cell_box.h = CELL_RESOLUTION;
    SDL_Rect chunk_box;
    chunk_box.w = chunk_box.h = CHUNK_SIZE*CELL_RESOLUTION;
    chunk_box.y = window_h/2 - CHUNK_SIZE*CELL_RESOLUTION/2;
    chunk_box.x = window_w/2 - CHUNK_SIZE*CELL_RESOLUTION/2;
    hexcode_u color;  

    
    cell_t cell;
    uint8_t cell_y, cell_x;
    uint8_t chunk_y, chunk_x;
    int camera_player_delta;
    int abs_delta;
    int world_offset_x = 0;
    int world_offset_y = 0;
    while(RUNNING){

        window_change(window);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        

        SDL_Rect rect = {
            .x = window_w/2,
            .y = window_h/2,
            .w = 8,
            .h = 16
        };



        //draw player()
        camera_player_delta = player.x_pos - camera.x;
        if(camera_player_delta<0) abs_delta = 0 - camera_player_delta;
        else abs_delta = camera_player_delta;

        if(abs_delta>(CHUNK_SIZE*CELL_RESOLUTION/2)){
            if(camera_player_delta>0){
                camera.x = player.x_pos - (CHUNK_SIZE*CELL_RESOLUTION/2);
            }else{
                camera.x = player.x_pos + (CHUNK_SIZE*CELL_RESOLUTION/2);
            }
        }

        int relative_player_x = player.x_pos - camera.x;
        int array_x = window_w/2 + relative_player_x;
        rect.x = array_x;

        world_offset_x = -camera.x;
        
        camera_player_delta = player.y_pos - camera.y;
        if(camera_player_delta<0) abs_delta = 0 - camera_player_delta;
        else abs_delta = camera_player_delta;

        if(abs_delta>(CHUNK_SIZE*CELL_RESOLUTION/2)){
            if(camera_player_delta>0){
                camera.y = player.y_pos - (CHUNK_SIZE*CELL_RESOLUTION/2);
            }else{
                camera.y = player.y_pos + (CHUNK_SIZE*CELL_RESOLUTION/2);
            }
        }

        int relative_player_y = player.y_pos - camera.y;
        int array_y = window_h/2 + relative_player_y;
        rect.y = array_y;

        world_offset_y = -camera.y;

        for(chunk_y=0; chunk_y<RENDER_DISTANCE; chunk_y++){
            chunk_box.y = (window_h/2 + world_offset_y) - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_y*CHUNK_SIZE*CELL_RESOLUTION;
            for(chunk_x=0; chunk_x<RENDER_DISTANCE; chunk_x++){
                chunk_box.x = (window_w/2 + world_offset_x) - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_x*CHUNK_SIZE*CELL_RESOLUTION;

                if(world.world_array[chunk_y][chunk_x].defined){
                    
                    for(cell_y=0; cell_y<CHUNK_SIZE; cell_y++){
                        
                        cell_box.y = (window_h/2 + world_offset_y) - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_y*CHUNK_SIZE*CELL_RESOLUTION + cell_y*CELL_RESOLUTION;

                        for(cell_x=0; cell_x<CHUNK_SIZE; cell_x++){

                            cell_box.x = (window_w/2 + world_offset_x) - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_x*CHUNK_SIZE*CELL_RESOLUTION + cell_x*CELL_RESOLUTION;
                            cell = world.world_array[chunk_y][chunk_x].chunk->cell_array[cell_y][cell_x];

                            switch(cell.terrain.type){
                                case(none_tt):{
                                    color.code = 0x000000;
                                $}case(grass_tt):{
                                    color.code = 0x44a83a;
                                $}case(stone_tt):{
                                    color.code = 0x4d4a4a;
                                $}case(water_tt):{
                                    color.code = 0x1055ed;
                                $}default:$
                            }
                            
                            SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);
                            SDL_RenderFillRect(renderer, &cell_box);

                            if(debug.cell_borders){
                                color.code = 0x0000FF;
                                SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);
                                SDL_RenderDrawRect(renderer, &cell_box);
                            }   

                        }
                    }
                }

                if(debug.chunk_borders){
                    color.code = 0xFF0000;
                    SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);
                    SDL_RenderDrawRect(renderer, &chunk_box);
                }
            }
        }

        color.code = 0xFF0000;
        SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);
        
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
        usleep(event_rate);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main(int argc, char **argv){
    if(argc>1){ 
        logging.detail("Starting game with args:");
        int i;
        for(i=0; i<argc; i++){
            logging.detail("%d) %s",i,argv[i]);
            if(!strcmp(argv[i],"keyup")){
                debug.keyup = 1;
            }else if(!strcmp(argv[i],"keydown")){
                debug.keydown = 1;
            }else if(!strcmp(argv[i],"win_dim")){
                debug.win_dim = 1;
            }else if(!strcmp(argv[i],"coords")){
                debug.coords = 1;
            }
        }
    }else logging.detail("Starting game");
    
    start_game();
    return 0;
}
