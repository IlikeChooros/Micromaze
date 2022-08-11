#include "src/input/Joystick_esp.h"
#include "src/input/Button.h"
#include "src/output/Matrix_map.h"
#include "src/output/Player_tft.h"
#include "src/output/Level_maps.h"
#include "src/game-functions/Door.h"
#include "src/game-functions/Maze-generator.h"
#include "src/game-functions/Camera.h"
#include "src/data-sturctures/LinkedList.h"
#include "src/data-sturctures/Point.h"
#include "src/data-sturctures/Point_extended.h"

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

#define BUTTON_PIN 25
#define ANALOG_X 32
#define ANALOG_Y 33
#define NUMBER_OF_ROWS_MAP 32
#define NUMBER_OF_COL_MAP 24

uint8_t number_of_cols = NUMBER_OF_COL_MAP*2;
uint8_t number_of_rows = NUMBER_OF_ROWS_MAP*2;

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

uint8_t map_idx=1;

uint8_t *all_maps[2]={
    first_map,
    second_map
};

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

bool set_starting_position(uint8_t y, uint8_t x_margin)
{
    uint8_t width_of_segment = tft.width()/(number_of_cols);
    uint8_t height_of_segment = tft.height()/(number_of_rows);

    for (uint8_t x = x_margin - 2;x<x_margin +3;x++)
    {
        if (sec_map[y*number_of_cols+x]==0)
        {
            starting_point.x=x;
            starting_point.y=y;
            tft.fillRect(x*width_of_segment, y*height_of_segment, width_of_segment, height_of_segment, TFT_WHITE);
            return true;
        }
    }
    return false;
}

void show_vision()
{
    button_pressed++;
    player_vision.draw_vision(player.get_current_player_position());
}

void reset()
{
    button_joystick.on_press(show_vision);

    tft.fillScreen(TFT_BLACK);

    for (uint8_t y = 1;y<number_of_rows-1;y++)
    {
        for(uint8_t x=1;x<number_of_cols-1;x++)
        {
            sec_map[y*number_of_cols+x]=0;
        }
    }

    maze_gen.create_generators(5,sec_map,number_of_cols,number_of_rows);
    maze_gen.generate_maze(6,5,13);
    maze_gen.delte_nodes();


    door.clear_map();
    door_dir = door.generate_door(9);

    uint8_t x_margin = number_of_cols/2;
    uint8_t y_margin = number_of_rows/2;

    if (completed_game_with_high_score>1)
    {
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
                break;
            default:
                break;
        }
    }

    for (uint8_t y=y_margin-2;y<y_margin+3;y++)
    {
        if (set_starting_position(y, x_margin))
        {
            break;
        }
    }
    player.set_player_posistion(starting_point);
    world_map.draw_map();
    map_time = millis();
    button_pressed=0;
    finished = false;
    timer_started = false;
}


void gradient_letters(const char str [], uint8_t starting_hue, float itr_val)
{
    text_color.hue = starting_hue;
    uint8_t index=0;
    while(str[index]!='0')
    {
        tft.print(str[index]);
        text_color.hue+=itr_val;
        text_color.hue = fmod(text_color.hue,360);
        tft.setTextColor(HSV_to_RGB(text_color));
        index++;
    }
    text_color.hue = starting_hue;
}

