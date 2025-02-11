#include <TinyGPSPlus.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <DFRobot_DF1201S.h>
#include <Stepper.h>

#define PIN    16  // Data pin for NeoPixel
#define NUMPIXELS 1  // Number of NeoPixels


// Define the GPS Serial Port (Use Hardware Serial1)
#define RXPin 0  // Feather M4 RX1 (connect to GPS TX)
#define TXPin 1  // Feather M4 TX1 (connect to GPS RX)
#define GPSBaud 9600



// Stepper motor rotation control variables
int Voltas_Motor = 3;
const int stepsPerRevolution = 1700;


// TinyGPSPlus object
TinyGPSPlus gps;
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
SoftwareSerial DF1201SSerial(25, 24);
DFRobot_DF1201S DF1201S;
Stepper stepper(stepsPerRevolution, 10, 11, 12, 13);


// Button Pin
#define BTN_PIN 6





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

// Earth's radius in meters for Haversine distance calculation
const float R = 6371000;



CoordinatesBirds *loadCoordinatesFiles(int &numCoordinates);
float haversineDistance(float lat1, float lon1, float lat2, float lon2);
float toRadians(float degree);
void TesteBegin();


void setup() {
  Serial.begin(115200);   // USB Serial for debugging
  Serial1.begin(GPSBaud); // GPS connected to Serial1
  strip.begin();
  DF1201SSerial.begin(115200);
  while(!DF1201S.begin(DF1201SSerial)){
    Serial.println("Init failed, please check the wire connection!");
    delay(1000);
  }


  // MP3 Module Settings
  DF1201S.setVol(100);
  DF1201S.switchFunction(DF1201S.MUSIC);
  DF1201S.setPlayMode(DF1201S.SINGLE);
  DF1201S.setPrompt(false);

  Serial.println(F("GPS Example for Adafruit Feather M4 Express"));
  Serial.print(F("Using TinyGPSPlus library v. "));
  Serial.println(TinyGPSPlus::libraryVersion());

  pinMode(BTN_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  stepper.setSpeed(60);




    // Load predefined bird coordinates
  coordinates = loadCoordinatesFiles(numCoordinates);


  // Perform hardware test
  TesteBegin();
  
  DF1201S.pause();
}

void loop() {
  // Read GPS data
  while (Serial1.available() > 0) {
    if (gps.encode(Serial1.read())) {
      displayInfo();
    }
  }

  // Check if GPS is connected
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("No GPS detected: check wiring."));
    while (true); // Stop program if no GPS detected
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
          DF1201S.pause();
          DF1201S.next();
          DF1201S.pause();
        }
        if (coordinates[i].species == 2) {
          DF1201S.playFileNum(3);
          delay(8000);
          DF1201S.pause();
          DF1201S.next();
          DF1201S.pause();
        }
        
      } else {
        Serial.println("Botão não pressionado");
        //DF1201S.pause();
      }
      
      strip.setPixelColor(0, strip.Color(255, 255, 255));  // Red color
      strip.show();
      flag = i;

      while (Voltas_Motor != 0) {
        /*for (int i = 0; i < 16; i++) {
          Serial.println("counterclockwise");
          stepper.step(stepsPerRevolution);
        }
        for (int j = 0; j < 16; j++) {
          Serial.println("counterclockwise");
          stepper.step(-stepsPerRevolution);
        }*/
        stepper.step(stepsPerRevolution);
        stepper.step(-stepsPerRevolution);
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
        if(BlinkState == HIGH){
          strip.setPixelColor(0, strip.Color(255, 255, 255));  // Red color
          strip.show();
        }
        if(BlinkState == LOW){
          strip.setPixelColor(0, strip.Color(0, 0, 0));  // Red color
          strip.show();
        }
      }
    }

    if (distancia > 50 && flag == i) {
      //DF1201S.pause();
      strip.setPixelColor(0, strip.Color(0, 0, 0));  // Red color
      strip.show();
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
    strip.setPixelColor(0, strip.Color(255, 255, 255));  
    strip.show();
    delay(500);
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.show();
    delay(500);
  }
  //for (int i = 0; i < 16; i++) stepper.step(stepsPerRevolution);
  //for (int j = 0; j < 16; j++) stepper.step(-stepsPerRevolution);
  stepper.step(stepsPerRevolution);
  stepper.step(-stepsPerRevolution);
  stepper.step(stepsPerRevolution);
  stepper.step(-stepsPerRevolution);
  stepper.step(stepsPerRevolution);
  stepper.step(-stepsPerRevolution);
  DF1201S.playFileNum(4);
  delay(2000);
  DF1201S.pause();
  Serial.println("Test Completed!");
  DF1201S.next();
  DF1201S.pause();
}
