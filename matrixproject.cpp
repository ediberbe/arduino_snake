/*
  The program works by infinitely looping a switch case based on STATE_ variables
*/

#include <LiquidCrystal.h>
#include "LedControl.h"

//================PINS================

//display
const int RS = 7;
const int enable = 6;
const int d4 = 5;
const int d5 = 4;
const int d6 = 3;
const int d7 = 2;

//joystick
const int VRx = A0;
const int VRy = A1;
const int SW = 0;

//matrix
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

//
const int buzzer = 9;

//================VAR================

String username = "john";
int difficulty = 0;

//states
int state = 9;
const int STATE_MAINMENU = 9;
const int STATE_MAINLOOP = 0;
const int STATE_HIGHSCORE = 1;
const int STATE_SETTINGS = 2;
const int STATE_ABOUT = 3;
const int STATE_SETNAME = 10;
const int STATE_SETDIFF = 11;
const int STATE_SETAUDIO = 12;
const int STATE_RESETHS = 13;

//display
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

byte rightArrow[8] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};

//menu
int menuSelection = 0;

String menuMain[5] = {"Start game", "Highscore", "Settings", "About", ""};
int menuMainCount = 4;

String menuSettings[4] = {"Set Difficulty", "Switch Audio", "Reset HS", ""};
int menuSettingsCount = 3;

String menuAbout[4] = {"Snake", "Edi Berbescu", "https://github.com/ediberbe/arduino_snake"};
int menuAboutCount = 3;

bool matrixAnimation = true;

//highscores
String highNames[3] = {"andr", "miha", "radu"};
int highScores[3] = {12, 8, 4};

//joystick
int xPosition = 0;
int yPosition = 0;
int SW_state = 1;
bool joyReady = true;
int SW_state_last = 1;

//matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2;
byte moveInterval = 200;
unsigned long long lastMoved = 0;
const byte matrixSize = 8;
bool matrixChanged = true;

bool matrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

//snake
int snakeSize = 1;
bool snakeChanged = false;
int snakePos[64][2] = {{0,0}};
bool isDead = false;

int yVel = 0;
int xVel = 0;
int LASTxVel = 0;
int LASTyVel = 0;

int xFood = 7;
int yFood = 7;
int score = 0;

int xBomb = random(1, 6);
int yBomb = random(1, 6);
int bombBlinkTimer;
int bombBlinkDuration = 100;
int bombBlinkState = 1;

// buzzer
bool isAudio = true;
int buzzTimer;
int buzzDuration = 150;

//================FUNC================