void ending_scene()
{
    float score=0;
    tft.setCursor(0,0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    tft.setRotation(1);
    tft.setTextSize(1);
    tft.println("       Your Score: ");
    tft.println(" --- Time spent looking at map: "+String(map_time/1000) + " s.");
    tft.println(" --- Time spent looking for exit: "+String(action_time/1000 - map_time/1000)+ " s.");
    tft.println(" --- Button pressed: "+String(button_pressed)+ " times.");

    if (completed_game_with_high_score>1)
    {
        score = action_time/140;
        score += map_time/30;
    }
    else{
        if (button_pressed>0)
        {
            score+=200;
        }
        score = action_time/80;
        score += map_time/15;
    }


    score += button_pressed*100;
    text_color.hue += score/5;

    if (score<450)
    {
        completed_game_with_high_score++;
    }
    else{
        completed_game_with_high_score = completed_game_with_high_score>0 ? completed_game_with_high_score-1:0;
    }
    text_color.hue = text_color.hue > 360 ? 360: text_color.hue;

    tft.setTextColor(HSV_to_RGB(text_color));
    tft.println("");
    tft.setTextSize(2);
    tft.println("Overall score: "+ String((uint32_t)score));

    tft.setTextSize(1);

    uint8_t rand_num;
    if (score<=200)
    {
        gradient_letters("YOU ARE FABULOUS!0", text_color.hue, 30);
    }
    else if (score<=350)
    {
        rand_num = random(3);
        if (rand_num==0)
        {
            gradient_letters("JUST BUILT DIFFERENT0", text_color.hue, 20);
        }
        else if(rand_num==1)
        {
            gradient_letters("NO OFFENSE, JUST SUPERIOR HUMAN BEING :^)0",text_color.hue, 9.5);
        }
        else{
            gradient_letters("THAT WAS AMAZING!0", text_color.hue, 23);
        }
    }
    else if (score<=450)
    {
        gradient_letters("Great performance.0",text_color.hue, 8);
    }
    else if (score<550)
    {
        gradient_letters("Mediocare performance.0",text_color.hue, 5);
    }
    else if (score<650)
    {
        gradient_letters("Pathetic. :^)0",text_color.hue,7);
    }
    else if (score<850)
    {
        gradient_letters("That was something, I guess...0",text_color.hue,5);
    }
    else
    {
        rand_num = random(6);
        if (rand_num<2)
        {
            tft.print("My eyes are bleeding...");
        }
        else if(rand_num==3)
        {
            tft.print("What an awful performance!");
        }
        else if(rand_num==4)
        {
            tft.print("Ludicrous preformance!");
        }
        else
        {
            tft.print("Were you asleep while playing this game?");
        }
    }

    tft.println("");
    tft.println("");
    tft.setTextColor(TFT_WHITE);
    tft.println("The more time spent, the bigger the score is,");
    tft.println("so low score == great performance");
    tft.println("");

    text_color.hue = 100;
    tft.setTextColor(HSV_to_RGB(text_color)) ;tft.print("0 -");
    for (uint i=130;i<360;i+=20)
    {
        text_color.hue = i;
        tft.setTextColor(HSV_to_RGB(text_color));
        tft.print(" "+String((i-100)*5)+" -");
    }
    text_color.hue = 360;
    tft.setTextColor(HSV_to_RGB(text_color)) ;tft.print(" 1400+");

    tft.setRotation(0);
    text_color.hue = 100;
    tft.setTextColor(TFT_WHITE);
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
            ending_scene();
            return;
        }
    }
    player_vision.draw_vision_with_ray_cast(0, player.get_current_player_position());
    player_vision.clear_prev_player_position(player.get_current_player_position());
    player_vision.draw_player(player.get_current_player_position(), 0);
}

void start()
{
    tft.setCursor(0,0);
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);

    map_time = millis();

    world_map.draw_map();

    button_joystick.on_press(show_vision);
    finished=false;

    for (uint8_t y=number_of_rows/2-2;y<number_of_rows/2+3;y++)
    {
        if (set_starting_position(y, number_of_cols/2))
        {
            break;
        }
    }
    player.set_player_posistion(starting_point);
    player_vision.load_player(convert_to_RGB(176, 168, 111),convert_to_RGB(99, 5, 14), 0, player.get_current_player_position());
}

void fading_effect(const char str [], uint8_t size ,HSV starting_color, uint8_t itr)
{
    uint8_t hue = starting_color.hue;
    uint8_t i=0;
    while (i<size)
    {
        tft.setTextColor(HSV_to_RGB(starting_color));
        tft.print(str[i]);
        if (i<size/2-1)
        {
            starting_color.hue += itr;
        }
        else{
            starting_color.hue -= itr;
        }
        i++;
    }
    starting_color.hue = hue;
}

void introduction()
{
    tft.setCursor(80,0);
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(2);
    tft.setRotation(1);
    tft.setTextSize(3);

    HSV fading_color = {200,100,100};
    fading_effect("MAZE 2D", 8, fading_color, 20);
    tft.println("");
    tft.println("");
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.print(" Press Joystick to start...");
}

void setup()
{
    button_joystick._init_();
    button_joystick.on_press(start);
    joystick._init_();
    joystick.on_dir_down(down);
    joystick.on_dir_up(up);
    joystick.on_dir_left(left);
    joystick.on_dir_right(right);

    Serial.begin(921600);
    tft.init();

    maze_gen._init_();
    maze_gen.create_generators(5, sec_map,number_of_cols,number_of_rows);
    maze_gen.generate_maze(6,5,13);
    maze_gen.delte_nodes();

    world_map.set_map(sec_map,number_of_rows,number_of_cols, 0, 128, 153);
    tft.fillScreen(TFT_BLACK);

    door.generate_door(9);

    player_vision.load_map(sec_map,number_of_cols, number_of_rows, NUMBER_OF_COL_MAP, NUMBER_OF_ROWS_MAP,convert_to_RGB(2, 96, 173));
    player_vision.load_ray_casting(0.035, 6.28, 10, color);

    player._init_();
    introduction();
}

void loop()
{
    button_joystick.read();
    if (!finished)
    {
        joystick.read();
    } 
}   