#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define D    512 // Width & height (dimensions)
#define S      8 // scaled to
#define MSW 2048 // map scaled to
#define MSH  512 // map scaled to
#define JH    20 // jump
#define PS     5 // Player speed

typedef enum 
{
    RIGHT,
    LEFT
} dir;

typedef struct 
{
    int x;
    int y;
} coords;

typedef struct 
{
    int x;
    int y;
    dir dir;
} bullet;

typedef struct 
{
    ALLEGRO_BITMAP* right;
    ALLEGRO_BITMAP* left;
} Etexture;


typedef struct 
{
    coords cor; 
    dir dir;
    Etexture tex;
    int hp;
    bool dead;
    ALLEGRO_COLOR clr;
} entity;

typedef struct 
{
    coords cor;
    ALLEGRO_BITMAP* tex;

} level;

void drawEntity(entity e) {

    if(e.dir == RIGHT)
        al_draw_scaled_bitmap(e.tex.right, 0, 0, al_get_bitmap_width(e.tex.right), al_get_bitmap_height(e.tex.right), e.cor.x, e.cor.y, al_get_bitmap_width(e.tex.right)*S, al_get_bitmap_height(e.tex.right)*S, 0);
    if(e.dir == LEFT)
        al_draw_scaled_bitmap(e.tex.left, 0, 0, al_get_bitmap_width(e.tex.right), al_get_bitmap_height(e.tex.right), e.cor.x, e.cor.y, al_get_bitmap_width(e.tex.right)*S, al_get_bitmap_height(e.tex.right)*S, 0);


}

void drawEnemy(entity *e) {

    if(e->dir == RIGHT)
        al_draw_tinted_scaled_bitmap(e->tex.right, e->clr, 0, 0, al_get_bitmap_width(e->tex.right), al_get_bitmap_height(e->tex.right), e->cor.x, e->cor.y, al_get_bitmap_width(e->tex.right)*S, al_get_bitmap_height(e->tex.right)*S, 0);
    if(e->dir == LEFT)
        al_draw_tinted_scaled_bitmap(e->tex.left, e->clr, 0, 0, al_get_bitmap_width(e->tex.right), al_get_bitmap_height(e->tex.right), e->cor.x, e->cor.y, al_get_bitmap_width(e->tex.right)*S, al_get_bitmap_height(e->tex.right)*S, 0);


}

void drawLevel(level l){

    al_draw_scaled_bitmap(l.tex, 0, 0, al_get_bitmap_width(l.tex), al_get_bitmap_height(l.tex), l.cor.x, l.cor.y, MSW, MSH, 0);

}

void AIUpdate(entity p, entity *e){
    if(p.cor.x<=e->cor.x){
        e->dir=LEFT;
        e->cor.x-=2;
    }
    else{
        e->dir = RIGHT;
        e->cor.x+=2;
    }
}




