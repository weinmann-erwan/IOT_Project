#include <Wire.h>
#include "rgb_lcd.h"
#include <Arduino.h>

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 255;


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
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB
  }

  // Display a startup message
  lcd.setCursor(0, 0);
  lcd.print("Room Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  delay(2000);

  Serial.println("ARDUINO : Waiting for room ID...");
}

void loop() {
  const int duration = 10000; // 10 seconds
  const int interval = 100; // Interval between readings in milliseconds
  const int numReadings = duration / interval;
  String room_id;

  float totalTemperature = 0;
  float totalSound = 0;
  float totalLight = 0;

  // Wait for the server to send the room_id
  while (Serial.available() == 0) {
    // Wait for server input
  }
  room_id = Serial.readStringUntil('\n');
  Serial.println("ARDUINO : Room ID set to: " + room_id);

  for (int i = 0; i < numReadings; i++) {
    // Read sensor values
    float temperature = analogRead(tempPin) * (5.0 / 1023.0) * 100; // Adjust conversion for your sensor
    float sound = analogRead(soundPin);
    float light = analogRead(lightPin);

    // Accumulate the readings
    totalTemperature += temperature;
    totalSound += sound;
    totalLight += light;

    // Display remaining time on the LCD
    int secondsLeft = (duration - (i * interval)) / 1000;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(String(secondsLeft) + " sec left");

    // Wait for the next reading
    delay(interval);
  }

    // Compute the averages
  float avgTemperature = totalTemperature / numReadings;
  float avgSound = totalSound / numReadings;
  float avgLight = totalLight / numReadings;

  // Calculate score
  int score = calculateScore(avgTemperature, avgSound, avgLight);

  // Create a data collection to sent to the server
  String data = room_id + "," + String(avgTemperature, 1) + "," + String(avgSound, 1) + "," + String(avgLight, 1) + "," + String(score);
  Serial.println(data);

  // Display data on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(avgTemperature, 1) + " S:" + String(avgSound, 0));
  lcd.setCursor(0, 1);
  lcd.print("L:" + String(avgLight, 0) + " Sc:" + String(score));

  // Print "Finish" on the LCD and Serial
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Finish");
  Serial.println("ARDUINO : Finish");

  // Stop the code
  while (true) {
    // Infinite loop to stop the code
  }
}

