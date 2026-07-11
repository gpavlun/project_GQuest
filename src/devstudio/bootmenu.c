#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "shared.h"
#include "gcurses.h"
#include "dev.h"
#include "logging.h"

#define nopts 7
#define optw 20
char options[nopts][optw]={
    "run game           ",
    "kill game          ",
    "build+run game     ",
    "run editor         ",
    "kill editor        ",
    "build+run editor   ",
    "quit               "
};

void launch(launch_t *config){

    int pid = fork();
    if(!pid){
        execlp(config->app_term,
            config->app_term,
            "-e",
            config->app_path,
            NULL);
    }
    config->pid = pid;

}


int handle_input(char input, char *out, int *selected, launch_t *config[2]){
    FILE *output;
    char buff[1];    
    int status;
    if(waitpid(config[0]->pid, &status, WNOHANG)==config[0]->pid){
        config[0]->running = 0;
    }
    if(waitpid(config[1]->pid, &status, WNOHANG)==config[1]->pid){
        config[1]->running = 0;
    }
    


    if(input!=EOF && input!=0){
        start:
        switch(input){
            case('\n'):{
                input = *selected + 0x31;
                goto start;
            $}case('w'):{
                if((*selected) > 0) (*selected)--;
            $}case('s'):{
                if((*selected) < nopts-1) (*selected)++;
            $}case('1'):{
                *selected = 0;
                if(!config[0]->running){
                    launch(config[0]);
                    strcpy(out, "launched game");
                    config[0]->running = 1;
                }else{
                    strcpy(out, "game is running");
                }
            $}case('2'):{
                *selected = 1;
                if(config[0]->running){
                    kill(config[0]->pid, SIGTERM);
                    strcpy(out, "killed game");
                    config[0]->running = 0;
                }else{
                    strcpy(out, "game is not running");
                }
            $}case('3'):{
                *selected = 2;
                if(!config[0]->running){
                    output = popen("./scripts/build-game.sh", "r");
                    if(output && fread(buff, 1, 1, output) > 0){
                        strcpy(out, "error building game");
                        system("./scripts/build-game.sh 1");
                    }else{
                        launch(config[0]);
                        strcpy(out, "launched game");
                        config[0]->running = 1;
                    }
                    pclose(output);
                }else{
                    strcpy(out, "game is running");
                }
            $}case('4'):{
                *selected = 3;
                if(!config[1]->running){
                    launch(config[1]);
                    strcpy(out, "launched editor");
                    config[1]->running = 1;
                }else{
                    strcpy(out, "editor is running");
                }
            $}case('5'):{
                *selected = 4;
                if(config[1]->running){
                    kill(config[1]->pid, SIGTERM);
                    strcpy(out, "killed editor");
                    config[1]->running = 0;
                }else{
                    strcpy(out, "editor is not running");
                }
            $}case('6'):{
                *selected = 5;
                if(!config[1]->running){
                    output = popen("./scripts/build-editor.sh", "r");
                    if(output && fread(buff, 1, 1, output) > 0){
                        strcpy(out, "error building editor");
                        system("./scripts/build-editor.sh 1");
                    }else{
                        launch(config[1]);
                        strcpy(out, "launched editor");
                        config[1]->running = 1;
                    }
                    pclose(output);
                }else{
                    strcpy(out, "editor is running");
                }         
            $}case('7'):{
                *selected = 6;
                return 0;
            $}case(0x1B):{
                return 0;
            $}case('q'):{
                return 0;
            $}default:{
                strcpy(out, "not an option");
            $} 
        }
    }
    return 1;
}

