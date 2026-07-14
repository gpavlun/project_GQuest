#include <SDL2/SDL.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>

#include <gio.h>
#include <logging.h>

#include "editor.h"

boolean_t modes;
debug_t debug;

world_t world;
pthread_mutex_t origin_lock;

uint32_t mouse_x;
uint32_t mouse_y;

int origin_chunk_x;
int origin_chunk_y;

uint32_t selected_terrain;
uint8_t painting;

int window_w = CHUNK_SIZE*CELL_RESOLUTION + 200;
int window_h = CHUNK_SIZE*CELL_RESOLUTION + 200;



void load_world(void){

    pthread_mutex_lock(&origin_lock);

    char path[64];
    FILE *chunk_file;

    int temp_chunk_x = origin_chunk_x - RENDER_DISTANCE/2 - (!(RENDER_DISTANCE%2));
    int temp_chunk_y = origin_chunk_y - RENDER_DISTANCE/2 - (!(RENDER_DISTANCE%2));

    int chunk_x = temp_chunk_x;
    int chunk_y = temp_chunk_y;
    int world_x,world_y;

    for(world_y=0;world_y<RENDER_DISTANCE;world_y++) for(world_x=0;world_x<RENDER_DISTANCE;world_x++){
        if(world.world_array[world_y][world_x].defined){ 
            save_chunk(world.world_array[world_y][world_x]);
            free(world.world_array[world_y][world_x].chunk);
            world.world_array[world_y][world_x].chunk = NULL;
            world.world_array[world_y][world_x].defined=0;

        }else{
            logging.detail("chunk %d %d not defined",world_x,world_y);
        }

    }

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
                fclose(chunk_file);
                world.world_array[world_y][world_x].defined=1;
            }
        }        
    }

    pthread_mutex_unlock(&origin_lock);
}



void paint(void){
    int cell_x = ( mouse_x - ( ( window_w - (CHUNK_SIZE*CELL_RESOLUTION) ) / 2 ) ) / CELL_RESOLUTION;
    int cell_y = (mouse_y - (window_h-CHUNK_SIZE*CELL_RESOLUTION)/2)/CELL_RESOLUTION;
    if(cell_x>=0&&cell_y>=0&&cell_x<CHUNK_SIZE&&cell_y<CHUNK_SIZE){
        world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].chunk->cell_array[cell_y][cell_x].terrain.type = selected_terrain;
    }
}



void *editor_event_handler(void *args){
    SDL_Event event;
    const uint8_t *keyscan = SDL_GetKeyboardState(NULL);
    while(modes.RUNNING){

        while(SDL_PollEvent(&event)){
        
            switch(event.type){
                case(SDL_QUIT):{
                
                    modes.RUNNING = 0;
                
                $}case(SDL_KEYUP):{

                    switch(event.key.keysym.sym){
                        case(SDLK_ESCAPE):{ // window ctrls
                            modes.RUNNING = 0;
                        $}case(SDLK_o):{ // dev
                            debug.chunk_borders = !debug.chunk_borders;
                            if(debug.chunk_borders) logging.info("Chunk borders enabled");
                            else logging.info("Chunk borders disabled");
                        $}case(SDLK_p):{
                            debug.cell_borders = !debug.cell_borders;
                            if(debug.cell_borders) logging.info("Cell borders enabled");
                            else logging.info("Cell borders disabled");
                        $}case(SDLK_0):{ // brush ctrls
                            selected_terrain = none_tt;
                            logging.data("Selected terrain: ",selected_terrain);
                        $}case(SDLK_1):{
                            selected_terrain = grass_tt;
                            logging.data("Selected terrain: ",selected_terrain);
                        $}case(SDLK_2):{
                            selected_terrain = stone_tt;
                            logging.data("Selected terrain: ",selected_terrain);
                        $}case(SDLK_3):{
                            selected_terrain = water_tt;
                            logging.data("Selected terrain: ",selected_terrain);
                        $}default:{
                            if(modes.DYNAMIC){
                                switch(event.key.keysym.sym){
                                    case(SDLK_UP):{  
                                        origin_chunk_y--;
                                        load_world();
                                    $}case(SDLK_DOWN):{
                                        origin_chunk_y++;
                                        load_world();
                                    $}case(SDLK_LEFT):{
                                        origin_chunk_x--;                                    
                                        load_world();
                                    $}case(SDLK_RIGHT):{
                                        origin_chunk_x++;                                    
                                        load_world();
                                    $}case(SDLK_RETURN):{
                                        if(!world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].defined){
                                            create_dynamic(origin_chunk_x,origin_chunk_y);
                                            load_world();
                                        }                                     
                                    $}case(SDLK_DELETE):{
                                        if(world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].defined){
                                            delete_dynamic(origin_chunk_x,origin_chunk_y);
                                            load_world();
                                        }
                                    $}default:$
                                }
                            }
                        $}
                    }


                $}case(SDL_MOUSEBUTTONDOWN):{
                    if(event.button.button == SDL_BUTTON_LEFT)
                        if(!painting){
                            painting = 1;
                            logging.data("Painting: ", painting);
                            paint();
                            
                        }
                $}case(SDL_MOUSEBUTTONUP):{
                    if(event.button.button == SDL_BUTTON_LEFT){
                        painting = 0;
                        logging.data("Painting: ", painting);
                    }
                $}case(SDL_MOUSEMOTION):{
                    mouse_x = event.motion.x;
                    mouse_y = event.motion.y;
                    if(painting)
                        paint();
                $}default:$
            }
        }
        usleep(event_rate);       
    }

    pthread_exit(0);
}





