#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "dev.h"


#define LINE_SIZE 256
int parse_config(FILE *file, const char *app_name, launch_t *data){
    char line[LINE_SIZE];
    char section[LINE_SIZE];

    int found = 0;

    data->name = malloc(strlen(app_name)+1);
    strcpy(data->name,app_name);

    snprintf(section, sizeof(section), "<<%s>>", app_name);

    while(fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = 0;
        if(!strcmp(line, section)){
            found = 1;
            break;
        }
    }

    if(!found) return 1;


    while(fgets(line, sizeof(line), file)){
        line[strcspn(line, "\n")] = 0;

        // next section means we're done
        if (!strncmp(line, "<<", 2))
            break;

        char key[128];
        char value[128];

        if(sscanf(line, "%127[^=]=%127s", key, value)==2){
            if(!strcmp(key, "binary")){
                
                data->app_path = malloc(strlen(value) + 1);
                if (data->app_path) strcpy(data->app_path, value);

            }else if(!strcmp(key, "terminal")){
                
                data->app_term = malloc(strlen(value) + 1);
                if (data->app_term) strcpy(data->app_term, value);
            }
        }
    }

    return 0;
}