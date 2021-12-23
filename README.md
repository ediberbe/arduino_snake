# arduino_snake
Snake game using an arduino and various electrical components.

![WhatsApp Image 2021-12-23 at 2 43 47 PM](https://user-images.githubusercontent.com/60759315/147242427-0460abba-9f7e-4c8b-aaa5-0e18c946136d.jpeg)
https://youtu.be/DOaoat5Dq_A

**Task Requirements**: Create a game. It must be fun and intuitive, and also remember that it is your game! Pick something that you like and have fun creating it.

**Menu Requirements**: Create a menu for your game, emphasis on the game. You
should scroll on the LCD with the joystick. Remember you have quite a
lot of flexibility here, but do not confuse that with a free ticket to slack
off. The menu should include the following functionality:
  1. When powering up a game, a greeting message should be shown for a few moments.  
  2. Should contain roughly the following categories:  
    (a) Start game, starts the initial level of your game  
    (b) Highscore:  
        – Initially, we have 0.  
        – Update it when the game is done. Highest possible score should be achieved by starting at a higher level.  
        – Save the top 3 values in EEPROM with name and score  
    (c) Settings:  
        – Enter name. The name should be shown in highscore. Maybe somewhere else, as well?  
        – Starting level: Set the starting level value. The idea is to be able to start from a higher level as well. Can be replaced with difficulty.  
        – LCD contrast control (must replace potentiometer!).  
        – LCD brightness control (must change LED wire that’s directly connected to 5v).  
        – Matrix brightness control (see function setIntesnity from the ledControl library).  
        – Extra stuff can include items specific to the game mechanics, or other settings such as chosen theme song etc.  
    (d) About:  
        should include details about the creator(s) of the game. At least game name, author and github link (use scrolling text?)  
  3. While playing the game: display all relevant info  
        – Lives  
        – Level  
        – Score  
        – Time?  
        – Player name?  
        – etc  
  4. Upon game ending:  
    (a) Screen 1: a message such as ”Congratulations on reaching level/score X”. ”You did better than y people.”. etc. Switches to screen 2 upon interaction (button press) or after a few moments.  
    (b) Screen 2: display relevant game info: score, time, lives left etc. Must inform player if he/she beat the highscore. This menu should only be closed by the player, pressing a button.
    
 **Game Requirements**:  
    – Minimal components: an LCD, a joystick, a buzzer and the led matrix.  
    – You must add basic sounds to the game (when ”eating” food, when dying, when finishing the level etc). Extra: add theme songs.  
    – It must be intuitive and fun to play.  
    – It must make sense in the current setup. Study the idea of a panning camera - aka the 8x8 led doesn’t need to be the entire map. It can only be the current field of view of the player.  
    – The levels must progress dynamically. Aka the level difficulty, score and other properties should be a function of the level number or time. However, the challenge here is for it to grow in a playable way  
    - not too easy for a long time so it becomes boring, not too hard too fast so it’s not attractive. Also, it’s nice to have an ending, a final level, a boss etc. It shouldn’t necessarily go on forever (but that’s fine, though).  

**Components Used**: 16 Segment Led Display, 8x8 Led Matrix, MAX7219 Driver, Joystick, Buzzer, 10k Potentiometer, Capacitors, Resistors and Wires.
