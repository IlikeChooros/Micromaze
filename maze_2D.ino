#include "src/input/Joystick_esp.h"
#include "src/output/Matrix_map.h"
#include "src/output/Player_tft.h"
// #include "src/output/Ray-casting-2D.h"
#include "src/output/Level_maps.h"
#include "src/game-functions/Door.h"
#include "src/game-functions/Maze-generator.h"
#include "src/game-functions/LinkedList.h"
#include "src/game-functions/Point.h"
#include "src/game-functions/Point_extended.h"
#include "src/game-functions/Camera.h"

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>


#define BUTTON_PIN 25
#define ANALOG_X 32
#define ANALOG_Y 33
#define NUMBER_OF_ROWS_MAP 32
#define NUMBER_OF_COL_MAP 24

TFT_eSPI tft = TFT_eSPI();

Joystick joystick(ANALOG_X, ANALOG_Y);


Matrix_map world_map(&tft);

Point_extended starting_pos_player = {12,3,10.0,3.0};


Player_tft player(starting_pos_player,&tft,&world_map);

HSV color[]={ // 0 -> wall, 1-> door color
    {155.0,100.0,100.0},
    {170.0,100.0,100.0}
};

//Ray_casting ray_casting (&tft, &world_map, 10, 0.035, 2.24, color);

Door door(&tft, &world_map);

uint8_t map_idx=1;

uint8_t *all_maps[2]={
    first_map,
    second_map
};

uint8_t number_of_doors[]={4,4};

Point_extended collision_point;

Maze_generator maze_gen;

Camera player_vision(&tft);

uint32_t convert_to_RGB(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t RGB;
    r = (r>>3);
    g = (g>>2);
    b = (b>>3);

    RGB = RGB | (r<<11);
    RGB = RGB | (g<<5);
    RGB = RGB | (b);

    return RGB;
}

uint32_t HSV_to_RGB(HSV color_hsv)
{
    float s = color_hsv.satrutaion/100;
    float v = color_hsv.value/100;

    float C = s*v;
    float X = C * ( 1 - abs(fmod(color_hsv.hue/60,2) - 1));
    float m = v - C; 

    float r,g,b;

    if (color_hsv.hue < 60)
    {
        r = C, g=X, b=0;
    }
    else if (color_hsv.hue<120)
    {
        r=X, g=C, b=0;
    }    
    else if (color_hsv.hue<180)
    {
        r=0, g=C, b=X;
    }    
    else if (color_hsv.hue<240)
    {
        r=0, g=X, b=C;
    }    
    else if (color_hsv.hue<300)
    {
        r=X, g=0, b=C;
    }    
    else
    {
        r=C, g=0, b=X;
    }

    return convert_to_RGB(
        (r+m)*255,
        (g+m)*255,
        (b+m)*255
    );
}

void down()
{
    tft.fillScreen(TFT_BLACK);
    collision_point = player.move_backwards();
    if (collision_point.x != -1)
    {
        if (door.check_collision_with_player(collision_point))
        {
            //tft.fillScreen(TFT_BLACK);
            //door.clear_map();
            // world_map.set_map(maze,NUMBER_OF_ROWS_MAP, NUMBER_OF_COL_MAP, 0,0,0);
            // door.load_map(number_of_doors[map_idx]);
            // player.set_player_posistion(starting_pos_player);
            // map_idx++;
        }
    }

    player_vision.draw_vision(player.get_current_player_position());
    player_vision.draw_vision_with_ray_cast(player.get_current_angle(), player.get_current_player_position());
    player_vision.clear_prev_player_position(player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), player.get_current_angle());
    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}
void up()
{
    tft.fillScreen(TFT_BLACK);
    collision_point = player.move_forward();
    if (collision_point.x != -1)
    {
        if (door.check_collision_with_player(collision_point))
        {
            //tft.fillScreen(TFT_BLACK);
            // //door.clear_maps();
            // world_map.set_map(maze,NUMBER_OF_ROWS_MAP, NUMBER_OF_COL_MAP, 0,0,0);
            // player.set_player_posistion(starting_pos_player);
            // map_idx++;
        }
    }

    player_vision.draw_vision(player.get_current_player_position());
    player_vision.draw_vision_with_ray_cast(player.get_current_angle(), player.get_current_player_position());
    player_vision.clear_prev_player_position(player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), player.get_current_angle());
    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}
void left()
{
    player.move_left();
    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
    player_vision.draw_vision_with_ray_cast(player.get_current_angle(), player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), player.get_current_angle());
}
void right()
{
    player.move_right();
    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
    player_vision.draw_vision_with_ray_cast(player.get_current_angle(), player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), player.get_current_angle());
}

void setup()
{
    joystick._init_();
    joystick.on_dir_down(down);
    joystick.on_dir_up(up);
    joystick.on_dir_left(left);
    joystick.on_dir_right(right);

    Serial.begin(921600);
    tft.init();

    maze_gen._init_();
    maze_gen.create_generators(5, sec_map,NUMBER_OF_COL_MAP*2,NUMBER_OF_ROWS_MAP*2);
    //maze_gen.generate_maze(5,3,10);
    maze_gen.generate_maze(5,5,13);

    world_map.set_map(sec_map,NUMBER_OF_ROWS_MAP*2,NUMBER_OF_COL_MAP*2, 229,180,22);
    tft.fillScreen(TFT_BLACK);

    door.generate_door(9);

    //world_map.draw_map();

    Point_extended test_point = {30,30,0,0};

    player_vision.load_map(sec_map,NUMBER_OF_COL_MAP*2, NUMBER_OF_ROWS_MAP*2, NUMBER_OF_COL_MAP, NUMBER_OF_ROWS_MAP,convert_to_RGB(100, 28, 173));
    player_vision.load_ray_casting(0.035, 2.1, 10, color);

    for (uint8_t y=NUMBER_OF_ROWS_MAP-5;y<NUMBER_OF_ROWS_MAP+5;y++)
    {
        for (uint8_t x = NUMBER_OF_COL_MAP-5;x<NUMBER_OF_COL_MAP+5;x++)
        {
            if (!sec_map[y*NUMBER_OF_COL_MAP*2+x])
            {
                test_point.x=x;
                test_point.y=y;
                player.set_player_posistion(test_point);
            }
        }
    }

    player._init_();
    player_vision.load_player(convert_to_RGB(176, 168, 111),convert_to_RGB(99, 5, 14), player.get_current_angle(), player.get_current_player_position());

    //ray_casting._init_();

    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}

void loop()
{
    joystick.read();

}   