#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define W    512 // Width (dimensions)
#define H    512 // Height (dimensions)
#define S    0.5 // scaled to
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
        al_draw_scaled_bitmap(e.tex.right, 0, 0, al_get_bitmap_width(e.tex.right), al_get_bitmap_height(e.tex.right), e.cor.x, e.cor.y, S, S, 0);
    if(e.dir == LEFT)
        al_draw_scaled_bitmap(e.tex.left, 0, 0, al_get_bitmap_width(e.tex.right), al_get_bitmap_height(e.tex.right), e.cor.x, e.cor.y, S, S, 0);


}

void drawEnemy(entity *e) {

    if(e->dir == RIGHT)
        al_draw_tinted_scaled_bitmap(e->tex.right, e->clr, 0, 0, al_get_bitmap_width(e->tex.right), al_get_bitmap_height(e->tex.right), e->cor.x, e->cor.y, S, S, 0);
    if(e->dir == LEFT)
        al_draw_tinted_scaled_bitmap(e->tex.left, e->clr, 0, 0, al_get_bitmap_width(e->tex.right), al_get_bitmap_height(e->tex.right), e->cor.x, e->cor.y, S, S, 0);


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

    // srand(time(NULL));

    if(!al_init()) { printf("INIT_ERR: Couldn't initialize allegro!\n"); return -1; } else { printf("INIT: Initialized allegro.\n");}
    if(!al_install_keyboard()) { printf("INIT_ERR: Couldn't install keyboard!\n"); return -1;} else { printf("INIT: Initialized keyboard.\n");}
    if(!al_install_audio()) { printf("INIT_ERR: Couldn't install audio!\n"); return -1;} else { printf("INIT: Initialized audio.\n");}
    if(!al_init_image_addon()) { printf("INIT_ERR: Couldn't initilize image addon\n"); return -1;} else { printf("INIT: Initialized image addon.\n");}
    if(!al_init_primitives_addon()) { printf("INIT_ERR: Couldn't initilize primitives addon\n"); return -1;} else { printf("INIT: Initialized primitives addon.\n");}
    if(!al_init_ttf_addon()) { printf("INIT_ERR: Couldn't initilize ttf addon\n"); return -1;} else { printf("INIT: Initialized ttf addon.\n");}
    if(!al_init_acodec_addon()) { printf("INIT_ERR: Couldn't initilize acodec addon\n"); return -1;} else { printf("INIT: Initialized acodec addon.\n");}

    al_reserve_samples(3);
   // al_set_new_display_flags(ALLEGRO_DISPLAY_ORIENTATION_90_DEGREES);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); if(!timer) { printf("INIT_ERR: Couldn't create timer!\n"); return -1;} else{ printf("INIT: Created timer.\n");}
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); if(!queue) { printf("INIT_ERR: Couldn't create event queue!\n"); return -1;} else{ printf("INIT: Created event queue.\n");}
    ALLEGRO_DISPLAY* disp = al_create_display(W, H); if(!disp) { printf("INIT_ERR: Couldn't create display!\n"); return -1;} else{ printf("INIT: Created display.\n");}
    ALLEGRO_FONT* font24 = al_load_ttf_font("font.ttf", 24, 0); if(!font24) { printf("INIT_ERR: Couldn't create font!\n"); return -1;} else{ printf("INIT: Created font.\n");}
    ALLEGRO_FONT* font40 = al_load_ttf_font("font.ttf", 40, 0); if(!font40) { printf("INIT_ERR: Couldn't create font!\n"); return -1;} else{ printf("INIT: Created font.\n");}
    ALLEGRO_SAMPLE* music = al_load_sample("music.ogg"); if(!music) { printf("INIT_ERR: Couldn't create music!\n"); return -1;} else{ printf("INIT: Created music.\n");}
    ALLEGRO_SAMPLE_INSTANCE* musicInstance = al_create_sample_instance(music); if(!music) { printf("INIT_ERR: Couldn't create music!\n"); return -1;} else{ printf("INIT: Created music.\n");}

    al_set_sample_instance_playmode(musicInstance, ALLEGRO_PLAYMODE_BIDIR);
    al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    #define KEY_SEEN     1
    #define KEY_RELEASED 2

    unsigned char key[ALLEGRO_KEY_MAX];
    memset(key, 0, sizeof(key));

    entity player;
    player.tex.right = al_load_bitmap("player_right.png"); if(!player.tex.right) { printf("INIT_ERR: Couldn't create bitmap of player!\n"); return -1;} else{ printf("INIT: Created bitmap of player.\n");}
    player.tex.left = al_load_bitmap("player_left.png"); if(!player.tex.left) { printf("INIT_ERR: Couldn't create bitmap of player!\n"); return -1;} else{ printf("INIT: Created bitmap of player.\n");}
    player.dir = RIGHT;
    player.cor.x = 128;
    player.cor.y = 368;
    player.hp = 20;
    player.dead=false;

    level level;
    level.tex = al_load_bitmap("map.png"); if(!player.tex.right) { printf("INIT_ERR: Couldn't create bitmap of map!\n"); return -1;} else{ printf("INIT: Created bitmap of map.\n");}
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

                bool sprinting = key[ALLEGRO_KEY_RSHIFT];

                if((key[ALLEGRO_KEY_LEFT] || key[ALLEGRO_KEY_RIGHT]) && player.hp != 0) {
                    int speed = sprinting ? PS * 2 : PS;
                    if(key[ALLEGRO_KEY_LEFT]) {
                        player.cor.x -= speed;
                        player.dir = LEFT;
                    }
                    else {
                        player.cor.x += speed;
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
                // }

                // else if(player.cor.x>=D){
                //     level.cor.x-=D;
                //     player.cor.x-=D; if(player.cor.x == 0) player.cor.x++;
                //     score++;
                //     levelIndex++;
                //     newLevel=true;
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
                // }

                // sprintf(sLevel, "%ld", levelIndex);

                //------------JUMPING------------
                if(jumping) {

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

                        // int r = rand() % 256;
                        // int c = rand() % 155;

                        enemy[i] = calloc(sizeof(entity), 1);
                        enemy[i]->tex.right = al_load_bitmap("player_right.png");
                        enemy[i]->tex.left = al_load_bitmap("player_left.png");
                        enemy[i]->dir = RIGHT;
                        enemy[i]->cor.x = 256+r;
                        enemy[i]->cor.y = 368;
                        enemy[i]->hp = 5;
                        enemy[i]->dead = false;
                        enemy[i]->clr = al_map_rgb(c+100, 0, 0);
                    }

                    if (enemy[i]!=NULL && !player.dead){
                        AIUpdate(player, enemy[i]);

                        if ((enemy[i]->cor.x<=player.cor.x || enemy[i]->cor.x<=player.cor.x+S) && (enemy[i]->cor.x+S>=player.cor.x || enemy[i]->cor.x+S>=player.cor.x+S) && enemy[i]->cor.y-95<=player.cor.y && (score<=1000? index%30==0:index%15))
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
                                enemy[i] = NULL;
                                score+=10;
                                break;
                            }
                            
                        }
                    }

                    
                }

                newLevel=false;
                
                //------------HP------------
                if(score>=1000 && score%500 == 0){
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
                            bullet[i]->y = player.cor.y+48;
                            bullet[i]->dir = player.dir;
                            break;
                        }
                    }
                }
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;
                
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(dpy);
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
            if(levelIndex!=0){
                al_draw_filled_rectangle(0, 0, W, 35, al_map_rgba(81, 82, 116, 225));
                al_draw_text(font24, al_map_rgb(188, 231, 132), 500, 0, ALLEGRO_ALIGN_RIGHT, sScore);
                al_draw_text(font24, al_map_rgb(188, 231, 132), 244, 0, ALLEGRO_ALIGN_CENTER, sLevel);
                al_draw_text(font24, al_map_rgb(188, 231, 132), 12, 0, ALLEGRO_ALIGN_LEFT, sHP);
                // al_draw_pixel(100 , 100, al_map_rgb(188, 231, 132));
            }

            for (int i = 0; i < 10; i++)
            {
                if (bullet[i] != NULL)
                {
                    al_draw_filled_rectangle(bullet[i]->x, bullet[i]->y, bullet[i]->x+8, bullet[i]->y+8,  al_map_rgb(188, 231, 132));
                    if(bullet[i]->dir == RIGHT){
                        bullet[i]->x+=10;
                    } 
                    else {
                        bullet[i]->x-=10;
                    }
                    
                    if(bullet[i]->x >= W || bullet[i]->x <= 0){
                        free(bullet[i]);
                        bullet[i] = NULL;
                    }
                
                }
            
            }

            for (int i = 0; i < 10; i++)
            {
                if (oldLevel){
                    free(enemy[i]);
                    enemy[i]=NULL;
                }
                if(enemy[i]!=NULL){
                    drawEnemy(enemy[i]);
                }
            }
            oldLevel=false;
            if (player.dead)
            {
                al_draw_filled_rectangle(0,0,512,512, al_map_rgba(81, 82, 116, 225));
                al_draw_text(font40, al_map_rgb(188, 231, 132), 256, 200, ALLEGRO_ALIGN_CENTRE, "You have died!");
                al_draw_text(font24, al_map_rgb(188, 231, 132), 256, 256, ALLEGRO_ALIGN_CENTRE, deadScore);
                al_draw_text(font24, al_map_rgb(188, 231, 132), 256, 285, ALLEGRO_ALIGN_CENTRE, deadTime);
                al_draw_text(font24, al_map_rgb(188, 231, 132), 256, 314, ALLEGRO_ALIGN_CENTRE, deadLevel);
            }
            
            
            
            
            
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
