#include "src/input/Joystick_esp.h"
#include "src/input/Button.h"
#include "src/output/Matrix_map.h"
#include "src/output/Player_tft.h"
#include "src/output/Options.h"
#include "src/game-functions/Door.h"
#include "src/game-functions/Maze-generator.h"
#include "src/game-functions/Camera.h"
#include "src/data-sturctures/LinkedList.h"
#include "src/data-sturctures/Point.h"
#include "src/data-sturctures/Point_extended.h"
#include "src/data-sturctures/hsv_rgb.h"
#include "src/on-run-game-functions/aesthetic.h"
#include "src/on-run-game-functions/gen_depth.h"
#include "src/on-run-game-functions/closest_to_div.h"

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

#define BUTTON_PIN 12
#define ANALOG_X 33
#define ANALOG_Y 27
#define NUMBER_OF_ROWS_MAP 32
#define NUMBER_OF_COL_MAP 24


uint16_t number_of_cols = NUMBER_OF_COL_MAP*2;
uint16_t number_of_rows = NUMBER_OF_ROWS_MAP*2;

TFT_eSPI tft = TFT_eSPI();

Joystick joystick(ANALOG_X, ANALOG_Y);
Button button_joystick(BUTTON_PIN);

Matrix_map world_map(&tft);

HSV color[]={ // 0 -> wall, 1-> door color, 3 -> node color (wall)
    {190.0,100.0,100.0},
    {40.0,100.0,100.0},
    {190.0,100.0,100.0}
};

Door door(&tft, &world_map);

Point collision_point;
Point starting_point (0,0);

Maze_generator maze_gen;

Camera player_vision(&tft);

bool timer_started = false;
double map_time=0;
double action_time=0;
uint8_t button_pressed=0;
uint8_t completed_game_with_high_score=0;
Wall_dir door_dir;

bool finished = true;

HSV text_color = {100, 100, 100};

Player_tft player(&tft,&world_map);

Dir current_dir=RIGHT;
Dir option_dir;

Options options(&tft);

int8_t current_layer = 0;
int8_t current_option = 0;
int8_t num_of_gen = -1;
uint16_t val_to_use;

int8_t prev_pick_opt_2 = 0;
int8_t prev_pick_opt_3 = 1;

bool default_spawn = true;
bool center_spawn = false;
bool default_num_gen = true;

void do_nothing()
{
    return;
}

void show_vision()
{
    button_pressed++;
    player_vision.draw_vision(player.get_current_player_position());
}

bool set_starting_position(uint16_t y, uint8_t x_margin)
{
    uint16_t width_of_segment = tft.width()/(number_of_cols);
    uint16_t height_of_segment = tft.height()/(number_of_rows);

    for (uint16_t x = x_margin - 2;x<x_margin +3;x++)
    {
        if (maze_gen.get_maze()[y*number_of_cols+x]==0)
        {
            starting_point.x=x;
            starting_point.y=y;
            //tft.fillRect(x*width_of_segment, y*height_of_segment, width_of_segment, height_of_segment, TFT_WHITE);
            return true;
        }
    }
    return false;
}

void set_opposite_spawn_point()
{
    uint16_t x_margin = number_of_cols/2;
    uint16_t y_margin = number_of_rows/2;

    switch (door_dir)
        {
            case TOP_L:
                x_margin = 3;  // DOWN_R
                y_margin = number_of_rows - 4;
                break;
            case TOP_R:
                x_margin = 3; // DOWN_L
                y_margin = 3;
                break;
            case DOWN_L:
                x_margin = number_of_cols - 4; // TOP_R
                y_margin = number_of_rows - 4;
                break;
            case DOWN_R:
                x_margin = number_of_cols - 4; // TOP_L
                y_margin = 3;
        }

    for (uint16_t y=y_margin-2;y<y_margin+3;y++)
    {
        if (set_starting_position(y, x_margin))
        {
            return;
        }
    }
}

void set_center_spawn_point()
{
    for (uint8_t y=number_of_rows/2-2;y<number_of_rows/2+3;y++)
    {
        if (set_starting_position(y, number_of_cols/2))
        {
            return;
        }
    }
}

