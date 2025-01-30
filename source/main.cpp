#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <DFRobot_DF1201S.h>
#include <Stepper.h>

/*************************************
  Pin and Communication Configurations
*************************************/
// GPS Module Serial Communication
static const int RXPin = D7, TXPin = D6;
static const uint32_t GPSBaud = 9600;

// MP3 Module Serial Communication
static const int RXPin_mp3 = 7, TXPin_mp3 = 21;
static const uint32_t BaudRate_mp3 = 115200;

// Stepper Motor Control Pins
#define IN1 3
#define IN2 4
#define IN3 5
#define IN4 6

// Button Pin
#define BTN_PIN D8

// Earth's radius in meters for Haversine distance calculation
const float R = 6371000;

// LED Pin
const int led = D10;

// Stepper motor rotation control variables
int Voltas_Motor = 3;
const int stepsPerRevolution = 100;

// Control flags
int flag;
int intervalBlink = 1000;
int BlinkState = HIGH;
int previousBlink = 0;

// Structure to hold coordinates and species information
struct CoordinatesBirds {
  float longitude;
  float latitude;
  int species;
};

// Number of coordinates and pointer to coordinate array
int numCoordinates;
CoordinatesBirds *coordinates;

/*************************************
  Function Prototypes
*************************************/
float haversineDistance(float lat1, float lon1, float lat2, float lon2);
float toRadians(float degree);
CoordinatesBirds *loadCoordinatesFiles(int &numCoordinates);
void TesteBegin();


// GPS, Stepper, and MP3 Module Objects
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
Stepper myStepper(stepsPerRevolution, IN4, IN3, IN2, IN1);
DFRobot_DF1201S DF1201S;
HardwareSerial DF1201SSerial(0);

/*************************************
  Function: setup
  Description: Initializes components and configurations.
*************************************/
void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(BTN_PIN, INPUT_PULLUP);
  ss.begin(GPSBaud);

  // Initialize MP3 Module
  Serial.println("Initializing MP3 module...");
  DF1201SSerial.begin(BaudRate_mp3, SERIAL_8N1, RXPin_mp3, TXPin_mp3);
  while (!DF1201S.begin(DF1201SSerial)) {
    Serial.println("MP3 Init failed, check wiring!");
    delay(1000);
  }
  
  // MP3 Module Settings
  DF1201S.setVol(100);
  DF1201S.switchFunction(DF1201S.MUSIC);
  DF1201S.setPlayMode(DF1201S.SINGLE);
  DF1201S.setPrompt(false);

  // Set stepper motor speed
  myStepper.setSpeed(1200);

  // Load predefined bird coordinates
  coordinates = loadCoordinatesFiles(numCoordinates);

  // Perform hardware test
  TesteBegin();
}

/*************************************
  Function: loop
  Description: Continuously checks GPS data and updates output.
*************************************/
void loop() {
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  // Check if GPS data is being received
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("No GPS detected: check wiring.");
    while (true);
  }
}

/*************************************
  Function: displayInfo
  Description: Displays GPS info, controls music, LED, and motor.
*************************************/
void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
  Serial.println(numCoordinates);

  for (int i = 0; i < numCoordinates; i++) {
    float distancia = haversineDistance(coordinates[i].latitude, coordinates[i].longitude, gps.location.lat(), gps.location.lng());

    Serial.println(distancia);
    Serial.println();

    
    if (distancia < 40) {
      Serial.println("Musica");
      if (digitalRead(BTN_PIN) == LOW) {
        Serial.println("Botão pressionado");
        if (coordinates[i].species == 1) {
          DF1201S.playFileNum(2);
          delay(8000);
        }
        if (coordinates[i].species == 2) {
          DF1201S.playFileNum(3);
          delay(8000);
        }
      } else {
        Serial.println("Botão não pressionado");
        DF1201S.pause();
      }
      
      digitalWrite(D10, HIGH);
      flag = i;

      while (Voltas_Motor != 0) {
        for (int i = 0; i < 16; i++) {
          Serial.println("counterclockwise");
          myStepper.step(stepsPerRevolution);
        }
        for (int j = 0; j < 16; j++) {
          Serial.println("counterclockwise");
          myStepper.step(-stepsPerRevolution);
        }
        Voltas_Motor = Voltas_Motor - 1;
      }
    }

    if (distancia >= 40 && distancia <= 50) {
      unsigned long currentMILLIS = millis();
      if (currentMILLIS - previousBlink >= intervalBlink) {
        previousBlink = currentMILLIS;
        if (BlinkState == HIGH) {
          BlinkState = LOW;
        } else {
          BlinkState = HIGH;
        }
        digitalWrite(D10, BlinkState);
      }
    }

    if (distancia > 50 && flag == i) {
      DF1201S.pause();
      digitalWrite(D10, LOW);
      Voltas_Motor = 3;
    }
  }
}


/*************************************
  Function: haversineDistance
  Description: Calculates distance between two coordinates.
*************************************/
float haversineDistance(float lat1, float lon1, float lat2, float lon2)
{
  // Convert latitude and longitude from degrees to radians
  lat1 = toRadians(lat1);
  lon1 = toRadians(lon1);
  lat2 = toRadians(lat2);
  lon2 = toRadians(lon2);

  // Difference in coordinates
  float dLat = lat2 - lat1;
  float dLon = lon2 - lon1;

  // Haversine formula
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(lat1) * cos(lat2) *
                sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));

  // Distance in meters
  float distance = R * c;

  return distance;
}

/*************************************
  Function: toRadians
  Description: Converts degrees to radians.
*************************************/
float toRadians(float degree) {
  return degree * (PI / 180);
}

/*************************************
  Function: loadCoordinatesFiles
  Description: Loads predefined coordinates.
*************************************/
CoordinatesBirds *loadCoordinatesFiles(int &numCoordinates) {
  static CoordinatesBirds coordinates[] = {
      {-9.140070, 38.720580, 2},
      {-9.139885, 38.722155, 2},
      {-9.141329, 38.720367, 1},
  };
  numCoordinates = sizeof(coordinates) / sizeof(coordinates[0]);
  return coordinates;
}

/*************************************
  Function: TesteBegin
  Description: Performs hardware tests.
*************************************/
void TesteBegin() {
  Serial.println("Starting Test...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
  for (int i = 0; i < 16; i++) myStepper.step(stepsPerRevolution);
  for (int j = 0; j < 16; j++) myStepper.step(-stepsPerRevolution);
  DF1201S.playFileNum(4);
  delay(2000);
  DF1201S.pause();
  Serial.println("Test Completed!");
}