void draw_display(term_w_t *terminal, char *message,  int selected){

    tile_t array[nopts][optw] = {0};

    tframe_t title_box;
    init_tframe(&title_box);
    
    title_box.set.min_w(strlen("GQ DEVELOPER STUDIO") + 2);

    title_box.set.h(3);
    title_box.dim.tile.color = GCS_BLUE;

    rect_t body_box;
    body_box.tile.color = GCS_GREEN;
    body_box.tile.bg_color = GCS_BG_DEFAULT;
    rect_t ui_box;
    ui_box.tile.color = GCS_YELLOW;
    ui_box.tile.bg_color = GCS_BG_DEFAULT;
    rect_t cmd_box;
    cmd_box.tile.color = GCS_BLUE;
    cmd_box.tile.bg_color = GCS_BG_DEFAULT;
  
    str_t str = {0};
    fstr_t fstr = {0};

        terminal->frame_resize();
        
        title_box.set.w(terminal->ncols);
        
        terminal->draw_frame(title_box.dim);

        
        str.r = title_box.dim.r+1;
        str.c = terminal->ncols/2 - strlen("GQ DEVELOPER STUDIO")/2;
        str.source = "GQ DEVELOPER STUDIO";
        terminal->horz_strdisp(str);
        
        body_box.c = title_box.dim.c;
        body_box.r = title_box.dim.r+title_box.dim.h;
        body_box.h = terminal->nrows-title_box.dim.h*2;
        body_box.w = 21;

        terminal->draw_frame(body_box);

        ui_box.c = body_box.c+body_box.w;
        ui_box.r = title_box.dim.r + title_box.dim.h;
        ui_box.h = terminal->nrows - title_box.dim.h*2;
        ui_box.w = terminal->ncols-body_box.w;

        terminal->draw_frame(ui_box);

        cmd_box.c = 0;
        cmd_box.r = body_box.r + body_box.h;
        cmd_box.h = 3;
        cmd_box.w = terminal->ncols;

        terminal->draw_frame(cmd_box);
        
        str.r = cmd_box.r + 1;
        str.c = cmd_box.c + 2;
        str.source = ">";
        terminal->horz_strdisp(str);

        str.r = cmd_box.r + 1;    
        str.c = cmd_box.c + 4;
        
        str.source = "                      ";
        terminal->horz_strdisp(str);
        str.source = message;
        terminal->horz_strdisp(str);
        move_cursor(terminal, cmd_box.r + 1, cmd_box.c + 4 + strlen(message));
        
        int j, i;
        for (j=0; j<nopts; j++) {

            for (i=0; options[j][i] != '\0'; i++) {

                if (selected == j) {
                    array[j][i].bg_color = GCS_BG_WHITE;
                    array[j][i].color = GCS_BLACK;
                } else {
                    array[j][i].bg_color = GCS_BG_DEFAULT;
                    array[j][i].color = GCS_DEFAULT;
                }
                array[j][i].symbol[0] = options[j][i];
                array[j][i].symbol[1] = '\0';

            }

            array[j][i].bg_color = GCS_BG_BLACK;
            array[j][i].color = GCS_WHITE;
            array[j][i].symbol[0] = 0;

            fstr.r = body_box.r+1+j;
            fstr.c = body_box.c+1;
            fstr.source = array[j];
            terminal->horz_tiledisp(fstr);        
        }
        terminal->present();

}


void boot_menu(void){
    
    term_w_t terminal;
    init_tui(&terminal);
    terminal.io_block(0);
    terminal.clear();
    terminal.present();
    
    char buff [1024];
    char input = EOF;
    char message[32];

    strcpy(message, "startup complete");

    int selected = 0;    
    int running = 1;    
    
    launch_t *config[2];
    config[0] = calloc(1, sizeof(launch_t));
    config[1] = calloc(1, sizeof(launch_t));
    FILE *file = fopen("launcher.conf", "r");
    if(!file) logging.error(404, "launcher.conf not found!");
    parse_config(file, "GAME", config[0]);
    parse_config(file, "EDITOR", config[1]);

    while(running){

        running = handle_input(input, message, &selected, config);
        draw_display(&terminal, message, selected);

        input = get_input();
        usleep(10000);
    }    


    terminal.clear();
    terminal.present();
    terminal.io_block(1);
    terminal.cursor(1);
    move_cursor(&terminal, terminal.nrows, 0);    
}

int main(void){
    boot_menu();
    return 0;
}