void generate_maze()
{
    val_to_use = number_of_cols > number_of_rows? number_of_cols : number_of_rows;

    num_of_gen = default_num_gen ? calculate_gen_depth(val_to_use) : num_of_gen;

    val_to_use = number_of_cols < number_of_rows ? number_of_cols : number_of_rows;
    val_to_use = random(val_to_use/8,val_to_use/4) + 2;
    maze_gen.create_generators(val_to_use,number_of_cols,number_of_rows);
    maze_gen.generate_maze(num_of_gen,5,13);
    maze_gen.delete_nodes();
}

void load_player_vision()
{
    uint8_t cols_div[14] = {1,2,3,4,5,6,8,10,12,15,16,20,24,30}; // cols 240
    uint8_t rows_div[10] = {1,2,4,5,8,10,16,20,32,40}; // rows 320

    uint8_t idx_240, idx_320, col, rows, ray_lenght;
    float ray_angle = 0.0175; // ~ 1 deg.
    rows = closest_to_divider(number_of_rows/2, rows_div, 10, &idx_320);
    col = closest_to_divider(number_of_cols/2, cols_div, 14, &idx_240);
   
    if (col < rows)
    {
        ray_lenght = col/2;
    }
    else{
        ray_lenght = rows/2;
    }

    player_vision.load_map(maze_gen.get_maze(),number_of_cols, number_of_rows, col , rows ,convert_to_RGB(2, 96, 173));

    if (ray_lenght < 4)
    {
        ray_angle = 0.28; // ~ 16 deg.
    }
    else if (ray_lenght < 5)
    {
        ray_angle = 0.14; // ~ 8 deg.
    }
    else if (ray_lenght < 7)
    {
        ray_angle = 0.07; // ~ 4 deg.
    }
    else if (ray_lenght < 13)
    {
        ray_angle = 0.035; // ~ 2 deg.
    }

    //Serial.println("RAY_L: "+String(ray_lenght));

    player_vision.load_ray_casting(ray_angle, 6.28, ray_lenght, color);
}

void draw_player_on_map()
{
    uint8_t w = tft.width()/(number_of_cols);
    uint8_t h = tft.height()/(number_of_rows);
    tft.fillRect(starting_point.x * w, starting_point.y * h, w, h, TFT_WHITE);
}

void reset()
{
    button_joystick.on_press(show_vision);

    tft.fillScreen(TFT_BLACK);

    maze_gen.delete_map();
    generate_maze();

    world_map.set_map(maze_gen.get_maze(),number_of_rows,number_of_cols, 0, 128, 153);

    door.clear_map();
    door_dir = door.generate_door(9);

    load_player_vision();

    if (center_spawn || (default_spawn && !(completed_game_with_high_score > 1)))
    {
        set_center_spawn_point();
    }
    else{
        set_opposite_spawn_point();
    }

    player.set_player_posistion(starting_point);

    world_map.draw_map();
    draw_player_on_map();

    map_time = millis();
    button_pressed=0;
    finished = false;
    timer_started = false;
}

void down()
{
    move(DOWN);
}
void up()
{
    move(UP);
}
void left()
{
    move(LEFT);
}
void right()
{
    move(RIGHT);
}

void move(uint8_t dir)
{
    if(!timer_started)
    {
        map_time = millis() - map_time;
        timer_started = true;
        action_time = millis();
    }
    tft.fillScreen(TFT_BLACK);
    collision_point = player.move(dir);
    if (collision_point.x != 0)
    {
        if (door.check_collision_with_player(collision_point))
        {
            tft.fillScreen(TFT_BLACK);
            action_time = millis() - action_time;
            button_joystick.on_press(reset);
            finished = true;
            ending_scene(text_color, map_time, action_time, button_pressed, center_spawn, default_spawn, completed_game_with_high_score, number_of_cols, number_of_rows, tft);
            return;
        }
    }
    player_vision.draw_vision_with_ray_cast(0, player.get_current_player_position());
    player_vision.clear_prev_player_position(player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), 0);
}

