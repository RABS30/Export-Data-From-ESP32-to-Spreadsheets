#include <ezBuzzer.h>
#include <max6675.h>
#include <Keypad.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;
const int pinBuzzer = A3;
const int ROW_NUM = 4;     //four rows
const int COLUMN_NUM = 4;  //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte pin_rows[ROW_NUM] = { 13, 12, 11, 10 };
byte pin_column[COLUMN_NUM] = { 9, 8, 7, 3 };
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
ezBuzzer buzzer(pinBuzzer);

int SO_PIN = 4;  // Serail Out (SO) pin
int CS_PIN = 5;  // Chip Select (CS) pin
int SCK_PIN = 6; // Clock (SCK) pin
MAX6675 thermocouple(SCK_PIN, CS_PIN, SO_PIN);

#include "Countimer.h"
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

String stringTempt;
String stringTime;
long intTempt;
long intTime;
char key;
int suhuPos = 6;

bool manualInput = false;
bool autoInput = false;
bool abInput = false;
bool ab2Input = false;
bool cInput = false;
bool c2Input = false;
bool CDT = false;
bool blockedButton = false;
bool runningCountinggg = false;
bool finishDisplay = false;
bool pausetimer = false;
bool stoptimer = false;
int pinRelay = 2;
float temptValue;

Countimer timer;


void setup() {
  Serial.begin(9600);
  lcd.begin(LCD_COLS, LCD_ROWS);
  pinMode(pinRelay, OUTPUT);
  lcd.clear();
  setDisplay();
  stringTempt.reserve(10);
  stringTime.reserve(10);
  timer.setInterval(refreshClock, 1000);
}
void temptSensor() {
  temptValue = thermocouple.readCelsius();
  // Serial.println(temptValue);
}
void refreshClock() {
  Serial.print("Current count time is: ");
  Serial.println(timer.getCurrentTime());
}
void Adisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu : 80 C ");
  lcd.setCursor(0, 1);
  lcd.print("Timer : ");
}
void Bdisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu : 50 C");
  lcd.setCursor(0, 1);
  lcd.print("Timer : ");
}
void Cdisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu : ");
  lcd.setCursor(11, 0);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Timer : ");
}
void setDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("Press A,B, or C");
  lcd.setCursor(0, 1);
  lcd.print("to Start");
}
void doneDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("heating is");
  lcd.setCursor(0, 1);
  lcd.print("complete");
}
void charKeypad() {
  key = keypad.getKey();
  if (key) {
    buzzer.beep(100);
    if (key == 'A') {
      intTempt = 80;
      Adisplay();
      suhuPos = 6;
      manualInput = false;
      autoInput = true;
    } else if (key == 'B') {
      intTempt = 50;
      Bdisplay();
      suhuPos = 6;
      manualInput = false;
      autoInput = true;
    } else if (key == 'C') {
      Cdisplay();
      suhuPos = 6;
      manualInput = true;
      autoInput = false;
    } else if (key == '*') {
      stringTempt = "";
      stringTime = "";
      suhuPos = 6;
      setDisplay();
    }
  }
  if (manualInput == true) {  //tombol C
    if (key >= '0' && key <= '9') {
      if (cInput == false) {
        stringTempt += key;
        suhuPos = suhuPos + 1;
        lcd.setCursor(suhuPos, 0);
        lcd.print(key);
      }
    }
    if (key == '#') {
      cInput = true;  // nilai suhu
      suhuPos = 6;
      if (stringTempt.length() > 0) {
        intTempt = stringTempt.toInt();
        stringTempt = "";
      }
    }
    if (cInput == true) {
      if (key >= '0' && key <= '9') {
        stringTime += key;
        suhuPos = suhuPos + 1;
        lcd.setCursor(suhuPos, 1);
        lcd.print(key);
      }
      if (key == 'D') {
        c2Input = true;
      }
    }
    if (c2Input == true) {  //nilai timer
      if (stringTime.length() > 0) {
        intTime = stringTime.toInt();
      }
      countDownTimer();
      if (temptValue <= intTempt) {
        digitalWrite(pinRelay, HIGH);
      } else {
        digitalWrite(pinRelay, LOW);
      }
      suhuPos = 6;
      cInput = false;
    }
  } else if (autoInput == true) {
    if (key >= '0' && key <= '9') {
      stringTime += key;
      suhuPos = suhuPos + 1;
      lcd.setCursor(suhuPos, 1);
      lcd.print(key);
      abInput = true;
    }
    if ((key == 'D') && (abInput == true)) {  // nilai timer
      ab2Input = true;
    }
    if (ab2Input == true) {
      if (stringTime.length() > 0) {
        intTime = stringTime.toInt();
      }
      countDownTimer();
    }
  }
}
void lastDisplay() {
  if (finishDisplay == false) {
    lcd.clear();
    doneDisplay();
    delay(1000);
    setDisplay();
    finishDisplay = true;
  }
}
void blokingRestart() {
  if (blockedButton == true) {
    finishDisplay = false;
    lastDisplay();
    timer.stop();
    digitalWrite(pinRelay, LOW);
    suhuPos = 6;
    CDT = false;
    abInput = false;
    ab2Input = false;
    cInput = false;
    c2Input = false;
    manualInput = false;
    autoInput = false;
    stringTempt = "";
    stringTime = "";
    blockedButton = false;
  }
}
void onComplete() {
  blockedButton = true;
}
void countDownTimer() {
  if (CDT == false) {
    timer.setCounter(0, intTime, 0, timer.COUNT_DOWN, onComplete);
    CDT = true;
  }
  if (key == 'D') {
    stoptimer = !stoptimer;
  }
  if (key == '#') {
    pausetimer = !pausetimer;
  }
  if (!pausetimer && !stoptimer) {
    timer.run();
    timer.start();
    lcd.setCursor(0, 0);
    lcd.print("Suhu : " + String(temptValue) + " C");
    lcd.setCursor(7, 1);
    lcd.print(timer.getCurrentTimeMin());

    if (temptValue <= intTempt) {
      digitalWrite(pinRelay, HIGH);
    } else {
      digitalWrite(pinRelay, LOW);
    }
  }
  if (pausetimer) {
    timer.pause();
    digitalWrite(pinRelay, LOW);
  }
  if (stoptimer) {
    timer.stop();
    blockedButton = true;
  }
}
void actWhenStop() {
  if (!timer.isStopped()) {
    runningCountinggg = true;
  }
  if ((runningCountinggg == true) && (timer.isStopped())) {
    runningCountinggg = false;
  }
}
void loop() {
  Serial.println(stoptimer);
  temptSensor();
  charKeypad();
  buzzer.loop();
  blokingRestart();
  delay(300);
}
