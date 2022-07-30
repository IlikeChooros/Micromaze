#include "src/input/Joystick_esp.h"
#include "src/output/Matrix_map.h"
#include "src/output/Player_tft.h"
#include "src/output/Ray-casting-2D.h"
#include "src/output/Level_maps.h"
#include "src/game-functions/Door.h"
#include "src/game-functions/Maze-generator.h"
#include "src/game-functions/LinkedList.h"
#include "src/game-functions/Point.h"

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

Point_exetened starting_pos_player = {12,3,10.0,3.0};


Player_tft player(starting_pos_player,&tft,&world_map);

HSV color[]={ // 0 -> wall, 1-> door color
    {155.0,100.0,100.0},
    {170.0,100.0,100.0}
};

Ray_casting ray_casting (&tft, &world_map, 10, 0.035, 2.24, color);

Door door(&tft, &world_map);

uint8_t map_idx=1;

uint8_t *all_maps[2]={
    first_map,
    second_map
};

uint8_t number_of_doors[]={4,4};

Point_exetened collision_point;

Maze_generator maze_gen (NUMBER_OF_COL_MAP, NUMBER_OF_ROWS_MAP, maze, 5);

void down()
{
    collision_point = player.move_backwards();
    if (collision_point.x != -1)
    {
        if (door.check_collision_with_player(collision_point))
        {
            //door.clear_map();
            world_map.set_map(all_maps[map_idx%2],NUMBER_OF_ROWS_MAP, NUMBER_OF_COL_MAP, 0,0,0);
            door.load_map(number_of_doors[map_idx]);
            player.set_player_posistion(starting_pos_player);
            map_idx++;
        }
    }

    ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}
void up()
{
    collision_point = player.move_forward();
    if (collision_point.x != -1)
    {
        if (door.check_collision_with_player(collision_point))
        {
            //door.clear_map();
            world_map.set_map(all_maps[map_idx%2],NUMBER_OF_ROWS_MAP, NUMBER_OF_COL_MAP, 0,0,0);
            door.load_map(number_of_doors[map_idx]);
            player.set_player_posistion(starting_pos_player);
            map_idx++;
        }
    }
    ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}
void left()
{
    player.move_left();
    door.check_collision_with_player(player.get_current_player_position());
    ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}
void right()
{
    player.move_right();
    door.check_collision_with_player(player.get_current_player_position());
    ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
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

    for (uint8_t i=0;i<5;i++)
    {
        maze_gen._init_();
        maze_gen.generate_maze(5);

        world_map.set_map(maze,NUMBER_OF_ROWS_MAP,NUMBER_OF_COL_MAP, 229,180,22);
        tft.fillScreen(TFT_BLACK);
        world_map.draw_map();

        maze_gen.reset();
        delay(1000);
    }

    //maze_gen._init_();
    

    //player._init_();
    //ray_casting._init_();

    //door.load_map(number_of_doors[map_idx]);

    //ray_casting.draw(player.get_current_player_position(), player.get_current_angle());
}

void loop()
{
    //joystick.read();

}   