void start()
{
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0);
    tft.setRotation(0);
    tft.setTextSize(1);

    map_time = millis();

    joystick.on_dir_down(down);
    joystick.on_dir_left(left);
    joystick.on_dir_right(right);
    joystick.on_dir_up(up);
    joystick.set_interval(100);

    generate_maze();

    world_map.set_map(maze_gen.get_maze(),number_of_rows,number_of_cols, 0, 128, 153);

    door_dir = door.generate_door(9);

    if (center_spawn || default_spawn)
    {
        set_center_spawn_point();
    }
    else{
        set_opposite_spawn_point();
    }

    load_player_vision();

    player._init_();

    world_map.draw_map();

    button_joystick.on_press(show_vision);
    finished=false;

    player.set_player_posistion(starting_point);

    draw_player_on_map();

    player_vision.load_player(convert_to_RGB(176, 168, 111),convert_to_RGB(99, 5, 14), 0, player.get_current_player_position());
}

void increment_slider()
{
    if (current_layer == 5 && current_option == 0 && default_num_gen)
    {
        default_num_gen = false;
        options.set_mark(false, 0, 5);
    }
    tft.fillScreen(TFT_BLACK);
    options.increment_slider(current_layer, current_option);
    options.draw(current_layer, current_option);
}

void decrement_slider()
{
    if (current_layer == 5 && current_option == 0 && default_num_gen)
    {
        default_num_gen = false;
        options.set_mark(false, 0, 5);
    }
    tft.fillScreen(TFT_BLACK);
    options.decrement_slider(current_layer, current_option);
    options.draw(current_layer, current_option);
}

void move_opt()
{
    uint8_t num_of_options = options.get_num_of_options_in_layer(current_layer);
    //Serial.println("NUM OF OPTIONS = "+String(num_of_options)+" CURRENT_OPT: "+String(current_option) + "  CURRENT LAYER: "+String(current_layer));
    if (current_layer == 0)
    {
        print_game_title(tft);
    }
    switch(option_dir)
    {
        case UP:
            current_option = current_option > 0 ? current_option-1 : num_of_options-1; 
            break;
        case DOWN:
            current_option = current_option == num_of_options-1 ? 0 : current_option+1;
            break;
    }
    options.draw(current_layer, current_option);
}

void pick_option()
{
    tft.fillScreen(TFT_BLACK);
    if (current_layer == 0)
    {
        switch(current_option)
        {
            case 0:
                start();
                return;
            case 1:
                current_layer++;
                current_option=0;
                break;
        }
    }

    else if(current_layer == 1)
    {
        switch(current_option)
        {
            case 0:
                current_layer++;
                break;
            case 1:
                current_layer=3;
                current_option=0;
                break;
            case 2:
                current_option = 0;
                current_layer = 5;

                joystick.on_dir_left(decrement_slider);
                joystick.on_dir_right(increment_slider);
                joystick.set_interval(100);

                val_to_use = number_of_cols < number_of_rows ? number_of_cols : number_of_rows;

                if (default_num_gen)
                {
                    options.set_value(5,0, calculate_gen_depth(val_to_use));
                }
                break;
            case 3:
                current_layer--;
                current_option=0;
        }
    }

    else if(current_layer == 2)
    {
        switch(current_option)
        {
            case 0:
                default_spawn = true;
                center_spawn = false;
                options.set_mark(false,prev_pick_opt_2, 2);
                options.set_mark(true,current_option, 2);

                prev_pick_opt_2 = current_option; // 0
                break;
            case 1:
                default_spawn = false;
                center_spawn = true;

                options.set_mark(false,prev_pick_opt_2, 2);
                options.set_mark(true,current_option, 2);

                prev_pick_opt_2 = current_option; // 1
                break;
            case 2:
                default_spawn = false;
                center_spawn = false;

                options.set_mark(false,prev_pick_opt_2, 2);
                options.set_mark(true,current_option, 2);

                prev_pick_opt_2 = current_option;
                break;
            case 3:
                current_layer--;
                current_option=0;
        }
    }

    else if (current_layer == 3)
    {
        switch (current_option)
        {
            case 0:
                number_of_cols = 120;
                number_of_rows = 160;

                options.set_mark(false,prev_pick_opt_3, 3);
                options.set_mark(true,current_option, 3);

                prev_pick_opt_3 = current_option;
                break;
            case 1:
                number_of_cols = 48;
                number_of_rows = 64;
                options.set_mark(false,prev_pick_opt_3, 3);
                options.set_mark(true,current_option, 3);

                prev_pick_opt_3 = current_option;
                break;
            case 2:
                number_of_cols = 24;
                number_of_rows = 32;

                options.set_mark(false,prev_pick_opt_3, 3);
                options.set_mark(true,current_option, 3);

                prev_pick_opt_3 = current_option;
                break;
            case 3:
                current_layer++;
                current_option=0;
                        
                joystick.on_dir_left(decrement_slider);
                joystick.on_dir_right(increment_slider);
                joystick.set_interval(100);

                options.set_value(4,0, number_of_rows);
                options.set_value(4,1, number_of_cols);
                break;

            case 4:
                current_layer = 1;
                current_option=0;
        }
    }

    else if(current_layer==4)
    {
        switch(current_option)
        {
            case 0: // rows
                break;
            case 1: // cols
                break;
            case 2:
                joystick.on_dir_left(do_nothing);
                joystick.on_dir_right(do_nothing);
                joystick.set_interval(200);
                for (uint8_t i=0;i<4;i++)
                {
                    options.set_mark(false, i, 3);
                }
                
                number_of_rows = options.get_value(4,0);
                number_of_cols = options.get_value(4,1);

                current_layer--;
                current_option=0;
        }
    }

    else{
        switch(current_option)
        {
            case 0: // gen_depth 
                break;
            case 1:
                if (!default_num_gen)
                {
                    default_num_gen = true;
                    options.set_value(5,0,calculate_gen_depth(val_to_use));
                    options.set_mark(true, 0,5);
                }
                break;
            case 2:
                current_layer = 1;
                current_option=0;

                joystick.on_dir_left(do_nothing);
                joystick.on_dir_right(do_nothing);
                joystick.set_interval(200);

                num_of_gen = options.get_value(5,0);
        }
    }


    if (current_layer == 0)
    {
        print_game_title(tft);
    }

    options.draw(current_layer,current_option);
}

