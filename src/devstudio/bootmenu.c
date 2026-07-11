#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "gcurses.h"

#define nopts 5
char options[nopts][64]={
    "run game           ",
    "build-run game     ",
    "run editor         ",
    "build-run editor   ",
    "quit               "
};


int handle_input(char input, char *out, int *selected){
    FILE *output;
    char old[32];
    char buff[4096];    
    

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
                system("./scripts/run-game.sh");
                strcpy(out, "launched game");
            $}case('2'):{
                *selected = 1;
                output = popen("./scripts/build-game.sh", "r");
                if(output && fread(buff, 1, 1, output) > 0){
                    strcpy(out, "error building game");
                    system("./scripts/build-game.sh 1");
                }else{
                    strcpy(out, "built game");
                    system("./scripts/run-game.sh");
                }
            $}case('3'):{
                *selected = 2;
                system("./scripts/run-editor.sh");
                strcpy(out, "launched editor");
            $}case('4'):{
                *selected = 3;
                output = popen("./scripts/build-editor.sh", "r");
                if(output && fread(buff, 1, 1, output) > 0){
                    strcpy(out, "error building editor");
                    system("./scripts/build-editor.sh 1");
                }else{
                    strcpy(out, "built editor");
                    system("./scripts/run-editor.sh");
                }
            $}case('5'):{
                *selected = 4;
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



int boot_menu(int argc, char **argv){
    
    term_w_t terminal;
    init_tui(&terminal);
    terminal.io_block(0);

    //terminal.cursor(0);

    char text[10];
    char input = EOF;
    char message[32];
    strcpy(message, "startup complete");
    char message_prev[32] = {0};
    cursor_origin();
    terminal.clear();
    terminal.present();

    tile_t array[nopts][20] = {0};


    int selected = 0;    
    
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
  
    str_t str;
    fstr_t fstr;
    str_t mess;

    int running = 1;

    

    while(running){
        
        strcpy(message_prev, message);
        running = handle_input(input, message, &selected);


        terminal.frame_resize();
        
        title_box.set.w(terminal.ncols);
        
        terminal.draw_frame(title_box.dim);

        
        str.r = title_box.dim.r+1;
        str.c = terminal.ncols/2 - strlen("GQ DEVELOPER STUDIO")/2;
        str.source = "GQ DEVELOPER STUDIO";
        terminal.horz_strdisp(str);
        
        body_box.c = title_box.dim.c;
        body_box.r = title_box.dim.r+title_box.dim.h;
        body_box.h = terminal.nrows-title_box.dim.h*2;
        body_box.w = 21;

        terminal.draw_frame(body_box);

        ui_box.c = body_box.c+body_box.w;
        ui_box.r = title_box.dim.r + title_box.dim.h;
        ui_box.h = terminal.nrows - title_box.dim.h*2;
        ui_box.w = terminal.ncols-body_box.w;

        terminal.draw_frame(ui_box);

        cmd_box.c = 0;
        cmd_box.r = body_box.r + body_box.h;
        cmd_box.h = 3;
        cmd_box.w = terminal.ncols;

        terminal.draw_frame(cmd_box);
        
        str.r = cmd_box.r + 1;
        str.c = cmd_box.c + 2;
        str.source = ">";
        terminal.horz_strdisp(str);

        str.r = cmd_box.r + 1;    
        str.c = cmd_box.c + 4;
        
        //if(strcmp(message_prev, message)){
            str.source = "                      ";
            terminal.horz_strdisp(str);
            str.source = message;
            terminal.horz_strdisp(str);
            move_cursor(&terminal, cmd_box.r + 1, cmd_box.c + 4 + strlen(message));
        //}

        //sleep(2);
        
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
            terminal.horz_tiledisp(fstr);        
        }

        terminal.present();
        input = get_input();
        usleep(10000);
    }   

    terminal.clear();
    terminal.present();
    terminal.io_block(1);
    terminal.cursor(1);
    move_cursor(&terminal, terminal.nrows, 0);

    char buff[4096];
    
}

int main(int argc, char **argv){
    boot_menu(argc,argv);
    return 0;
}
