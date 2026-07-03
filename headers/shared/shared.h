#ifndef SHARED_H
#define SHARED_H

#include <termios.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

#define NO_RETRUN 500
#define $ break;

#define RENDER_DISTANCE 5
#define CHUNK_SIZE 32
#define CELL_RESOLUTION 16 //pixels

typedef enum terrain_enum{
    none_tt = 0, grass_tt, stone_tt, water_tt
}terrain_e;

typedef enum biome_enum{
    none_bt = 0, plains_bt, ocean_bt
}biome_e;

typedef struct tags_data{
    uint32_t Vision: 1;
    uint32_t Movement: 1;
    uint32_t Liquid: 1;
}tags_t;

typedef struct terrain_data{
    uint16_t type;
    tags_t tags;
}terrain_t;

#define CELL_SIZE 1 //1 meter

typedef struct cell_data{
    terrain_t terrain;

}cell_t;

typedef struct chunk_data{
    cell_t cell_array[CHUNK_SIZE][CHUNK_SIZE];
    uint16_t biome;
}chunk_t;

typedef struct chunk_meta_data{
    chunk_t *chunk;
    int x_pos;
    int y_pos;
    uint8_t defined;

}world_tile_t;


typedef struct world_data{
    world_tile_t world_array[RENDER_DISTANCE][RENDER_DISTANCE];
}world_t;



typedef struct hex_color_field{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t opacity;
}hexcode_t;

typedef union hexcode_union{
    uint32_t code;
    hexcode_t rgba;
}hexcode_u;

#define PATH_HEADER "./world_data"

char get_input(void);
int input_int(int *value);

#endif
