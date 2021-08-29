/*
  Author: Aditya Dalvi
  Date:17/8/2021
  Name: code.ino
  Project: Agrosmart
*/

#include <Wire.h>
#include <DS3231.h>
#include <SPI.h>
#include <SD.h>
#define numOfSensors 2
#define UpMapCon 700
#define LowMapCon 200

DS3231 rtc(SDA, SCL);

int val[numOfSensors];
Time t;

#define ledPin 2
#define pumpPin 7
int ledState = LOW;
unsigned long previousMillis = 0;
bool SDInit = true;

String csvString = "";

void setup() {
  delay(2000);
  Serial.begin(9600);
  rtc.begin();
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    SDInit = false;
  } else {
    Serial.println("initialization done.");
  }
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  csvString += rtc.getTimeStr() + String(" ") + rtc.getDateStr() + String(",");
  if (dataFile && SDInit) {
    for (int pin = A0, count = 0 ; count < numOfSensors ; count++) {
      csvString += map(analogRead(pin), UpMapCon, LowMapCon, 0, 100);
      if (count != numOfSensors - 1) {
        csvString += String(",");
      }
      pin++;
    }
    dataFile.println(csvString);
    dataFile.close();
    Serial.println("written");
  } else {
    Serial.println("error opening datalog.txt");
  }
  Serial.println("Setup finished");
}

void loop() {
  int sum = 0 , avg = 0;
  t = rtc.getTime();

  csvString = rtc.getTimeStr() + String(" ") + rtc.getDateStr() + String(",");

  for (int pin = A0, count = 0 ; count < numOfSensors ; count++) {
    val[count] = map(analogRead(pin), UpMapCon, LowMapCon, 0, 100);
    csvString += val[count];
    if (count != numOfSensors - 1) {
      csvString += String(",");
    }
    pin++;
    sum = sum + val[count];
  }
  avg = sum / numOfSensors;
  Serial.println(csvString);
  if (/*t.min == 00 && */t.sec == 00 && SDInit) {
    Serial.println("Logging");
    digitalWrite(2, HIGH);
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(csvString);
      dataFile.close();
      Serial.println("written");
      delay(1000);
      digitalWrite(2, LOW);
    } else {
      Serial.println("error opening datalog.txt");
      SDInit = false;
    }
  }

  if (/*t.hour == 10 && t.min == 00 && */t.sec == 10) {
    Serial.print("water ");
    Serial.println(avg);
    water(avg);
  }
  delay(1000);
}
void water(int avg) {
  int duration = map(avg, 0 , 100 , 10 , 1 );
  digitalWrite(pumpPin, LOW);
  if (duration < 0) {
    duration = 0;
  }
  Serial.print("starting watering, duration = ");
  Serial.println(duration);
  delay(duration * 1000);
  digitalWrite(pumpPin, HIGH);
  Serial.println(".....done watering");
}
