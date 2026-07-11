Run bind_method(self *, func) or bind_action the same way to get the method
to generate. You will need to create a constructor. Then you can use def_method / def_action above a function to define it. You also need to 
include the arg type in the the method. for multiple args, pass a struct

demo

```
typedef struct dog_data{
    char name[16];
    int sitting;

    void (*sit)(void);
    void (*stand)(void);
    void (*stats)(void);
    void (*rename)(char *);
} dog_t;

void sit  (dog_t *self);
void stand(dog_t *self);
void stats(dog_t *self);
void set_name(dog_t *self, char *name);

def_action(sit)
void sit(dog_t *self){
    self->sitting = 1;
}

def_action(stand)
void stand(dog_t *self){
    self->sitting = 0;
}

def_action(stats)
void stats(dog_t *self){
    puts("-dog status-");
    printf("name:\t%s\n",self->name);
    printf("state:\t%s\n\n",self->sitting?"sitting":"standing");
}

def_method(set_name, char *)
void set_name(dog_t *self, char *name){
    strcpy(self->name, name);
}


void construct_dog(dog_t *dog){
    dog->sit   = bind_action(dog, sit);
    dog->stand = bind_action(dog, stand);
    dog->stats = bind_action(dog, stats);
    dog->rename = bind_method(dog, set_name);
}

void main(void){

    dog_t dog1 = {0};
    construct_dog(&dog1);

    dog_t dog2 = {0};
    construct_dog(&dog2);

    dog1.stats();
    dog2.stats();

    dog1.rename("rover");
    dog2.rename("fido");

    dog1.stats();
    dog2.stats();
    
    dog1.sit();

    dog1.stats();
    dog2.stats();
}

```