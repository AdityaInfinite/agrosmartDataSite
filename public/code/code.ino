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

const int ledPin = 2;
int ledState = LOW;
unsigned long previousMillis = 0;
bool SDInit = true;

String csvString = "";

void setup() {
  delay(2000);
  Serial.begin(9600);
  rtc.begin();
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  Serial.println("Setup finished");
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    SDInit = false;
  } else {
    Serial.println("initialization done.");
  }
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  csvString += rtc.getTimeStr() + String(" ") + rtc.getDateStr() + String(",");
  if (dataFile) {
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
  digitalWrite(3, LOW);
  if (duration < 0) {
    duration = 0;
  }
  Serial.print("starting watering, duration = ");
  Serial.println(duration);
  delay(duration * 1000);
  digitalWrite(3, HIGH);
  Serial.println(".....done watering");
}
