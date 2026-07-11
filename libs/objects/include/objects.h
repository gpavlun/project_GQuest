#define magic_call_needle 0x1234567812345678ULL
#define magic_func_needle 0x8765432187654321ULL
#define METHOD_PAGE_SIZE 4096

#define def_action(name)\
__attribute__((noinline,used))                                \
void GGG_##name##_method_wrapper(void)               \
{                                                             \
    void *caller = (void *)magic_call_needle;                 \
    void (*fn)(void *) =                              \
                (void (*)(void *))magic_func_needle;  \
    fn(caller);                                   \
}                                                             \
__asm__(                                                      \
    ".global GGG_" #name "_method_wrapper_end\n"              \
    "GGG_" #name "_method_wrapper_end:\n"                     \
);\
extern unsigned char GGG_##name##_method_wrapper_end[];


#define def_method(name, argtype)\
__attribute__((noinline,used))                                \
void GGG_##name##_method_wrapper(argtype arg)               \
{                                                             \
    void *caller = (void *)magic_call_needle;                 \
    void (*fn)(void *, argtype) =                              \
                (void (*)(void *, argtype))magic_func_needle;  \
    fn(caller, arg);                                   \
}                                                             \
__asm__(                                                      \
    ".global GGG_" #name "_method_wrapper_end\n"              \
    "GGG_" #name "_method_wrapper_end:\n"                     \
);\
extern unsigned char GGG_##name##_method_wrapper_end[];



#define bind_method(caller, name)                             \
make_method(                                                   \
    caller,                                                    \
    name,                                                      \
    GGG_##name##_method_wrapper,                               \
    ((size_t)((unsigned char *)GGG_##name##_method_wrapper_end -\
              (unsigned char *)GGG_##name##_method_wrapper))   \
)
#define bind_action(caller, name)                             \
make_action(                                                   \
    caller,                                                    \
    name,                                                      \
    GGG_##name##_method_wrapper,                               \
    ((size_t)((unsigned char *)GGG_##name##_method_wrapper_end -\
              (unsigned char *)GGG_##name##_method_wrapper))   \
)





typedef void (*action_t)(void);
action_t make_action(void *caller, void *function, void *wrapper, size_t size);

typedef void (*method_t)(void);
method_t make_method(void *caller, void *function, void *wrapper, size_t size);