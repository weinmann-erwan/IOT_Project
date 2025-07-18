#include <Wire.h>
#include "rgb_lcd.h"
#include <Arduino.h>

// Initialize the LCD object
rgb_lcd lcd;

// Define RGB color values
const int colorR = 255;
const int colorG = 0;
const int colorB = 255;

// Pins for sensors
int B = 3975;            // B value of the thermistor
const int tempPin = A0;
const int soundPin = A1;
const int lightPin = A2;

// Structure to define a range with minimum and maximum values
struct Range {
    float min;
    float max;
};

// Ranges for normalization
const Range tempRange = {15.0, 25.0};    // Range for temperature
const Range lightRange = {0.0, 10.0};    // Range for light
const Range soundRange = {0.0, 10.0};    // Range for sound

// Weights for the score calculation
const float weightTemp = 0.34;
const float weightLight = 0.33;
const float weightSound = 0.33;

// Function to normalize a value within a range
float normalize(float value, float min, float max) {
    float normalized = (value - min) / (max - min);
    //return constrain(normalized, 0.0, 1.0); // Constrain between 0 and 1
    return normalized;
}

// Function to calculate the score
float calculateScore(float temp, float sound, float light) {
    float normalizedTemp = normalize(temp, tempRange.min, tempRange.max);
    Serial.println("Normalized Temperature: " + String(normalizedTemp));
    float normalizedSound = normalize(sound, soundRange.min, soundRange.max);
    Serial.println("Normalized Sound: " + String(normalizedSound));
    float normalizedLight = normalize(light, lightRange.min, lightRange.max);
    Serial.println("Normalized Light: " + String(normalizedLight));
    
    // Calculate the weighted score
    float score = (weightTemp * normalizedTemp) +
                  (weightSound * normalizedSound) +
                  (weightLight * normalizedLight);

    Serial.println("Score: " + String(score));
    return score;
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

  Serial.println("ARDUINO: Waiting for room ID...");
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
  Serial.println("ARDUINO: Room ID set to: " + room_id);

  for (int i = 0; i < numReadings; i++) {
    // Read sensor values

    // Calculate the temperature and convert to degrees
    int val = analogRead(tempPin);                               
    float resistance = (float)(1023 - val) * 10000 / val;
    float temperature = 1 / (log(resistance / 10000) / B + 1 / 298.15) - 273.15;
    
    // Adjust conversion for your sensor
    float s = analogRead(soundPin);
    float sound = (s / 1023.0) * 10.0;

    float l = analogRead(lightPin);
    float light = (l / 1023.0) * 10.0;

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
  float score = calculateScore(avgTemperature, avgSound, avgLight);

  // Create a data collection to send to the server
  String data = room_id + "," + String(avgTemperature, 1) + "," + String(avgSound, 1) + "," + String(avgLight, 1) + "," + String(score);
  Serial.println(data);

  // Display data on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:" + String(avgTemperature, 1) + " S:" + String(avgSound, 1));
  lcd.setCursor(0, 1);
  lcd.print("L:" + String(avgLight, 1) + " Sc:" + String(score));

  Serial.println("ARDUINO: Finish");

  // Stop the code
  while (true) {
    // Infinite loop to stop the code
  }
}