void updateMatrix() {
  //turns on/off matrix leds acording to matrix[][]
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void clearMatrix() {
  //turns off every led
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

bool isNewHighscore = false;
void updateHighscore(){
  //could've used a sorting algorithm but decided against it

  //first case place 1
  if(score >= highScores[0]){
    highScores[2] = highScores[1];
    highNames[2] = highNames[1];
    highScores[1] = highScores[0];
    highNames[1] = highNames[0];
    highNames[0] = username;
    highScores[0] = score;
  }

  //second case place 2
  else if(score >= highScores[1]){
    highScores[2] = highScores[1];
    highNames[2] = highNames[1];
    highNames[1] = username;
    highScores[1] = score;
  }

  //third case place 3
  else if(score >= highScores[2]){
    highNames[2] = username;
    highScores[2] = score;
  }
}

void updatePositions() {
  // makes the snake move based on xVel,xPos

  matrixChanged = true;
  //removes snake from matrix
  for(int i=0;i<snakeSize;i++){
    matrix[snakePos[i][0]][snakePos[i][1]] = 0;
  }

  //moves each snake block one position to the right in list
  for(int i=snakeSize;i>0;i--){
    snakePos[i][0] = snakePos[i-1][0];
    snakePos[i][1] = snakePos[i-1][1];
  }

  //determines new head position and adds it first in list
  snakePos[0][0] = snakePos[0][0] + yVel;
  snakePos[0][1] = snakePos[0][1] + xVel;
  LASTxVel = xVel;
  LASTyVel = yVel;

  //increments size if snake has changed
  if(snakeChanged){
    snakeChanged = false;
    snakeSize++;
  }

  //adds snake to matrix
  for(int i=0;i<snakeSize;i++){
    matrix[snakePos[i][0]][snakePos[i][1]] = 1;
  }

  // check for tail bite
  for(int i=1;i<snakeSize;i++)
    if(snakePos[0][0]==snakePos[i][0] && snakePos[0][1]==snakePos[i][1])
      isDead = true;

  // check for bomb bite
  if(xBomb == snakePos[0][0] && yBomb == snakePos[0][1] && difficulty ==2)
      isDead = true;

  // death condition
  if(snakePos[0][0]<0 || snakePos[0][0]>7 || snakePos[0][1]<0 || snakePos[0][1]>7 || isDead == true){
    state = 9;
    menuSelection = 0;
    snakePos[0][0] = 0;
    snakePos[0][1] = 0;
    yVel = 0;
    xVel = 0;
    matrix[xFood][yFood] = 0;
    xFood = 7;
    yFood = 7;
    snakeSize = 1;
    snakeChanged = false;
    isDead = false;
    matrixAnimation = true;

    // if new highscore then proceed to selecting name
    if(score >= highScores[2]){
      state = 10;
      isNewHighscore = true;
      matrixAnimation = true;
      lcd.clear();
    }

    if(isNewHighscore == false) score = 0;
  }

}

void joystickMovement() {
  //joystick control used in-game

  //read joystick movement
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
      
  if(xPosition<200 && LASTxVel != 1) {
    //Joystick Left = Go Left
    yVel = 0;
    xVel = -1;
  }

  if(xPosition>800 && LASTxVel != -1) {
    //Joystick Right = Go Right
    yVel = 0;
    xVel = 1;
  }
        
  if(yPosition<200 && LASTyVel != 1) {
    //Joystick Up = Go Up
    yVel = -1;
    xVel = 0;
  }

  if(yPosition>800 && LASTyVel != -1) {
    //Joystick Down = Go Down
    yVel = 1;
    xVel = 0;
  }

  //if joystick has been clicked
  if(SW_state == 0 && SW_state_last == 1) {}
  SW_state_last = SW_state;
}

void updateFood() {
  //updating food used in-game

  matrix[xFood][yFood] = 1;

  //if snake eats food
  if(snakePos[0][0] == xFood && snakePos[0][1] == yFood){
    score=score+1+difficulty;
    snakeChanged = true;

    //find new valid food position
    bool isValidFoodPos = false;
    while(!isValidFoodPos)
    {
      isValidFoodPos = true;

      xFood = random(0, 7);
      yFood = random(0, 7);

      for(int i=0;i<snakeSize;i++)
        if(snakePos[i][0] == xFood && snakePos[i][1] == yFood)
          isValidFoodPos = false;
      if(xBomb == xFood && yBomb == yFood && difficulty==2)
        isValidFoodPos = false;
    }

    matrix[xFood][yFood] = 1;
    matrixChanged = true;

    //turns on eating sound cue
    if(isAudio)tone(buzzer, 1000);
    buzzTimer = millis();
  }
}

void updateBomb() {
  //updating bombs used in-game in difficulty 2

  if(millis()- bombBlinkTimer > bombBlinkDuration){
    if(bombBlinkState == 1) bombBlinkState = 0;
    else bombBlinkState = 1;
    bombBlinkTimer = millis();
  }
  matrix[xBomb][yBomb] = bombBlinkState;
  matrixChanged = true;
}

void updateLCD() {
  //updating lcd used in-game to display stats

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
}

void matrixPrintAnimation(int index) {
  //prints designs on the matrix

  if(matrixAnimation){
    clearMatrix();
    matrixAnimation = false;

    switch (index) {
      case 1: //M
        clearMatrix();
        matrix[1][1] = 1;
        matrix[2][1] = 1;
        matrix[3][1] = 1;
        matrix[4][1] = 1;
        matrix[5][1] = 1;
        matrix[2][2] = 1;
        matrix[3][3] = 1;
        matrix[2][4] = 1;
        matrix[1][5] = 1;
        matrix[2][5] = 1;
        matrix[3][5] = 1;
        matrix[4][5] = 1;
        matrix[5][5] = 1;
        updateMatrix();
        break;
      case 2: //H
        clearMatrix();
        matrix[1][1] = 1;
        matrix[2][1] = 1;
        matrix[3][1] = 1;
        matrix[4][1] = 1;
        matrix[5][1] = 1;
        matrix[3][2] = 1;
        matrix[3][3] = 1;
        matrix[3][4] = 1;
        matrix[1][5] = 1;
        matrix[2][5] = 1;
        matrix[3][5] = 1;
        matrix[4][5] = 1;
        matrix[5][5] = 1;
        updateMatrix();
        break;
      case 3: //S
        clearMatrix();
        matrix[1][1] = 1;
        matrix[2][1] = 1;
        matrix[3][1] = 1;
        matrix[5][1] = 1;
        matrix[1][2] = 1;
        matrix[1][3] = 1;
        matrix[1][4] = 1;
        matrix[3][2] = 1;
        matrix[3][3] = 1;
        matrix[3][4] = 1;
        matrix[5][2] = 1;
        matrix[5][3] = 1;
        matrix[5][4] = 1;
        matrix[1][5] = 1;
        matrix[3][5] = 1;
        matrix[4][5] = 1;
        matrix[5][5] = 1;
        updateMatrix();
        break;
      case 4: //A
        clearMatrix();
        matrix[1][1] = 1;
        matrix[2][1] = 1;
        matrix[3][1] = 1;
        matrix[4][1] = 1;
        matrix[5][1] = 1;
        matrix[1][2] = 1;
        matrix[1][3] = 1;
        matrix[1][4] = 1;
        matrix[3][2] = 1;
        matrix[3][3] = 1;
        matrix[3][4] = 1;
        matrix[1][5] = 1;
        matrix[2][5] = 1;
        matrix[3][5] = 1;
        matrix[4][5] = 1;
        matrix[5][5] = 1;
        updateMatrix();
        break;
      default:
        break;
    }
  }
}

//================MAIN================

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, rightArrow);
  Serial.begin(9600);

  //MATRIX
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  //Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Welcome to snake");
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      matrix[i][j]=1;
    }
    updateMatrix();
    delay(100);
  }
  clearMatrix();
  lcd.clear();
}

