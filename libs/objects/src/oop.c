#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "objects.h"

typedef struct {
    uint8_t *base;
    uint8_t *current;
    size_t remaining;
} method_page_t;

static method_page_t current_method_page = {0};

static void new_method_page(void){
    void *page = mmap(
        NULL,
        METHOD_PAGE_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if(page == MAP_FAILED){
        perror("mmap");
        exit(1);
    }else{
        current_method_page.base = (uint8_t *)page;
        current_method_page.current = (uint8_t *)page;
        current_method_page.remaining = METHOD_PAGE_SIZE;
    }
}

static void seal_method_page(void){
    if(!current_method_page.base){
        return;
    }else{
        if( mprotect( current_method_page.base,
                METHOD_PAGE_SIZE, 
                PROT_READ | PROT_EXEC ) == -1){
            perror("mprotect");
            exit(1);
        }else{
            return;
        }
    }
}

void *method_alloc(size_t size){
    size = (size + 15) & ~15;

    if(size > METHOD_PAGE_SIZE){
        return NULL;
    }else{
        if(!current_method_page.current || current_method_page.remaining < size){
            seal_method_page();
            new_method_page();
        }

        void *result = current_method_page.current;

        current_method_page.current += size;
        current_method_page.remaining -= size;

        return result;
    }
}


typedef void (*method_t)(void);
method_t make_method(void *caller, void *function, void *wrapper, size_t size){

    unsigned char *code = method_alloc(size);
    memcpy(code, wrapper, size);
    uint64_t replacement = (uint64_t)caller;

    for(size_t i = 0; i <= size - sizeof(uint64_t); i++){
        uint64_t *p = (uint64_t *)(code + i);

        if(*p == magic_call_needle){
            *p = (uint64_t)replacement;
        }else if (*p == magic_func_needle)
            *p = (uint64_t)function;
        }

    return (method_t)code;
}

typedef void (*action_t)(void);
action_t make_action(void *caller, void *function, void *wrapper, size_t size){

    unsigned char *code = method_alloc(size);
    memcpy(code, wrapper, size);
    uint64_t replacement = (uint64_t)caller;

    for(size_t i = 0; i <= size - sizeof(uint64_t); i++){
        uint64_t *p = (uint64_t *)(code + i);

        if(*p == magic_call_needle){
            *p = (uint64_t)replacement;
        }else if (*p == magic_func_needle)
            *p = (uint64_t)function;
        }

    return (action_t)code;
}






