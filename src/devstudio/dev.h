#ifndef DEV_H
#define DEV_H

typedef struct launcher_data{
    char *app_path;
    char *app_term;
    char *name;
    char tmp_path[64];
    int running;
    int pid;
}launch_t;

typedef struct status_data{

}status_t;

int parse_config(FILE *file, const char *app_name, launch_t *data);

#endif