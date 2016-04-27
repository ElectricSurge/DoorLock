#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define lockLED 5
#define unlockLED 4
#define beepPin 3
LiquidCrystal_I2C lcd(0x3F,20,4);

const byte ROWS = 4;
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13,12,11,10}; 
byte colPins[COLS] = {9,8,7,6}; 

const float Note_C5 = 523.251;
const float Note_D5 = 587.330;
const float Note_E5 = 659.255;
const float Note_F5 = 698.456;
const float Note_G5 = 783.991;
const float Note_A5 = 880.000;
const float Note_B5 = 987.767;
const float Note_C6 = 1108.73;

Servo doorServo;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

int DoorLocked = 0;
char PassKeys[4] = {'1','2','3','4'};



void setup(){
  Serial.begin(9600);
  lcd.init();
  pinMode(lockLED, OUTPUT);
  pinMode(unlockLED, OUTPUT);
  pinMode(beepPin, OUTPUT);
  ShowBanner();
  doorServo.attach(2);
  wave(beepPin, Note_C5, 200);
  wave(beepPin, Note_C6, 200);
}
  
void loop(){
  char customKey;
  while (customKey=customKeypad.getKey()){
  } // wait until key released
  ShowBanner();
  while (customKey==0) {
    customKey = customKeypad.getKey();
  } // wait until key pressed
  switch (customKey) {
    case '*':
      if (DoorLocked==0) {
        ChangePassword();
      } else {
      }
      break;
    case '#':
        SetDoorLock(1);
      break;
    default:
      if (VerifyPassword()==1) {
        SetDoorLock(0);
      };  
  }
}

void wave(int pin, float frequency, int duration) {
  float period=1 / frequency * 1000 * 1000;
  long int startTime=millis();
  while(millis()-startTime < duration) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(period/2);
    digitalWrite(pin,LOW);
    delayMicroseconds(period/2);
  }
}
void ShowBanner() {
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("     Welcome!      ");
  if (DoorLocked==1) {    
    digitalWrite(lockLED, HIGH);
    digitalWrite(unlockLED, LOW);
    delay(500);
    digitalWrite(lockLED, LOW);
    lcd.setCursor(0,1);
    lcd.print("[ Door is Locked ]");
    lcd.noBacklight();
  } else {
    digitalWrite(unlockLED, HIGH);
    digitalWrite(lockLED, LOW);
    delay(500);
    digitalWrite(unlockLED, LOW);
    lcd.setCursor(0,1);
    lcd.print("[ Door is Opened ]");
    lcd.setCursor(0,2);
    lcd.print(" '#' : Lock Door ");
    lcd.setCursor(0,3);
    lcd.print(" '*' : Change Key");
  }
  lcd.noCursor();
}

void ChangePassword() {
  char Keys[4];
  lcd.backlight();
  lcd.setCursor(0,2);
  lcd.print("Set new password:");
  lcd.setCursor(0,3);
  if (EnterDigits(4,Keys)==4) { 
    strncpy(PassKeys,Keys,4);
    lcd.setCursor(0,3);
    lcd.print("Password Changed....");
    wave(beepPin, Note_C5, 100);
    wave(beepPin, Note_D5, 100);
    wave(beepPin, Note_E5, 100);
    wave(beepPin, Note_F5, 100);
    wave(beepPin, Note_G5, 100);
    delay(2000);
  }
}

int VerifyPassword() {
  char Keys[4];
  lcd.backlight();
  lcd.setCursor(0,2);
  lcd.print("Enter the password:");
  lcd.setCursor(0,3);
  if (EnterDigits(4,Keys)==4) { 
    if (strncmp(Keys,PassKeys,4)==0) { 
       lcd.print(" ..Matched!");
       return 1;
    }
  }
  lcd.print(" ..Incorrect!");
  wave(beepPin, 220, 500);
  return 0;
}

int EnterDigits(int nrDigit,char Keys[]) {
  int var = 0;
  char customKey;
  lcd.backlight();
  lcd.cursor();
  lcd.setCursor(0,3);
  lcd.print("                    ");
  while(1) {
    lcd.setCursor(var,3);
    switch (customKey = customKeypad.getKey()) {
    case 0:
      break;
    case '*':
      if (var>0) var--;
      lcd.setCursor(var,3);
      lcd.print(" ");
      lcd.setCursor(var,3);
      break;
    case '#':
      return var;
      break;
    default:
      if(var <nrDigit) {
        lcd.print(customKey);
        Keys[var]=customKey;
        var++;
      }
      else {
        wave(beepPin, 440, 200);
      }
      break;
    } 
  }
  return 0;
}

int CheckInputKey(char Key) {
  char customKey;
  while (1) {
    if ( customKey = customKeypad.getKey()) {
       lcd.print(customKey);
       if (customKey==Key) {
          return 1;
       }
       else {
          return 0;
       }
    }   
  }
}

void SetDoorLock(int Locked) {
  if (Locked) {
    lcd.setCursor(0,3);
    lcd.print("##  Door Locked  ##");
    DoorLocked=1;
    doorServo.write(180);
    wave(beepPin, 540, 200);
    wave(beepPin, 580, 200);
    wave(beepPin, 620, 200);
    delay(500);
 } else {
    lcd.setCursor(0,3);
    lcd.print("## Door Unlocked ##");
    DoorLocked=0;
    doorServo.write(0);
    wave(beepPin, 880, 200);
    wave(beepPin, 440, 300);
    delay(500);
 }
}
