# Micromaze

![micromaze](https://user-images.githubusercontent.com/105538405/189667758-9ec85738-0a25-4835-8db1-6b99c14963fe.png)


Micromaze is a game in which u are forced to solve mazes, by playing as a cube in a maze and trying to get to the exit as fast as possible.
By getting to the exit you will get information of your performance with valid opinion about your abblities in maze solving.

Has customizable settings, in which you can set to your liking: spawn point, maze size and generation depth.

This game was made for esp32 with: display tft lcd 240x320 ILI9341 and PS2 thumbstick.

# Requirements

  i.   ESP-32
  ii.  LCD 320x240 TFT with ILI9341 driver
  iii. PS2 thumbstick
  
# Setup
  Connect PS2 and display to ESP 32 using the pins to your liking. However you must change predefined pins in 'main.ino' to yours.
  
![image](https://user-images.githubusercontent.com/105538405/200512973-2ac1ce83-2542-4b2c-be37-7d69bb02c2fb.png)




Below, I will describe options in Micromaze and explain how they are affecting the game.


# SPAWN POINT


  DEFAULT -> It works as follows: At the beginning, you will be placed at the center of the map, if you succeed to get score below 450 (you will gain 1 
  great_performance_point') two times, you will be placed in the opposite side of the exit, but if dont you keep 'great_preformance_point' above 1,by getting score
  above 450, you will lose 1 great_performance_point, you will be placed again in the center.

  CENTER -> You will be placed always in the center of the map.

  OPPOSITE -> You will be placed always in the opposite side of the exit (diagonally actually).

# MAZE SIZE


  You can set to your liking maze dimensions. It affects the speed of maze generation, the bigger the map, the longer it will take to finish generation of a maze.
  

# PARAMETERS


  GENERATOR DEPTH -> This greatly affects the maze generation, this number indicates how many times will the maze generation procedure be repeated.
  The bigger the number the more sophisticated the maze and more time will be spent on generating procedure. The most optimal number is the default one, assigned using modified taylor series of a logarythmic function: 
  
  
  ![image](https://user-images.githubusercontent.com/105538405/190658950-d399c8dc-df59-4530-bccc-2e2e7b96efbb.png)
  
  
  Where x is the bigger number between: number of columns and number of rows.
  
  
  
  ![image](https://user-images.githubusercontent.com/105538405/190659111-94b4fb3e-7254-401b-92ee-514f6b58bcfd.png)


  However, using taylor series, where a = 100, I would get the following polynomial: 
  P(x) = 9 + 9*(x-100)/(ln100 * 100) - 9 * (x-100)^2 / ( 2 * 100^2 * ln100) + 3*(x-100)^3 / (100^3 * ln100) 
  
  On graph:
  
  
  ![image](https://user-images.githubusercontent.com/105538405/190664773-e8e02698-20de-472d-8840-d8c0dcfd3c01.png)
  
  
  
  
  Although I could use this function, I wasnt satisfied with it, values for bigger x ~ 250 were too small, so I'm using this:
  
  ![image](https://user-images.githubusercontent.com/105538405/190665627-63aee9c8-dac5-4451-81ef-0a903d6f1326.png)




  On graph:
  
  ![image](https://user-images.githubusercontent.com/105538405/190666137-8eae0c2e-4ffe-4b70-9834-b5e2a07bb681.png)


  And all of them:
  
  ![image](https://user-images.githubusercontent.com/105538405/190666261-1e0fdb5d-e24d-4744-83c3-ba0a7ded40b6.png)






