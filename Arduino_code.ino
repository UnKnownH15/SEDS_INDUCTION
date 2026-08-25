#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 8;
const int ledPin = 9;
const int buzzerPin = 10;
const int trigPin = 6;
const int echoPin = 7;
const int ldrPin = A0;

enum State { OPEN_SEA, ANCHOR_DROPPED, STORM, CHARYBDIS, WRECKED };
State currentState = OPEN_SEA;
State lastState = WRECKED;


bool anchorDropped = false;
unsigned long dangerStartTime = 0;
const unsigned long dangerThreshold = 5000; // 5 seconds


int buttonState = HIGH;
int lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  lcd.begin(16, 2);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  //Read Sensors
  int distance = getDistance();
  int lightLevel = analogRead(ldrPin); 
  
  //Read Button
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) { // Button pressed
        anchorDropped = !anchorDropped;
      }
    }
  }
  lastButtonReading = reading;

    if (currentState != WRECKED) { 
    if (anchorDropped) {
      currentState = ANCHOR_DROPPED;
    } else {
      switch(currentState) {
        
        case OPEN_SEA:
        case ANCHOR_DROPPED: // If anchor is raised, return to OPEN_SEA and check sensors
          if (lightLevel < 512) {
            currentState = STORM;
            dangerStartTime = millis();
          } else if (distance < 100) {
            currentState = CHARYBDIS;
            dangerStartTime = millis();
          } else {
            currentState = OPEN_SEA;
          }
          break;

        case STORM:
          if (lightLevel >= 512) {
            currentState = OPEN_SEA; // Escaped storm
          } else if (millis() - dangerStartTime >= dangerThreshold) {
            currentState = WRECKED;
          }
          break;

        case CHARYBDIS:
          if (distance >= 100) {
            currentState = OPEN_SEA; // Escaped Charybdis
          } else if (millis() - dangerStartTime >= dangerThreshold) {
            currentState = WRECKED;
          }
          break;
      }
    }
  }

  updateOutputs();
  updateLCD();
}
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

void updateOutputs() {

  if (currentState == WRECKED) {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    return;
  }

  // Handle Storm LED
  if (currentState == STORM) {
    digitalWrite(ledPin, (millis() / 250) % 2); // Blink every 250ms
  } else {
    digitalWrite(ledPin, LOW);
  }


  if (currentState == CHARYBDIS) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }
}

void updateLCD() {
  if (currentState != lastState) {
    lcd.clear();
    lcd.setCursor(0, 0);
    
    switch(currentState) {
      case OPEN_SEA:       lcd.print("OPEN SEA"); break;
      case ANCHOR_DROPPED: lcd.print("ANCHOR DROPPED"); break;
      case STORM:          lcd.print("STORM!"); break;
      case CHARYBDIS:      lcd.print("CHARYBDIS!"); break;
      case WRECKED:        lcd.print("WRECKED"); break;
    }
    lastState = currentState;
  }
}
