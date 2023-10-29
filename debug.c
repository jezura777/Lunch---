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

#define W    800 // Width & height (dimensions)
#define H    450 // Width & height (dimensions)
#define S    3.5 // scaled to
#define JH    20 // jump
#define PS     5 // Player speed


#define DEFAULT_PLAYER_Y (H/2)+10

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

    al_draw_scaled_bitmap(l.tex, 0, 0, al_get_bitmap_width(l.tex), al_get_bitmap_height(l.tex), l.cor.x, l.cor.y, al_get_bitmap_width(l.tex)*S, al_get_bitmap_height(l.tex)*S, 0);

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
    if(!al_install_mouse()) { printf("\033[1;31mINIT_ERR: Couldn't install mouse!\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized mouse.\033[0m\n");}
    if(!al_install_audio()) { printf("\033[1;31mINIT_ERR: Couldn't install audio!\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized audio.\033[0m\n");}
    if(!al_init_image_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize image addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized image addon.\033[0m\n");}
    if(!al_init_primitives_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize primitives addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized primitives addon.\033[0m\n");}
    if(!al_init_ttf_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize ttf addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized ttf addon.\033[0m\n");}
    if(!al_init_font_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize font addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized font addon.\033[0m\n");}
    if(!al_init_acodec_addon()) { printf("\033[1;31mINIT_ERR: Couldn't initilize acodec addon\033[0m\n"); return -1;} else { printf("\033[32mINIT: Initialized acodec addon.\033[0m\n");}

    al_reserve_samples(3);

    int range[] = {0x0020, 0x007F,  /* ASCII */};

    ALLEGRO_BITMAP* font_bitmap = al_load_bitmap("mecha.png");
    if (!font_bitmap) {
        printf("unable to find font texture!");
        return -1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); if(!timer) { printf("\033[1;31mINIT_ERR: Couldn't create timer!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created timer.\033[0m\n");}
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue(); if(!queue) { printf("\033[1;31mINIT_ERR: Couldn't create event queue!\033[0mn\n"); return -1;} else{ printf("\33[;32mINIT: Created event queue.\033[0m\n");}
    ALLEGRO_DISPLAY* disp = al_create_display(W, H); if(!disp) { printf("\033[1;31mINIT_ERR: Couldn't create display!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created display.\033[0m\n");}
    ALLEGRO_FONT* font = al_load_ttf_font("PixelifySans.ttf", 30, 0); if(!font) { printf("\033[1;31mINIT_ERR: Couldn't create font!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created font.\033[0m\n");}
    // ALLEGRO_FONT* font40 = al_load_ttf_font("font.ttf", 40, 0); if(!font40) { printf("\033[1;31mINIT_ERR: Couldn't create font!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created font.\033[0m\n");}
    ALLEGRO_SAMPLE* music = al_load_sample("music.ogg"); if(!music) { printf("\033[1;31mINIT_ERR: Couldn't create music!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created music.\033[0m\n");}
    ALLEGRO_SAMPLE_INSTANCE* musicInstance = al_create_sample_instance(music); if(!music) { printf("\033[1;31mINIT_ERR: Couldn't create music!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created music.\033[0m\n");}
    ALLEGRO_BITMAP* title = al_load_bitmap("Lunch!.png"); if(!title) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of title!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of title.\033[0m\n");}
    ALLEGRO_BITMAP* bullet_tex = al_load_bitmap("knife.png"); if(!bullet_tex) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of bullet!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of title.\033[0m\n");}
    // ALLEGRO_BITMAP* player = al_load_bitmap("player.png"); if(!player) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of player.\033[0m\n");}
    // ALLEGRO_BITMAP* enemy = al_load_bitmap("enemy.png"); if(!enemy) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of enemy!\033[0m\n"); return -1;} else{ printf("\33[;32mINIT: Created bitmap of enemy.\033[0m\n");}
    ALLEGRO_MOUSE_STATE state;

    al_set_sample_instance_playmode(musicInstance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
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
    player.cor.y = DEFAULT_PLAYER_Y;
    player.hp = 20;
    player.dead=false;

    level level1;
    level1.tex = al_load_bitmap("map.png"); if(!level1.tex) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of level 1!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of level 1.\033[0m\n"); }
    level1.cor.x = 0;
    level1.cor.y = 0;

    level level2;
    level2.tex = al_load_bitmap("map.png"); if(!level2.tex) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of level 1!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of level 1.\033[0m\n"); }
    level2.cor.x = 0;
    level2.cor.y = 0;

    level level3;
    level3.tex = al_load_bitmap("map.png"); if(!level3.tex) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of level 1!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of level 1.\033[0m\n"); }
    level3.cor.x = 0;
    level3.cor.y = 0;

    level level4;
    level4.tex = al_load_bitmap("map.png"); if(!level4.tex) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of level 1!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of level 1.\033[0m\n"); }
    level4.cor.x = 0;
    level4.cor.y = 0;

    level2.cor.x = level1.cor.x+al_get_bitmap_width(level1.tex);
    level3.cor.x = level2.cor.x+al_get_bitmap_width(level2.tex);
    level4.cor.x = level3.cor.x+al_get_bitmap_width(level3.tex);

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
    entity *enemy[50] = {0};

    al_play_sample_instance(musicInstance);

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {

            case ALLEGRO_EVENT_TIMER:
                //------------MOVEMENT------------
                if((key[ALLEGRO_KEY_W] || key[ALLEGRO_KEY_SPACE]) && !inAir) {
                    jumping = true;
                    inAir=true;
                }

                if(key[ALLEGRO_KEY_ENTER] && !player.dead) {
                    if (player.dead) {
                        in_title_screen = true;
                    } else {
                        in_title_screen = false;
                    }
                }

                if (in_title_screen){
                    goto just_title_screen;
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

                if((key[ALLEGRO_KEY_A] || key[ALLEGRO_KEY_D]) && !player.dead) {
                    int speed = sprinting ? PS * 2 : PS;
                    if(key[ALLEGRO_KEY_A]) {
                        level1.cor.x += speed;
                        player.dir = LEFT;
                    }
                    else {
                        level1.cor.x -= speed;
                        player.dir = RIGHT;
                    }
                }

                //*-----------LEVEL_TILING-------

                if ((level1.cor.x+al_get_bitmap_width(level1.tex)*S) <= 0){
                    level1.cor.x = 0;
                } else if (level1.cor.x >= 0) {
                    level1.cor.x = 0;
                }

                level2.cor.x = level1.cor.x+(al_get_bitmap_width(level1.tex)*S);
                level3.cor.x = level2.cor.x+(al_get_bitmap_width(level2.tex)*S);
                level4.cor.x = level3.cor.x+(al_get_bitmap_width(level3.tex)*S);


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

                if(player.cor.y == DEFAULT_PLAYER_Y){
                    decereaseBy = 0;
                    inAir = false;
                }
                player.cor.y+=decereaseBy;
                decereaseBy++;
            
                //------------SCORE------------

                sprintf(sScore, "Score: %ld", score);

                
                //------------ENEMYES------------
                for (int i = 0; i < 50; i++)
                {
                    if(enemy[i] == NULL && (index%((60+(rand()%100)*60))) == 0 && index !=0) {
                        int r = rand() % 50;
                        int j = rand() % 10;
                        int c = rand() % 155;

                        enemy[i] = calloc(sizeof(entity), 1);
                        enemy[i]->tex.right = al_load_bitmap("enemy1.png");// if(!player.tex.right) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
                        enemy[i]->tex.left = al_load_bitmap("enemy2.png");// if(!player.tex.left) { printf("\033[1;31mINIT_ERR: Couldn't create bitmap of player!\033[0m\n"); return -1;} else{ printf("\33[32mINIT: Created bitmap of player.\033[0m\n");}
                        enemy[i]->dir = (j == 5)? LEFT : RIGHT;
                        enemy[i]->cor.x = W+r;
                        enemy[i]->cor.y = DEFAULT_PLAYER_Y;
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
                
                //------------HP------------
                if(score >=1000 && score%100 == 0){
                    player.hp+=2;
                }
                if(player.hp <= 0){
                    player.dead=false;
                    player.hp=20;
                    in_title_screen = true;
                    for (int i = 0; i < 50; i++)
                    {
                        free(enemy[i]);
                        enemy[i] == NULL;
                    }
                }


                // printf("\033[36mINFO: Player x is %i\033[0m\n", player.cor.x);
                // printf("\033[36mINFO: Player y is %i\033[0m\n", player.cor.y);

                // printf("\033[36mINFO: Level x is %i\033[0m\n", l1.cor.x);
                // printf("\033[36mINFO: Level y is %i\033[0m\n", l1.cor.y);
just_title_screen:
                if(key[ALLEGRO_KEY_ESCAPE])
                    done = true;

                for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
                    key[i] &= KEY_SEEN;

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                key[event.keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
                break;
            case ALLEGRO_EVENT_KEY_UP:
                key[event.keyboard.keycode] &= KEY_RELEASED;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (event.mouse.button == 1)
                {
                    if(!player.dead){
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
                }
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            if (!in_title_screen) {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                drawLevel(level1);
                drawLevel(level2);
                drawLevel(level3);
                drawLevel(level4);
                drawEntity(player);


                for (int i = 0; i < 10; i++)
                {
                    if (bullet[i] != NULL)
                    {
                        al_draw_scaled_bitmap(bullet_tex, 0, 0, al_get_bitmap_width(bullet_tex), al_get_bitmap_height(bullet_tex), bullet[i]->x, bullet[i]->y,  al_get_bitmap_width(bullet_tex)*S, al_get_bitmap_height(bullet_tex)*S, 0);
                        // printf("Bullet %d drawn\n", i);
                        if(bullet[i]->dir == RIGHT){
                            bullet[i]->x+=10;
                        }
                        else {
                            bullet[i]->x-=10;
                        }
                        
                        if(bullet[i]->x >= W || bullet[i]->x <= 0){
                            free(bullet[i]);
                            bullet[i] = NULL;
                            printf("Bullet %d destroyed when colliding with a wall\n", i);
                        }
                    
                    }
                
                }
                oldLevel=false;

                al_draw_textf(font, (player.hp<5)? al_map_rgba(255, 0, 0, 255) : al_map_rgba(255, 255, 255, 255), W/2, 10, ALLEGRO_ALIGN_CENTRE, "HP:  %3d", player.hp);

                if (player.dead)
                {
                    al_draw_filled_rectangle(0,0, W, H, al_map_rgba(255, 161, 0, 100));
                    al_draw_text(font, al_map_rgb(0, 0, 0), 256, 200, ALLEGRO_ALIGN_CENTRE, "You have died!");
                    al_draw_text(font, al_map_rgb(0, 0, 0), 256, 256, ALLEGRO_ALIGN_CENTRE, deadScore);
                    al_draw_text(font, al_map_rgb(0, 0, 0), 256, 285, ALLEGRO_ALIGN_CENTRE, deadTime);
                    al_draw_text(font, al_map_rgb(0, 0, 0), 256, 314, ALLEGRO_ALIGN_CENTRE, deadLevel);
                }

            } else {
                al_clear_to_color(al_map_rgba(255, 161, 0, 100));
                al_draw_scaled_bitmap(title,
                                      0, 0, al_get_bitmap_width(title), al_get_bitmap_height(title),
                                      W/5, H/5, al_get_bitmap_width(title)*S, al_get_bitmap_height(title)*S, 0);
                al_draw_text(font, al_map_rgba(0, 0, 0, 255), W/2, H/2+H/3, ALLEGRO_ALIGN_CENTRE, "Press ENTER to start.");
                
                
            }
            al_flip_display();

            redraw = false;
        }

        index++;
    }

    al_destroy_font(font);
    // al_destroy_font(font40);
    al_destroy_bitmap(player.tex.left);
    al_destroy_bitmap(player.tex.right);
    al_destroy_bitmap(level1.tex);
    al_destroy_bitmap(level2.tex);
    al_destroy_bitmap(level3.tex);
    al_destroy_bitmap(level4.tex);
    al_destroy_sample(music);
    al_destroy_sample_instance(musicInstance);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    

    return 0;
}