#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
  
const int chipSelect = 10;
const int SAMPLING_RATE = 500;

unsigned long timer = 0;
unsigned long previous_time = 0;
byte hall_state = 1;
byte rpm_state = 1;

byte previous_rpm_state = 0;
byte previous_hall_state = 0;
float velocityCounter = 0.0;
float rpmCounter = 0.0;

const int GEAR_TEETH = 10;
const float WHEEL_DIAMETER = 23;
const float inchesTometers = 0.0254;
const float pi = 3.14;

String data = "";

double velocity = 0.0;
double rpm = 0.0;     

const int RPM_PIN = 3;
const int HALL_PIN = 4;

File logFile;
int testCount = 0;
String acquisition = "logger";

void setup() {
  // put your setup code here, to run once:
  pinMode(HALL_PIN, INPUT);
  pinMode(RPM_PIN, INPUT);

  /*
  Serial.begin(38400);
  while (!Serial) {
  }
  
  Serial.println("Initializing SD card..."); 
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");    
    return;
  }
  Serial.println("card initialized.");
  */
  testCount = EEPROM.read(1);
  if (testCount >= 100)
  {
    testCount = 0;
  }
  testCount++;
  EEPROM.write(1, testCount);
  //Serial.println(testCount);

  acquisition += String(testCount) + ".csv";
  //Serial.println(acquisition);

  
  char fileNameCharArray[acquisition.length()];
  acquisition.toCharArray(fileNameCharArray, acquisition.length() + 1);
  
  logFile = SD.open(fileNameCharArray, O_WRITE | O_CREAT | O_APPEND);
  if (logFile)
  {
    logFile.println();
    logFile.close();
  }
}

void loop() {
  // put your main code here, to run repeatedly: 
  hall_state = digitalRead(HALL_PIN);
  rpm_state = digitalRead(RPM_PIN);

  while ((timer - previous_time) <= SAMPLING_RATE){
    timer = millis();
    if (previous_hall_state != hall_state){
      if (hall_state == 0){
        velocityCounter++;
        previous_hall_state = 0;
      }
      else{
        previous_hall_state = 1;
      }
    }
  }

  previous_time = timer;
  
  while ((timer - previous_time) <= SAMPLING_RATE){
    timer = millis();
    if (previous_rpm_state != rpm_state){
      if (rpm_state == 0){
        rpmCounter++;
        previous_rpm_state = 0;
      }
      else{
        previous_rpm_state = 1;
      }
    }
  }

  
  
  // Record to SD CARD
  velocity = getSpeed(velocityCounter);
  rpm = getRpm(rpmCounter);
  
 
  
  char fileNameCharArray[acquisition.length()];
  acquisition.toCharArray(fileNameCharArray, acquisition.length() + 1);
  logFile = SD.open(fileNameCharArray, O_WRITE | O_APPEND);
  if (logFile)
  {
    data  = String(velocity) + ", " + String(rpm);
    logFile.println(data);
    logFile.close();
  }
  else{
    //Serial.println("error opening files");
  }
    
    velocity = 0.0;
    rpm = 0.0;
    previous_time = timer;
    velocityCounter = 0.0;
    rpmCounter = 0.0;
    timer = millis();
    previous_time = timer;
 }
  


double getSpeed(float a){
  double speed = 0.0;
  double distance = 0.0;
  
  distance = ((2 * pi * inchesTometers * WHEEL_DIAMETER / 2) / GEAR_TEETH) * a;
  //speed = (distance / (SAMPLING_RATE)) * 3600;
  speed = distance * 3600;
  speed = speed / SAMPLING_RATE;
  return speed;
}

double getRpm(float a){
  float sample_rpm = 0;
  sample_rpm = a * 2;
  sample_rpm = sample_rpm * 60;
  return sample_rpm;
}


