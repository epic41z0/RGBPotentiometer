#define redPin 13
#define redButton 3
#define greenPin 12
#define greenButton 2
#define bluePin 5
#define potentioMeter A0
#include <Arduino.h>

int redState = LOW;
int greenState = LOW;
int brightness = 255;
int potentioMeterState = HIGH;

bool button1Enabled = true;
bool button2Enabled = true;
bool potentioMeterEnabled = true;

bool redButtonPressed = false;
bool greenButtonPressed = false;
int lastPressedButton = 0;

unsigned long debounceDelay = 50;
unsigned long lastRedDebounceTime = 0;
unsigned long lastGreenDebounceTime = 0;

void redInterrupt() {
  if (millis() - lastRedDebounceTime > debounceDelay) {
    lastRedDebounceTime = millis();
    redButtonPressed = true;
  }
}

void greenInterrupt() {
  if (millis() - lastGreenDebounceTime > debounceDelay) {
    lastGreenDebounceTime = millis();
    greenButtonPressed = true;
  }
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(redButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(redButton), redInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(greenButton), greenInterrupt, FALLING);

  Serial.begin(9600);

  Serial.println("Available commands:");
  Serial.println("enable <button/potentiometer_number>");
  Serial.println("disable <button/potentiometer_number>");
  Serial.println("ledon <led_number>");
  Serial.println("ledoff <led_number>");
  Serial.println("ledpower <power_value>");
}

void loop() {
  int val = 0;

  if (potentioMeterEnabled) {
    val = analogRead(potentioMeter);
    brightness = map(val, 0, 1023, 0, 255);
    analogWrite(bluePin, brightness);
  }

  if (redButtonPressed) {
    redButtonPressed = false;
    if (button1Enabled) {
      lastPressedButton = 1;
      redState = !redState;
      digitalWrite(redPin, redState);
    }
  }

  if (greenButtonPressed) {
    greenButtonPressed = false;
    if (button2Enabled) {
      lastPressedButton = 2;
      greenState = !greenState;
      digitalWrite(greenPin, greenState);
    }
  }

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil(' ');
    command.toLowerCase();

    Serial.println("Received command: " + command);


    if (command.equalsIgnoreCase("enable")) {
      int component = Serial.parseInt();
      if (component == 1) {
        button1Enabled = true;
        Serial.println("Button 1 enabled");
      } else if (component == 2) {
        button2Enabled = true;
        Serial.println("Button 2 enabled");
      } else if (component == 3) {
        potentioMeterEnabled = true;
        Serial.println("Potentiometer enabled");
      }
    } else if (command.equalsIgnoreCase("disable")) {
      int component = Serial.parseInt();
      if (component == 1) {
        button1Enabled = false;
        Serial.println("Button 1 disabled");
      } else if (component == 2) {
        button2Enabled = false;
        Serial.println("Button 2 disabled");
      } else if (component == 3) {
        potentioMeterEnabled = false;
        Serial.println("Potentiometer disabled");
      }
    } else if (command.equalsIgnoreCase("ledon")) {
      int led = Serial.parseInt();
      if (led == 1) {
        button1Enabled = false;
        digitalWrite(redPin, HIGH);
        Serial.println("LED 1 turned on");
      } else if (led == 2) {
        digitalWrite(greenPin, HIGH);
        button2Enabled = false;
        Serial.println("LED 2 turned on");
      }
    } else if (command.equalsIgnoreCase("ledoff")) {
      int led = Serial.parseInt();
      if (led == 1) {
        digitalWrite(redPin, LOW);
        Serial.println("LED 1 turned off");
      } else if (led == 2) {
        digitalWrite(greenPin, LOW);
        Serial.println("LED 2 turned off");
      }
    } else if (command.equalsIgnoreCase("ledpower")) {
      int power = Serial.parseInt();
      Serial.print("Received ledpower command with value: ");
      Serial.println(power);
      Serial.println("Setting LED power...");
      analogWrite(bluePin, power);
    }
  }
  
}
