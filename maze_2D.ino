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

#include <TFT_eSPI.h> 
#include <SPI.h>
#include <math.h>

#define BUTTON_PIN 25
#define ANALOG_X 32
#define ANALOG_Y 33
#define NUMBER_OF_ROWS_MAP 32
#define NUMBER_OF_COL_MAP 24
#define LN_10 2.30258509
#define a 100

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

int8_t prev_pick_opt_2 = 0;
int8_t prev_pick_opt_3 = 1;

bool default_spawn = true;
bool center_spawn = false;

void do_nothing()
{
    return;
}

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

void show_vision()
{
    button_pressed++;
    player_vision.draw_vision(player.get_current_player_position());
}

bool set_starting_position(uint8_t y, uint8_t x_margin)
{
    uint8_t width_of_segment = tft.width()/(number_of_cols);
    uint8_t height_of_segment = tft.height()/(number_of_rows);

    for (uint8_t x = x_margin - 2;x<x_margin +3;x++)
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
    uint8_t x_margin = number_of_cols/2;
    uint8_t y_margin = number_of_rows/2;

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

    for (uint8_t y=y_margin-2;y<y_margin+3;y++)
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
    uint8_t num_of_gen;
    uint8_t x;

    float f;

    x = number_of_cols > number_of_rows? number_of_cols : number_of_rows;

    // f(x) = 9 * log_10 (x) - 9
    // Using taylor series 
    // for a = 100
    // P(x) = f(a) + f'(a)*(x-a) + 1/2! * f''(a)*(x-a)^2 + 1/3! * f'''(a)*(x-a)^3 + ... 
    // i get polynomial P(x) = 9 * log_10(100) - 9 + 9*(x-100) / (ln(100) * 100) - 9*(x-100)^2 / (2 * 100^2 * ln(100)) + 3 * (x-100)^3 / (100^3 * ln(100))
    // P(x) = 9 + 9*(x-100)/(ln100 * 100) - 9 * (x-100)^2 / ( 2 * 100^2 * ln100) + 3*(x-100)^3 / (100^3 * ln100) 
    // i used ln10 instead of ln100, to increase the slope

    f = 10;
    f +=9*(x-a)/(LN_10 * a);
    f -= 9*(x-a)*(x-a)/(2*a*a* LN_10);
    f += 3*(x-a)*(x-a)*(x-a)/(a*a*a * LN_10);
    //Serial.println(" f(" + String(x) + ") = "+String(f));

    num_of_gen = round(f);

    x = number_of_cols < number_of_rows ? number_of_cols : number_of_rows;
    x = random(x/8,x/4) + 2;
    maze_gen.create_generators(x,number_of_cols,number_of_rows);
    maze_gen.generate_maze(num_of_gen,5,13);
    maze_gen.delete_nodes();
}

uint8_t closest_to_divider(uint8_t num, uint8_t divisors[], uint8_t number_of_divsors, uint8_t *idx)
{
    int16_t val,min=100, abs_val;
    for (uint8_t i=0;i<number_of_divsors;i++)
    {
        val = divisors[i] - num;
        abs_val = abs(val);
        if (abs_val < min && val <= 0)
        {
            min = abs_val;
            *idx = i;
        }
    }
    return divisors[*idx];
}