int main() {

    srand(time(NULL));

    if(!al_init()) { printf("\033[1;31mINIT_ERR: Couldn't initialize allegro!\033[0m\n"); return -1; } else { printf("\033[32mINIT: Initialized allegro.\033[0m\n");}
    if(!al_install_keyboard()) { printf("\033[1;31mINIT_ERR: Couldn't install keyboard!\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized keyboard.\033[0m\n");}
    if(!al_install_audio()) { printf("\033[1;31mINIT_ERR: Couldn't install audio!\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized audio.\033[0m\n");}
    if(!al_init_image_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize image addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized image addon.\033[0m\n");}
    if(!al_init_primitives_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize primitives addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized primitives addon.\033[0m\n");}
    if(!al_init_ttf_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize ttf addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized ttf addon.\033[0m\n");}
    if(!al_init_acodec_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize acodec addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized acodec addon.\033[0m\n");}

    al_reserve_samples(3);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); if(!timer) { printf("\033[1;31mINIT_ERR: Couldn't create timer!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created timer.\033[0m\n");}
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); if(!queue) { printf("\033[1;31mINIT_ERR: Couldn't create event queue!\033[0mn\n"); return -1;} else{ printf("\33[;32mINIT: Created event queue.\033[0m\n");}
    ALLEGRO_DISPLAY* disp = al_create_display(D, D); if(!disp) { printf("\033[1;31mINIT_ERR: Couldn't create display!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created display.\033[0m\n");}
    ALLEGRO_FONT* font24 = al_load_ttf_font("font.ttf", 24, 0); if(!font24) { printf("\033[1;31mINIT_ERR: Couldn't create font!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created font.\033[0m\n");}
    ALLEGRO_FONT* font40 = al_load_ttf_font("font.ttf", 40, 0); if(!font40) { printf("\033[1;31mINIT_ERR: Couldn't create font!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created font.\033[0m\n");}
    ALLEGRO_SAMPLE* music = al_load_sample("music.ogg"); if(!music) { printf("\033[1;31mINIT_ERR: Couldn't create music!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created music.\033[0m\n");}
    ALLEGRO_SAMPLE_INSTANCE* musicInstance = al_create_sample_instance(music); if(!music) { printf("\033[1;31mINIT_ERR: Couldn't create music!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created music.\033[0m\n");}
    // ALLEGRO_BITMAP* tile = al_load_bitmap("tile.png"); if(!tile) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of tile!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of tile.\033[0m\n");}
    // ALLEGRO_BITMAP* player = al_load_bitmap("player.png"); if(!player) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of player.\033[0m\n");}
    // ALLEGRO_BITMAP* enemy = al_load_bitmap("enemy.png"); if(!enemy) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of enemy!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of enemy.\033[0m\n");}

    al_set_sample_instance_playmode(musicInstance, ALLEGRO_PLAYMODE_BIDIR);
    al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    bool in_title_screen = true;
    ALLEGRO_EVENT event;

    #define KEY_SEEN     1
    #define KEY_RELEASED 2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    entity player;
    player.tex.right = al_load_bitmap("player_right.png"); if(!player.tex.right) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
    player.tex.left = al_load_bitmap("player_left.png"); if(!player.tex.left) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
    player.dir = RIGHT;
    player.cor.x = 128;
    player.cor.y = 368;
    player.hp = 20;
    player.dead=false;

    level level;
    level.tex = al_load_bitmap("map.png"); if(!player.tex.right) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of level 1!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of level 1.\033[0m\n"); }
    level.cor.x = 0;
    level.cor.y = 0;

    bool inAir = false;
    bool jumping = false;
    bool newLevel = true;
    bool oldLevel = false;
    long score = 0;
    long levelIndex = 0;
    int decereaseBy = 1;
    int incereaseBy = JH;
    long index = 0;
    char sScore[20];
    char deadScore[20];
    char sHP[20];
    char deadTime[50];
    char deadLevel[50];
    char sLevel[20];
    bullet *bullet[10] = {0};
    entity *enemy[20] = {0};

    al_play_sample_instance(musicInstance);

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:

                //------------MOVEMENT------------
                if(key[ALLEGRO_KEY_UP] && !inAir) {
                    jumping = true;
                    inAir=true;
                }
                // if(key[ALLEGRO_KEY_RSHIFT])
                    
                // if(key[ALLEGRO_KEY_LEFT] && player.hp != 0){
                //     player.cor.x-=PS;
                //     player.dir = LEFT;
                // }
                // if(key[ALLEGRO_KEY_RIGHT] && player.hp != 0){
                //     player.cor.x+=PS;
                //     player.dir = RIGHT;
                // }

                bool sprinting = key[ALLEGRO_KEY_RSHIFT];

                if((key[ALLEGRO_KEY_LEFT] || key[ALLEGRO_KEY_RIGHT]) && player.hp != 0) {
                    int speed = sprinting ? PS * 2 : PS;
                    if(key[ALLEGRO_KEY_LEFT]) {
                        level.cor.x += speed;
                        player.dir = LEFT;
                    }
                    else {
                        level.cor.x -= speed;
                        player.dir = RIGHT;
                    }
                }
                
                // //------------MAP------------


                // if(player.cor.x>=D && level.cor.x==(-3*D)){
                //     level.cor.x=(-2*D);
                //     player.cor.x-=D; if(player.cor.x == 0) player.cor.x++;
                //     score++;
                //     levelIndex++;
                //     newLevel=true;
                //     printf("levelIndex: %d\n", levelIndex);
                // }

                // else if(player.cor.x>=D){
                //     level.cor.x-=D;
                //     player.cor.x-=D; if(player.cor.x == 0) player.cor.x++;
                //     score++;
                //     levelIndex++;
                //     newLevel=true;
                //     printf("levelIndex: %d\n", levelIndex);
                // }

                // if(player.cor.x<=0 && levelIndex==0){
                //     done = true;
                // }
                
                // else if(player.cor.x<=0 && level.cor.x==(-2*D) && levelIndex>3)
                // {
                //     player.cor.x+=D; if(player.cor.x == D) player.cor.x--;
                //     level.cor.x=(-3*D);
                //     score--;
                //     levelIndex--;
                //     oldLevel = true;
                // }

                // else if(player.cor.x<=0){
                //     level.cor.x+=D;
                //     player.cor.x+=D; if(player.cor.x == D) player.cor.x--;
                //     score--;
                //     levelIndex--;
                //     oldLevel = true;
                //     printf("levelIndex: %d\n", levelIndex);
                // }

                // sprintf(sLevel, "%ld", levelIndex);

                //------------JUMPING------------
                if(jumping) {

                    // printf("Player jumped\n");
                    player.cor.y-=incereaseBy;
                    incereaseBy--;
                    if(incereaseBy == 0) {
                        jumping = false;
                        incereaseBy = JH;
                    }

                }

                if(player.cor.y == 368){
                    decereaseBy = 0;
                    inAir = false;
                }
                player.cor.y+=decereaseBy;
                decereaseBy++;
            
                //------------SCORE------------

                sprintf(sScore, "Score: %ld", score);

                
                //------------ENEMYES------------
                for (int i = 0; i < (levelIndex<20 ? levelIndex : 20); i++)
                {
                    if (newLevel){
                        free(enemy[i]);
                        enemy[i]=NULL;

                        int r = rand() % 256;
                        int c = rand() % 155;

                        enemy[i] = calloc(sizeof(entity), 1);
                        enemy[i]->tex.right = al_load_bitmap("player_right.png");// if(!player.tex.right) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
                        enemy[i]->tex.left = al_load_bitmap("player_left.png");// if(!player.tex.left) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
                        enemy[i]->dir = RIGHT;
                        enemy[i]->cor.x = 256+r;
                        enemy[i]->cor.y = 368;
                        enemy[i]->hp = 5;
                        enemy[i]->dead = false;
                        enemy[i]->clr = al_map_rgb(c+100, 0, 0);
                        printf("Created enemy: %d\n", i);
                    }

                    if (enemy[i]!=NULL && !player.dead){
                        AIUpdate(player, enemy[i]);

                        if ((enemy[i]->cor.x<=player.cor.x || enemy[i]->cor.x<=player.cor.x+S) && (enemy[i]->cor.x+S>=player.cor.x || enemy[i]->cor.x+S>=player.cor.x+S) && enemy[i]->cor.y-95<=player.cor.y && (score<=1000? index%30==0:index%10))
                        {
                            player.hp--;
                        }

                        for (int j = 0; j < 10; j++)
                        {
                            
                            if (bullet[j]==NULL || enemy[i]==NULL){
                                break;
                            }
                            if(bullet[j]!= NULL && (enemy[i]->cor.x<=bullet[j]->x || enemy[i]->cor.x<=bullet[j]->x+8) && (enemy[i]->cor.x+S>=bullet[j]->x || enemy[i]->cor.x+S>=bullet[j]->x+8) && enemy[i]->cor.y<=bullet[j]->y){
                                free(bullet[j]);
                                bullet[j] = NULL;
                                free(enemy[i]);
                                printf("Destroyed enemy: %d\n", i);
                                enemy[i] = NULL;
                                score+=10;
                                break;
                            }
                            
                        }
                    }

                    
                }

                newLevel=false;
                
                //------------HP------------
                if(score >=1000 && score%100 == 0){
                    player.hp+=2;
                }
                if(player.hp <= 0){
                    player.dead=true;
                    player.hp=0;
                    sprintf(deadScore, "Your score: %ld.", score);
                    sprintf(deadTime, "You lasted %ld minutes.", (index/60)/60);
                    sprintf(deadLevel, "You lasted %ld levels.", levelIndex);
                }
                sprintf(sHP, "HP: %d", player.hp);

                // printf("\033[36mINFO: Player x is %i\033[0m\n", player.cor.x);
                // printf("\033[36mINFO: Player y is %i\033[0m\n", player.cor.y);

                // printf("\033[36mINFO: Level x is %i\033[0m\n", l1.cor.x);
                // printf("\033[36mINFO: Level y is %i\033[0m\n", l1.cor.y);

                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                if(event.keyboard.keycode == ALLEGRO_KEY_RCTRL && !player.dead){
                    for (int i = 0; i < 10; i++)
                    {
                        if(bullet[i] == NULL){
                            bullet[i] = calloc(sizeof(bullet), 1);

                            bullet[i]->x = player.cor.x+48;
                            bullet[i]->y = player.cor.y+48; // TODO SHOOTING
                            bullet[i]->dir = player.dir;
                            printf("Bullet %d created\n", i);
                            break;
                        }
                    }
                }
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            drawLevel(level);
            drawEntity(player);


            for (int i = 0; i < 10; i++)
            {
                if (bullet[i] != NULL)
                {
                    al_draw_filled_rectangle(bullet[i]->x, bullet[i]->y, bullet[i]->x+8, bullet[i]->y+8,  al_map_rgb(188, 231, 132));
                    // printf("Bullet %d drawn\n", i);
                    if(bullet[i]->dir == RIGHT){
                        bullet[i]->x+=10;
                    } 
                    else {
                        bullet[i]->x-=10;
                    }
                    
                    if(bullet[i]->x >= D || bullet[i]->x <= 0){
                        free(bullet[i]);
                        bullet[i] = NULL;
                        printf("Bullet %d destroyed when colliding with a wall\n", i);
                    }
                
                }
            
            }

            for (int i = 0; i < 10; i++)
            {
                if (oldLevel){
                    free(enemy[i]);
                    enemy[i]=NULL;
                    printf("destroyed enemy from last level\n");
                }
                if(enemy[i]!=NULL){
                    drawEnemy(enemy[i]);
                    // printf("Enemy %d drawn\n", i);
                }
            }
            oldLevel=false; 
            
            al_flip_display();

            redraw = false;
        }

        index++;
    }

    al_destroy_font(font24);
    al_destroy_font(font40);
    al_destroy_bitmap(player.tex.left);
    al_destroy_bitmap(player.tex.right);
    al_destroy_bitmap(level.tex);
    al_destroy_sample(music);
    al_destroy_sample_instance(musicInstance);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    

    return 0;
}