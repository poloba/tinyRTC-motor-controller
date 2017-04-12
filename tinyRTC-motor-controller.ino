#include <Wire.h>

#include <TimeLib.h>
#include <TimeLibPort.h>

#include <RTCLib.h>

int        ledUp=2;
int      ledDown=4;

int      buttonA=5;
int      buttonB=7;
int      buttonC=8;

int      motorUp=10; // Pin 2 of L293D
int    motorDown=11; // Pin 7 of L293D

int    switchTop=12;
int switchBottom=13;

void setup() {
    // TinyRTC
    pinMode(A3, OUTPUT);
    digitalWrite(A3, HIGH);
    pinMode(A2, OUTPUT);
    digitalWrite(A2, LOW);

    // Motor, switches & leds
    pinMode(motorUp, OUTPUT);
    pinMode(motorDown, OUTPUT);
    pinMode(switchTop, OUTPUT);
    pinMode(switchBottom, OUTPUT);
    pinMode(ledUp, OUTPUT);
    pinMode(ledDown, OUTPUT);

    // Remote control
    pinMode(buttonA, INPUT);
    pinMode(buttonB, INPUT);
    pinMode(buttonC, INPUT);

    Serial.begin(9600);
    while (!Serial); // wait for serial
    delay(200);
}

void loop() {

    if (digitalRead(buttonA)) {
        Serial.println("Button A, door down");
        doorDown();
    }
    if (digitalRead(buttonB)) {
        Serial.println("Button B, door up");
        doorUp();
    }
    if (digitalRead(buttonC)) {
        Serial.println("Button C, door stop");
        doorStop();
    }

    struct tm datetime;

    if(RTC.read(datetime)) {
        Serial.print("OK, Time = ");
        print2digits(datetime.tm_hour);
        Serial.write(':');
        print2digits(datetime.tm_min);
        Serial.write(':');
        print2digits(datetime.tm_sec);
        Serial.print(", Date (D/M/Y) = ");
        Serial.print(datetime.tm_mday);
        Serial.write('/');
        Serial.print(datetime.tm_mon);
        Serial.write('/');
        Serial.print(time_tm2y2k(datetime.tm_year));
        Serial.println();

        // Door going down
        if(datetime.tm_hour == 00 && datetime.tm_min == 11 && datetime.tm_sec == 21) {
          doorDown();
        }
        doorAction(motorDown,switchBottom);

        // Door going up
        if(datetime.tm_hour == 18 && datetime.tm_min == 47 && datetime.tm_sec == 20) {
          doorUp();
        }
        doorAction(motorUp,switchTop);
        
        delay(1000);
    } else {
        if (RTC.chipPresent()) {
          // Clock stopped, needed to set time config.
          Serial.println("The DS1307 is stopped.");
          Serial.println("Run timeset example.");
          Serial.println();
        }
        else {
          // Not detected
          Serial.println("DS1307 Not Detected.");
          Serial.println("Check hardware connections and reset board.");
          Serial.println();
        }
        // Wait forever
        for(;;);
    }
}


void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}

void motorDirection(int directionUp, int directionDown) {
    digitalWrite(motorUp, directionUp);
    digitalWrite(motorDown, directionDown);
}
void ledSwitch(int directionUp, int directionDown) {
    digitalWrite(ledUp, directionUp);
    digitalWrite(ledDown, directionDown);
}
void doorUp() {
    motorDirection(HIGH,LOW);
    ledSwitch(HIGH,LOW);
}
void doorDown() {
    motorDirection(LOW,HIGH);
    ledSwitch(LOW,HIGH);
}
void doorStop() {
    motorDirection(LOW,LOW);
    ledSwitch(LOW,LOW);
}

void doorAction(int motorDir, int switchPosition) {
  if(digitalRead(motorDir) == HIGH) {
      if(digitalRead(motorDown) == HIGH) {
          Serial.println("Door going down");
      } else {
          Serial.println("Door going up");
      }
      while(digitalRead(switchPosition) == LOW && digitalRead(motorDir) == HIGH) {
          Serial.println("Door closed");
          doorStop();
      }
  }
}

