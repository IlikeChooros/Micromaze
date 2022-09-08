#include "src/input/Joystick_esp.h"
#include "src/input/Button.h"
#include "src/output/Options.h"
#include "src/game-functions/Maze-generator.h"
#include "src/data-sturctures/Point.h"
#include "src/data-sturctures/Point_extended.h"
#include "src/data-sturctures/Stack.h"

#include <TFT_eSPI.h> 
#include <SPI.h>

#define BUTTON_PIN 25
#define ANALOG_X 32
#define ANALOG_Y 33
#define NUMBER_OF_ROWS_MAP 32
#define NUMBER_OF_COL_MAP 24

enum Dir{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

uint8_t number_of_cols = NUMBER_OF_COL_MAP;
uint8_t number_of_rows = NUMBER_OF_ROWS_MAP;

Joystick joystick(ANALOG_X, ANALOG_Y);
Button button_joystick(BUTTON_PIN);

TFT_eSPI tft = TFT_eSPI();

Maze_generator maze_gen(&tft);

Dir dir;

int8_t prev_pick_opt = 1;
int8_t prev_pick_layer = 2;
int8_t current_layer=0;
int8_t current_option=0;

Options options(&tft);

bool pressed = false;

void move()
{
    uint8_t num_of_options = options.get_num_of_options_in_layer(current_layer);
    //Serial.println("NUM OF OPTIONS = "+String(num_of_options)+" CURRENT_OPT: "+String(current_option) + "  CURRENT LAYER: "+String(current_layer));
    switch(dir)
    {
        case UP:
            current_option = current_option > 0 ? current_option-1 : num_of_options-1; 
            break;
        case DOWN:
            current_option = current_option == num_of_options-1 ? 0 : current_option+1;
            break;
    }
    tft.fillScreen(TFT_BLACK);
    options.draw(current_layer, current_option);
}

void down()
{
    dir = DOWN;
    move();
}

void up()
{
    dir = UP;
    move();
}

void set_pressed()
{
    pressed = true;
}

void maze_generating()
{
    button_joystick.on_press(set_pressed);


    tft.fillScreen(TFT_WHITE);
    maze_gen.load_map(number_of_cols, number_of_rows);
    maze_gen.generate_maze(new Point(random(number_of_cols), random(number_of_rows)));

    tft.fillScreen(TFT_WHITE);
    maze_gen.draw_maze();

    while (!pressed)
    {
        button_joystick.read();
    }

    pressed = false;

    button_joystick.on_press(pick_option);

    delay(1000);

    maze_gen.clear();
}

void pick_option()
{
    Serial.println("PICKING:  OPTION: "+ String(current_option)+ " LAYER: "+ String(current_layer));
    if (current_layer == 0)
    {
        switch(current_option)
        {
            case 0:
                maze_generating();
                break;
            case 1:
                current_layer++;
                current_option=0;
                break;
            default:
                break;
        }
        options.draw(current_layer, current_option);
    }

    else if (current_layer == 1)
    {
        switch (current_option)
        {
        case 0:
            current_layer++;
            current_option=0;
            break;
        case 1:
            current_layer--;
            current_option=0;
            break;
        default:
            break;
        }
        options.draw(current_layer, current_option);
    }

    else if(current_layer == 2)
    {
        switch(current_option)
        {
            case 0:
                number_of_cols=48;
                number_of_rows=64;
                options.set_mark(false, prev_pick_opt, prev_pick_layer);
                options.set_mark(true, current_option, current_layer);
                prev_pick_layer = current_layer;
                prev_pick_opt = current_option;
                break;
            case 1:
                number_of_cols = 24;
                number_of_rows=32;

                options.set_mark(false, prev_pick_opt, prev_pick_layer);
                options.set_mark(true, current_option, current_layer);
                prev_pick_layer = current_layer;
                prev_pick_opt = current_option;
                break;
            case 2:
                number_of_cols = 12;
                number_of_rows=16;
                options.set_mark(false, prev_pick_opt, prev_pick_layer);
                options.set_mark(true, current_option, current_layer);
                prev_pick_layer = current_layer;
                prev_pick_opt = current_option;
                break;
            case 3:
                number_of_cols = 6;
                number_of_rows=8;
                options.set_mark(false, prev_pick_opt, prev_pick_layer);
                options.set_mark(true, current_option, current_layer);
                prev_pick_layer = current_layer;
                prev_pick_opt = current_option;
                break;
            case 4:
                current_layer--;
                current_option=0;
                break;
            default:
                break;
        }
        options.draw(current_layer, current_option);
    }

    
}

void setup()
{
    button_joystick._init_();
    button_joystick.on_press(pick_option);
    joystick._init_();
    joystick.on_dir_down(down);
    joystick.on_dir_up(up);

    Serial.begin(921600);
    tft.init();

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(3);

    //maze_gen.generate_maze(new Point(random(number_of_cols),random(number_of_rows)));

    // tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    // tft.setRotation(1);
    
    // tft.print("OPTIONS");


    options._init_(9);
    options.create_option(0,0, "START", 3, false);
    options.create_option(0,1, "OPTIONS",3, false);
    options.create_option(1,0, "SIZES", 3, false);
    options.create_option(1,1, "BACK",3, false);
    options.create_option(2,0, "64x48",3, false);
    options.create_option(2,1, "32x24", 3, true);
    options.create_option(2,2, "16x12", 3, false);
    options.create_option(2,3, "8x6", 3, false);
    options.create_option(2,4, "BACK", 3, false);

    options.draw(current_layer, current_option);

    //tft.fillScreen(TFT_WHITE);
    
    // Serial.println("FIRST");
    // options.draw(0,0);
    // delay(2500);
    // Serial.println("SECOND");
    // options.draw(1,0);
    // delay(2500);
    // Serial.println("THIRD");
    // options.draw(2,0);
    // delay(2500);
    
    // Serial.println("END");
    //maze_gen.draw_maze();

    // maze_gen._init_();
    // maze_gen.create_generators(5, sec_map,number_of_cols,number_of_rows);
    // maze_gen.generate_maze(6,5,13);
    // maze_gen.delte_nodes();

    // world_map.set_map(sec_map,number_of_rows,number_of_cols, 0, 128, 153);
    // tft.fillScreen(TFT_BLACK);

    // door.generate_door(9);

    // player_vision.load_map(sec_map,number_of_cols, number_of_rows, NUMBER_OF_COL_MAP, NUMBER_OF_ROWS_MAP,convert_to_RGB(2, 96, 173));
    // player_vision.load_ray_casting(0.035, 6.28, 10, color);

    // player._init_();
    // introduction();
}

void loop()
{
    button_joystick.read();
    joystick.read();
}   