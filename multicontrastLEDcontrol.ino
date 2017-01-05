/*

 Timer and multicontrast control software for photographic printing.
 Tim Bowman
 
 Built on example code by  Nathan Seidle for the SparkFun 7-segment serial display.
 
 Serial7Segment is attached to Arduino as follows:
 A5 to SCL
 A4 to SDA
 VIN to PWR
 GND to GND
 
 Rotary encoder:
 red to D2
 green to D3
 black to ground
 
 This encoder apparently works by momentarily grounding the green wire for a clockwise 
 click, and momentarily grounding the red wire for a counter-clockwise click. It is 
 also useful to connect red and green to ground with a 0.1uf capacitor.
 
 Pushbutton switches:
 Green  (separate red wire) D7
 UV     (red wire)          D8
 Power  (green wire)        D9 
 Start  (yellow wire)       D10
 common (black wire) to ground
 
 LEDs associated with pushbutton switches:
 A0 to A3
 
 PWM lines for controlling LED power:
 G to D9
 UV1 to D10
 UV2 to D11
 
 
 */

#include <Wire.h>

const int DISPLAY_ADDRESS1 = 0x71; //This is the default address of the OpenSegment with both solder jumpers open
const int ROTARY_A = 2;
const int ROTARY_B = 3;
const int ROTARY_DEBOUNCE_DELAY = 30;   // in millis
const int SWITCH_DEBOUNCE_DELAY = 200;  // in millis
volatile unsigned long lastUpdate = millis();
volatile int counter = 0;
int greenCounter = counter;
int greenPower = 255;
int uvCounter = counter;
int uvPower = 255;
const unsigned long BLINK_INTERVAL = 125;
boolean blinkOn = false;
unsigned long previousBlinkMillis = millis();
const int GREEN_BUTTON = 4;
const int UV_BUTTON = 5;
const int POWER_BUTTON = 6;
const int START_BUTTON = 7;
boolean greenButtonState = true; // pullup resistor is on
boolean uvButtonState = true; // pullup resistor is on
boolean powerButtonState = true; // pullup resistor is on
boolean startButtonState = true; // pullup resistor is on
enum LEDMode { 
  ON, OFF, BLINK };
LEDMode greenButtonLedState = BLINK;
LEDMode uvButtonLedState = OFF;
LEDMode powerButtonLedState = OFF;
LEDMode startButtonLedState = ON;
String talk = String("1234");
String oldTalk = talk;
enum State { 
  GREEN, GREEN_SET_TIME, GREEN_SET_POWER, 
  UV, UV_SET_TIME, UV_SET_POWER, RUN_GREEN, RUN_UV };
State timerMode = GREEN;


void setup() 
{
  Wire.begin(); //Join the bus as master
  Serial.begin(9600); //Start serial communication at 9600 for debug statements
  Serial.println("Multicontrast LED Control");

  //Send the reset command to the display - this forces the cursor to return to the beginning of the display
  Wire.beginTransmission(DISPLAY_ADDRESS1);
  Wire.write('v');
  Wire.endTransmission();

  talk = "upup";
  oldTalk = talk;

  pinMode(ROTARY_A, INPUT);
  pinMode(ROTARY_B, INPUT);
  digitalWrite(ROTARY_A, HIGH); // turn on pull-up resistor
  digitalWrite(ROTARY_B, HIGH);
  attachInterrupt(0, rotaryChange, FALLING);
  attachInterrupt(1, rotaryChange, FALLING);

  i2cSendString(talk); //Send the four characters to the display

  // Set up buttons
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(UV_BUTTON, INPUT_PULLUP);
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(START_BUTTON, INPUT_PULLUP);
  // Set up button LEDs
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

}

void loop() 
{


  //Serial.println("Sample number"); //Just a debug statement
  //delay(250);
  //if (talk != oldTalk) {
  //  i2cSendString(talk);
  //  oldTalk = talk;

  talk = String(counter);
  if (talk != oldTalk){
    i2cSendString(talk);
    oldTalk = talk;
  }

  buttonListen();
  blinkenlights();


}

