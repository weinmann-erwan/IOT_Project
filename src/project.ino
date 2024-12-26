#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

// Pins for sensors
const int tempPin = A0;
const int soundPin = A1;
const int lightPin = A2;

// Scoring algorithm
int calculateScore(float temperature, float sound, float light) {
  // Example scoring formula: adjust based on your requirements
  return (int)(temperature * 0.4 + sound * 0.3 + light * 0.3);
}

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
    
  lcd.setRGB(colorR, colorG, colorB);

  // Initialize Serial communication
  Serial.begin(9600);

  // Display a startup message
  lcd.setCursor(0, 0);
  lcd.print("Room Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
}

void loop() {
  // Read sensor values
  float temperature = analogRead(tempPin) * (5.0 / 1023.0) * 100; // Adjust conversion for your sensor
  float sound = analogRead(soundPin);
  float light = analogRead(lightPin);

  // Calculate score
  int score = calculateScore(temperature, sound, light);

  // Create a data dictionary
  String room_id = "Room1";
  String data = "{";
  data += "\"room_id\":\"" + room_id + "\",";
  data += "\"temperature\":" + String(temperature, 1) + ",";
  data += "\"sound\":" + String(sound, 1) + ",";
  data += "\"light\":" + String(light, 1) + ",";
  data += "\"score\":" + String(score);
  data += "}";

  // Send data over Serial
  Serial.println(data);

  // Display data on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(temperature, 1) + " S:" + String(sound, 0));
  lcd.setCursor(0, 1);
  lcd.print("L:" + String(light, 0) + " Sc:" + String(score));

  // Wait before the next iteration
  delay(1000);
}