void start_editor(void){

    modes.RUNNING = 1;
    logging.info("Begin program");
    

    SDL_Window *window = init_editor_window();
    if(window)  
            logging.info("Window initialized");
    else    logging.error(NO_RETRUN, "Window failed");
       
    SDL_Renderer* renderer = init_editor_renderer(window);
    if(renderer)
            logging.info("Renderer initialized");
    else    logging.error(NO_RETRUN, "Renderer failed");

    pthread_t event_thread;
    if(pthread_create(&event_thread, NULL, editor_event_handler, NULL))
            logging.error(NO_RETRUN, "Event thread failed");        
    else    logging.info("Event thread initialized");

    SDL_GetMouseState(&mouse_x, &mouse_y);


    cell_t cell;
    uint8_t cell_y, cell_x;
    uint8_t chunk_y, chunk_x;

    SDL_Rect cell_box;
    cell_box.w = cell_box.h = CELL_RESOLUTION;
    SDL_Rect chunk_box;
    chunk_box.w = chunk_box.h = CHUNK_SIZE*CELL_RESOLUTION;
    chunk_box.y = window_h/2 - CHUNK_SIZE*CELL_RESOLUTION/2;
    chunk_box.x = window_w/2 - CHUNK_SIZE*CELL_RESOLUTION/2;
    hexcode_u color;



    while(modes.RUNNING){
        window_change(window);

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        pthread_mutex_lock(&origin_lock);
        for(chunk_y=0; chunk_y<RENDER_DISTANCE; chunk_y++){
            chunk_box.y = window_h/2 - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_y*CHUNK_SIZE*CELL_RESOLUTION;
            for(chunk_x=0; chunk_x<RENDER_DISTANCE; chunk_x++){
                chunk_box.x = window_w/2 - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_x*CHUNK_SIZE*CELL_RESOLUTION;

                if(world.world_array[chunk_y][chunk_x].chunk){

                    draw_chunk()
                    
                }

                if(debug.chunk_borders){
                    color.code = 0xFF0000;
                    SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);
                    SDL_RenderDrawRect(renderer, &chunk_box);
                }
                //logging.data("x",chunk_x);
                //logging.data("y",chunk_y);
                //usleep(125000);
                //SDL_RenderPresent(renderer);
            }
        }
        pthread_mutex_unlock(&origin_lock);


        SDL_RenderPresent(renderer);
        usleep(event_rate);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return;
}


int main (void){
    boot_menu();
    logging.detail("This is the map editor\n\to - enable chunk borders\n\tp - enable cell borders\n\t# - terrain type");
    return 0;
}