void move_up_opt()
{
    option_dir = UP;
    move_opt();
}

void move_down_opt()
{
    option_dir = DOWN;
    move_opt();
}

void setup()
{
    button_joystick._init_();
    button_joystick.on_press(pick_option);

    joystick._init_();
    joystick.on_dir_up(move_up_opt);
    joystick.on_dir_down(move_down_opt);
    joystick.set_interval(200);

    Serial.begin(921600);
    tft.init();

    maze_gen._init_();

    options._init_(18,3,6);

    options.create_option(0,0,"START",3,false);
    options.create_option(0,1, "SETTINGS",3,false);

    options.create_option(1,0, "SPAWN POINT", 3, false);
    options.create_option(1,1, "MAZE SIZE",3,false);
    options.create_option(1,2,"PARAMETERS", 3, false);
    options.create_option(1,3,"BACK",3,false);

    options.create_option(2,0, "DEFAULT",3,true); // spawn point
    options.create_option(2,1, "CENTER",3,false);
    options.create_option(2,2, "OPPOSITE", 3, false);
    options.create_option(2,3, "BACK", 3,false);

    options.create_option(3,0, "160x120",3, false); // sizes
    options.create_option(3,1,"64x48",3, true ); 
    options.create_option(3,2, "32x24", 3, false);
    options.create_option(3,3, "CUSTOM",3, false);
    options.create_option(3,4,"BACK", 3,false);

    options.create_slider(4,0, "(|||) ROWS", 64, 8, 320, 3, 2); //custom
    options.create_slider(4,1, "(---) COLS", 48, 8, 240, 3, 2);
    options.create_option(4,2,"BACK",3,false);

    options.create_slider(5,0, "GENERATOR DEPTH", 0, 0, 25, 2,1); // parameters
    options.create_option(5,1, "DEFAUTL DEPTH", 2,true);
    options.create_option(5,2, "BACK", 2,false);

    tft.fillScreen(TFT_BLACK);
    print_game_title(tft);
    options.draw(0,0);
    finished = false;
}

void loop()
{
    button_joystick.read();
    if (!finished)
    {
        joystick.read();
    } 
}   