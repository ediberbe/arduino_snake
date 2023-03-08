# arduino_snake
Snake game using an arduino and various electrical components.  

![](https://user-images.githubusercontent.com/60759315/147242427-0460abba-9f7e-4c8b-aaa5-0e18c946136d.jpeg)

## Prerequisites  

Before you continue, ensure you have met the following requirements:  
* You have the setup in the picture above or recreate it: Arduino UNO, 16 Segment Led Display, 8x8 Led Matrix, MAX7219 Driver, Joystick, Buzzer, 10k Potentiometer, Capacitors, Resistors and Wires.  
* You have loaded the source code on your Arduino.  
* You have connected the Arduino to a power source.  

## Start  

The game starts automatically when Arduino is powered on.  

## Main Menu  

Controls:
- Joystick UP/DOWN -> Navigate menu  
- Joystick RIGHT -> Enter sub-menu  
- Joystick LEFT -> Leave sub-menu  
- Joystick CLICK -> Some settings require the use of this function  

Features:
- Start Game  
- Highscore  
- Settings  
  - Set difficulty  
  - Toggle audio  
  - Reset Highscores  
- About  


## Game  

Controls:
- Joystick UP/DOWN/LEFT/RIGHT -> Control snake direction  

8x8 Led Matrix:
- The moving dot is the snake.
- The stationary dot is the target location you need to reach.  
- Touching the flickering dot makes you lose the game.  
- Touching the walls makes you lose the game.  

16 Segment Led Display:
- Displays current score.  
- When game ends, if you set a high enough score you will be asked to enter your name.  
