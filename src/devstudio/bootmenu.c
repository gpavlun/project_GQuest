#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "shared/shared.h"

#define nopts 5
char options[nopts][64]={
    "run game",
    "build-run game",
    "run editor",
    "build-run editor",
    "quit"
};


int boot_menu(int argc, char **argv){
    puts("==== GREG QUEST ====");
    puts("options:");
    for(int i=0; i<nopts; i++){
        printf("  %d) %s\n", i+1, options[i]);
    }
    write(STDOUT_FILENO,"\n> ",3);
    char selection;
    int out = 0;
    char back[1] = {8};
    char clr[1] = {' '};
    FILE *output;
    char buff[4096];
    while(1){
        selection = get_input();
        for(int i=0;i<out;i++) write(STDOUT_FILENO,back,1);
        for(int i=0;i<out;i++) write(STDOUT_FILENO,clr,1);
        for(int i=0;i<out;i++) write(STDOUT_FILENO,back,1);
        switch(selection){
            case('1'):{
                out = strlen("launched game");
                system("./scripts/run-game.sh");
                write(STDOUT_FILENO,"launched game",out);
            $}case('2'):{
                output = popen("./scripts/build-game.sh", "r");
                if(output && fread(buff, 1, 1, output) > 0){
                    out = strlen("error building game");
                    write(STDOUT_FILENO,"error building game",out);
                    system("./scripts/build-game.sh 1");
                }else{
                    out = strlen("built game");
                    write(STDOUT_FILENO,"built game",out);
                    system("./scripts/run-game.sh");
                }
            $}case('3'):{
                out = strlen("launched editor");
                system("./scripts/run-editor.sh");
                write(STDOUT_FILENO,"launched editor",out);
            $}case('4'):{
                output = popen("./scripts/build-editor.sh", "r");
                if(output && fread(buff, 1, 1, output) > 0){
                    out = strlen("error building editor");
                    write(STDOUT_FILENO,"error building editor",out);
                    system("./scripts/build-editor.sh 1");
                }else{
                    out = strlen("built editor");
                    write(STDOUT_FILENO,"built editor",out);
                    system("./scripts/run-editor.sh");
                }
            $}case('5'):{
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
}

int main(int argc, char **argv){
    boot_menu(argc,argv);
    return 0;
}
