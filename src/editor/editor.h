#ifndef EDITOR_H
#define EDITOR_H

#include <gio.h>
#include <SDL2/SDL.h>
#include <sys/stat.h>

#define event_rate 10000 //100x a sec

#define draw_chunk()\
    for(cell_y=0; cell_y<CHUNK_SIZE; cell_y++){\
        cell_box.y = window_h/2 - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_y*CHUNK_SIZE*CELL_RESOLUTION + cell_y*CELL_RESOLUTION;\
        for(cell_x=0; cell_x<CHUNK_SIZE; cell_x++){\
            cell_box.x = window_w/2 - RENDER_DISTANCE*CHUNK_SIZE*CELL_RESOLUTION/2 + chunk_x*CHUNK_SIZE*CELL_RESOLUTION + cell_x*CELL_RESOLUTION;\
            cell = world.world_array[chunk_y][chunk_x].chunk->cell_array[cell_y][cell_x];\
            switch(cell.terrain.type){\
                case(none_tt):{\
                    color.code = 0x000000;\
                $}case(grass_tt):{\
                    color.code = 0x44a83a;\
                $}case(stone_tt):{\
                    color.code = 0x4d4a4a;\
                $}case(water_tt):{\
                    color.code = 0x1055ed;\
                $}default:$\
            }\
            SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);\
            SDL_RenderFillRect(renderer, &cell_box);\
            if(debug.cell_borders){\
                color.code = 0x0000FF;\
                SDL_SetRenderDrawColor(renderer, color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.opacity);\
                SDL_RenderDrawRect(renderer, &cell_box);\
            }   \
        }\
    }


typedef struct boolean_struct{
    uint8_t RUNNING: 1;
    uint8_t DYNAMIC: 1;
}boolean_t;
extern boolean_t modes;

typedef struct debug_selector{
    uint8_t chunk_borders:1;
    uint8_t cell_borders:1;
}debug_t;
extern debug_t debug;

extern int window_w;
extern int window_h;

extern world_t world;
extern pthread_mutex_t origin_lock;

extern uint32_t mouse_x;
extern uint32_t mouse_y;

extern int origin_chunk_x;
extern int origin_chunk_y;

extern uint32_t selected_terrain;
extern uint8_t painting;

void boot_menu(void);

int window_change(SDL_Window *window);
SDL_Window *init_editor_window(void);
SDL_Renderer *init_editor_renderer(SDL_Window *window);

void save_chunk(world_tile_t chunk);
void create_dynamic(int x, int y);
void delete_dynamic(int x, int y);
void dynamic_loader(void);
void create_chunk(char *path);
void load_chunk_single(char *path);
void init_cell(cell_t *cell);
void init_chunk(chunk_t *chunk);
void chunk_editor(int x, int y);
void chunk_creator(int x, int y);

void load_world(void);
void paint(void);
void *editor_event_handler(void *);
void start_editor(void);


#endif