void loop() {

  //end sound
  if(millis()-buzzTimer>buzzDuration)noTone(buzzer);

  switch (state) {
    case STATE_MAINMENU: //================mainmenu================    

      //lcd display
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.print(menuMain[menuSelection]);
      lcd.setCursor(0, 1);
      lcd.print(menuMain[menuSelection+1]);

      //matrix display
      matrixPrintAnimation(1);
      
      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();

        if(xPosition<100) {
          //Joystick Left
        }

        if(xPosition>900) {
          //Joystick Right = Select Option
          state = menuSelection;
          menuSelection = 0;
          lastMoved = millis();
          matrixAnimation = true;
          bombBlinkTimer = millis();
        }

        if(yPosition<100) {
          //Joystick Up = Scroll menu up
          if(menuSelection != 0) menuSelection--;
        }

        if(yPosition>900) {
          //Joystick Down = Scroll menu down
          if(menuSelection != menuMainCount-1) menuSelection++;
        }
      }

      //if joystick has been clicked
      if(SW_state == 0 && SW_state_last == 1) {}
      SW_state_last = SW_state;

      break;

    case STATE_MAINLOOP: //================Main game loop================

      matrixPrintAnimation(0);
      joystickMovement();
      if(difficulty == 2) updateBomb();

      //update game at interval
      if (millis() - lastMoved > moveInterval) {
        updateLCD();
        updateFood();
        updatePositions();
        lastMoved = millis();
      }

      //update matrix
      if (matrixChanged == true) {
        updateMatrix();
        matrixChanged = false;
      }

      break;

    case STATE_HIGHSCORE: //================Highscore================

      //lcd display
      lcd.setCursor(0, 0);
      lcd.print(highNames[menuSelection]);
      lcd.print(": ");
      lcd.print(highScores[menuSelection]);
      lcd.setCursor(0, 1);
      lcd.print(highNames[menuSelection+1]);
      lcd.print(": ");
      lcd.print(highScores[menuSelection+1]);
      
      //matrix display
      matrixPrintAnimation(2);

      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();
        
        if(xPosition<100) {
          //Joystick Left = Go back to main-menu
          state = STATE_MAINMENU;
          menuSelection = 0;
          matrixAnimation = true;
        }

        if(xPosition>900) {
          //Joystick Right
        }
        
        if(yPosition<100) {
          //Joystick Up = Scroll menu up
          if(menuSelection != 0) menuSelection--;
        }

        if(yPosition>900) {
          //Joystick Down = Scroll menu down
          if(menuSelection != 1) menuSelection++;
        }
      }

      //if joystick has been clicked
      if(SW_state == 0 && SW_state_last == 1) {}
      SW_state_last = SW_state;

      break;
      
    case STATE_SETTINGS: //================Settings================

      //lcd display
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
      lcd.print(menuSettings[menuSelection]);
      lcd.setCursor(0, 1);
      lcd.print(menuSettings[menuSelection+1]);
      
      //matrix display
      matrixPrintAnimation(3);

      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();

        if(xPosition<100) {
          //Joystick Left = Go back to main-menu
          state = STATE_MAINMENU;
          menuSelection = 0;
          matrixAnimation = true;
        }

        if(xPosition>900) {
          //Joystick right = Select option
          if(menuSelection == 0) state = STATE_SETDIFF;
          if(menuSelection == 1) state = STATE_SETAUDIO;
          if(menuSelection == 2) state = STATE_RESETHS;
          menuSelection = 0;
        }

        if(yPosition<100) {
          //Joystick Up = Scroll menu up
          if(menuSelection != 0) menuSelection--;
        }

        if(yPosition>900) {
          //Joystick Down = Scroll menu down
          if(menuSelection != menuSettingsCount-1) menuSelection++;
        }
      }

      //if joystick has been clicked
      if(SW_state == 0 && SW_state_last == 1) {}
      SW_state_last = SW_state;

      break;
      
    case STATE_ABOUT: //================About================

      //lcd display
      lcd.setCursor(0, 0);
      lcd.print(menuAbout[menuSelection]);
      lcd.setCursor(0, 1);
      lcd.print(menuAbout[menuSelection+1]);
      
      //matrix display
      matrixPrintAnimation(4);

      //scrollLeft if text too big
      if((menuAbout[menuSelection].length() > 16 || menuAbout[menuSelection+1].length() > 16)
          && millis() - lastMoved > 500){
        lcd.scrollDisplayLeft();
        lastMoved = millis();
      }

      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();
        lastMoved = millis();
        
        if(xPosition<100) {
          //Joystick Left = Go back to settings
          state = STATE_MAINMENU;
          menuSelection = 0;
          matrixAnimation = true;
        }

        if(xPosition>900) {//Joystick right
        }
        
        if(yPosition<100) {
          //Joystick Up = Scroll menu up
          if(menuSelection != 0) menuSelection--;
        }

        if(yPosition>900) {
          //Joystick Down = Scroll menu down
          if(menuSelection != menuAboutCount-2) menuSelection++;
        }
      }

      //if joystick has been clicked
      if(SW_state == 0 && SW_state_last == 1) {}
      SW_state_last = SW_state;

      break;
      
    case STATE_SETNAME: //================Set Highscore Name================

      //lcd display
      lcd.setCursor(0, 0);
      lcd.print(username);
      lcd.setCursor(menuSelection, 1);
      lcd.print('X');
    
      //matrix display
      matrixPrintAnimation(2);

      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();

        if(xPosition<100) {
          //Joystick Left = Select letter on left
          if(menuSelection != 0) menuSelection--;
        }

        if(xPosition>900) {
          //Joystick Right = Select letter on right
          if(menuSelection != 15) menuSelection++;
        }

        if(yPosition<100) {
          //Joystick Up = Change letter
          username[menuSelection]--;
        }

        if(yPosition>900) {
          //Joystick Down = Change letter
          username[menuSelection]++;
        }
      }

      //if joystick has been clicked SET NAME and go back to MAINMENU
      if(SW_state == 0 && SW_state_last == 1) {
          isNewHighscore = false;
          updateHighscore();
          score = 0;
          state = STATE_MAINMENU;
          menuSelection = 0;
          matrixAnimation = true;
      }
      SW_state_last = SW_state;
      break;

    case STATE_SETDIFF: //================Settings-Difficulty================

      //lcd display
      lcd.setCursor(0, 0);
      lcd.print(difficulty);
    
      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();

        if(xPosition<100) {
          //Joystick Left
        }

        if(xPosition>900) {
          //Joystick Right
        }

        if(yPosition<100) {
          //Joystick Up = Increase difficulty
          if(difficulty != 2) difficulty++;
        }

        if(yPosition>900) {
          //Joystick Down = Decrease difficulty
          if(difficulty != 0) difficulty--;
        }
      }

      //if joystick has been clicked SET DIFFICULTY and go back to settings
      if(SW_state == 0 && SW_state_last == 1) {
        state = STATE_SETTINGS;
        menuSelection = 0;

        if(difficulty == 0) moveInterval = 200;
        else if(difficulty == 1) moveInterval = 150;
        else if(difficulty == 2) moveInterval = 150;
      }
      SW_state_last = SW_state;
      break;

    case STATE_SETAUDIO: //================Settings-Audio================
     
      //lcd display
      lcd.setCursor(0, 0);
      lcd.print(isAudio);
    
      //read joystick movement
      xPosition = analogRead(VRx);
      yPosition = analogRead(VRy);
      SW_state = digitalRead(SW);
      
      //if joystick has moved
      if(xPosition>100 && xPosition<900 && yPosition>100 && yPosition<900) joyReady = true;
      else if(joyReady == true){
        joyReady = false;
        lcd.clear();

        if(xPosition<100) {
          //Joystick Left
        }

        if(xPosition>900) {
          //Joystick Right
        }

        if(yPosition<100) {
          //Joystick Up = Toggle Audio
          isAudio = !isAudio;
        }

        if(yPosition>900) {
          //Joystick Down = Toggle Audio
          isAudio = !isAudio;
        }
      }

      //if joystick has been clicked and go back to settings
      if(SW_state == 0 && SW_state_last == 1) {
        state = STATE_SETTINGS;
        menuSelection = 0;
      }

      SW_state_last = SW_state;
      break;
    case STATE_RESETHS: //================Settings-ResetHS================
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Done!");
      highNames[0] = "null";
      highNames[1] = "null";
      highNames[2] = "null";
      highScores[0] = 0;
      highScores[1] = 0;
      highScores[2] = 0;
      state = STATE_SETTINGS;
      menuSelection = 0;
      delay(1000);
    break;
  }
  
}