void load_player_vision()
{
    uint8_t divisors_[18] = {1,2,3,4,5,6,8,10,12,15,16,20,24,30}; // cols 240
    uint8_t divisors[12] = {1,2,4,5,8,10,16,20,32,40}; // rows 320

    uint8_t idx_240, idx_320, col, rows, ray_lenght;
    float ray_angle = 0.0177; // ~ 0.5 deg.
    float deg = 0.5;
    rows = closest_to_divider(number_of_rows/2, divisors, 12, &idx_320);
    col = closest_to_divider(number_of_cols/2, divisors_, 18, &idx_240);
    // Serial.println("");
    //Serial.println("COL: " + String(number_of_cols/2) + "  divisors_240["+String(idx_240)+"] = "+String(col));
    //Serial.println("ROW: " + String(number_of_rows/2) + "  divisors_320["+String(idx_320)+"] = "+String(rows));
    
   
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
        ray_angle = 0.28; // ~ 8 deg.
        deg = 8;
    }
    else if (ray_lenght < 5)
    {
        ray_angle = 0.14; // ~ 4 deg.
        deg = 4;
    }
    else if (ray_lenght < 7)
    {
        ray_angle = 0.07; // ~ 2 deg.
        deg = 2;
    }
    else if (ray_lenght < 13)
    {
        ray_angle = 0.035; // ~ 1 deg.
        deg = 1;
    }

    //Serial.println("RAY_L: "+String(ray_lenght) + "    DEG: "+String(deg));

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
    tft.println(" --- Time spent looking for exit: "+String(action_time/1000)+ " s.");
    tft.println(" --- Button pressed: "+String(button_pressed)+ " times.");


    // the smaller the is score the better performance
    // keep the score in 0 - 1400
    // for 64x48:
    // score = action_time/80
    // score += map_time/15
    // score += button_pressed*100

    // center:
    //time spent on map: 4 sec
    // time spent looking for exit: 20 sec
    // score = 20*1000/80 + 4*1000/15 = 516

    // opposite:
    // score = 276

    // center:
    //time spent on map: 6 sec
    // time spent looking for exit: 25 sec
    // score = 25*1000/80 + 6*1000/15 = 712

    // score ~ 1/map_size, the bigger the map the smaller (the better) score is.
    // score ~ exit_time 
    // score ~ map_time
    // and if score1 and score2 for the same exit_time and map_time but score1 with opposite spawn and score2 with center spawn, score1 (opposite)< score2 (center) 
    // score = (p * map_time + q * exit_time)*k/map_size, sloving this eqation with parameters listed above we get: p = 94, q = 24, if center spawn k=1, else k=1,87

    float k = 1.87;
    uint8_t p=141,q=48;

    if (!center_spawn || (default_spawn && completed_game_with_high_score>1))
    {
        k = 1;
    }

    score = (p * map_time + q * action_time + 400000*button_pressed)*k;
    score /= number_of_cols;
    score /= number_of_rows;


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
            gradient_letters("A-MAZING!0", text_color.hue, 23);
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

    tft.setTextFont(1);
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
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0,0);
    tft.setRotation(0);
    tft.setTextSize(1);

    map_time = millis();

    joystick.on_dir_down(down);
    joystick.on_dir_left(left);
    joystick.on_dir_right(right);
    joystick.on_dir_up(up);

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

void fading_effect(const char str [], uint8_t size ,HSV starting_color, uint8_t itr)
{
    uint8_t hue = starting_color.hue;
    uint8_t i=0;

    uint8_t half_size = (size >> 1);
    while (i<size)
    {
        tft.setTextColor(HSV_to_RGB(starting_color));
        tft.print(str[i]);
        if (i<half_size)
        {
            starting_color.hue += itr;
        }
        else{
            starting_color.hue -= itr;
        }
        starting_color.hue = fmod(starting_color.hue, 360);
        i++;
    }
    starting_color.hue = hue;
}

void print_game_title()
{
    tft.setCursor(90,30);
    tft.setRotation(1);
    tft.setTextSize(3);

    HSV fading_color = {185,82,90};
    fading_effect("MICROMAZE", 9, fading_color, 5.3);

}

void increment_slider()
{
    tft.fillScreen(TFT_BLACK);
    options.increment_slider(current_layer, current_option);
    options.draw(current_layer, current_option);
}

void decrement_slider()
{
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
        print_game_title();
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

                prev_pick_opt_2 = current_option;
                break;
            case 1:
                default_spawn = false;
                center_spawn = true;

                options.set_mark(false,prev_pick_opt_2, 2);
                options.set_mark(true,current_option, 2);

                prev_pick_opt_2 = current_option;
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
                break;

            case 4:
                current_layer = 1;
                current_option=0;
        }
    }

    else{
        switch(current_option)
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                joystick.on_dir_left(do_nothing);
                joystick.on_dir_right(do_nothing);
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

    if (current_layer == 0)
    {
        print_game_title();
    }

    else if(current_layer == 4)
    {
        joystick.on_dir_left(decrement_slider);
        joystick.on_dir_right(increment_slider);
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

    Serial.begin(921600);
    tft.init();

    maze_gen._init_();

    options._init_(15,2,5);


    options.create_option(0,0,"START",3,false);
    options.create_option(0,1, "SETTINGS",3,false);

    options.create_option(1,0, "SPAWN POINT", 3, false);
    options.create_option(1,1, "MAZE SIZE",3,false);
    options.create_option(1,2,"BACK",3,false);

    options.create_option(2,0, "DEFAULT",3,true); // spawn point
    options.create_option(2,1, "CENTER",3,false);
    options.create_option(2,2, "OPPOSITE", 3, false);
    options.create_option(2,3, "BACK", 3,false);

    options.create_option(3,0, "160x120",3, false); // sizes
    options.create_option(3,1,"64x48",3, true ); 
    options.create_option(3,2, "32x24", 3, false);
    options.create_option(3,3, "CUSTOM",3, false);
    options.create_option(3,4,"BACK", 3,false);

    options.create_slider(4,0, "(|||) ROWS", 64,3,2); //custom
    options.create_slider(4,1, "(---) COLS",48,3,2);
    options.create_option(4,2,"BACK",3,false);

    tft.fillScreen(TFT_BLACK);
    print_game_title();
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