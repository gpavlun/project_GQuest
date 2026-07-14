#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include <gio.h>
#include <gcurses.h>
#include <logging.h>

#include "dev.h"

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
int readpid(const char *path){
    FILE *f;
    pid_t pid;

    while(1){
        f = fopen(path, "r");

        if(f){
            if(fscanf(f, "%d", &pid)==1){
                fclose(f);
                return pid;
            }
            fclose(f);
        }

        usleep(10000);
    }
}
void launch(launch_t *config){
    sprintf(config->tmp_path,"./tmp/%s.pid",config->name);
    int pid = fork();
    if (!pid) {
        execlp(config->app_term,
            config->app_term,
            "-e",
            "./bin/launcher-wrapper",
            config->tmp_path,
            config->app_path,
            NULL);

        exit(1);
    }
    config->pid = readpid(config->tmp_path);
}

void get_status(launch_t *config){
    FILE *f;
    if(config->tmp_path&&config->tmp_path[0]){
        f = fopen(config->tmp_path, "r");
        if(f){
            config->running = 1;
            fclose(f);
        }else{
            config->running = 0;
        }
    }else{
        config->running = 0;
    }
}

int handle_input(char input, char *out, int *selected, launch_t *config[2]){
    FILE *output;
    char buff[1];    
    
    get_status(config[0]);
    get_status(config[1]);


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
                }else{
                    strcpy(out, "game is running");
                }
            $}case('2'):{
                *selected = 1;
                if(config[0]->running){
                    kill(config[0]->pid, SIGKILL);
                    remove(config[0]->tmp_path);
                    config[0]->tmp_path[0] = 0;
                    strcpy(out, "killed game");
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
                }else{
                    strcpy(out, "editor is running");
                }
            $}case('5'):{
                *selected = 4;
                if(config[1]->running){
                    kill(config[1]->pid, SIGKILL);
                    remove(config[1]->tmp_path);
                    config[1]->tmp_path[0] = 0;
                    strcpy(out, "killed editor");
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
        str.r = ui_box.r + 1;
        str.c = ui_box.c + 1;
        str.source = "1. w and s to select options";
        terminal->horz_strdisp(str);
        str.r = ui_box.r + 2;
        str.c = ui_box.c + 1;
        str.source = "2. enter to run option";
        terminal->horz_strdisp(str);
        str.r = ui_box.r + 3;
        str.c = ui_box.c + 1;
        str.source = "3. 1-9 are hotkeys for select";
        terminal->horz_strdisp(str);
        str.r = ui_box.r + 4;
        str.c = ui_box.c + 1;
        str.source = "4. esc or q to quick exit";
        terminal->horz_strdisp(str);

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
    
    char input = EOF;
    char message[64];

    strcpy(message, "startup complete");

    int selected = 0;    
    int running = 1;    
    
    launch_t *config[2];
    config[0] = calloc(1, sizeof(launch_t));
    config[1] = calloc(1, sizeof(launch_t));
    FILE *file = fopen("launcher.conf", "r");
    if(!file) logging.error(404, "launcher.conf not found!");
    rewind(file);
    if(parse_config(file, "GAME", config[0])) logging.error(500, "Game config malformed");
    rewind(file);
    if(parse_config(file, "EDITOR", config[1])) logging.error(500, "Editor config malformed");

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
    system("rm ./tmp/*");
    boot_menu();
    return 0;
}