void buttonListen() {
  // Pays attention to button presses and handles state-switching

  // Don't even try if the last update is too recent.
  if (millis() < (lastUpdate + SWITCH_DEBOUNCE_DELAY)) 
    return;

  boolean buttonState;
  buttonState = digitalRead(GREEN_BUTTON);
  if (buttonState != greenButtonState) {
    greenButtonState = buttonState;
    lastUpdate = millis();
    // Do things only if button is pressed (pulled low.)
    if (!greenButtonState) {
      switch (timerMode) {
      case GREEN:
        timerMode = GREEN_SET_TIME;
        Serial.println("timerMode set to GREEN_SET_TIME");
        break;
      case GREEN_SET_TIME:
        timerMode = GREEN;
        Serial.println("timerMode set to GREEN");
        break;
      case GREEN_SET_POWER:
        timerMode = GREEN;
        Serial.println("timerMode set to GREEN_SET_POWER");
        break;
      case UV:
        timerMode = GREEN;
        Serial.println("timerMode set to GREEN");
        break;
      }
    }
  }

  buttonState = digitalRead(UV_BUTTON);
  if (buttonState != uvButtonState) {
    uvButtonState = buttonState;
    lastUpdate = millis();
    // Do things only if button is pressed (pulled low.)
    if (!uvButtonState) {
      switch (timerMode) {
      case UV:
        timerMode = UV_SET_TIME;
        Serial.println("timerMode set to UV_SET_TIME");
        break;
      case UV_SET_TIME:
        timerMode = UV;
        Serial.println("timerMode set to UV");
        break;
      case UV_SET_POWER:
        timerMode = UV;
        Serial.println("timerMode set to UV");
        break;
      case GREEN:
        timerMode = UV;
        Serial.println("timerMode set to UV");
        break;
      }
    }
  }

  buttonState = digitalRead(POWER_BUTTON);
  if (buttonState != powerButtonState) {
    powerButtonState = buttonState;
    lastUpdate = millis();
    // Do things only if button is pressed (pulled low.)
    if (!powerButtonState) {
      switch (timerMode) {
      case GREEN:
        timerMode = GREEN_SET_POWER;
        Serial.println("timerMode set to GREEN_SET_POWER");
        break;
      case UV:
        timerMode = UV_SET_POWER;
        Serial.println("timerMode set to UV_SET_POWER");
        break;
      case GREEN_SET_POWER:
        timerMode = GREEN;
        Serial.println("timerMode set to GREEN");
        break;
      case UV_SET_POWER:
        timerMode = UV;
        Serial.println("timerMode set to UV");
        break;
      }
    }
  }

  buttonState = digitalRead(START_BUTTON);
  if (buttonState != startButtonState) {
    startButtonState = buttonState;
    lastUpdate = millis();
    // Do things only if button is pressed (pulled low.)
    if (!startButtonState) {
      switch (timerMode) {
      case GREEN:
        timerMode = RUN_GREEN;
        Serial.println("timerMode set to RUN_GREEN");
        break;
      case UV:
        timerMode = RUN_UV;
        Serial.println("timerMode set to RUN_UV");
        break;
      case RUN_GREEN:
        timerMode = GREEN;
        Serial.println("timerMode set to GREEN");
        break;
      case RUN_UV:
        timerMode = UV;
        Serial.println("timerMode set to UV");
        break;
      }
    }
  }

}

void blinkenlights() {
  // Handles any on-off switching or blinking required of the button lights

    switch (timerMode) {
  case GREEN:
    greenButtonLedState = ON;
    uvButtonLedState = OFF;
    powerButtonLedState = OFF;
    startButtonLedState = OFF;
    break;
  case GREEN_SET_TIME:
    greenButtonLedState = BLINK;
    uvButtonLedState = OFF;
    powerButtonLedState = OFF;
    startButtonLedState = OFF;
    break;
  case GREEN_SET_POWER:
    greenButtonLedState = BLINK;
    uvButtonLedState = OFF;
    powerButtonLedState = BLINK;
    startButtonLedState = OFF;
    break;
  case UV:
    greenButtonLedState = OFF;
    uvButtonLedState = ON;
    powerButtonLedState = OFF;
    startButtonLedState = OFF;
    break;
  case UV_SET_TIME:
    greenButtonLedState = OFF;
    uvButtonLedState = BLINK;
    powerButtonLedState = OFF;
    startButtonLedState = OFF;
    break;
  case UV_SET_POWER:
    greenButtonLedState = OFF;
    uvButtonLedState = BLINK;
    powerButtonLedState = BLINK;
    startButtonLedState = OFF;
    break;
  case RUN_GREEN:
    greenButtonLedState = ON;
    uvButtonLedState = OFF;
    powerButtonLedState = OFF;
    startButtonLedState = BLINK;
    break;
  case RUN_UV:
    greenButtonLedState = OFF;
    uvButtonLedState = ON;
    powerButtonLedState = OFF;
    startButtonLedState = BLINK;
    break;
  }

  boolean blinkSwitchNow = false;
  unsigned long now = millis();
  if (now > (previousBlinkMillis + BLINK_INTERVAL)) {
    blinkSwitchNow = true;
    previousBlinkMillis = millis();
    blinkOn = !blinkOn;
  }

  switch (greenButtonLedState) {
  case ON:
    digitalWrite(A0, HIGH);
    break;
  case OFF:
    digitalWrite(A0, LOW);
    break;
  case BLINK:
    if (blinkSwitchNow) {
      digitalWrite(A0, blinkOn);
    }
    break;
  }

  switch (uvButtonLedState) {
  case ON:
    digitalWrite(A1, HIGH);
    break;
  case OFF:
    digitalWrite(A1, LOW);
    break;
  case BLINK:
    if (blinkSwitchNow) {
      digitalWrite(A1, blinkOn);
    }
    break;
  }

  switch (powerButtonLedState) {
  case ON:
    digitalWrite(A2, HIGH);
    break;
  case OFF:
    digitalWrite(A2, LOW);
    break;
  case BLINK:
    if (blinkSwitchNow) {
      digitalWrite(A2, blinkOn);
    }
    break;
  }

  switch (startButtonLedState) {
  case ON:
    digitalWrite(A3, HIGH);
    break;
  case OFF:
    digitalWrite(A3, LOW);
    break;
  case BLINK:
    if (blinkSwitchNow) {
      digitalWrite(A3, blinkOn);
    }
    break;
  }


}
void rotaryChange(){
  if (millis() > (lastUpdate + ROTARY_DEBOUNCE_DELAY)) {
    if (digitalRead(ROTARY_A) && !digitalRead(ROTARY_B)) {
      // increment
      lastUpdate = millis();
      counter ++;
    } 
    else if (!digitalRead(ROTARY_A) && digitalRead(ROTARY_B)) {
      // decrement
      lastUpdate = millis();
      counter --;
    }
  }
}



//Given a string, i2cSendString chops up the string and sends out the first four characters over i2c
void i2cSendString(String toSend)
{
  while (toSend.length() < 4) {
    toSend = " " + toSend;
  }
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  for(byte x = 0 ; x < 4 ; x++) 
    Wire.write(toSend.charAt(x)); //Send a character from the array out over I2C
  Wire.endTransmission(); //Stop I2C transmission
}


























