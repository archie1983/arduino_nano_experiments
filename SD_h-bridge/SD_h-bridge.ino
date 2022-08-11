/*
 This example shows how to read data from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 */

#include <SPI.h>
#include <SD.h>

File myFile;
int motor2_forward_pin = 5;
int motor2_back_pin = 4;

int motor1_forward_pin = 3;
int motor1_back_pin = 2;


void setup() {
  pinMode(motor1_forward_pin, OUTPUT);
  pinMode(motor1_back_pin, OUTPUT);
  pinMode(motor2_forward_pin, OUTPUT);
  pinMode(motor2_back_pin, OUTPUT);

  makeMotorsIdle();
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }

  // re-open the file for reading:
  myFile = SD.open("commands.txt");
  if (!myFile) {
    // if the file didn't open, print an error:
    Serial.println("error opening commands.txt");
    while (1);
  }
}

char strBuf[10];
char chSymbol;
int intIndex = 0;

void loop() {
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      chSymbol = myFile.read();

      //Serial.write(chSymbol);

      if (chSymbol == '\n') {
        if (strncmp(strBuf, "FWD", 3) == 0) {
          //# go forward
          Serial.println("forward");
          runMotor1_fwd();
          runMotor2_fwd();          
        } else if (strncmp(strBuf, "BACK", 4) == 0) {
          //# go backward
          Serial.println("backward");
          runMotor1_back();
          runMotor2_back();          
        } else if (strncmp(strBuf, "LEFT", 4) == 0) {
          //# turn left
          Serial.println("left");
          runMotor1_fwd();
          runMotor2_back();
        } else if (strncmp(strBuf, "RIGHT", 5) == 0) {
          //# turn right
          Serial.println("right");
          runMotor1_back();
          runMotor2_fwd();
        }
        delay(1000);
        makeMotorsIdle();
        delay(2000);
        //strBuf[intIndex] = 0;
        intIndex = 0;
      } else {
        strBuf[intIndex] = chSymbol;
        intIndex++;
      }
    }
    // close the file:
    myFile.close();

    // finish work
    while (1);
}

void runMotor1_fwd() {
  digitalWrite(motor1_forward_pin, HIGH);
  digitalWrite(motor1_back_pin, LOW);
}

void runMotor1_back() {
  digitalWrite(motor1_forward_pin, LOW);
  digitalWrite(motor1_back_pin, HIGH);
}

void runMotor2_fwd() {
  digitalWrite(motor2_forward_pin, HIGH);
  digitalWrite(motor2_back_pin, LOW);
}

void runMotor2_back() {
  digitalWrite(motor2_forward_pin, LOW);
  digitalWrite(motor2_back_pin, HIGH);
}

void makeMotorsIdle() {
  digitalWrite(motor1_forward_pin, LOW);
  digitalWrite(motor1_back_pin, LOW);
  digitalWrite(motor2_forward_pin, LOW);
  digitalWrite(motor2_back_pin, LOW);
}

void spin_around() {
  runMotor1_back();
  runMotor2_fwd();
}
