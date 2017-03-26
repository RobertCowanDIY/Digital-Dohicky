
#include <SparkFunMAX31855k.h> // Using the max31855k driver
#include <SPI.h>  // Included here too due Arduino IDE; Used in above header
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// Define SPI Arduino pin numbers (Arduino Pro Mini)
const uint8_t CHIP_SELECT_PIN = 10; // Using standard CS line (SS)
// SCK & MISO are defined by Arduiino
const uint8_t VCC = 14; // Powering board straight from Arduino Pro Mini
const uint8_t GND = 15;

float min_temp = 0;
float max_temp = 0;
float temp_change = 0;
float laser_power = 0;

SparkFunMAX31855k probe(CHIP_SELECT_PIN, VCC, GND);

//smoothing code
const int numReadings = 8;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average

int reset_button = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("\nBeginning...");
  pinMode(2, INPUT_PULLUP);

  
  //clear initial readings
  for (int thisReading = 0; thisReading < numReadings; thisReading++) 
  {
    readings[thisReading] = 0;
  }
 
  delay(50);  // Let IC stabilize or first readings will be garbage

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  //display.display();
  //delay(5000);
  display.clearDisplay();
  display_header();
  delay(1000);  //wait for program to start
  take_temp_reading();
  take_temp_reading();
  min_temp = average;
  max_temp = average;
  Serial.print("Starting Temp: ");
  Serial.println(min_temp);
  delay(100);
  
}

void loop() 

{
  take_temp_reading();

  reset_button = digitalRead(2);
  
  if(reset_button == LOW)
  {
    min_temp = average;
    max_temp = average;
    laser_power = 0;        
    delay(250);
  }
  if(average > max_temp)
  {
    max_temp = average;
    Serial.print("Max Temp: ");
    Serial.print(max_temp);
    Serial.print("  |  ");
    Serial.print("Laser Power: ");
    temp_change = max_temp - min_temp;
    laser_power = temp_change * 2;
    Serial.print(laser_power);
    Serial.println("W");
  }   
  delay(10);
  display_header();
  display.setTextColor(WHITE);
  display.setCursor(0,9);
  display.print("NOW: ");
  display.print(average);
  display.print(" MAX: ");
  display.print(max_temp);
  display.setCursor(8,19);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("  ");
  display.print(laser_power); 
  display.println("W");
  display.display();
  display.clearDisplay();
}

void display_header()
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println("  Laser Power Meter  ");
}

void take_temp_reading()
{
  for(int i=0;i<37;i++)
  {
    // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = probe.readTempC();
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(average);
  delay(1);        // delay in between reads for stability
  }
}

