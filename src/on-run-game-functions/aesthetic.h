#ifndef AESTHETIC_H
#define AESTHETIC_H

#include "Arduino.h"
#include "TFT_eSPI.h"
#include "SPI.h"
#include "../data-sturctures/hsv_rgb.h"

void inline fading_effect(const char str [], uint8_t size ,HSV starting_color, uint8_t itr, TFT_eSPI tft)
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

void inline print_game_title(TFT_eSPI tft)
{
    tft.setCursor(90,30);
    tft.setRotation(1);
    tft.setTextSize(3);

    HSV fading_color = {185,82,90};
    fading_effect("MICROMAZE", 9, fading_color, 5.3, tft);

}

void inline gradient_letters(const char str [], uint8_t starting_hue, float itr_val, TFT_eSPI tft, HSV text_color)
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

void inline ending_scene(HSV text_color, double map_time, double action_time, uint8_t button_pressed, bool center_spawn, bool default_spawn, uint8_t& completed_game_with_high_score, uint8_t number_of_cols, uint8_t number_of_rows, TFT_eSPI tft)
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
        gradient_letters("YOU ARE FABULOUS!0", text_color.hue, 30, tft, text_color);
    }
    else if (score<=350)
    {
        rand_num = random(3);
        if (rand_num==0)
        {
            gradient_letters("JUST BUILT DIFFERENT0", text_color.hue, 20, tft, text_color);
        }
        else if(rand_num==1)
        {
            gradient_letters("NO OFFENSE, JUST SUPERIOR HUMAN BEING :^)0",text_color.hue, 9.5, tft, text_color);
        }
        else{
            gradient_letters("A-MAZING!0", text_color.hue, 23, tft, text_color);
        }
    }
    else if (score<=450)
    {
        gradient_letters("Great performance.0",text_color.hue, 8, tft, text_color);
    }
    else if (score<550)
    {
        gradient_letters("Mediocare performance.0",text_color.hue, 5, tft, text_color);
    }
    else if (score<650)
    {
        gradient_letters("Pathetic. :^)0",text_color.hue,7, tft, text_color);
    }
    else if (score<850)
    {
        gradient_letters("That was something, I guess...0",text_color.hue,5, tft, text_color);
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



#endif