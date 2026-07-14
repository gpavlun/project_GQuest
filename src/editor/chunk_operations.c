#include <sys/stat.h>

#include <gio.h>
#include <logging.h>

#include "editor.h"

void save_chunk(world_tile_t chunk){
    FILE *chunk_file;
    char path[64];
    sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER, chunk.x_pos, chunk.y_pos);

    chunk_file = fopen(path, "wb+");
    if(chunk_file){
        if(fwrite(chunk.chunk, sizeof(chunk_t), 1, chunk_file)!=1) logging.error(500,"Couldn't write data!");
        fclose(chunk_file);
    }else{
        logging.info("couldnt open save");
    }

    return;
}
void create_dynamic(int x, int y){
    char path[64];
    sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER,x,y);
    create_chunk(path);
    return;
} 
void delete_dynamic(int x, int y){
    char path[64];
    sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER,x,y);
    logging.info(path);
    if(remove(path)) logging.error(500,"File could not be deleted!");
    else logging.info("Deleted chunk");
    free(world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].chunk);
    world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].chunk = NULL;
    world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].defined = 0;

    return;   
}
void dynamic_loader(void){
    load_world();
    modes.DYNAMIC = 1;
    start_editor();
    modes.DYNAMIC = 0;
    logging.info("Saved Chunk");
    logging.info("press any key to return to menu");
    get_input();
    return;
}

void create_chunk(char *path){
    FILE *chunk_file = fopen(path, "wb+");
    if(chunk_file) logging.info("Created file");
    else logging.error(500,"File could not be created!");
    
    chunk_t chunk;
    init_chunk(&chunk);
    logging.info("Created Chunk");


    logging.info("Finished Editing");
    if(fwrite(&chunk, sizeof(chunk_t), 1, chunk_file)!=1) logging.error(500,"Couldn't write data!");
    fclose(chunk_file);
    logging.info("Saved Chunk");

    return;
} 

void load_chunk_single(char *path){
    FILE *chunk_file = fopen(path, "rb+");
    if(chunk_file) logging.info("Opened file");
    else logging.error(500,"File could not be opened!");  
    
    chunk_t chunk;
    if(fread(&chunk, sizeof(chunk_t), 1, chunk_file)!=1) logging.error(500,"Couldn't read data!");
    fseek(chunk_file, 0, SEEK_SET);
    logging.info("Read chunk data");

    if(RENDER_DISTANCE%2){
        world.world_array[RENDER_DISTANCE/2][RENDER_DISTANCE/2].chunk = &chunk;
    }else{
        logging.error(405,"Renderdistance cannot be even!");
    }

    start_editor();
    logging.info("Finished Editing");

    if(fwrite(&chunk, sizeof(chunk_t), 1, chunk_file)!=1) logging.error(500,"Couldn't write data!");
    fclose(chunk_file);
    logging.info("Saved Chunk");
    logging.info("press any key to return to menu");
    get_input();

    return;
}
void init_cell(cell_t *cell){
    terrain_t terrain = {0};
    cell->terrain = terrain;

    return;
}
void init_chunk(chunk_t *chunk){
    
    biome_e biome = none_bt;    
    chunk->biome = biome;
    
    uint8_t cell_y, cell_x;
    for(cell_y=0; cell_y<CHUNK_SIZE; cell_y++){
        for(cell_x=0; cell_x<CHUNK_SIZE; cell_x++){
            
            init_cell(&chunk->cell_array[cell_y][cell_x]);
        
        }
    }

    return;
}
void chunk_editor(int x, int y){

    char path[64];
    sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER,x,y);

    char c;
    struct stat st;

    if(stat(path, &st)){
        puts("Chunk not found!");
        puts("Create New? y/n");
        c = get_input();
        if(c=='y'||c=='Y'||c=='1'){ 
            create_chunk(path);
            load_chunk_single(path);
        }
    }else{
        load_chunk_single(path);
    }

    return;
}

void chunk_creator(int x, int y){
    char path[64];
    sprintf(path,"%s/_%d_%d_.ck",PATH_HEADER,x,y);

    char c;
    struct stat st;    

    if(stat(path, &st)){
        create_chunk(path);
        load_chunk_single(path); 
    }else{
        puts("Chunk already exists!");
        puts("Override? y/n");
        c = get_input();
        if(c=='y'||c=='Y'||c=='1'){
            create_chunk(path);
            load_chunk_single(path);  
        }
    }

    return;
}