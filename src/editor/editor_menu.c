#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#include <gio.h>
#include <logging.h>

#include "editor.h"

#define nopts 4
char options[nopts][64]={
    "load chunk",
    "new chunk",
    "dynamic",
    "quit"
};


void boot_menu(void){
    
    int x, y;
    struct stat st;
    char path[64];
    char c;
    char selection;
    int out = 0;
    FILE *output;
    char buff[4096];
    int i;
    pthread_mutex_init(&origin_lock, NULL);

    start_label:

        origin_chunk_x = 0;
        origin_chunk_y = 0;

        system("clear");
        puts("==== Map Editor ====");
        puts("options:");
        for(i=0; i<nopts; i++){
            printf("  %d) %s\n", i+1, options[i]);
        }
        while(1){
            selection = get_input();
            switch(selection){
                case('1'):{
                    goto find_label;
                $}case('2'):{
                    goto create_label;
                $}case('3'):{
                    dynamic_loader();
                    goto start_label;
                $}case('4'):{
                    puts("goodbye!");
                    exit(0);
                $}case(0x1B):{
                    puts("goodbye!");
                    exit(0);
                $}case('q'):{
                    puts("goodbye!");
                    exit(0);
                $}default:{
                    out = strlen("not an option");
                    write(STDOUT_FILENO,"not an option",out);
                $} 
            }
        }



    find_label:
        printf("\nEnter X coordinate:");
        input_int(&x);
        printf("Enter Y coordinate:");
        input_int(&y);

        chunk_editor(x, y);

        goto start_label;

    create_label:
        printf("\nEnter X coordinate:");
        input_int(&x);
        printf("Enter Y coordinate:");
        input_int(&y);

        chunk_creator(x, y);

        goto start_label;
}