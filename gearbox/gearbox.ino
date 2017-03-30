#include <SPI.h>
#include <SD.h>
  
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

String dataString = "";

double velocity = 0.0;
double rpm = 0.0;     

const int RPM_PIN = 3;
const int HALL_PIN = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(HALL_PIN, INPUT);
  pinMode(RPM_PIN, INPUT);
  Serial.begin(38400);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
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
  
  File dataFile = SD.open("datalog.txt",  FILE_WRITE);
  
  if (dataFile){
    dataString = "Speed: ," + String(velocity) + " ,RPM: ," + rpm;
    dataFile.println(dataString);
    dataFile.close();
    Serial.println(dataString);
  }
  else{
    Serial.println("error opening files");
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


